/*
 * Simple html link extractor used to show
 * File/Regex/list/String usage
 */

#include <edelib/Regex.h>
#include <stdio.h>

EDELIB_NS_USE

void help(void) {
	puts("elinks [FILE]");
	puts("Extract html links from [FILE] and print them to stdout");
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
	char buf[8094];

	FILE* f = fopen(argv[1], "r");
	if(!f)
		return 1;

	while(fgets(buf, sizeof(buf), f)) {
		pos = r.match(buf, 0, &mv);
		if(pos < 1)
			continue;

		if(mv.size() == 0)
			continue;

		for(it = mv.begin(); it != mv.end(); ++it) {
			for(int i = (*it).offset; i < (*it).length + (*it).offset && i < (int)sizeof(buf); ++i)
				putchar(buf[i]);
			putchar('\n');
		}
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
