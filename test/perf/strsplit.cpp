#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct _StrSplitList {
	char*  data;
	struct _StrSplitList* next;
} StrSplitList;


char** strsplit(const char* string, const char* delim, int maxtok) {
	StrSplitList *slist, *string_list = NULL, *new_list;
	char** ret;
	unsigned int i = 0, j = 0, len, toksz;
	unsigned int n = 0;

	if(maxtok < 1)
		maxtok = INT_MAX;

	len = strlen(string);

	while(i < len) {
		while((i < len) && (strchr(delim, string[i]) != NULL))
			i++;

		if(i == len)
			break;

		j = i + 1;
		while((j < len) && (strchr(delim, string[j]) == NULL))
			j++;

		toksz = j - i;

		if(toksz == len)
			break;

		new_list = (StrSplitList*)malloc(sizeof(StrSplitList));
		new_list->data = (char*)malloc(toksz + 1);

		strncpy(new_list->data, &string[i], toksz);
		new_list->data[toksz] = 0;
		new_list->next = string_list;

		string_list = new_list;

		n++;

		//printf("start is %c len %i\n", string[i], j-i);

		i = j + 1;
	}

	ret = (char**)malloc(sizeof(char*) * (n + 1));
	ret[n] = NULL;

	for(slist = string_list; slist; slist = slist->next) {
		ret[--n] = slist->data;
	}

	for(slist = string_list; slist; slist = new_list) {
		new_list = slist->next;
		free(slist);
	}
	
	return ret;
}

void strsplit_free(char** str) {
	unsigned int i = 0;
	for(; str[i]; i++)
		free(str[i]);
	free(str);
}

#if 0
int main() {
	int i = 0;
	char** s = strsplit("bla xxxx dasdas asdasdas asdasds b|b|b|b|b", " ", -1);
	while(s[i]) {
		printf("%s\n", s[i]);
		++i;
	}

	strsplit_free(s);
	return 0;
}
#endif
