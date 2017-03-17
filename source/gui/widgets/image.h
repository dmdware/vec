







#ifndef IMAGE_H
#define IMAGE_H

#include "../widget.h"
#include "../../algo/bool.h"

struct Image
{
	Widget base;
	
	float texc[4];	/* texture coordinates */
	unsigned int tex;
	float rgba[4];
};

typedef struct Image Image;

void Image_init(Image *i,
				Widget* parent, 
				const char* nm, 
				const char* filepath, 
				ecbool clamp, 
				void (*reframef)(Widget* w), 
				float r, 
				float g, 
				float b, 
				float a, 
				float texleft, 
				float textop, 
				float texright, 
				float texbottom);
	
void Image_draw(Image *i);

#endif
