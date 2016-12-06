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

void Rich_substr(unsigned int **ndest, unsigned int *src, int start, int length);	//icons counted as 1 chars, control blocks 0, attached to beginning of text they influence and repeated for following text blocks
void Rich_copy(unsigned int **ndest, unsigned int *src);
void Rich_init_i(unsigned int **rich, short i);
void Rich_add(unsigned int **ndest, unsigned int *src);
void pwver(char **ndest, char *src);	//asterisk-mask password string
void ParseTags(char **ndest, char *src, int *caret);

#endif
