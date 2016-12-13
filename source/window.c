


#include "window.h"
#include "algo/bool.h"
#include "gui/widget.h"
#include "gui/gui.h"
#include "render/shader.h"

#ifndef MATCHMAKER
#endif

ecbool g_quit = ecfalse;
ecbool g_background = ecfalse;
ecbool g_fs = ectrue;
Vec2i g_selres;
List g_ress; /* Vec2i */
List g_bpps; /* char */
double g_instantdrawfps = 0;
double g_instantupdfps = 0;
double g_drawfrinterval = 0;
double g_updfrinterval = 0;

#ifndef MATCHMAKER
int g_currw;
int g_currh;
int g_width = INI_WIDTH;
int g_height = INI_HEIGHT;
int g_bpp = INI_BPP;
Vec2i g_mouse;
Vec2i g_mousestart;
ecbool g_keyintercepted = ecfalse;
ecbool g_keys[SDL_NUM_SCANCODES] = {0};
ecbool g_mousekeys[5] = {0};
float g_zoom = INI_ZOOM;
ecbool g_mouseout = ecfalse;
int g_curst = CU_DEFAULT;	//cursor state
int g_kbfocus = 0;	//keyboad focus counter
#endif

void EnumDisp()
{
	short i;
	SDL_DisplayMode mode;
	ecbool found;
	Node* rit;	/* Vec2i */
	Vec2i* rp;
	Vec2i r;

	List_free(&g_ress);

	for(i=0; i<SDL_GetNumDisplayModes(0); ++i)
	{
		SDL_GetDisplayMode(0, i, &mode);

		found = ecfalse;

		for(rit=g_ress.head; rit; rit=rit->next)
		{
			rp = (Vec2i*)rit->data;

			if(rp->x == mode.w &&
				rp->y == mode.h)
			{
				found = ectrue;
				break;
			}
		}

		if(found)
			continue;

		r.x = mode.w;
		r.y = mode.h;
		List_pushback2(&g_ress, sizeof(r), &r);
	}
}

void Resize(int width, int height)
{
	Widget* gui;

	if(height == 0)
		height = 1;

	glViewport(0, 0, width, height);

	gui = (Widget*)&g_gui;

	g_width = width;
	g_height = height;

	Widget_reframe(gui);
}

void CalcDrawRate()
{
	static unsigned __int64 lasttime;
	static unsigned __int64 frametime;
	static int framecounter;

	unsigned __int64 currtime = GetTicks();

	g_drawfrinterval = (double)(currtime - frametime) / 1000.0f;

	frametime = currtime;

	++framecounter;

	if( currtime - lasttime > 1000 )
	{
		g_instantdrawfps = framecounter;

		lasttime = currtime;

		framecounter = 0;
	}
}

ecbool DrawNextFrame()
{
	static unsigned __int64 lastdrawtick;
	static unsigned __int64 elapseddrawtime;

	unsigned __int64 currentTime = GetTicks();
	float desiredFPMS = 1000.0f/(float)DRAW_FRAME_RATE;
	int deltaTime = (int)(currentTime - lastdrawtick);

	if(deltaTime >= desiredFPMS)
	{
		lastdrawtick = currentTime;
		return ectrue;
	}

	return ecfalse;
}


void CalcUpdRate()
{
	static unsigned __int64 frametime = 0;	
	static int framecounter = 0;
	static unsigned __int64 lasttime;

	unsigned __int64 currtime = GetTicks();

	g_updfrinterval = (double)(currtime - frametime) / 1000.0f;

	frametime = currtime;

	++framecounter;

	if( currtime - lasttime > 1000 )
	{
		g_instantupdfps = framecounter;

		lasttime = currtime;

		framecounter = 0;
	}
}

ecbool UpdNextFrame()
{
	static unsigned __int64 lastupdtick;
	static unsigned __int64 elapsedupdtime;

	unsigned __int64 currentTime = GetTicks();
	float desiredFPMS = 1000.0f/(float)SIM_FRAME_RATE;
	int deltaTime = (int)(currentTime - lastupdtick);

	if(deltaTime >= desiredFPMS)
	{
		lastupdtick = currentTime;
		return ectrue;
	}

	return ecfalse;
}

#ifndef MATCHMAKER

ecbool InitWindow()
{
	char path[DMD_MAX_PATH+1];
	LoadedTex* pixels;
	SDL_Surface* surf;

#ifndef PLATFORM_MOBILE
	FullPath("gui/i-64x64.png", path);
	pixels = LoadPNG(path);

	if(!pixels)
	{
		ErrMess("Error", "Couldn't load icon");
	}

	surf = SDL_CreateRGBSurfaceFrom(pixels->data, pixels->sizex, pixels->sizey, pixels->channels*8, pixels->channels*pixels->sizex, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

	if(!surf)
	{
		char message[256];
		sprintf(message, "Couldn't create icon: %s", SDL_GetError());
		ErrMess("Error", message);
	}

	SDL_SetWindowIcon(g_win, surf);

	LoadedTex_free(pixels);
	free(pixels);
#endif

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
#ifdef PLATFORM_GL14
	glClearDepth(1.0f);
#endif
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
#ifdef PLATFORM_GL14
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

	InitGLSL();
	LoadFonts();

	return ectrue;
}

SDL_bool IsFullScreen(SDL_Window *win)
{
	unsigned int flags = SDL_GetWindowFlags(win);

	if (flags & SDL_WINDOW_FULLSCREEN) return SDL_TRUE;

	return SDL_FALSE;
}

int SDL_ToggleFS(SDL_Window *win)
{
	if (IsFullScreen(win))
	{
		if (SDL_SetWindowFullscreen(win, SDL_FALSE) < 0)
		{
			fprintf(g_applog, "Setting windowed failed : %s\r\n", SDL_GetError());
			return -1;
		}

		return 0;
	}

	if (SDL_SetWindowFullscreen(win, SDL_TRUE) < 0)
	{
		fprintf(g_applog, "Setting fullscreen failed : %s\r\n", SDL_GetError());
		return -1;
	}

	return 1; 
}

void BreakWin(const char* title)
{
	char msg[256];
	unsigned int flags;
	Shader *s;

	FreeTextures();

	for(s=g_sh; s<g_sh+SHADERS; ++s)
		Sh_free(s);

	flags = (SDL_GetWindowFlags(g_win) & ~SDL_WINDOW_FULLSCREEN_DESKTOP);
	SDL_SetWindowFullscreen(g_win, flags);

	SDL_GL_DeleteContext(g_gx);

	SDL_DestroyWindow(g_win);

	SDL_QuitSubSystem(SDL_INIT_VIDEO);  /* exit black fullscreen on mac */

	if(SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		sprintf(msg, "SDL_Init: %s\n", SDL_GetError());
		ErrMess("Error", msg);
	}

	SDL_ShowCursor(SDL_FALSE);
}

ecbool MakeWin(const char* title)
{
	char msg[256];
	unsigned int flags;
	int startx;
	int starty;
	int winszx;
	int winszy;

#if 1
#ifdef PLATFORM_GL14
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
#endif
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
#endif

	if(g_fs)
	{
		startx = SDL_WINDOWPOS_UNDEFINED;
		starty = SDL_WINDOWPOS_UNDEFINED;
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
	}
	else
	{
		startx = SDL_WINDOWPOS_UNDEFINED;
		starty = SDL_WINDOWPOS_UNDEFINED;
		flags = SDL_WINDOW_OPENGL;
	}

	g_win = SDL_CreateWindow(
		title,   
		startx,    
		starty,   
		g_selres.x,    
		g_selres.y,  
		flags       
		);

	if (g_win == NULL)
	{
		sprintf(msg, "Could not create window: %s\n", SDL_GetError());
		ErrMess("Error", msg);
		return ecfalse;
	}

	g_gx = SDL_GL_CreateContext(g_win);

	if(!g_gx)
	{
		BreakWin(title);
		ErrMess("Error", "Couldn't create GL context");
		return ecfalse;
	}

	SDL_GL_MakeCurrent(g_win, g_gx);

	SDL_GL_SetSwapInterval(0);

	SDL_GetWindowSize(g_win, &winszx, &winszy);
	Resize(winszx, winszy);

	if(!InitWindow())
	{
		BreakWin(title);
		ErrMess("Error", "Initialization failed");
		return ecfalse;
	}

	CenterMouse();

	return ectrue;
}

#endif
