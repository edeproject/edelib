#include <stdio.h>
#include <string.h>
#include <edelib/TiXml.h>
#include <edelib/List.h>

#define PROG_NAME "edelib-dbus-xml2cpp"

#define STR_EQUAL(str1, str2) (strcmp(str1, str2) == 0)
#define SAFE_FREE(val) if(val) free(val)

#define FUNCTION_SIGNAL_TYPE 1
#define FUNCTION_METHOD_TYPE 2

using edelib::String;
using edelib::list;

struct FunctionStack;
typedef list<FunctionStack>  FuncStack;
typedef list<char*>          ArgList;

struct FunctionStack {
	int          type;
	char*        interface;
	char*        name;
	char*        ret;
	ArgList*     arguments;
};

void func_stack_append(FuncStack& stk, const char* interf_name, const char* method_name, 
		const char* ret, ArgList* args, int type) {

	/* allow duplicates */

	FunctionStack n;
	n.type = type;

	if(interf_name)
		n.interface = strdup(interf_name);
	else
		n.interface = NULL;

	if(method_name)
		n.name = strdup(method_name);
	else
		n.name = NULL;

	if(ret)
		n.ret = strdup(ret);
	else
		n.ret = NULL;

	n.arguments = args;
	stk.push_back(n);
}

void func_stack_clear_args(ArgList* args) {
	if(!args)
		return;

	ArgList::iterator it = args->begin(), it_end = args->end();
	while(it != it_end) {
		SAFE_FREE(*it);
		++it;
	}

	delete args;
}

void func_stack_clear(FuncStack& stk) {
	FuncStack::iterator it = stk.begin(), it_end = stk.end();

	while(it != it_end) {
		SAFE_FREE((*it).interface);
		SAFE_FREE((*it).name);
		SAFE_FREE((*it).ret);

		func_stack_clear_args((*it).arguments);
		++it;
	}
}

ArgList* scan_arguments(TiXmlNode* arg, const char* interf_name, const char* method_name) {
	if(!arg)
		return NULL;

	const char* name, *direction, *type;
	bool have_out = false;

	ArgList* arg_list = new ArgList;

	for(; arg; arg = arg->NextSibling()) {
		name = arg->ToElement()->Attribute("name");
		direction = arg->ToElement()->Attribute("direction");
		type = arg->ToElement()->Attribute("type");

		if(!direction) {
			fprintf(stderr, "Warning: no 'direction' attribute for %s.%s, skipping...\n", interf_name, method_name);
			continue;
		}

		if(!type) {
			fprintf(stderr, "Warning: no 'type' attribute %s.%s, skipping...\n", interf_name, method_name);
			continue;
		}

		// allow only one 'out'
		if(STR_EQUAL(direction, "out")) {
			//printf("%s %s.%s\n", type, interf_name, method_name);
			if(have_out == true)
				fprintf(stderr, "Warning: multiple return values ?\n");
			have_out = true;
		}

		if(STR_EQUAL(direction, "in")) {
			if(type) {
				// we are interested in 'type'; it contains returned type
				char* copy = strdup(type);
				arg_list->push_back(copy);
			}
		}
	}

	return arg_list;
}

void generate_code(FuncStack& stk) {
	printf("/* generated with %s */\n", PROG_NAME);
	puts("#include <edelib/EdbusConnection.h>");
	puts("#include <edelib/EdbusMessage.h>");
	printf("\n");

	FuncStack::iterator it = stk.begin(), it_end = stk.end();
	FunctionStack* p;

	// generate prototypes
	for(; it != it_end; ++it) {
		p = &(*it);

		// generate comment
		printf("/* %s %s.%s( ", p->ret, p->interface, p->name);

		if(p->arguments) {
			ArgList::iterator ait = p->arguments->begin(), ait_end = p->arguments->end();
			for(; ait != ait_end; ++ait) {
				printf("%s ", *ait);
			}

			printf(")\n");
			ait = p->arguments->begin();
			for(; ait != ait_end; ++ait)
				printf(" * %s - argument\n", *ait);
			printf(" */\n");

		} else {
			printf("void )\n");
		}

		// do real code
		printf("static int %s(const EdbusMessage* msg, void* data) {\n", p->name);
		printf("\treturn 1;\n");
		printf("}\n\n");
	}

	// now write table callback
	printf("/* methods */\n");
	printf("EdbusCallbackItem method_table[] = {\n");

	for(it = stk.begin(); it != it_end; ++it)
		printf("\t{ NULL, \"%s\", \"%s\", %s, NULL },\n", (*it).interface, (*it).name, (*it).name);

	printf("};\n");
}

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Usage: %s <xml>\n", PROG_NAME);
		printf("Generate C++ code from D-Bus introspection file\n");
		return 1;
	}

	TiXmlDocument doc(argv[1]);
	if(!doc.LoadFile()) {
		printf("Unable to load '%s'. ", argv[1]);
		printf("Is this correct D-Bus description XML file?\n");
		return 1;
	}

	/* String org.freedesktop.DBus.Introspectable.Introspect(void) */
	FuncStack stack_content;

	const char* interf_name;
	const char* method_name;

	TiXmlNode* method, *arg;
	TiXmlNode* node = doc.FirstChild("node");

	if(!node) {
		fprintf(stderr, "Unable to find master node. Is this correct D-Bus description XML file?\n");
		return 1;
	}

	method = arg = 0;

	for(node = node->FirstChildElement(); node; node = node->NextSibling()) {
		if(STR_EQUAL(node->Value(), "interface")) {
			interf_name = node->ToElement()->Attribute("name");

			method = node->FirstChildElement();
			if(!method)
				continue;

			for(; method; method = method->NextSibling()) {
				if(STR_EQUAL(method->Value(), "signal")) {
					fprintf(stderr, "Skipping signals for now...\n");
					continue;
				}

				method_name = method->ToElement()->Attribute("name");
				arg = method->FirstChildElement();

				ArgList* a = scan_arguments(arg, interf_name, method_name);
				func_stack_append(stack_content, interf_name, method_name, "void", a, FUNCTION_METHOD_TYPE);
			}
		}
	}

	generate_code(stack_content);
	func_stack_clear(stack_content);
	return 0;
}
