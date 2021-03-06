









#include "../../app/appmain.h"
#include "../../gui/gui.h"
#include "../../gui/widgets/image.h"
#include "../../gui/widgets/button.h"
#include "../../gui/viewlayer.h"

char g_lastsave[DMD_MAX_PATH+1];

static char g_restage = 0;

void SkipLogo()
{
	Widget *gui, *load;

	if(g_appmode != APPMODE_LOGO)
		return;

	g_appmode = APPMODE_LOADING;return;
	gui = (Widget*)&g_gui;

	Widget_hide( gui );
	load = Widget_get( gui, "loading" );
	Widget_show( load );
}

void UpdLogo()
{
	Widget *gui, *logo;
	Image *logo2;
	float a;
	static int stage = 0;

	SkipLogo();
	return;

	gui = (Widget*)&g_gui;

	if(stage < 60)
	{
		a = (float)stage / 60.0f;
		logo = Widget_get( gui, "logo");
		logo2 = (Image*)Widget_get( logo, "logo");
		logo2->rgba[3] = a;
	}
	else if(stage < 120)
	{
		a = 1.0f - (float)(stage-60) / 60.0f;
		logo = Widget_get( gui, "logo");
		logo2 = (Image*)Widget_get( logo, "logo");
		logo2->rgba[3] = a;
	}
	else
		SkipLogo();

	stage++;
}

void Resize_Fullscreen(Widget* w)
{
	w->pos[0] = 0;
	w->pos[1] = 0;
	w->pos[2] = (float)g_width;
	w->pos[3] = (float)g_height;
}

void Resize_FullscreenSq(Widget* w)
{
	float minsz;
	
	minsz = fmin((float)g_width-1, (float)g_height-1);

	w->pos[0] = g_width/2.0f - minsz/2.0f;
	w->pos[1] = g_height/2.0f - minsz/2.0f;
	w->pos[2] = g_width/2.0f + minsz/2.0f;
	w->pos[3] = g_height/2.0f + minsz/2.0f;
}

void Resize_AppLogo(Widget* w)
{
	w->pos[0] = 30;
	w->pos[1] = 30;
	w->pos[2] = 200;
	w->pos[3] = 200;
}

void Resize_AppTitle(Widget* w)
{
	w->pos[0] = 30;
	w->pos[1] = 30;
	w->pos[2] = (float)g_width-1;
	w->pos[3] = 100;
}

void Resize_But(Widget* w)
{
	w->pos[0] = 30;
	w->pos[1] = 30;
	w->pos[2] = 200;
	w->pos[3] = 100;
	CenterLabel((Button*)w);
}


void FillGUI()
{
	Widget *gui;
	ViewLayer *logo, *loading, *menu;
	Button *but;

	gui = (Widget*)&g_gui;
	GUI_init((GUI*)gui);

	logo = (ViewLayer*)malloc(sizeof(ViewLayer));
	loading = (ViewLayer*)malloc(sizeof(ViewLayer));
	menu = (ViewLayer*)malloc(sizeof(ViewLayer));
	but = (Button*)malloc(sizeof(Button));

	ViewLayer_init(logo, "logo", gui);
	ViewLayer_init(loading, "loading", gui);
	ViewLayer_init(menu, "menu", gui);
	Button_init(but, menu, "",
		"", "label", "tooltip",
		MAINFONT8, BUST_LINEBASED,
		Resize_But, NULL, NULL,
		NULL, NULL,
		NULL, -1, NULL);

	Widget_add(gui, (Widget*)logo);
	Widget_add(gui, (Widget*)loading);
	Widget_add(gui, (Widget*)menu);
	Widget_add(menu, (Widget*)but);

	Widget_hideall(gui);
	Widget_show((Widget*)logo);
}