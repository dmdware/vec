

#ifndef UTILS_H
#define UTILS_H

#include "platform.h"

extern FILE *g_applog;

int Max2Pow(int lowerbound);
int Max2Pow32(int lowerbound);
unsigned __int64 GetTicks();
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

#endif