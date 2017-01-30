










#include "gui.h"
#include "../texture.h"
#include "viewlayer.h"

void ViewLayer_init(ViewLayer *v, const char* n, Widget* parent)
{
	Widget *bw;
	bw = (Widget*)v;
	Widget_init(bw);
	strcpy(bw->name, n);
	bw->opened = ecfalse;
	bw->type = WIDGET_VIEWLAYER;
	bw->parent = parent;
	bw->hidden = ectrue;
	bw->reframefunc = Resize_Fullscreen;
	Widget_reframe(bw);
}
