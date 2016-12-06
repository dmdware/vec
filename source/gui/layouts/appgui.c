









#include "../../app/appmain.h"
#include "../../gui/gui.h"

char g_lastsave[DMD_MAX_PATH+1];

static char g_restage = 0;

void SkipLogo()
{
	Widget *gui, *load;

	if(g_appmode != APPMODE_LOGO)
		return;

	g_appmode = APPMODE_LOADING;
	gui = (Widget*)&g_gui;

	Widget_hide( gui );
	load = Widget_get( gui, "loading" );
	Widget_show( load );
}

void UpdLogo()
{
	Widget *gui, *logo;
	float a;
	static int stage = 0;

	SkipLogo();

	gui = (Widget*)&g_gui;

	if(stage < 60)
	{
		a = (float)stage / 60.0f;
		logo = Widget_get( gui, "logo");
		logo = Widget_get( logo, "logo");
		logo->rgba[3] = a;
	}
	else if(stage < 120)
	{
		a = 1.0f - (float)(stage-60) / 60.0f;
		logo = Widget_get( gui, "logo");
		logo = Widget_get( logo, "logo");
		logo->rgba[3] = a;
	}
	else
		SkipLogo();

	stage++;
}

void Resize_Fullscreen(Widget* w)
{
	w->m_pos[0] = 0;
	w->m_pos[1] = 0;
	w->m_pos[2] = (float)g_width;
	w->m_pos[3] = (float)g_height;
}

void Resize_FullscreenSq(Widget* w)
{
	float minsz;
	
	minsz = fmin((float)g_width-1, (float)g_height-1);

	w->m_pos[0] = g_width/2.0f - minsz/2.0f;
	w->m_pos[1] = g_height/2.0f - minsz/2.0f;
	w->m_pos[2] = g_width/2.0f + minsz/2.0f;
	w->m_pos[3] = g_height/2.0f + minsz/2.0f;
}

void Resize_AppLogo(Widget* w)
{
	w->m_pos[0] = 30;
	w->m_pos[1] = 30;
	w->m_pos[2] = 200;
	w->m_pos[3] = 200;
}

void Resize_AppTitle(Widget* w)
{
	w->m_pos[0] = 30;
	w->m_pos[1] = 30;
	w->m_pos[2] = (float)g_width-1;
	w->m_pos[3] = 100;
}

void FillGUI()
{
	Widget *gui, *logo;

	gui = (Widget*)&g_gui;

	logo = Widget_get(gui, "logo");
	Widget_hideall(gui);
	Widget_show(logo);
}