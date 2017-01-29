

#include "utils.h"
#include "algo/bool.h"

FILE *g_applog = NULL;

/* get a power of 2 number that is big enough to hold 'lowerbound' but does not exceed 2048 */
int Max2Pow(int lowerbound)
{
	int twopow = 2;

	while( twopow < lowerbound
#if 0
		&& twopow < 2048
#endif
		)
		twopow *= 2;

	return twopow;
}

int Max2Pow32(int lowerbound)
{
	int twopow = 32;

	while( twopow < lowerbound
#if 0
		&& twopow < 2048
#endif
		)
		twopow *= 2;

	return twopow;
}

unsigned __int64 GetTicks()
{
	//return time(0);
#ifdef PLATFORM_WIN
	
	SYSTEMTIME st;
	FILETIME ft;

	//return GetTickCount64();
	GetSystemTime (&st);
	SystemTimeToFileTime(&st, &ft);
	//LARGE_INTEGER lint;
	//lint.HighPart = ft.dwHighDateTime;
    //lint.LowPart = ft.dwLowDateTime;
	//convert from 100-nanosecond intervals to milliseconds
	return (*(unsigned __int64*)&ft)/(10*1000);
#else
	struct timeval tv;

	gettimeofday(&tv, NULL);

	return
    (unsigned __int64)(tv.tv_sec) * 1000 +
    (unsigned __int64)(tv.tv_usec) / 1000;
#endif
}

void CorrectSlashes(char *s)
{
	while(*s)
	{
		if(*s=='\\')
			*s='/';
		++s;
	}
}

void StripFile(char* inpath, char* path2)
{
	char temp[DMD_MAX_PATH+1], *lastof;
	strcpy(temp, inpath);
	CorrectSlashes(temp);

	lastof = strrchr(temp, '/');
	if(!lastof)
	{
		strcpy(path2, inpath);
		return;
	}

	++lastof;
	memcpy(path2, inpath, (lastof-temp)+1);
}

void StripPath(char* inpath, char* path2)
{
	char temp[DMD_MAX_PATH+1], *lastof;
	strcpy(temp, inpath);
	CorrectSlashes(temp);

	lastof = strrchr(temp, '/');
	if(!lastof)
	{
		strcpy(path2, inpath);
		return;
	}

	++lastof;
	memcpy(path2, inpath+(lastof-temp), strlen(lastof)+1);
}

void StripExt(char* inpath, char* path2)
{
	char *lastof;

	lastof = strrchr(inpath, '.');

	if(!lastof)
	{
		strcpy(path2, inpath);
		return;
	}

	if(lastof == inpath)
	{
		strcpy(path2, "");
		return;
	}

	--lastof;
	memcpy(path2, inpath, (lastof-inpath));
	path2[(lastof-inpath)+1] = 0;
}

void MakeRel(const char* full, char* rel)
{
	char temp[DMD_MAX_PATH+1];
	char exepath[DMD_MAX_PATH+1];
	char *pos;
	strcpy(temp, full);
	CorrectSlashes(temp);

	ExePath(exepath);
	CorrectSlashes(exepath);

	pos = strstr(temp, exepath);

	if(!pos)
	{
		strcpy(rel, temp);
		return;
	}

	memcpy(rel, pos, strlen(pos)+1);
}

void FullPath(const char* filename, char* full)
{
	char exepath[DMD_MAX_PATH+1];
	char path[DMD_MAX_PATH+1];
	char c;
	ExePath(exepath);
	StripFile(exepath, path);

	//char full[DMD_MAX_PATH+1];
	strcpy(full, path);

	c = full[ strlen(full)-1 ];
	if(c != '\\' && c != '/')
		strcat(full, "\\");
	//strcat(full, "/");

	strcat(full, filename);
	CorrectSlashes(full);
}

void ExePath(char* exepath)
{
#ifndef PLATFORM_IOS
#if 0
#ifdef PLATFORM_WIN
	//char buffer[WF_MAX_PATH+1];
	GetModuleFileName(NULL, exepath, WF_MAX_PATH+1);
	//std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
	//std::string strexepath = std::string( buffer ).substr( 0, pos);
	//strcpy(exepath, strexepath.c_str());
#else
	char szTmp[32];
	//char buffer[WF_MAX_PATH+1];
	sprintf(szTmp, "/proc/%d/exe", getpid());
	int32_t bytes = std::min((int32_t)readlink(szTmp, exepath, WF_MAX_PATH+1), WF_MAX_PATH);
	if(bytes >= 0)
		exepath[bytes] = '\0';
	//std::string strexepath = StripFile(std::string(buffer));
	//strcpy(exepath, strexepath.c_str());
#endif
#else
	char *base_path = SDL_GetBasePath();
#if 0
	if (base_path) {
        data_path = SDL_strdup(base_path);
        SDL_free(base_path);
    } else {
        data_path = SDL_strdup("./");
    }
#endif
	if(base_path)
	{
		strcpy(exepath, base_path);
		SDL_free(base_path);
	}
#endif
#endif
	
#ifdef PLATFORM_IOS
	char *base_path = SDL_GetBasePath();
	if(base_path)
	{
		strcpy(exepath, base_path);
		SDL_free(base_path);
	}
	strcat(exepath, "testfolder/");
#endif
}

void MakeDir(const char* fulldir)
{
#ifdef PLATFORM_WIN
	CreateDirectory(fulldir, NULL);
#else
	mkdir(fulldir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}

void ErrMess(const char* title, const char* message)
{
	//SDL_ShowCursor(ectrue);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, NULL);
	//SDL_ShowCursor(ecfalse);
}

void InfoMess(const char* title, const char* message)
{
	//SDL_ShowCursor(ectrue);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title, message, NULL);
	//SDL_ShowCursor(ecfalse);
}

void WarnMess(const char* title, const char* message)
{
	SDL_ShowCursor(ectrue);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, title, message, NULL);
	SDL_ShowCursor(ecfalse);
}