










#ifndef WIDGET_H
#define WIDGET_H

#include "../utils.h"
#include "../texture.h"
#include "font.h"
#include "../render/shader.h"
#include "../window.h"
#include "draw2d.h"
#include "richtext.h"
#include "inevent.h"

#define MAX_OPTIONS_SHOWN	7

#define WIDGET_GUI					0
#define WIDGET_IMAGE				1
#define WIDGET_EDITBOX				2

struct Widget
{
	/* TODO trim members delegate to sub's */

	char type;
	struct Widget* parent;
	float pos[4];
	float crop[4];
	
	ecbool over;
	ecbool ldown;	//was the left mouse button pressed while over this (i.e. drag)?
	char name[16];

	ecbool opened;
	ecbool hidden;

	List sub;	/* Widget* */
	void* extra;	/* extra user params */

	/*
	float texc[4];	//texture coordinates
	float tpos[4];	//text pos
	float scar[4];	//scrolling area rect for windows
	unsigned int tex;
	unsigned int bgtex;
	unsigned int bgovertex;
	RichText text;
	int font;
	unsigned int frametex, filledtex, uptex, downtex;
	Vector options; */ /* RichText */
	/*
	int selected;
	float scroll[2];
	ecbool mousescroll;
	float vel[2];
	int param;
	float rgba[4];
	char* value; /* /* RichText */
	/* int caret;
	ecbool passw;
	int maxlen;
	ecbool shadow;
	int lines;
	RichText label;
	ecbool popup;

	void (*clickfunc)();
	void (*clickfunc2)(int p);
	void (*overfunc)();
	void (*overfunc2)(int p);
	void (*outfunc)();
	void (*changefunc)();
	void (*changefunc2)(int p);
	void (*clickfunc3)(Widget* w);
	
		*/

	void (*reframefunc)(struct Widget* w);
};

typedef struct Widget Widget;

void Widget_init(Widget* w);
void Widget_free(Widget *w);
void Widget_draw(Widget *w);
void Widget_drawover(Widget *w);
void Widget_inev(Widget *w, InEv* ie);
void Widget_frameupd(Widget *w);
void Widget_reframe(Widget *w);	//resized or moved
void Widget_subframe(Widget *w, float* fr)
{
	memcpy((void*)fr, (void*)w->pos, sizeof(float)*4);
}
Widget* Widget_get(Widget *w, const char* name);
void Widget_add(Widget *w, Widget* neww);
void Widget_hide(Widget *w);
void Widget_show(Widget *w);
void Widget_chcall(Widget *w, Widget* ch, char type, void* data);	/* child callback */
void Widget_freech(Widget *w);	/* free subwidget children */
void Widget_tofront(Widget *w);	/* only used for windows. edit: needed for everything since droplist uses it on parent tree. */
void Widget_hideall(Widget *w);
void Widget_gainfocus(Widget *w);
void Widget_losefocus(Widget *w);

void CenterLabel(Widget *w, float *tpos);
void SubCrop(float *src1, float *src2, float *ndest);

#endif
