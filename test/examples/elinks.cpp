/*
 * Simple html link extractor used to show
 * File/Regex/list/String usage
 */

#include <edelib/File.h>
#include <edelib/Regex.h>
#include <stdio.h>

using namespace edelib;

void help(void) {
	puts("elinks [FILES...]");
	puts("Extract html links from [FILES] and print them to stdout");
}

void dump_links(list<String>& ls) {
	list<String>::iterator it, it_end;
	it = ls.begin();
	it_end = ls.end();

	while(it != it_end) {
		printf("%s\n", (*it).c_str());
		++it;
	}

	ls.clear();
}

int main(int argc, char** argv) {
	if(argc < 2) {
		help();
		return 1;
	}

	Regex r;
	r.compile("<a[^>]+href=(\"[^\"]*\"|[^[:space:]]+)[^>]*>", RX_EXTENDED | RX_ICASE);
	if(!r) {
		printf("Pattern error: %s\n", r.strerror());
		return 1;
	}

	String errstr;
	list<String> errors, links;
	list<String>::iterator eit;
	char buff[8094];
	File f;

	if(!f.open(argv[1])) {
		return 1;
	}

	for(int i = 1; i < argc; i++) {
		if(!f.open(argv[i])) {
			errstr = "Unable to open ";
			errstr += argv[i];
			errors.push_back(errstr);
			continue;
		}

		while(f.readline(buff, sizeof(buff)-1) >= 0) {
			r.split(buff, links);
			dump_links(links);
		}

		f.close();
	}

	if(!errors.empty()) {
		eit = errors.begin();
		while(eit != errors.end()) {
			printf("*** %s\n", (*eit).c_str());
			++eit;
		}
	}

	return 0;
}
