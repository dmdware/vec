











#ifndef SPRITE_H
#define SPRITE_H

#include "../texture.h"
#include "../algo/bool.h"

struct Sprite
{
	ecbool on;
	uint32_t difftexi;
	uint32_t teamtexi;
	uint32_t depthtexi;
	uint32_t elevtexi;
	float offset[4];	//the pixel texture coordinates centered around a certain point, for use for vertex positions when not cropping (old)
	float crop[4];	//the texture coordinates [0..1] of the cropped sprite, used for the texture coords
	float cropoff[4];	//the pixel texture coordinates of the cropped sprite, used for vertex positions
	LoadedTex* pixels;
	std::string fullpath;
};

typedef struct Sprite Sprite;

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
	std::string fullpath;

	int nsp;
	uint32_t* sprites;

	void free()
	{
		nsp = 0;
		delete [] sprites;
		sprites = NULL;
		on = false;
		nslices = 1;
	}

	SpList()
	{
		nsp = 0;
		sprites = NULL;
		on = false;
		nslices = 1;
	}

	~SpList()
	{
		free();
	}
};

typedef struct SpList SpList;

#define SPRITELISTS	4
extern SpList g_splist[SPRITELISTS];

class SpriteToLoad
{
public:
	std::string relative;
	uint32_t* spindex;
	ecbool loadteam;
	ecbool loaddepth;
};

extern std::vector<SpriteToLoad> g_spriteload;

extern int g_lastLSp;

ecbool Load1Sprite();
void FreeSprites();
ecbool LoadSprite(const char* relative, uint32_t* spindex, ecbool loadteam, ecbool loaddepth);
void QueueSprite(const char* relative, uint32_t* spindex, ecbool loadteam, ecbool loaddepth);
void ParseSprite(const char* relative, Sprite* s);
ecbool PlayAnim(float& frame, int first, int last, ecbool loop, float rate);
ecbool PlayAnimB(float& frame, int first, int last, ecbool loop, float rate);	//Play animation backwards
int SpriteRef(SpList* sl, int frame, int incline, int pitch, int yaw, int roll,
				  int slicex, int slicey);
ecbool LoadSpriteList(const char* relative, uint32_t* splin, ecbool loadteam, ecbool loaddepth, ecbool queue);
int NewSpriteList();
int NewSprite();

#endif
