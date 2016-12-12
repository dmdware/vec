










#ifndef DRAW2D_H
#define DRAW2D_H

#include "../platform.h"

void DrawImage(unsigned int tex, float left, float top, float right, float bottom, float texleft, float textop, float texright, float texbottom, float *crop);
void DrawSquare(float r, float g, float b, float a, float left, float top, float right, float bottom, float *crop);
void DrawLine(float r, float g, float b, float a, float x1, float y1, float x2, float y2, float *crop);

#endif
