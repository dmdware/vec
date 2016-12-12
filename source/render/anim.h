

#ifndef ANIM_H
#define ANIM_H

#include "../algo/bool.h"

ecbool PlayAnim(int* frame, int first, int last, ecbool loop, int rate);
ecbool PlayAnimB(int* frame, int first, int last, ecbool loop, int rate);

#endif