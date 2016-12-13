#ifndef UNICODE_H
#define UNICODE_H

#include "../platform.h"
#include "../ustring.h"

#define UTF8PAD	6

unsigned int *ToUTF32(const unsigned char *utf8);
unsigned char *ToUTF8(const unsigned int *unicode);
int UTF32Len(const unsigned int *ustr);

#endif