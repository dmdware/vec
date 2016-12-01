










#include "widget.h"
#include "gui.h"
#include "font.h"
#include "../window.h"
#include "icon.h"

void Widget_init(Widget* w)
{
	w->parent = NULL;
	w->name[0] = 0;
	w->opened = ecfalse;
	w->ldown = ecfalse;
	w->reframefunc = NULL;
	w->hidden = ecfalse;
	w->extra = NULL;
}

void Widget_free(Widget *w)
{
	Widget_freech(w);
	free(w->extra);
	free(w->value);
	w->extra = NULL;
	w->value = NULL;
}

void Widget_hideall(Widget *w)
{
	Node *i;
	Widget *iw;

	for(i=w->sub.head; i; i=i->next)
	{
		iw = (Widget*)iw->data;
		Widget_hide(iw);
	}
}

void Widget_frameupd(Widget *w)
{
	Node *i;
	Widget *iw;

	for(i=w->sub.head; i; i=i->next)
	{
		iw = (Widget*)iw->data;
		Widget_frameupd(iw);
	}
}

void Widget_reframe(Widget *w)	//resized or moved
{
	Node *i;
	Widget *iw;

	if(reframefunc)
		reframefunc(this);

	if(w->parent)
	{
		SubCrop(w->parent->crop, pos, crop);
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
		iw = (Widget*)iw->data;
		Widget_reframe(iw);
	}
}

void Widget::draw(Widget *w)
{
	Node *i;
	Widget *iw;

	for(i=w->sub.head; i; i=i->next)
	{
		iw = (Widget*)iw->data;

		if(iw->hidden)
			continue;

		Widget_draw(iw);
	}
}

void Widget::drawover(Widget *w)
{	
	Node *i;
	Widget *iw;

	for(i=w->sub.head; i; i=i->next)
	{
		iw = (Widget*)iw->data;

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
		iw = (Widget*)iw->data;
		i = i->prev;	/* safe, may shift during call */

		if(iw->hidden)
			continue;
		
		Widget_inev(iw, ie);
	}
}

void Widget_tofront(Widget *w)
{
	if(!w->parent)
		return;

	std::list<Widget*>* subs = &parent->sub;

	for(std::list<Widget*>::iterator wi=subs->begin(); wi!=subs->end(); wi++)
	{
		if(*wi == this)
		{
			subs->erase(wi);
			subs->push_back(this);
			break;
		}
	}
}

void CenterLabel(Widget* w)
{
	Font* f = &g_font[w->font];

	int texwidth = TextWidth(w->font, &w->label);

	w->tpos[0] = (w->pos[2]+w->pos[0])/2 - texwidth/2;
	w->tpos[1] = (w->pos[3]+w->pos[1])/2 - f->gheight/2;
}

Widget* Widget::get(const char* name)
{
	for(std::list<Widget*>::iterator i=sub.begin(); i!=sub.end(); i++)
		if(stricmp((*i)->name.c_str(), name) == 0)
			return *i;

	return NULL;
}

void Widget::add(Widget* neww)
{
	if(!neww)
		OUTOFMEM();

	sub.push_back(neww);
}

void Widget::gainfocus()
{
}

//TODO lose focus win blview members edit boxes

void Widget::losefocus()
{
	for(std::list<Widget*>::iterator i=sub.begin(); i!=sub.end(); i++)
		(*i)->losefocus();
}

void Widget::hide()
{
	hidden = ectrue;
	losefocus();
	
	//for(std::list<Widget*>::iterator i=sub.begin(); i!=sub.end(); i++)
	//	(*i)->hide();
}

void Widget::show()
{
	hidden = ecfalse;
	//necessary for window widgets:
	//tofront();	//can't break list iterator, might shift

	//for(std::list<Widget*>::iterator i=sub.begin(); i!=sub.end(); i++)
	//	(*i)->show();
}

void Widget::chcall(Widget* ch, int type, void* data)
{
}

//free subwidget children
void Widget::freech()
{
	std::list<Widget*>::iterator witer = sub.begin();
	while(witer != sub.end())
	{
		delete *witer;
		witer = sub.erase(witer);
	}
}

void SubCrop(float *src1, float *src2, float *dest)
{
	dest[0] = fmax(src1[0], src2[0]);
	dest[1] = fmax(src1[1], src2[1]);
	dest[2] = fmin(src1[2], src2[2]);
	dest[3] = fmin(src1[3], src2[3]);

	//purposely inverted frame means it's out of view
	
	//if(ndest[0] > ndest[2])
	//	ndest[0] = ndest[2]+1.0f;

	//if(ndest[1] > ndest[3])
	//	ndest[1] = ndest[3]+1.0f;
}