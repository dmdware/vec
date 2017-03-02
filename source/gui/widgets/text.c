#include "../widget.h"
#include "button.h"
#include "text.h"

void Text_init(Text *tw, Widget *parent, const char *n, const char *t, int f, void (*reframef)(Widget *w), ecbool shdw, float r, float g, float b, float a, ecbool multiline)
{
	Widget *bw;
	bw = &tw->base;
	Widget_init(bw);
	bw->parent = parent;
	bw->type = WIDGET_TEXT;
	strcpy(bw->name, n);
	tw->text = NULL;
	pstrset(&tw->text, t);
	tw->font = f;
	bw->reframefunc = reframef;
	bw->ldown = ecfalse;
	tw->rgba[0] = r;
	tw->rgba[1] = g;
	tw->rgba[2] = b;
	tw->rgba[3] = a;
	tw->shadow = shdw;
	tw->multiline = multiline;
	Widget_reframe(bw);
}

void Text_draw(Text *tw)
{
	Widget *bw;
	float cp[4];
	Shader *s;
	
	bw = &tw->base;

	SubCrop(bw->pos, bw->crop, cp);
	DrawTx(tw->font, bw->pos, cp, tw->text, tw->rgba, 0, -1, tw->shadow, tw->multiline);
	
	s = &g_sh[g_curS];
	glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);
}

