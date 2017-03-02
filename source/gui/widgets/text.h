#ifndef TEXT_H
#define TEXT_H

#include "../widget.h"

struct Text
{
	Widget base;
	ecbool multiline;
	char* text;
	unsigned char font;
	float rgba[4];
	ecbool shadow;
};

typedef struct Text Text;

void Text_init(Text *tw, Widget *parent, const char *n, const char *t, int f, void (*reframef)(Widget *w), ecbool shdw, float r, float g, float b, float a, ecbool multiline);
void Text_draw(Text *tw);
void Text_chval(Text *tw, const char *newv);

#endif
