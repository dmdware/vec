

#ifndef SHADER_H
#define SHADER_H

#include "../platform.h"
#include "../algo/bool.h"

#define SSLOT_TEXTURE0			0
#define SSLOT_POSITION			1
#define SSLOT_NORMAL			2
#define SSLOT_COLOR				3
#define SSLOT_WIDTH				4
#define SSLOT_HEIGHT			5
#define SSLOT_TEXCOORD0			6
#define SSLOTS					7

struct Shader
{
	ecbool hastexc;
	ecbool hasnorm;
	GLint slot[SSLOTS];

	// http://lists.apple.com/archives/mac-opengl/2005/Nov/msg00182.html
#if defined( PLATFORM_MAC ) || defined( PLATFORM_IOS )
	GLuint vertshader;
	GLuint fragshader;
	GLuint program;
#else
	GLhandleARB vertshader;
	GLhandleARB fragshader;
	GLhandleARB program;
#endif
};

typedef struct Shader Shader;

#define SH_ORTHO				0
#define SH_COLOR2D				1
#define SHADERS					2

extern Shader g_sh[SHADERS];
extern signed char g_curS;

void Sh_init(Shader* s);
void Sh_free(Shader* s);

GLint Sh_gu(Shader *s, const char* strvar);
GLint Sh_ga(Shader *s, const char* strvar);

void Sh_mu(Shader *s, int slot, const char* variable);
void Sh_ma(Shader *s, int slot, const char* variable);

void UseS(int shader);
void EndS();
void InitGLSL();
void LoadSh(int shader, 
			const char* namev,
			const char* namef,
			const char* strv, 
			const char* strf, 
			ecbool hastexc, 
			ecbool hasnorm);

extern const char *SHTEXT [SHADERS][4];

#endif


