#include <ctype.h>
#include "utf8.h"

/* found on http://canonical.org/~kragen/strlen-utf8.html */
int utf8_strlen(char *s) {
	int i = 0, j = 0;
	while(s[i]) {
		if((s[i] & 0xc0) != 0x80) j++;
		i++;
	}
	return j;
}

/* stolen from FreeDOS 32 project (c) Salvatore Isaja */
int utf8_stricmp(const char *s1, const char *s2) {
	for(;;) {
		if(!(*s2 & 0x80)) {
			if(toupper(*s1) != toupper(*s2)) return 1;
			if(*s1 == 0) return 0;
			s1++; s2++;
		} else {
			if(*s1 != *s2) return 1;
			if(*s1 == 0) return 0;
			s1++; s2++;
		}
	}

	return 0;
}
