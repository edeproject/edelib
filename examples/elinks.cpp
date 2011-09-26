/*
 * Simple html link extractor used to show
 * File/Regex/list/String usage
 */

#include <edelib/Regex.h>
#include <edelib/Functional.h>
#include <stdio.h>

EDELIB_NS_USE

#define BUFSZ 8094

static void help(void) {
	puts("elinks [FILE]");
	puts("Extract html links from [FILE] and print them to stdout");
}

static void printer(const RegexMatch& m, void *b) {
	const char *buf = (const char*)b;

	for(int i = m.offset; i < m.length + m.offset && i < BUFSZ; ++i)
		putchar(buf[i]);

	putchar('\n');
}

int main(int argc, char** argv) {
	if(argc < 2) {
		help();
		return 1;
	}

	Regex r;
	Regex::MatchVec mv;
	Regex::MatchVec::iterator it;

	r.compile("<a[^>]+href=(\"[^\"]*\"|[^[:space:]]+)[^>]*>", RX_EXTENDED | RX_CASELESS);
	if(!r) {
		printf("Pattern error: %s\n", r.strerror());
		return 1;
	}

	String errstr;
	list<String> errors, links;
	list<String>::iterator eit;
	int pos;
	char buf[BUFSZ];

	FILE* f = fopen(argv[1], "r");
	if(!f)
		return 1;

	while(fgets(buf, BUFSZ, f)) {
		pos = r.match(buf, 0, &mv);
		if(pos < 1)
			continue;

		if(mv.size() == 0)
			continue;

		for_each(printer, mv, buf);
	}

	fclose(f);

	if(!errors.empty()) {
		eit = errors.begin();
		while(eit != errors.end()) {
			printf("*** %s\n", (*eit).c_str());
			++eit;
		}
	}

	return 0;
}
