/*
 * Copyright (c) 2007 Alexey Vatchenko <av@bsdua.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * utf8: implementation of UTF-8 charset encoding (RFC3629).
 */

#ifndef RICHTEXT_H
#define RICHTEXT_H

#include "../platform.h"
#include "font.h"

//types of blocks
//max UTF32 = 0x7fffffff (INT_MAX)
#define RICH_GLYPH_START	0
#define RICH_GLYPH_END		(CODE_POINTS-1)
#define RICH_END			(0x7fffffff)
#define RICH_ICON_END		(RICH_END-1)
#define RICH_ICON_START		(RICH_ICON_END-ICONS)
#define RICH_COLOR			(RICH_ICON_START-1)
#define RICH_FONT			(RICH_COLOR-1)

//NOTE: color,font blocks may never be added because that complicates copying,psubstr,rawstr,parsetags too much

void pwver(char **ndest, char *src);	//asterisk-mask password string
void ParseTags(char **ndest, char *src, int *caret);
int Rich_len(const char *in);

#define UTF8_IGNORE_ERROR		0x01
#define UTF8_SKIP_BOM			0x02

int		utf8_to_wchar(const char *in, int insize, unsigned int *out,
		    int outsize, int flags);
int		wchar_to_utf8(const unsigned int *in, int insize, char *out,
		    int outsize, int flags);

//TODO absorb
char *FromGlyph(const unsigned int in, int *adv);
unsigned int ToGlyph(const char *in, int *index);

void pstrset(char **out, const char *in);
void pstradd(char **out, const char *in);
void psubstr(char **out, const char *in, int beg, int len);
void delprev(char **s, int *caret);
void delnext(char **s, int *caret);
int prevlen(char *s, int caret);
int nextlen(char *s, int caret);

#endif
