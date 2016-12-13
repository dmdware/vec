#ifndef DEBUG_H
#define DEBUG_H

#include "platform.h"
#include "gui/richtext.h"
#include "algo/bool.h"

#define	TIMER_FRAME				0
#define TIMER_EVENT				1
#define TIMER_UPDATE			2
#define TIMER_DRAW				3
#define TIMER_DRAWSETUP			4
#define TIMER_DRAWGUI			5
#define TIMER_DRAWMINIMAP		6
#define TIMER_UPDATEUNITS		7
#define TIMER_UPDATEBUILDINGS	8
#define TIMER_UPDUONCHECK		9
#define TIMER_UPDUNITAI			10
#define TIMER_MOVEUNIT			11
#define TIMER_ANIMUNIT			12
#define TIMER_DRAWBL			13
#define TIMER_DRAWUNITS			14
#define TIMER_DRAWRIM			15
#define TIMER_DRAWWATER			16
#define TIMER_DRAWCD		17
#define TIMER_DRAWPOWLS			18
#define TIMER_DRAWFOLIAGE		19
#define TIMER_SORTPARTICLES		20
#define TIMER_DRAWPARTICLES		21
#define TIMER_DRAWMAP			22
#define TIMER_DRAWSCENEDEPTH	23
#define TIMER_DRAWSKY			24
#define TIMER_DRAWROADS			25
#define TIMER_DRAWMAPDEPTH		26
#define TIMER_DRAWUNITSDEPTH	27
#define TIMER_DRAWUMAT			28
#define TIMER_DRAWUTEXBIND		29
#define TIMER_DRAWUGL			30
#define TIMER_MANAGETRIPS		31
#define TIMER_UPDLAB			32
#define TIMER_UPDTRUCK			33
#define TIMER_FINDJOB			34
#define TIMER_JOBLIST			35
#define TIMER_JOBSORT			36
#define TIMER_JOBPATH			37
#define TIMER_RESETPATHNODES	38
#define TIMER_DRAWLIST			39
#define TIMER_DRAWSORT			40
#define TIMERS					41

struct Timer
{
	char name[64];
	double averagems;
	int lastframe;
	int frames;
	//double framems;
	unsigned __int64 starttick;
	//double timescountedperframe;
	//double lastframeaverage;
	int lastframeelapsed;
	int inside;
};

typedef struct Timer Timer;

extern Timer g_profile[TIMERS];

void Timer_init(Timer *t);

void StartTimer(int i);
void StopTimer(int i);
void DefTimer(int i, int inside, const char *name);
void WriteProfiles(int in, int layer);
void DefTimers();

#ifdef GLDEBUG
void CheckGLError(char *file, int line);
#endif

#ifndef MATCHMAKER
#if !defined( PLATFORM_MAC ) && !defined( PLATFORM_IOS )
GLvoid APIENTRY GLMessageHandler(GLenum source, GLenum type, GLuint i, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam);
//DEBUGPROC GLMessageHandler(GLenum source, GLenum type, GLuint i, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam);
#endif
#endif


void OutOfMem(const char *file, int line);

#define OUTOFMEM()	OutOfMem(__FILE__,__LINE__)

#endif	//DEBUG_H
