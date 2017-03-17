










#ifndef APPGUI_H
#define APPGUI_H

#include "../../platform.h"
#include "../widget.h"

struct ViewLayer;
struct Widget;

extern char g_lastsave[DMD_MAX_PATH+1];

void Resize_Fullscreen(Widget* w);
void Resize_FullscreenSq(Widget* w);
void Resize_AppLogo(Widget* w);
void Resize_AppTitle(Widget* w);
void FillGUI();
void SkipLogo();
void UpdLogo();

#endif	//GGUI_H
