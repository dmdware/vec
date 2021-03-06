











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
extern Sprite g_sp[SPRITES];

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
	char* fullpath;

	int nsp;
	unsigned int* sprites;

};

typedef struct SpList SpList;

void SpList_init(SpList *sl);
void SpList_free(SpList *sl);

#define SPRITELISTS	4
extern SpList g_splist[SPRITELISTS];

struct SpToLoad
{
	char* relative;
	unsigned int* spin;
	ecbool loadteam;
	ecbool loaddepth;
};

typedef struct SpToLoad SpToLoad;

void SpLoad_init(SpToLoad *stl);
void SpLoad_free(SpToLoad *stl);

extern Vector g_spload;

extern int g_lastLSp;

ecbool Load1Sp();
void FreeSps();
ecbool LoadSp(const char* relative, unsigned int* spin, ecbool loadteam, ecbool loaddepth);
void QueueSp(const char* relative, unsigned int* spin, ecbool loadteam, ecbool loaddepth);
void ParseSp(const char* relative, Sprite* s);
int SpRef(SpList* sl, int frame, int incline, int pitch, int yaw, int roll,
				  int slicex, int slicey);
ecbool LoadSpList(const char* relative, unsigned int* splin, ecbool loadteam, ecbool loaddepth, ecbool queue);
int NewSpList();
int NewSp();

#define IN_0000		0
#define IN_0001		1
#define IN_0010		2
#define IN_0011		3
#define IN_0100		4
#define IN_0101		5
#define IN_0110		6
#define IN_0111		7
#define IN_1000		8
#define IN_1001		9
#define IN_1010		10
#define IN_1011		11
#define IN_1100		12
#define IN_1101		13
#define IN_1110		14
#define INCLINES	15

extern const char* INCLINENAME[INCLINES];

#endif
