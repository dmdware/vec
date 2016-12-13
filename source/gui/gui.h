










#ifndef GUI_H
#define GUI_H

#include "../platform.h"
#include "widget.h"

struct GUI
{
	Widget base;
	void (*keyupfunc[SDL_NUM_SCANCODES])();
	void (*keydownfunc[SDL_NUM_SCANCODES])();
	void (*anykeyupfunc)(int k);
	void (*anykeydownfunc)(int k);
	void (*mousemovefunc)(InEv* ie);
	void (*lbuttondownfunc)();
	void (*lbuttonupfunc)();
	void (*rbuttondownfunc)();
	void (*rbuttonupfunc)();
	void (*mbuttondownfunc)();
	void (*mbuttonupfunc)();
	void (*mousewheelfunc)(int delta);
};

typedef struct GUI GUI;

void GUI_init(GUI* gui);
void GUI_draw2(GUI* gui);
void GUI_inev2(GUI* gui, InEv* ie);

extern GUI g_gui;

ecbool MousePosition();
void CenterMouse();
void SetStatus(const char* status);
void Ortho(int width, int height, float r, float g, float b, float a);

#endif
