#include "icon.h"
#include "../texture.h"

Icon g_icon[ICONS];

void Icon_init(Icon *i)
{
	i->tag[0] = 0;
}

void Icon_free(Icon *i)
{
	i->tag[0] = 0;
}

void DefI(int type, char *relative, char *tag)
{
	Icon *i;
	Texture *t;
	
	i = &g_icon[type];

	strcpy(i->tag, tag);
	CreateTex(&i->tex, relative, ectrue, ecfalse);
	t = &g_tex[i->tex];
	i->width = t->width;
	i->height = t->height;
}
