










#include "widget.h"
#include "gui.h"
#include "font.h"
#include "../window.h"
#include "icon.h"
#include "../debug.h"

void Widget_init(Widget* w)
{
	w->parent = NULL;
	w->name[0] = 0;
	w->opened = ecfalse;
	w->ldown = ecfalse;
	w->reframefunc = NULL;
	w->hidden = ecfalse;
	w->extra = NULL;

	List_init(&w->sub);
}

void Widget_free(Widget *w)
{
	Widget_freech(w);
	List_free(&w->sub);
	free(w->extra);
	w->extra = NULL;

	switch(w->type)
	{
	//case WIDGET_EDITBOX:
		//EditBox_free((EditBox*)w);
	//	break;
	}
}

void Widget_chcall(Widget *w, Widget* ch, char type, void* data)
{
	/* TODO window scroll-bars call */
}

void Widget_hideall(Widget *w)
{
	Node *i;
	Widget *iw;

	for(i=w->sub.head; i; i=i->next)
	{
		iw = (Widget*)i->data;
		Widget_hide(iw);
	}
}

void Widget_frameupd(Widget *w)
{
	Node *i;
	Widget *iw;

	for(i=w->sub.head; i; i=i->next)
	{
		iw = (Widget*)i->data;
		Widget_frameupd(iw);
	}
}

void Widget_reframe(Widget *w)	//resized or moved
{
	Node *i;
	Widget *iw;

	if(w->reframefunc)
		w->reframefunc(w);

	if(w->parent)
	{
		SubCrop(w->parent->crop, w->pos, w->crop);
	}
	else
	{
		w->crop[0] = 0;
		w->crop[1] = 0;
		w->crop[2] = (float)g_width-1;
		w->crop[3] = (float)g_height-1;
	}

	for(i=w->sub.head; i; i=i->next)
	{
		iw = (Widget*)i->data;
		Widget_reframe(iw);
	}
}

void Widget_draw(Widget *w)
{
	Node *i;
	Widget *iw;

	switch(w->type)
	{
	default:
		break;
	}

	for(i=w->sub.head; i; i=i->next)
	{
		iw = (Widget*)i->data;

		if(iw->hidden)
			continue;

		Widget_draw(iw);
	}
	
	switch(w->type)
	{
	case WIDGET_GUI:
		GUI_draw2((GUI*)w);
		break;
	}
}

void Widget_drawover(Widget *w)
{	
	Node *i;
	Widget *iw;

	for(i=w->sub.head; i; i=i->next)
	{
		iw = (Widget*)i->data;

		if(iw->hidden)
			continue;

		Widget_drawover(iw);
	}
}

void Widget_inev(Widget *w, InEv* ie)
{
	Node *i;
	Widget *iw;
	ecbool intercepted;
	
	intercepted = ie->intercepted;
	
	i = w->sub.tail;
	/* safe, may shift during call */
	while(i)
	{
		iw = (Widget*)i->data;
		i = i->prev;	/* safe, may shift during call */

		if(iw->hidden)
			continue;
		
		Widget_inev(iw, ie);
	}

	switch(w->type)
	{
	case WIDGET_GUI:
		GUI_inev2((GUI*)w, ie);
		break;
	}
}

void Widget_tofront(Widget *w)
{
	Node *i;
	Widget *iw;
	List *parsub;

	if(!w->parent)
		return;

	parsub = &w->parent->sub;

	for(i=parsub->head; i; i=i->next)
	{
		iw = (Widget*)i->data;

		if(iw == w)
		{
			List_erase(parsub, i);
			List_pushback2(parsub, sizeof(Widget*), &iw);
			return;
		}
	}
}

Widget* Widget_get(Widget *w, const char* name)
{
	Node *i;
	Widget *iw;

	for(i=w->sub.head; i; i=i->next)
	{
		iw = (Widget*)i->data;
		if(!strcmp(iw->name, name))
			return iw;
	}

	return NULL;
}

void Widget_add(Widget *w, Widget *neww)
{
	if(!neww)
		OUTOFMEM();

	List_pushback2(&w->sub, sizeof(Widget*), &neww);

	fprintf(g_applog, "add %s\r\n", neww->name);
	fflush(g_applog);
}

void Widget_gainfocus(Widget *w)
{
}

void Widget_losefocus(Widget *w)
{
	Node *i;
	Widget *iw;

	/* TODO lose focus win blview members edit boxes */

	for(i=w->sub.head; i; i=i->next)
	{
		iw = (Widget*)i->data;
		Widget_losefocus(iw);
	}
}

void Widget_hide(Widget *w)
{
	w->hidden = ectrue;
	Widget_losefocus(w);
}

void Widget_show(Widget *w)
{
	w->hidden = ecfalse;
	/* necessary for window widgets: */
	//tofront();	//can't break list iterator, might shift

	//for(std::list<Widget*>::iterator i=sub.begin(); i!=sub.end(); i++)
	//	(*i)->show();
}

/* free subwidget children */
void Widget_freech(Widget *w)
{
	Node *i;
	Widget *iw;

	fprintf(g_applog, "free %s\r\n", w->name);
	fflush(g_applog);

	i = w->sub.head;
	while(i)
	{
		iw = (Widget*)&i->data[0];
		Widget_free(iw);
		free(iw);
		iw = NULL;
		i = i->next;
	}
	List_free(&w->sub);
}

void SubCrop(float *src1, float *src2, float *dest)
{
	dest[0] = ((src1[0]>src2[0])?src1[0]:src2[0]);
	dest[1] = ((src1[1]>src2[1])?src1[1]:src2[1]);
	dest[2] = ((src1[2]<src2[2])?src1[2]:src2[2]);
	dest[3] = ((src1[3]<src2[3])?src1[3]:src2[3]);
}
