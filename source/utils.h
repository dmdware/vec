

#ifndef UTILS_H
#define UTILS_H

#include "platform.h"

extern FILE *g_applog;

void OpenLog(const char* file, int ver);
int Max2Pow(int lowerbound);
int Max2Pow32(int lowerbound);
unsigned __int64 GetTicks();
void FullWrite(const char* filename, char* full);
void FullPath(const char* filename, char* full);
void MakeDir(const char* fulldir);
void ErrMess(const char* title, const char* message);
void InfoMess(const char* title, const char* message);
void WarnMess(const char* title, const char* message);
void StripFile(char* inpath, char* path2);
void StripPath(char* inpath, char* path2);
void StripExt(char* inpath, char* path2);
void CorrectSlashes(char *s);
void ExePath(char* exepath);
void MakeRel(const char* full, char* rel);

float fmax(const float a, const float b);
float fmin(const float a, const float b);

int imax(const int x, const int y);
int imin(const int x, const int y);

#define enmax(a,b) (((a)>(b))?(a):(b))
#define enmin(a,b) (((a)<(b))?(a):(b))

int iabs(int v);
int isign(int x);
double fsign(double x);
int iceil(const int num, const int denom);

#endif