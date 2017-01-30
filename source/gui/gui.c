










#include "gui.h"
#include "../render/shader.h"
#include "../texture.h"
#include "../app/appmain.h"
#include "viewlayer.h"

GUI g_gui;

void GUI_init(GUI* gui)
{
	Widget *w;
	int i;

	w = (Widget*)gui;

	Widget_init(w);

	w->type = WIDGET_GUI;

	for(i=0; i<SDL_NUM_SCANCODES; ++i)
	{
		gui->keyupfunc[i] = NULL;
		gui->keydownfunc[i] = NULL;
	}

	gui->anykeyupfunc = NULL;
	gui->anykeydownfunc = NULL;
	gui->lbuttondownfunc = NULL;
	gui->lbuttonupfunc = NULL;
	gui->rbuttondownfunc = NULL;
	gui->rbuttonupfunc = NULL;
	gui->mbuttondownfunc = NULL;
	gui->mbuttonupfunc = NULL;
	gui->mousewheelfunc = NULL;
	gui->mousemovefunc = NULL;

	w->hidden = ecfalse;
}

void GUI_draw2(GUI *gui)
{
	unsigned int spi;
	Sprite* sp;
	Widget *bw;

	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	CHECKGLERROR();
	Ortho(g_width, g_height, 1, 1, 1, 1);
	CHECKGLERROR();

	/* TODO
	in Widget::
	Widget_draw((Widget*)gui);
	CHECKGLERROR();
	Widget_drawover((Widget*)gui);
	*/

	if(g_appmode == APPMODE_PLAY &&
		g_keys[SDL_SCANCODE_TAB])
	{
//		Lobby_DrawPyL();
//		Lobby_DrawState();
	}

	spi = g_cursor[g_curst];
	sp = &g_sp[spi];

	bw = (Widget*)gui;

	bw->crop[0] = 0;
	bw->crop[1] = 0;
	bw->crop[2] = (float)g_width-1;
	bw->crop[3] = (float)g_height-1;

	DrawImage(g_tex[sp->difftexi].texname, 
		g_mouse.x+sp->offset[0], g_mouse.y+sp->offset[1], 
		g_mouse.x+sp->offset[2], g_mouse.y+sp->offset[3], 
		0,0,1,1, bw->crop);
	
	CHECKGLERROR();

	EndS();
	CHECKGLERROR();

	glEnable(GL_DEPTH_TEST);
}

void GUI_inev2(GUI *gui, InEv* ie)
{
	if(!ie->intercepted)
	{
		if(ie->type == INEV_MOUSEMOVE && gui->mousemovefunc) gui->mousemovefunc(ie);
		else if(ie->type == INEV_MOUSEDOWN && ie->key == MOUSE_LEFT && gui->lbuttondownfunc) gui->lbuttondownfunc();
		else if(ie->type == INEV_MOUSEUP && ie->key == MOUSE_LEFT && gui->lbuttonupfunc) gui->lbuttonupfunc();
		else if(ie->type == INEV_MOUSEDOWN && ie->key == MOUSE_MIDDLE && gui->mbuttondownfunc) gui->mbuttondownfunc();
		else if(ie->type == INEV_MOUSEUP && ie->key == MOUSE_MIDDLE && gui->mbuttonupfunc) gui->mbuttonupfunc();
		else if(ie->type == INEV_MOUSEDOWN && ie->key == MOUSE_RIGHT && gui->rbuttondownfunc) gui->rbuttondownfunc();
		else if(ie->type == INEV_MOUSEUP && ie->key == MOUSE_RIGHT && gui->rbuttonupfunc) gui->rbuttonupfunc();
		else if(ie->type == INEV_MOUSEWHEEL && gui->mousewheelfunc) gui->mousewheelfunc(ie->amount);
		else if(ie->type == INEV_KEYDOWN && gui->keydownfunc[ie->scancode]) gui->keydownfunc[ie->scancode]();
		else if(ie->type == INEV_KEYUP && gui->keyupfunc[ie->scancode]) gui->keyupfunc[ie->scancode]();
		else if(ie->type == INEV_COPY && gui->keydownfunc[ie->scancode]) gui->keydownfunc[ie->scancode]();
		else if(ie->type == INEV_PASTE && gui->keydownfunc[ie->scancode]) gui->keydownfunc[ie->scancode]();

		if(ie->type != INEV_MOUSEMOVE && gui->anykeydownfunc) gui->anykeydownfunc(-1);
	}
}

void SetStatus(const char* status)
{
	Widget *gui, *loading;
	Text *stat;

	gui = (Widget*)&g_gui;

	loading = Widget_get(gui, "loading");

	if(!loading)
		return;

	stat = (Text*)Widget_get(loading, "status");

	if(!stat)
		return;

	free(stat->text);
	stat->text = (char*)malloc(strlen(status)+1);
	strcpy(stat->text, status);
}

ecbool MousePosition()
{
	Vec2i old = g_mouse;
	SDL_GetMouseState(&g_mouse.x, &g_mouse.y);

	if(g_mouse.x == old.x && g_mouse.y == old.y)
		return ecfalse;

	return ectrue;
}

void CenterMouse()
{
	g_mouse.x = g_width/2;
	g_mouse.y = g_height/2;
	
	SDL_WarpMouseInWindow(g_window, g_mouse.x, g_mouse.y);
}

void Ortho(int width, int height, float r, float g, float b, float a)
{
	Shader* s;
	CHECKGLERROR();
	UseS(SHADER_ORTHO);
	s = g_sh+g_curS;
	glUniform1f(s->slot[SSLOT_WIDTH], (float)width);
	glUniform1f(s->slot[SSLOT_HEIGHT], (float)height);
	glUniform4f(s->slot[SSLOT_COLOR], r, g, b, a);
	g_currw = width;
	g_currh = height;
	CHECKGLERROR();
}
