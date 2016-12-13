


#ifndef BUTTON_H
#define BUTTON_H

#include "../widget.h"

/* styles */
#define BUST_LINEBASED		0
#define BUST_LEFTIMAGE		1

struct Button
{
	Widget base;
	char style;
	
	float texc[4];	//texture coordinates
	float tpos[4];	//text pos
	unsigned int tex;
	unsigned int bgtex;
	unsigned int bgovertex;
	char* tooltip;
	int font;

	int param;
	float rgba[4];

	char* label;

	void (*clickfunc)();
	void (*clickfunc2)(int p);
	void (*overfunc)();
	void (*overfunc2)(int p);
	void (*outfunc)();
	void (*changefunc)();
	void (*changefunc2)(int p);
	void (*clickfunc3)(Widget* w);
};

typedef struct Button Button;

void Button_init(Button *b, Widget* parent, 
				 const char* name, const char* filepath, const char* label, 
				 const char* tooltip, int f, int style, void (*reframef)(Widget* w), 
				 void (*click)(), void (*click2)(int p), void (*overf)(), 
				 void (*overf2)(int p), void (*out)(), int parm, void (*click3)(Widget* w));
void Button_draw(Button *b);
void Button_drawover(Button *b);
void Button_inev(Button *b, InEv* ie);
void CenterLabel(Button *w);

#endif
