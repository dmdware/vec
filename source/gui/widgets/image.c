







#include "../widget.h"
#include "image.h"
#include "text.h"
#include "../../debug.h"

void Image_init(Image *i,
				Widget* parent, 
				const char* nm, 
				const char* filepath, 
				ecbool clamp, 
				void (*reframef)(Widget* w), 
				float r, 
				float g, 
				float b, 
				float a, 
				float texleft, 
				float textop, 
				float texright, 
				float texbottom)
{
	Widget *bw;

	bw = (Widget*)i;
	Widget_init(bw);

	bw->parent = parent;
	bw->type = WIDGET_IMAGE;
	strcpy(bw->name, nm);
	CreateTex(&i->tex, filepath, clamp, ecfalse, ecfalse);
	bw->reframefunc = reframef;
	i->texc[0] = texleft;
	i->texc[1] = textop;
	i->texc[2] = texright;
	i->texc[3] = texbottom;
	bw->ldown = ecfalse;
	i->rgba[0] = r;
	i->rgba[1] = g;
	i->rgba[2] = b;
	i->rgba[3] = a;
	bw->pos[0] = 0;
	bw->pos[1] = 0;
	bw->pos[2] = 0;
	bw->pos[3] = 0;
	Widget_reframe(bw);
}

void Image_draw(Image *i)
{
	Shader *s;
	Widget *bw;
	Texture *tex;

	bw = (Widget*)i;
	s = g_sh+g_curS;
	tex = g_tex+i->tex;

	glUniform4fv(s->slot[SSLOT_COLOR], 1, i->rgba);
	DrawImage(tex->texname, 
		bw->pos[0], bw->pos[1], bw->pos[2], bw->pos[3], 
		i->texc[0], i->texc[1], i->texc[2], i->texc[3], 
		bw->crop);
	glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);
}

