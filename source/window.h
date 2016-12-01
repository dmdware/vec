



#ifndef WINDOW_H
#define WINDOW_H

#include "platform.h"
#include "math/3dmath.h"
#include "math/vec2i.h"
#include "algo/vector.h"
#include "algo/list.h"

#define INI_WIDTH			640
#define INI_HEIGHT			480
#define INI_BPP				32
#define DRAW_FRAME_RATE		30
#define SIM_FRAME_RATE		30
#define MIN_DISTANCE		1
#define MAX_DISTANCE		(5 * 1000 * 10)
#define FIELD_OF_VIEW		90
#define PROJ_RIGHT			600
#define MIN_ZOOM		0.05f
#define MAX_ZOOM		0.7f
#define INI_ZOOM			0.05f

extern dmbool g_quit;
extern dmbool g_background;
extern dmbool g_active;
extern dmbool g_fs;
extern double g_instantdrawfps;
extern double g_instantupdfps;
extern double g_updfrinterval;
extern double g_drawfrinterval;

extern Vec2i g_selres;
extern Vector g_ress;
extern Vector g_bpps;

#ifndef MATCHMAKER
extern Camera g_cam;
extern int g_currw;
extern int g_currh;
extern int g_width;
extern int g_height;
extern int g_bpp;
extern Vec2i g_mouse;
extern Vec2i g_mousestart;
extern dmbool g_keyintercepted;
extern dmbool g_keys[SDL_NUM_SCANCODES];
extern dmbool g_mousekeys[5];
extern float g_zoom;
extern dmbool g_mouseout;
extern dmbool g_moved;
extern dmbool g_canplace;
extern int g_bpcol;
extern int g_build;
extern Sel g_sel;
extern dmbool g_mouseoveraction;
extern int g_curst;	/* cursor state */
extern int g_kbfocus;	/* keyboad focus counter */

#endif

void CalcDrawRate();
dmbool DrawNextFrame();
void CalcUpdRate();
dmbool UpdNextFrame();
void EnumDisp();
void Resize(int width, int height);
void BreakWin(const char* title);
dmbool MakeWin(const char* title);

#endif
