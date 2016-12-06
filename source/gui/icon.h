#ifndef ICON_H
#define ICON_H

#include "../platform.h"

#define MAX_TAG		31	//not counting null terminator

struct Icon
{
	unsigned int tex;
	int w;
	int h;
	char tag[MAX_TAG];
};

typedef struct Icon Icon;

#define ICON_DOLLARS		0
#define STDICONS			1
#define	ICONS				(STDICONS+256)

extern Icon g_icon[ICONS];

void Icon_init(Icon *i);
void Icon_free(Icon *i);

#ifndef MATCHMAKER
void DefI(int type, char *relative, char *tag);
#endif

#endif
