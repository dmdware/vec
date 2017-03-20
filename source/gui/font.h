#ifndef FONT_H
#define FONT_H

#include "../platform.h"
#include "../algo/bool.h"

#define MAX_CHARS	256
#define CODE_POINTS	110000

struct Glyph
{
	short pixel[2];
	short texsize[2];
	short offset[2];
	short origsize[2];
	float texcoord[4];
};

typedef struct Glyph Glyph;

struct Font
{
	unsigned int texin;
	float width;    //image width
	float height;   //image height
	float gheight;  //glyph height
	Glyph glyph[CODE_POINTS];
};

typedef struct Font Font;

#define FONT_EUROSTILE32	0
#define FONT_MSUIGOTHIC16	1
#define FONT_SMALLFONTS8	2
#define FONT_GULIM32		3
#define FONT_EUROSTILE16	4
#define FONT_CALIBRILIGHT16	5
#define FONT_MSUIGOTHIC10	6
#define FONT_ARIAL10		7
#define FONT_TERMINAL10		8
#define FONT_SMALLFONTS10	9
#define FONTS				10
extern Font g_font[FONTS];

//#define MAINFONT8 (FONT_SMALLFONTS10)
#define MAINFONT8 (FONT_TERMINAL10)
//#define MAINFONT8 (FONT_MSUIGOTHIC10)
//#define MAINFONT8 (FONT_TERMINAL10)
#define MAINFONT16 (FONT_TERMINAL10)
//#define MAINFONT16 (FONT_MSUIGOTHIC16)
//#define MAINFONT16 (FONT_EUROSTILE16)
//#define MAINFONT16 (FONT_SMALLFONTS10)
#define MAINFONT32 (FONT_TERMINAL10)
//#define MAINFONT32 (FONT_SMALLFONTS10)

void LoadFonts();
void UseFontTex();
void UseIconTex(int icon);
void DrawGlyph2(float left, float top, float right, float bottom, float texleft, float textop, float texright, float texbottom);
void HighlGlyph2(float left, float top, float right, float bottom);
void SubDrawTxLine(int caret);
void DrawTxLine(int fnt, float *inframe, float *incrop, char *text, float *color, int caret);
void DrawCenterShadText(int fnt, float *inframe, float *incrop, char *text, float *color, int caret);
void DrawTx(int fnt, float *inframe, float *incrop, char *text, float *color, int ln, int caret, ecbool shadow, ecbool multiline);
int CountLines(char *text, int fnt, float *inframe);
int GetLineStart(char *text, int fnt, float *inframe, int getline);
int EndX(const char *text, int lastg, int fnt, float *inframe, ecbool multiline);
int MatchGlyph(char *text, int fnt, float *inframe, int matchx, int matchy, ecbool multiline);
void Highlight(int fnt, float *inframe, float *incrop, char *text, int highlstarti, int highlendi, ecbool multiline);
void NextLineBreak();
void AdvGlyph();
int TextWidth(int fnt, float *inframe, const char *text);

#endif
