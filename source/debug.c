#include "platform.h"
#include "debug.h"
#include "utils.h"
#include "gui/gui.h"
#include "gui/widget.h"
#include "window.h"
#include "app/appmain.h"
#include "algo/bool.h"

Timer g_profile[TIMERS];
ecbool g_debuglines = ecfalse;

void OutOfMem(const char *file, int line)
{
	char msg[2048];
	sprintf(msg, "Failed to allocate memory in %s on line %d.", file, line);
	ErrMess("Out of memory", msg);
	//g_quit = ectrue;
}

void Timer_init(Timer *t)
{
	t->averagems = 0.0;
	t->lastframeelapsed = 0;
	t->lastframe = 0;
	t->frames = 0;
	t->inside = -1;
}

void StartTimer(int i)
{
	g_profile[i].starttick = GetTicks();
}

void StopTimer(int i)
{
    unsigned __int64 elapsed;
    
	if(g_appmode != APPMODE_PLAY)
		return;

	elapsed = GetTicks() - g_profile[i].starttick;
	g_profile[i].starttick = GetTicks();
	g_profile[i].lastframeelapsed += (int)elapsed;

	if(i == TIMER_FRAME || g_profile[i].lastframe < g_profile[TIMER_FRAME].lastframe)
	{
		g_profile[i].averagems = ( (double)g_profile[i].lastframeelapsed + g_profile[i].averagems*(double)g_profile[i].frames ) / (double)(g_profile[i].frames+1);
		g_profile[i].frames++;
		g_profile[i].lastframeelapsed = 0;
		g_profile[i].lastframe = g_profile[TIMER_FRAME].lastframe;
	}
	if(i == TIMER_FRAME)
		g_profile[i].lastframe++;
}

void WriteProfiles(int in, int layer)
{
    double parentavgms, ofparentpct, totalms, totalofparentpct, percentage;
    int subprofiles, k;
    char full[DMD_MAX_PATH+1];
    static FILE *f;
    Timer *t;
	char j;
    
    subprofiles = 0;

	if(in == -1)
	{
		FullPath("profiles.txt", full);
		f = fopen(full, "w");
        t = &g_profile[TIMER_FRAME];
		parentavgms = t->averagems;
	}
	else
	{
		parentavgms = g_profile[in].averagems;
	}

    totalms = 0;
    totalofparentpct = 0;
    subprofiles = 0;

	for(j=0; j<TIMERS; j++)
    {
        t = &g_profile[j];
        
		if(t->inside != in)
			continue;

		totalms += g_profile[j].averagems;
	}

	for(j=0; j<TIMERS; j++)
	{
        t = &g_profile[j];
        
		if(t->inside != in)
			continue;

		percentage = 100.0 * t->averagems / totalms;
		ofparentpct = 100.0 * t->averagems / parentavgms;
		totalofparentpct += ofparentpct;
		subprofiles++;

		for(k=0; k<layer; k++)
			fprintf(f, "\t");

		fprintf(f, "%s\t...\t %lf ms per frame, %lf%% of this level's total\r\n", t->name, t->averagems, percentage);

		WriteProfiles(j, layer+1);
	}

	if(subprofiles > 0)
	{
        for(k=0; k<layer; k++)
            fprintf(f, "\t");

		fprintf(f, "level total sum: %lf ms per frame, that means %lf%% of this parent's duration underwent profiling\r\n", totalms, totalofparentpct);
	}

	if(in == -1)
	{
        fclose(f);
	}
}

void DefTimer(int i, int inside, const char *name)
{
    Timer *t;
    
    t = &g_profile[i];
    
    t->inside = inside;
	strcpy(t->name, name);
}

void DefTimers()
{
	DefTimer(TIMER_FRAME, -1, "Frame");
	DefTimer(TIMER_EVENT, TIMER_FRAME, "EventProc");
	DefTimer(TIMER_DRAW, TIMER_FRAME, "Draw();");
	DefTimer(TIMER_DRAWSCENEDEPTH, TIMER_DRAW, "DrawSceneDepth();");
	DefTimer(TIMER_DRAWSETUP, TIMER_DRAW, "Draw(); setup");
	DefTimer(TIMER_DRAWGUI, TIMER_DRAW, "DrawGUI();");
	DefTimer(TIMER_DRAWMINIMAP, TIMER_DRAW, "DrawMinimap();");
	DefTimer(TIMER_UPDATE, TIMER_FRAME, "Update();");
	DefTimer(TIMER_RESETPATHNODES, TIMER_UPDATE, "ResetPathNodes();");
	DefTimer(TIMER_MANAGETRIPS, TIMER_UPDATE, "ManageTrips();");
	DefTimer(TIMER_UPDATEUNITS, TIMER_UPDATE, "UpdMvs();");
	DefTimer(TIMER_UPDUONCHECK, TIMER_UPDATEUNITS, "Upd U On Ch");
	DefTimer(TIMER_UPDUNITAI, TIMER_UPDATEUNITS, "Upd Mv AI");
	DefTimer(TIMER_UPDLAB, TIMER_UPDUNITAI, "UpdLab();");
	DefTimer(TIMER_UPDTRUCK, TIMER_UPDUNITAI, "UpdTruck();");
	DefTimer(TIMER_FINDJOB, TIMER_UPDLAB, "FindJob();");
	DefTimer(TIMER_JOBLIST, TIMER_FINDJOB, "Job list collection");
	DefTimer(TIMER_JOBSORT, TIMER_FINDJOB, "Job list sort");
	DefTimer(TIMER_JOBPATH, TIMER_FINDJOB, "Job prepathing");
	DefTimer(TIMER_MOVEUNIT, TIMER_UPDATEUNITS, "Move Mv");
	DefTimer(TIMER_ANIMUNIT, TIMER_UPDATEUNITS, "Anim Mv");
	DefTimer(TIMER_UPDATEBUILDINGS, TIMER_UPDATE, "UpdBl();");
	DefTimer(TIMER_DRAWBL, TIMER_DRAW, "DrawBuildings();");
	DefTimer(TIMER_DRAWUNITS, TIMER_DRAW, "DrawMvs();");
	DefTimer(TIMER_DRAWRIM, TIMER_DRAW, "DrawRim();");
	DefTimer(TIMER_DRAWWATER, TIMER_DRAW, "DrawWater();");
	DefTimer(TIMER_DRAWCD, TIMER_DRAW, "DrawCrPipes();");
	DefTimer(TIMER_DRAWPOWLS, TIMER_DRAW, "DrawPowls();");
	DefTimer(TIMER_DRAWFOLIAGE, TIMER_DRAW, "DrawFol();");
	DefTimer(TIMER_DRAWLIST, TIMER_DRAW, "gather drawing list");
	DefTimer(TIMER_DRAWSORT, TIMER_DRAW, "sort drawing list");
	DefTimer(TIMER_SORTPARTICLES, TIMER_DRAW, "SortParticles();");
	DefTimer(TIMER_DRAWPARTICLES, TIMER_DRAW, "DrawParticles();");
	DefTimer(TIMER_DRAWMAP, TIMER_DRAW, "DrawMap();");
	//DefTimer(SHADOWS, TIMER_DRAW, "Shadows");
	DefTimer(TIMER_DRAWSKY, TIMER_DRAW, "DrawSky();");
	DefTimer(TIMER_DRAWROADS, TIMER_DRAW, "DrawRoads();");
	//DefTimer(DRAWMODEL1, TIMER_DRAWBL, "Draw model 1");
	//DefTimer(DRAWMODEL2, TIMER_DRAWBL, "Draw model 2");
	//DefTimer(DRAWMODEL3, TIMER_DRAWBL, "Draw model 3");
	DefTimer(TIMER_DRAWMAPDEPTH, TIMER_DRAWSCENEDEPTH, "DrawMap(); depth");
	DefTimer(TIMER_DRAWUMAT, TIMER_DRAWUNITS, "CPU-side matrix math etc.");
	DefTimer(TIMER_DRAWUGL, TIMER_DRAWUNITS, "GPU-side");
	DefTimer(TIMER_DRAWUTEXBIND, TIMER_DRAWUNITS, "texture bind");
}

#ifdef GLDEBUG
void CheckGLError(char *file, int line)
{
	//char msg[2048];
	//sprintf(msg, "Failed to allocate memory in %s on line %d.", file, line);
	//ErrMess("Out of memory", msg);
	int error = glGetError();

	if(error == GL_NO_ERROR)
		return;

	fprintf(g_applog, "GL Error #%d in %s on line %d using shader #%d\r\n", error, file, line, g_curS);
}
#endif

#if !defined( PLATFORM_MAC ) && !defined( PLATFORM_IOS )
GLvoid APIENTRY GLMessageHandler(GLenum source, GLenum type, GLuint i, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam)
//DEBUGPROC GLMessageHandler(GLenum source, GLenum type, GLuint i, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam)
{
	//ErrMess("GL Error", message);
	fprintf(g_applog, "GL Message: %s\r\n", message);
}
#endif


