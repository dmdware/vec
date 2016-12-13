



#include "../widget.h"
#include "button.h"
#include "../icon.h"
#include "../widget.h"
#include "../gui.h"
#include "../font.h"
#include "../richtext.h"

void Button_init(Button *b, Widget* parent, const char* name, const char* filepath, 
				 const char* label, const char* tooltip, int f, int style,
				 void (*reframef)(Widget* w), void (*click)(), void (*click2)(int p), 
				 void (*overf)(), void (*overf2)(int p), void (*out)(), int parm, 
				 void (*click3)(Widget* w))
{
	Widget *bw;
	int length;

	bw = (Widget*)b;
	Widget_init(bw);

	bw->parent = parent;
	bw->type = WIDGET_BUTTON;
	strcpy(bw->name, name);
	b->style = style;
	pstrset(&b->tooltip, tooltip);
	pstrset(&b->label, label);
	b->font = f;

	length = EndX(b->tooltip, Rich_len(b->tooltip), b->font, bw->pos, ecfalse);
	bw->over = ecfalse;
	bw->ldown = ecfalse;
	CreateTex(&b->tex, filepath, ectrue, ecfalse);

	if(style == BUST_CORRODE)
	{
		CreateTex(&b->bgtex, "gui/corrodbutton.png", ectrue, ecfalse);
		CreateTex(&b->bgovertex, "gui/corrodbuttonover.png", ectrue, ecfalse);
	}

	bw->reframefunc = reframef;
	b->clickfunc = click;
	b->clickfunc2 = click2;
	b->overfunc = overf;
	b->overfunc2 = overf2;
	b->outfunc = out;
	b->param = parm;
	b->clickfunc3 = click3;
	Widget_reframe(bw);
}

void Button_inev(Button *b, InEv* ie)
{
	Widget *bw;

	bw = (Widget*)b;

	if(ie->type == INEV_MOUSEUP && ie->key == MOUSE_LEFT && !ie->intercepted)
	{
		//mousemove();

		if(bw->over && bw->ldown)
		{
			if(b->clickfunc != NULL)
				b->clickfunc();

			if(b->clickfunc2 != NULL)
				b->clickfunc2(b->param);

			if(b->clickfunc3 != NULL)
				b->clickfunc3(bw);

			//over = ecfalse;
			bw->ldown = ecfalse;

			ie->intercepted = ectrue;
			
			return;	// intercept mouse event
		}

		if(bw->ldown)
		{
			bw->ldown = ecfalse;
			ie->intercepted = ectrue;
			return;
		}

		bw->over = ecfalse;
	}
	else if(ie->type == INEV_MOUSEDOWN && ie->key == MOUSE_LEFT && !ie->intercepted)
	{
		//mousemove();

		if(bw->over)
		{
			bw->ldown = ectrue;
			ie->intercepted = ectrue;
			return;	// intercept mouse event
		}
	}
	else if(ie->type == INEV_MOUSEMOVE)
	{
		if(g_mouse.x >= bw->pos[0] && g_mouse.x <= bw->pos[2] && g_mouse.y >= bw->pos[1] && g_mouse.y <= bw->pos[3])
		{
		}
		else
		{
			if(bw->over && b->outfunc != NULL)
				b->outfunc();

			bw->over = ecfalse;
		}

		if(!ie->intercepted)
		{
			if(g_mouse.x >= bw->pos[0] && g_mouse.x <= bw->pos[2] && g_mouse.y >= bw->pos[1] && g_mouse.y <= bw->pos[3])
			{
				if(b->overfunc != NULL)
					b->overfunc();
				if(b->overfunc2 != NULL)
					b->overfunc2(b->param);

				bw->over = ectrue;

				ie->intercepted = ectrue;
				return;
			}
		}
	}
}

void Button_draw(Button *b)
{
	Widget *bw;
	Shader *s;
	float midcolor[] = {0.7f,0.7f,0.7f,0.8f};
	float lightcolor[] = {0.8f,0.8f,0.8f,0.8f};
	float darkcolor[] = {0.5f,0.5f,0.5f,0.8f};
	char i;
	float w;
	float h;
	float minsz;
	float gheight;
	float texttop;
	float textleft;
	Font *f;

	bw = (Widget*)b;
	f = g_font+b->font;

	if(b->style == BUST_CORRODE)
	{
		if(bw->over)
			DrawImage(g_texture[b->bgovertex].texname, bw->pos[0], bw->pos[1], bw->pos[2], bw->pos[3], 0,0,1,1, bw->crop);
		else
			DrawImage(g_texture[b->bgtex].texname, bw->pos[0], bw->pos[1], bw->pos[2], bw->pos[3], 0,0,1,1, bw->crop);

		DrawImage(g_texture[b->tex].texname, bw->pos[0], bw->pos[1], bw->pos[2], bw->pos[3], 0,0,1,1, bw->crop);
		
		DrawTx(b->font, b->tpos, bw->crop, b->label, NULL, Rich_len(b->label), -1, ectrue, ecfalse);
	}
	else if(b->style == BUST_LEFTIMAGE)
	{
		EndS();

		UseS(SH_COLOR2D);
		s = g_sh+g_curS;
		glUniform1f(s->slot[SSLOT_WIDTH], (float)g_currw);
		glUniform1f(s->slot[SSLOT_HEIGHT], (float)g_currh);

		if(bw->over)
		{
			for(i=0; i<3; ++i)
			{
				midcolor[i] = 0.8f;
				lightcolor[i] = 0.9f;
				darkcolor[i] = 0.6f;
			}
		}

		DrawSquare(midcolor[0], midcolor[1], midcolor[2], midcolor[3], bw->pos[0], bw->pos[1], bw->pos[2], bw->pos[3], bw->crop);

		DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], bw->pos[2], bw->pos[1], bw->pos[2], bw->pos[3]-1, bw->crop);
		DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], bw->pos[0], bw->pos[1], bw->pos[2]-1, bw->pos[1], bw->crop);

		DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], bw->pos[0]+1, bw->pos[3], bw->pos[2], bw->pos[3], bw->crop);
		DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], bw->pos[2], bw->pos[1]+1, bw->pos[2], bw->pos[3], bw->crop);

		EndS();
		CHECKGLERROR();
		Ortho(g_currw, g_currh, 1, 1, 1, 1);

		w = bw->pos[2]-bw->pos[0]-2;
		h = bw->pos[3]-bw->pos[1]-2;
		minsz = fmin(w, h);

		DrawImage(g_texture[tex].texname, bw->pos[0]+1, bw->pos[1]+1, bw->pos[0]+minsz, bw->pos[1]+minsz, 0,0,1,1, bw->crop);

		gheight = f->gheight;
		texttop = bw->pos[1] + h/2.0f - gheight/2.0f;
		textleft = bw->pos[0]+minsz+1;

		//TODO rewrite font.cpp/h to better deal with cropping
		DrawTx(b->font, b->tpos, bw->crop, b->label, NULL, Rich_len(b->label), -1, ectrue, ecfalse);
	}
	else if(b->style == BUST_LINEBASED)
	{
		EndS();

		UseS(SH_COLOR2D);
		s = g_sh+g_curS;
		glUniform1f(s->slot[SSLOT_WIDTH], (float)g_currw);
		glUniform1f(s->slot[SSLOT_HEIGHT], (float)g_currh);

		if(bw->over)
		{
			for(i=0; i<3; i++)
			{
				midcolor[i] = 0.8f;
				lightcolor[i] = 0.9f;
				darkcolor[i] = 0.6f;
			}
		}

		DrawSquare(midcolor[0], midcolor[1], midcolor[2], midcolor[3], bw->pos[0], bw->pos[1], bw->pos[2], bw->pos[3], bw->crop);

		DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], bw->pos[2], bw->pos[1], bw->pos[2], bw->pos[3]-1, bw->crop);
		DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], bw->pos[0], bw->pos[1], bw->pos[2]-1, bw->pos[1], bw->crop);

		DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], bw->pos[0]+1, bw->pos[3], bw->pos[2], bw->pos[3], bw->crop);
		DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], bw->pos[2], bw->pos[1]+1, bw->pos[2], bw->pos[3], bw->crop);

		EndS();
		CHECKGLERROR();
		Ortho(g_currw, g_currh, 1, 1, 1, 1);

		w = pos[2]-pos[0]-2;
		h = pos[3]-pos[1]-2;
		minsz = fmin(w, h);

		//TODO fix resolution change on settings reload on mobile

		//2015/10/27 fixed now button text is cropped
		//DrawImage(g_texture[tex].texname, bw->pos[0]+1, bw->pos[1]+1, bw->pos[0]+minsz, bw->pos[1]+minsz);
		CenterLabel(b);
		DrawTx(b->font, b->tpos, bw->crop, b->label, NULL, Rich_len(b->label), -1, ectrue, ecfalse);
	}

	//if(_stricmp(name.c_str(), "choose texture") == 0)
	//	Log("draw choose texture");
}

void Button_drawover(Button *b)
{
	if(bw->over)
	{
		//DrawShadowedText(font, tpos[0], tpos[1], text.c_str());
		DrawShadowedText(font, g_mouse.x, g_mouse.y-g_font[font].gheight, &text);
		//DrawBoxShadText(font, g_mouse.x, g_mouse.y-g_font[font].gheight, &text);
	}
}

