



#include "../widget.h"
#include "button.h"
#include "../icon.h"
#include "../../ustring.h"
#include "../gui.h"

void Button_init(Button *b)
{
	Widget *bw;

	bw = (Widget*)b;

	Widget_init(bw);

	bw->parent = NULL;
	type = WIDGET_BUTTON;
	//text = RichText("");
	font = MAINFONT8;
	//float length = 0;
	//for(int i=0; i<strlen(t); i++)
	//    length += g_font[f].glyph[t[i]].w;
	//tpos[0] = (left+right)/2.0f - length/2.0f;
	//tpos[1] = (top+bottom)/2.0f - g_font[f].gheight/2.0f;
	style = BUST_LINEBASED;
	over = ecfalse;
	ldown = ecfalse;
	CreateTex(tex, "gui/transp.png", ectrue, ecfalse);
	CreateTex(bgtex, "gui/corrodbutton.png", ectrue, ecfalse);
	CreateTex(bgovertex, "gui/corrodbuttonover.png", ectrue, ecfalse);
	reframefunc = NULL;
	clickfunc = NULL;
	overfunc = NULL;
	clickfunc2 = NULL;
	overfunc2 = NULL;
	outfunc = NULL;
	param = -1;
	clickfunc3 = NULL;
	//reframe();
}

Button::Button(Button *b, Widget* parent, const char* name, const char* filepath, const RichText label, const RichText tooltip, int f, int style, void (*reframef)(Widget* w), void (*click)(), void (*click2)(int p), void (*overf)(), void (*overf2)(int p), void (*out)(), int parm, void (*click3)(Widget* w)) : Widget()
{
	Widget *bw;
	int length;

	bw = (Widget*)b;
	Widget_init(bw);

	bw->parent = parent;
	bw->type = WIDGET_BUTTON;
	strcpy(bw->name, name);
	b->style = style;
	pstrset(b->text, tooltip);
	pstrset(b->label, label);
	b->font = f;

	length = 0;
	length = EndX(text, Rich_rawlen(text), font, 0, 0);
	b->over = ecfalse;
	b->ldown = ecfalse;
	CreateTex(bw->tex, filepath, ectrue, ecfalse);

	if(style == BUST_CORRODE)
	{
		CreateTex(b->bgtex, "gui/corrodbutton.png", ectrue, ecfalse);
		CreateTex(b->bgovertex, "gui/corrodbuttonover.png", ectrue, ecfalse);
	}

	b->reframefunc = reframef;
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

		if(b->over && b->ldown)
		{
			if(b->clickfunc != NULL)
				b->clickfunc();

			if(b->clickfunc2 != NULL)
				b->clickfunc2(param);

			if(b->clickfunc3 != NULL)
				b->clickfunc3(bw);

			//over = ecfalse;
			b->ldown = ecfalse;

			ie->intercepted = ectrue;
			
			return;	// intercept mouse event
		}

		if(b->ldown)
		{
			b->ldown = ecfalse;
			ie->intercepted = ectrue;
			return;
		}

		b->over = ecfalse;
	}
	else if(ie->type == INEV_MOUSEDOWN && ie->key == MOUSE_LEFT && !ie->intercepted)
	{
		//mousemove();

		if(b->over)
		{
			b->ldown = ectrue;
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
			if(b->over && b->outfunc != NULL)
				b->outfunc();

			b->over = ecfalse;
		}

		if(!ie->intercepted)
		{
			if(g_mouse.x >= bw->pos[0] && g_mouse.x <= bw->pos[2] && g_mouse.y >= bw->pos[1] && g_mouse.y <= bw->pos[3])
			{
				if(b->overfunc != NULL)
					b->overfunc();
				if(b->overfunc2 != NULL)
					b->overfunc2(param);

				b->over = ectrue;

				ie->intercepted = ectrue;
				return;
			}
		}
	}
}

void Button_draw()
{
	if(style == BUST_CORRODE)
	{
		if(over)
			DrawImage(g_texture[bgovertex].texname, pos[0], pos[1], pos[2], pos[3], 0,0,1,1, crop);
		else
			DrawImage(g_texture[bgtex].texname, pos[0], pos[1], pos[2], pos[3], 0,0,1,1, crop);

		DrawImage(g_texture[tex].texname, pos[0], pos[1], pos[2], pos[3], 0,0,1,1, crop);

		DrawShadowedText(font, tpos[0], tpos[1], &label);
	}
	else if(style == BUST_LEFTIMAGE)
	{
		EndS();

		UseS(SHADER_COLOR2D);
		glUniform1f(g_shader[g_curS].slot[SSLOT_WIDTH], (float)g_currw);
		glUniform1f(g_shader[g_curS].slot[SSLOT_HEIGHT], (float)g_currh);

		float midcolor[] = {0.7f,0.7f,0.7f,0.8f};
		float lightcolor[] = {0.8f,0.8f,0.8f,0.8f};
		float darkcolor[] = {0.5f,0.5f,0.5f,0.8f};

		if(over)
		{
			for(int i=0; i<3; i++)
			{
				midcolor[i] = 0.8f;
				lightcolor[i] = 0.9f;
				darkcolor[i] = 0.6f;
			}
		}

		DrawSquare(midcolor[0], midcolor[1], midcolor[2], midcolor[3], pos[0], pos[1], pos[2], pos[3], crop);

		DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], pos[2], pos[1], pos[2], pos[3]-1, crop);
		DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], pos[0], pos[1], pos[2]-1, pos[1], crop);

		DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], pos[0]+1, pos[3], pos[2], pos[3], crop);
		DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], pos[2], pos[1]+1, pos[2], pos[3], crop);

		EndS();
		CHECKGLERROR();
		Ortho(g_currw, g_currh, 1, 1, 1, 1);

		float w = pos[2]-pos[0]-2;
		float h = pos[3]-pos[1]-2;
		float minsz = fmin(w, h);

		DrawImage(g_texture[tex].texname, pos[0]+1, pos[1]+1, pos[0]+minsz, pos[1]+minsz, 0,0,1,1, crop);

		float gheight = g_font[font].gheight;
		float texttop = pos[1] + h/2.0f - gheight/2.0f;
		float textleft = pos[0]+minsz+1;

		//TODO rewrite font.cpp/h to better deal with cropping
		DrawShadowedTextF(font, textleft, texttop, crop[0], crop[1], crop[2], crop[3], &label);
	}
	else if(style == BUST_LINEBASED)
	{
		//InfoMess("lb", "lb");
		Player* py = &g_player[g_localP];

		EndS();

		UseS(SHADER_COLOR2D);
		glUniform1f(g_shader[g_curS].slot[SSLOT_WIDTH], (float)g_currw);
		glUniform1f(g_shader[g_curS].slot[SSLOT_HEIGHT], (float)g_currh);

		float midcolor[] = {0.7f,0.7f,0.7f,0.8f};
		float lightcolor[] = {0.8f,0.8f,0.8f,0.8f};
		float darkcolor[] = {0.5f,0.5f,0.5f,0.8f};

		if(over)
		{
			for(int i=0; i<3; i++)
			{
				midcolor[i] = 0.8f;
				lightcolor[i] = 0.9f;
				darkcolor[i] = 0.6f;
			}
		}

		DrawSquare(midcolor[0], midcolor[1], midcolor[2], midcolor[3], pos[0], pos[1], pos[2], pos[3], crop);

		DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], pos[2], pos[1], pos[2], pos[3]-1, crop);
		DrawLine(lightcolor[0], lightcolor[1], lightcolor[2], lightcolor[3], pos[0], pos[1], pos[2]-1, pos[1], crop);

		DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], pos[0]+1, pos[3], pos[2], pos[3], crop);
		DrawLine(darkcolor[0], darkcolor[1], darkcolor[2], darkcolor[3], pos[2], pos[1]+1, pos[2], pos[3], crop);

		EndS();
		CHECKGLERROR();
		Ortho(g_currw, g_currh, 1, 1, 1, 1);

		float w = pos[2]-pos[0]-2;
		float h = pos[3]-pos[1]-2;
		float minsz = fmin(w, h);

		//TODO fix resolution change on settings reload on mobile

		//2015/10/27 fixed now button text is cropped
		//DrawImage(g_texture[tex].texname, pos[0]+1, pos[1]+1, pos[0]+minsz, pos[1]+minsz);
		CenterLabel(this);
		DrawShadowedTextF(font, tpos[0], tpos[1], crop[0], crop[1], crop[2], crop[3], &label);
	}

	//if(_stricmp(name.c_str(), "choose texture") == 0)
	//	Log("draw choose texture");
}

void Button::drawover()
{
	if(over)
	{
		Player* py = &g_player[g_localP];
		//DrawShadowedText(font, tpos[0], tpos[1], text.c_str());
		DrawShadowedText(font, g_mouse.x, g_mouse.y-g_font[font].gheight, &text);
		//DrawBoxShadText(font, g_mouse.x, g_mouse.y-g_font[font].gheight, &text);
	}
}

