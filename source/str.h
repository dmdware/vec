#ifndef STR_H
#define STR_H

#include "platform.h"

void pstrset(char **out, const char *in);
void pstradd(char **out, const char *in);
void psubstr(char **out, const char *in, int beg, int len);
void delprev(char **s, int *caret);
void delnext(char **s, int *caret);
int prevlen(char *s, int caret);
int nextlen(char *s, int caret);

#endif
