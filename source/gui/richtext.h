#ifndef RICHTEXT_H
#define RICHTEXT_H

#include "../platform.h"
#include "../str.h"
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
int Rich_len(char *in);

#endif
