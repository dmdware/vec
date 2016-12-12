











#ifndef SPRITE_H
#define SPRITE_H

#include "../texture.h"
#include "../algo/bool.h"
#include "../algo/vector.h"

struct Sprite
{
	ecbool on;
	unsigned int difftexi;
	unsigned int teamtexi;
	unsigned int depthtexi;
	unsigned int elevtexi;
	float offset[4];	//the pixel texture coordinates centered around a certain point, for use for vertex positions when not cropping (old)
	float crop[4];	//the texture coordinates [0..1] of the cropped sprite, used for the texture coords
	float cropoff[4];	//the pixel texture coordinates of the cropped sprite, used for vertex positions
	LoadedTex* pixels;
	char* fullpath;
};

typedef struct Sprite Sprite;

void Sprite_init(Sprite *s);
void Sprite_free(Sprite *s);

#define SPRITES	40
extern Sprite g_sprite[SPRITES];

struct SpList
{
	ecbool on;
	ecbool inclines;
	ecbool sides;
	int nsides;
	ecbool rotations;
	ecbool frames;
	int nframes;
	int nslices;
	char fullpath;

	int nsp;
	unsigned int* sprites;

};

typedef struct SpList SpList;

void SpList_init(SpList *sl);
void SpList_free(SpList *sl);

#define SPRITELISTS	4
extern SpList g_splist[SPRITELISTS];

struct SpriteToLoad
{
	char* relative;
	unsigned int* spindex;
	ecbool loadteam;
	ecbool loaddepth;
};

typedef struct SpriteToLoad SpriteToLoad;

void SpLoad_init(SpriteToLoad *stl);
void SpLoad_free(SpriteToLoad *stl);

extern Vector g_spriteload;

extern int g_lastLSp;

ecbool Load1Sprite();
void FreeSprites();
ecbool LoadSprite(const char* relative, unsigned int* spindex, ecbool loadteam, ecbool loaddepth);
void QueueSprite(const char* relative, unsigned int* spindex, ecbool loadteam, ecbool loaddepth);
void ParseSprite(const char* relative, Sprite* s);
int SpriteRef(SpList* sl, int frame, int incline, int pitch, int yaw, int roll,
				  int slicex, int slicey);
ecbool LoadSpriteList(const char* relative, unsigned int* splin, ecbool loadteam, ecbool loaddepth, ecbool queue);
int NewSpriteList();
int NewSprite();

#endif
