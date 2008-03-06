#ifndef __STRSPLIT_H__
#define __STRSPLIT_H__


char** strsplit(const char* string, 
                const char* delim, 
                int maxtok);

void strsplit_free(char** str);

#endif
