











#include "sprite.h"
#include "../utils.h"
#include "../texture.h"
#include "../gui/gui.h"

Vector g_spriteload; /* SpriteToLoad */
int g_lastLSp = -1;
Sprite g_sprite[SPRITES];
SpList g_splist[SPRITELISTS];

void Sprite_init(Sprite *s)
{
	s->on = ecfalse;
	s->difftexi = 0;
	s->teamtexi = 0;
	s->pixels = NULL;
}

void Sprite_free(Spite *s)
{
	if(s->pixels)
		LoadedTex_free(s->pixels);
	free(s->pixels);
	s->pixels = NULL;
	s->on = false;
}

void SpList_init(SpList *sl)
{
	sl->nsp = 0;
	sl->sprites = NULL;
	sl->on = ecfalse;
	sl->nslices = 1;
}

void SpList_free(SpList *sl)
{
	sl->nsp = 0;
	free(sl->spites);
	sl->sprites = NULL;
	sl->on = ecfalse;
	sl->nslices = 1;
}

void SpLoad_init(SpriteToLoad *stl)
{
	stl->relative = NULL;
	stl->spindex = NULL;
}

void SpLoad_free(SpriteToLoad *stl)
{
	free(stl->relative)
	free(stl->spindex);
	stl->relative = NULL;
	stl->spindex = NULL;
}

void FreeSprites()
{
	Sprite *s;
	SpList *sl;

	for(s=g_sprites; s<g_sprites+SPRITES; ++s)
	{
		if(!s->on)
			continue;

		Sprite_free(s);
	}
	
	for(sl=g_splist; sl<g_splist+SPRITELISTS; ++sl)
	{
		if(!sl->on)
			continue;

		SpList_free(sl);
	}
}

ecbool Load1Sprite()
{
	SpriteToLoad* s;
	char m[128];

	if(g_lastLSp+1 < g_spriteload.size)
		SetStatus(g_spriteload[g_lastLSp+1].relative.c_str());

	CHECKGLERROR();

	if(g_lastLSp >= 0)
	{
		s = g_spriteload+g_lastLSp;
		if(!LoadSprite(s->relative, s->spindex, s->loadteam, s->loaddepth))
		{
			sprintf(m, "Failed to load sprite %s", s->relative);
			ErrMess("Error", m);
		}
	}

	g_lastLSp ++;

	if(g_lastLSp >= g_spriteload.size)
	{
		return ecfalse;	// Done loading all
	}

	return ectrue;	// Not finished loading
}

void QueueSprite(const char* relative, unsigned int* spindex, ecbool loadteam, ecbool loaddepth)
{
	SpriteToLoad stl;
	pstrset(&stl.relativ, relative);
	stl.spindex = spindex;
	stl.loadteam = loadteam;
	stl.loaddepth = loaddepth;
	Vector_pushback2(&g_spriteload, sizeof(SpriteToLoad), &stl);
}

int NewSprite()
{
	Sprite* s;
	int i;

	for(i=0; i<SPRITES; ++i)
	{
		s = g_sprite+i;

		if(!s->on)
			return i;
	}

	return -1;
}

int NewSpriteList()
{
	SpList* sl;
	int i;

	for(i=0; i<SPRITELISTS; ++i)
	{
		sl = g_splist+i;

		if(!sl->on)
			return i;
	}

	return -1;
}

ecbool FindSprite(unsigned int *spriteidx, const char* relative)
{
	char corrected[DMD_MAX_PATH+1];
	char fullpath[DMD_MAX_PATH+1];
	int i;
	Sprite* s;
	strcpy(corrected, relative);
	CorrectSlashes(corrected);
	FullPath(corrected, fullpath);

	for(i=0; i<SPRITES; ++i)
	{
		s = g_sprite+i;

		if(s->on && strcmp(s->fullpath, fullpath) == 0)
		{
			*spriteidx = i;
			return true;
		}
	}

	return false;
}

/*
TODO
Convert to C90
All upper case to lower case
CreateTex -> createtex
All class to struct
LoadedTex -> loadedtex_t or loadedtex
*/

ecbool LoadSprite(const char* relative, unsigned int* spindex, ecbool loadteam, ecbool loaddepth)
{
	int i;
	Sprite* s;
	char full[DMD_MAX_PATH+1];
	char reltxt[DMD_MAX_PATH+1];
	char reldiff[DMD_MAX_PATH+1];
	char relteam[DMD_MAX_PATH+1];
	char reldepth[DMD_MAX_PATH+1];
	char pixfull[DMD_MAX_PATH+1];

	if(FindSprite(*spindex, relative))
		return ectrue;

	i = NewSprite();

	if(i < 0)
		return ecfalse;

	s = g_sprite+i;
	s->on = ectrue;
	*spindex = i;

	FullPath(relative, full);
	CorrectSlashes(full);
	s->fullpath = full;

	sprintf(reltxt, "%s.txt", relative);
	sprintf(reldiff, "%s.png", relative);
	sprintf(relteam, "%s_team.png", relative);
	sprintf(reldepth, "%s_depth.png", relative);
	ParseSprite(reltxt, s);

	CreateTex(s->difftexi, reldiff, true, false);
	if(loadteam)
		CreateTex(s->teamtexi, relteam, true, false);
	if(loaddepth)
		CreateTex(s->depthtexi, reldepth, true, false);]
	
	FullPath(reldiff, pixfull);
	s->pixels = LoadTexture(pixfull);

	if(!s->pixels)
	{
		fprintf(g_applog, "Failed to load sprite %s\r\n", relative);
		return ecfalse;
	}
	else
		fprintf(g_applog, "%s\r\n", relative);

	return ectrue;
}

ecbool LoadSpriteList(const char* relative, unsigned int* splin, ecbool loadteam, ecbool loaddepth, ecbool queue)
{
	int i;
	SpList* sl;
	char full[DMD_MAX_PATH+1];
	char txtpath[DMD_MAX_PATH+1];
	int nsides;
	ecbool dorots;	//rotations
	ecbool dosides;	//sides
	ecbool doincls;	//inclines
	ecbool doframes;	//frames
	int nframes;
	FILE* fp;
	char line[128];
	char* tok;
	int ncombos;
	int sidechunk;
	int inclchunk;
	int frameschunk;
	int incli;
	int pitchi;
	int yawi;
	int rolli;
	int sidei;
	int framei;
	int ci;
	char combo[DMD_MAX_PATH+1];
	char frame[32];
	char side[32];
	char stage[32];

	i = NewSpriteList();

	if(i < 0)
		return ecfalse;

	sl = &g_splist[i];
	sl->on = true;
	*splin = i;
	
	FullPath(relative, full);
	CorrectSlashes(full);
	sl->fullpath = full;

	sprintf(txtpath, "%s_list.txt", full);

	nsides = 1;
	dorots = ecfalse;	//rotations
	dosides = ecfalse;	//sides
	doincls = ecfalse;	//inclines
	doframes = ecfalse;	//frames
	nframes = 1;

	fp = fopen(txtpath, "r");

	if(!fp)
		return ecfalse;

	do
	{
		fgets(line, 100, fp);

		tok = strtok(line, " ");

		if(!tok)
			continue;

		if(strcmp(tok, "inclines") == 0)
		{
			tok = strtok(NULL, " ");

			if(!tok)
				continue;

			sscanf(tok, "%hhu", &doincls);
		}
		else if(strcmp(tok, "frames") == 0)
		{
			tok = strtok(NULL, " ");

			if(!tok)
				continue;

			sscanf(tok, "%hhu", &doframes);

			tok = strtok(NULL, " ");

			if(!doframes || !tok)
				continue;

			nframes = StrToInt(tok);
		}
		else if(strcmp(tok, "sides") == 0)
		{
			tok = strtok(NULL, " ");

			if(!tok)
				continue;

			sscanf(tok, "%hhu", &dosides);

			tok = strtok(NULL, " ");

			if(!dosides || !tok)
				continue;

			nsides = StrToInt(tok);
		}
		else if(strcmp(tok, "rotations") == 0)
		{
			tok = strtok(NULL, " ");

			if(!tok)
				continue;

			sscanf(tok, "%hhu", &dorots);

			tok = strtok(NULL, " ");

			if(!dorots || !tok)
				continue;

			sscanf(tok, "%hhu", &nsides);
		}
	}while(!feof(fp));

	fclose(fp);

	ncombos = 1;
	sidechunk = 1;
	inclchunk = 1;
	frameschunk = 1;

	if(dorots)
	{
		ncombos *= nsides;
		ncombos *= nsides;
		ncombos *= nsides;
		sidechunk = nsides * nsides * nsides;
	}
	else if(dosides)
	{
		ncombos *= nsides;
		sidechunk = nsides;
	}

	inclchunk = sidechunk;

	if(doincls)
	{
		ncombos *= INCLINES;
		inclchunk = sidechunk * INCLINES;
	}

	frameschunk = inclchunk;

	if(doframes)
	{
		ncombos *= nframes;
		frameschunk = inclchunk * nframes;
	}

	sl->frames = doframes;
	sl->nframes = nframes;
	sl->inclines = doincls;
	sl->nsides = nsides;
	sl->sides = dosides;
	sl->rotations = dorots;
	sl->sprites = (unsigned int*)malloc( sizeof(unsigned int) * ncombos );

	if(!sl->sprites)
		OUTOFMEM();

	incli = 0;
	pitchi = 0;
	yawi = 0;
	rolli = 0;
	sidei = 0;
	framei = 0;

	for(ci=0; ci<ncombos; ci++)
	{
		if(dorots)
		{
			yawi = ci % nsides;
			pitchi = (ci / nsides) % nsides;
			rolli = (ci / nsides / nsides) % nsides;
		}
		else if(dosides)
		{
			sidei = ci % nsides;
		}

		if(doincls)
		{
			incli = (ci / sidechunk) % INCLINES;
		}

		if(doframes)
		{
			framei = (ci / inclchunk) % nframes;
		}

		strcpy(frame, "");
		strcpy(side, "");

		if(doframes)
			sprintf(frame, "_fr%03d", framei);

		if(dosides && !dorots)
			sprintf(side, "_si%d", sidei);
		else if(dorots)
			sprintf(side, "_y%dp%dr%d", yawi, pitchi, rolli);

		//std::string incline = "";
		char incline[32] = "";

		//TODO use INCLINENAME
		if(doincls)
		{
#if 0
			if(incli == IN_0000)	incline = "_inc0000";
			else if(incli == IN_0001)	incline = "_inc0001";
			else if(incli == IN_0010)	incline = "_inc0010";
			else if(incli == IN_0011)	incline = "_inc0011";
			else if(incli == IN_0100)	incline = "_inc0100";
			else if(incli == IN_0101)	incline = "_inc0101";
			else if(incli == IN_0110)	incline = "_inc0110";
			else if(incli == IN_0111)	incline = "_inc0111";
			else if(incli == IN_1000)	incline = "_inc1000";
			else if(incli == IN_1001)	incline = "_inc1001";
			else if(incli == IN_1010)	incline = "_inc1010";
			else if(incli == IN_1011)	incline = "_inc1011";
			else if(incli == IN_1100)	incline = "_inc1100";
			else if(incli == IN_1101)	incline = "_inc1101";
			else if(incli == IN_1110)	incline = "_inc1110";
#else
			sprintf(incline, "_inc%s", INCLINENAME[incli]);
#endif
		}

		strcpy(stage, "");
#if 0
		if(rendstage == RENDSTAGE_TEAM)
			stage = "_team";
		else if(rendstage == RENDSTAGE_DEPTH)
			stage = "_depth";
#endif

		sprintf(combo, "%s%s%s%s%s", relative, side, frame, incline, stage.c_str());

		if(queue)
			QueueSprite(combo, &sl->sprites[ci], loadteam, loaddepth);
		else
			LoadSprite(combo, &sl->sprites[ci], loadteam, loaddepth);
	}

	return ectrue;
}

int SpriteRef(SpList* sl, int frame, int incline, int pitch, int yaw, int roll,
				  int slicex, int slicey)
{
	//int ncombos = 1;
	int sidechunk;
	int inclchunk;
	int frameschunk;
	int sliceschunk;
	int ci;

	sidechunk = 1;
	inclchunk = 1;
	frameschunk = 1;
	sliceschunk = 1;

	if(sl->rotations)
	{
		sidechunk = sl->nsides * sl->nsides * sl->nsides;
	}
	else if(sl->sides)
	{
		sidechunk = sl->nsides;
	}
	
	inclchunk = sidechunk;

	if(sl->inclines)
	{
		inclchunk = sidechunk * INCLINES;
	}

	frameschunk = inclchunk;

	if(sl->frames)
	{
		frameschunk = inclchunk * sl->nframes;
	}
	
	sliceschunk = frameschunk;

	if(sl->nslices > 1)
	{
		sliceschunk = frameschunk * sl->nslices * sl->nslices;
	}

	ci = 0;
	
	if(sl->rotations)
	{
		ci += yaw + pitch * sl->nsides + roll * sl->nsides * sl->nsides;
	}
	else if(sl->sides)
	{
		ci += yaw;
	}

	if(sl->inclines)
	{
		ci += sidechunk * incline;
	}

	if(sl->frames)
	{
		ci += inclchunk * frame;
	}

	if(sl->nslices > 1)
	{
		ci += frameschunk * ( sl->nslices * slicey + slicex );
	}

	return ci;
}

//TODO size up to power of 2 for mobile etc
void ParseSprite(const char* relative, Sprite* s)
{
	char fullpath[DMD_MAX_PATH+1];
	FILE* fp;
	float centerx;
	float centery;
	float width;
	float height;
	float clipszx, clipszy;
	float clipminx, clipminy, clipmaxx, clipmaxy;

	FullPath(relative, fullpath);

	fp = fopen(fullpath, "r");
	if(!fp) return;

	fscanf(fp, "%f %f", &centerx, &centery);
	fscanf(fp, "%f %f", &width, &height);
	fscanf(fp, "%f %f", &clipszx, &clipszy);
	fscanf(fp, "%f %f %f %f", &clipminx, &clipminy, &clipmaxx, &clipmaxy);
	
	s->offset[0] = -centerx;
	s->offset[1] = -centery;
	s->offset[2] = s->offset[0] + width;
	s->offset[3] = s->offset[1] + height;

	s->crop[0] = 0;
	s->crop[1] = 0;
	s->crop[2] = clipszx / width;
	s->crop[3] = clipszy / height;

	s->cropoff[0] = -centerx;
	s->cropoff[1] = -centery;
	s->cropoff[2] = clipszx - centerx;
	s->cropoff[3] = clipszy - centery;

	fclose(fp);
}
