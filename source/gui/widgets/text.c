#include "../widget.h"
#include "barbutton.h"
#include "button.h"
#include "checkbox.h"
#include "editbox.h"
#include "droplist.h"
#include "image.h"
#include "insdraw.h"
#include "link.h"
#include "listbox.h"
#include "text.h"
#include "textarea.h"
#include "textblock.h"
#include "touchlistener.h"

void Text_init(Text *tw)
{
	Widget *bw;
	bw = &tw->base;
	Widget_init(bw);
	bw->parent = NULL;
	bw->type = WIDGET_TEXT;
	strcpy(bw->name, "");
	bw->font = MAINFONT8;
	bw->reframefunc = NULL;
	bw->ldown = ecfalse;
	bw->rgba[0] = 1;
	bw->rgba[1] = 1;
	bw->rgba[2] = 1;
	bw->rgba[3] = 1;
	bw->shadow = ecfalse;
}

void Text_init(Text *tw, Widget *parent, const char *n, const char *t, int f, void (*reframef)(Widget *w), ecbool shdw, float r, float g, float b, float a, ecbool multiline)
{
	Widget *bw;
	bw = &tw->base;
	Widget_init(bw);
	bw->parent = parent;
	bw->type = WIDGET_TEXT;
	strcpy(bw->name, n);
	free(bw->text);
	strset(&bw->text, t);
	bw->font = f;
	bw->reframefunc = reframef;
	bw->ldown = ecfalse;
	bw->rgba[0] = r;
	bw->rgba[1] = g;
	bw->rgba[2] = b;
	bw->rgba[3] = a;
	bw->shadow = shdw;
	tw->multiline = multiline;
	Widget_reframe(bw);
}

void Text_draw(Text *tw)
{
	Widget *bw;
	float cp[4];
	Shader *s;
	
	bw = &tw->base;

	SubFrame(bw->pos, bw->crop, cp);
	DrawText(bw->font, bw->pos, cp, bw->text, bw->rgba, 0, -1, bw->shadow, tw->multiline);
	
	s = &g_shader[g_curS];
	glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);
}

void Text_chval(Text *tw, const char *newv)
{
	Widget *bw;
	bw = &tw->base;
	free(bw->val);
	strset(&bw->val, newv);
	if(bw->caret > strlen(newv))
		bw->caret = strlen(newv);
	bw->lines = CountLines(bw->val, bw->font, pos);
}

