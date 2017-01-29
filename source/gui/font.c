#include "../platform.h"
#include "font.h"
#include "../texture.h"
#include "../render/shader.h"
#include "../window.h"
#include "../utils.h"
#include "gui.h"
#include "icon.h"
#include "richtext.h"
#include "../algo/bool.h"
#include "../algo/list.h"

Font g_font[FONTS];

static short gx;
static short gy;
static int nextlb;  //next [i] to skip line
static int lastspace;
static int j;
static short x0;
static int i;
static short x;
static short y;
static int w;
static int h;
static int line;
//static char *g_str;
static char *g_rtext;
static int g_currfont;
static int startline;
static int starti;
static short gstartx;
static short goffstartx;
static float frame[4];
static float crop[4];
static int glyphi;	//RichText part's [i] index
static unsigned int k;
static float currcolor[4];
static ecbool debugtest = ecfalse;

void Glyph_init(Glyph *g)
{
	g->pixel[0] = 0;
	g->pixel[1] = 0;
	g->texsize[0] = 0;
	g->texsize[1] = 0;
	g->offset[0] = 0;
	g->offset[1] = 0;
	g->origsize[0] = 0;
	g->origsize[1] = 0;
	g->texcoord[0] = 0;
	g->texcoord[1] = 0;
	g->texcoord[2] = 0;
	g->texcoord[3] = 0;
}

void Font_init(Font *f)
{
	f->width = 0;
	f->height = 0;
	f->gheight = 0;
}

void BreakLine()
{
	Font *f;
	
	f = &g_font[g_currfont];
	
	++line;
	x = goffstartx;
	y += f->gheight;
}

void PrepLineBreak()
{
	Font *f;
	Glyph *g;
	int lastspace, adv;
	short x0, iconi;
	Icon *icon;
	float hscale;
	
	f = &g_font[g_currfont];
	
	lastspace = -1;
	x0 = gstartx;

	for(j=i; g_rtext[j]; j+=adv)
	{
		adv = 0;
		k = ToGlyph(&g_rtext[j], &adv);
		
		if(k <= RICH_GLYPH_END)
		{
			if(k == '\n')
			{
				nextlb = j+1;
				return;
			}

			g = &f->glyph[k];
			x0 += g->origsize[0];

			if(k == ' ' || k == '\t')
				lastspace = j;
		}
		else if(k >= RICH_ICON_START &&
				k <= RICH_ICON_END)
		{
			iconi = k - RICH_ICON_START;
			icon = &g_icon[iconi];
			hscale = f->gheight / (float)icon->h;
			x0 += (float)icon->w * hscale;
		}

		if(x0 > w+gstartx)
		{
			if(lastspace < 0)
			{
				nextlb = imax(j, i+1);
				return;
			}

			nextlb = lastspace+1;
			return;
		}
	}
}

void NextLineBreak()
{
	Font *f;
	
	f = &g_font[g_currfont];

	if(nextlb != starti)
		BreakLine();

	PrepLineBreak();
}

void DrawGlyph()
{
	Font *f;
	Icon *icon;
	float hscale;
	short left, right, top, bottom, iconi;
	Shader *s;
	Glyph *g;
	
	s = &g_shader[g_curS];
	f = &g_font[g_currfont];
	k = ToGlyph(&g_rtext[i], NULL);

	if(k >= RICH_ICON_START &&
	   k <= RICH_ICON_END)
	{
		iconi = k - RICH_ICON_START;
		icon = &g_icon[iconi];
		hscale = f->gheight / (float)icon->height;

		UseIconTex(iconi);
		glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);

		left = x;
		right = (short)( left + (float)icon->width * hscale );
		top = y;
		bottom = top + (short)f->gheight;
		DrawGlyph2((float)left, (float)top, (float)right, (float)bottom, 0, 0, 1, 1);

		UseFontTex();
		glUniform4f(s->slot[SSLOT_COLOR], currcolor[0], currcolor[1], currcolor[2], currcolor[3]);
	}
	else if(k <= RICH_GLYPH_END)
	{
		g = &f->glyph[k];

		left = x + g->offset[0];
		right = left + g->texsize[0];
		top = y + g->offset[1];
		bottom = top + g->texsize[1];
		DrawGlyph2((float)left, (float)top, (float)right, (float)bottom, g->texcoord[0], g->texcoord[1], g->texcoord[2], g->texcoord[3]);
	}
}

void HighlGlyph()
{
	Font *f;
	Icon *icon;
	float hscale;
	short left, right, top, bottom, iconi;
	Shader *s;
	Glyph *g;
	
	s = &g_shader[g_curS];
	f = &g_font[g_currfont];
	k = ToGlyph(&g_rtext[i], NULL);
	
	if(k >= RICH_ICON_START &&
	   k <= RICH_ICON_END)
	{
		iconi = k - RICH_ICON_START;
		icon = &g_icon[iconi];
		hscale = f->gheight / (float)icon->height;

		UseIconTex(iconi);

		left = x;
		right = (short)( left + (float)icon->width * hscale );
		top = y;
		bottom = top + (short)f->gheight;
		HighlGlyph2((float)left, (float)top, (float)right, (float)bottom);

		UseFontTex();
	}
	else if(k <= RICH_GLYPH_END)
	{
		g = &f->glyph[k];

		left = x + g->offset[0];
		right = left + g->texsize[0];
		top = y + g->offset[1];
		bottom = top + g->texsize[1];
		HighlGlyph2((float)left, (float)top, (float)right, (float)bottom);
	}
}

void DrawCaret()
{
	Font *f;
	Glyph *g;
	short left, right, top, bottom;
	
	f = &g_font[g_currfont];
	g = &f->glyph['|'];

	left = x - g->origsize[1]/14;
	right = left + g->texsize[0];
	top = y + g->offset[1];
	bottom = top + g->texsize[1];
	DrawGlyph((float)left, (float)top, (float)right, (float)bottom, g->texcoord[0], g->texcoord[1], g->texcoord[2], g->texcoord[3]);
}

void AdvGlyph()
{
	Font *f;
	Glyph *g;
	short iconi;
	Icon *icon;
	float hscale;
	
	f = &g_font[g_currfont];
	k = ToGlyph(&g_rtext[i], NULL);
	
	if(k >= RICH_ICON_START &&
	   k <= RICH_ICON_END)
	{
		iconi = k - RICH_ICON_START;
		icon = &g_icon[iconi];
		hscale = f->gheight / (float)icon->h;
		x += (float)icon->w * hscale;
		
		++glyphi;
	}
	else if(k <= RICH_GLYPH_END)
	{
		k = p->text.glyphs[glyphi];
		g = &f->glyph[k];
		x += g->origsize[0];

		++glyphi;
	}
}

void UseFontTex()
{
	Shader *s;
	s = &g_shader[g_curS];
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_tex[ g_font[g_currfont].texin ].texname);
	glUniform1i(s->slot[SSLOT_TEXTURE0], 0);
}

void UseIconTex(int icon)
{
	Shader *s;
	s = &g_shader[g_curS];
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_tex[ g_icon[icon].tex ].texname);
	glUniform1i(s->slot[SSLOT_TEXTURE0], 0);
}

void StartText(char *text, int fnt, int ln, int realstartx, float *inframe, float *incrop)
{
	frame[0] = inframe[0];
	frame[1] = inframe[1];
	frame[2] = inframe[2];
	frame[3] = inframe[3];
	crop[0] = incrop[0];
	crop[1] = incrop[1];
	crop[2] = incrop[2];
	crop[3] = incrop[3];
	g_currfont = fnt;
	g_rtext = text;
	w = (int)(frame[2]-frame[0]);
	h = (int)(frame[3]-frame[1]);
	startline = ln;
	starti = 0;
	gstartx = realstartx;
}

void TextLayer(int offstartx, int offstarty)
{
	int adv;
	
	x = offstartx;
	y = offstarty;
	goffstartx = offstartx;
	line = 0;
	i = starti;
	glyphi = 0;
	nextlb = -1;  //next [i] to break line
	PrepLineBreak();

	for(; g_rtext[i] && line<startline; i+=adv)
	{
		if(i == nextlb)
			NextLineBreak();
		adv = 0;
		ToGlyph(&g_rtext[i], &adv);
		AdvGlyph();
	}

	for(; g_rtext[i] && i<starti; i+=adv)
	{
		if(i == nextlb)
			NextLineBreak();
		adv = 0;
		ToGlyph(&g_rtext[i], &adv);
		AdvGlyph();
	}

	x = offstartx;
	y = offstarty;
	goffstartx = offstartx;
}

void LoadFont(int i, char *fontfile)
{
	Font *f;
	Glyph *g;
	char extfile[128], fullfont[DMD_MAX_PATH+1];
	FILE *fp;
	unsigned int n;
	short pixel[2], texsize[2], offset[2], origsize[2];
	
	f = &g_font[i];
	
	Font_init(f);
	
	for(i=0; i<CODE_POINTS; ++i)
	{
		g = &f->glyph[i];
		Glyph_init(g);
	}
	
	strcpy(extfile, fontfile);
	FindTextureExtension(extfile);

	CreateTex(&f->texin, extfile, ectrue, ecfalse);
	f->width = g_tex[f->texin].width;
	f->height = g_tex[f->texin].height;

	extfile[0] = 0;
	strcat(extfile, fontfile);
	strcat(extfile, ".fnt");
	
	FullPath(extfile, fullfont);
	fp = fopen(fullfont, "rb");
	if(!fp)
	{
		fprintf(g_applog, "Error loading font %s\r\n", font);
		fprintf(g_applog, "Full path: %s", fullfont);
		return;
	}

	fseek(fp, 0, SEEK_END);

	//skip 2 lines
	do
	{
		i = fgetc(fp);
	}while(i!='\n' && i!=EOF);
	do
	{
		i = fgetc(fp);
	}while(i!='\n' && i!=EOF);

	do
	{
		fscanf(fp, "g_mv %h %h %h %h %h %h %h %h %h\r\n",
			   &n,
			   &pixel[0], &pixel[1],
			   &texsize[0], &texsize[1],
			   &offset[0], &offset[1],
			   &origsize[0], &origsize[1]);
		
		g = &f->glyph[n];
		g->pixel[0] = pixel[0];
		g->pixel[1] = pixel[1];
		g->texsize[0] = texsize[0];
		g->texsize[1] = texsize[1];
		g->offset[0] = offset[0];
		g->offset[1] = offset[1];
		g->origsize[0] = origsize[0];
		g->origsize[1] = origsize[1];
		g->texcoord[0] = (float)g->pixel[0] / f->width;
		g->texcoord[1] = (float)g->pixel[1] / f->height;
		g->texcoord[2] = (float)(g->pixel[0]+g->texsize[0]) / f->width;
		g->texcoord[3] = (float)(g->pixel[1]+g->texsize[1]) / f->height;
	}while(!feof(fp));

	f->gheight = f->glyph['A'].origsize[1] + 1;
	
	fclose(fp);
	fprintf(g_applog, "%s.fnt\r\n", fontfile);
}

void DrawGlyph2(float left, float top, float right, float bottom, float texleft, float textop, float texright, float texbottom)
{
	float vertices[4*6], newleft, newtop, newright, newbottom, newtexleft, newtextop, newtexright, newtexbottom;
	Shader *s;
	
	newleft = left;
	newtop = top;
	newright = right;
	newbottom = bottom;
	newtexleft = texleft;
	newtextop = textop;
	newtexright = texright;
	newtexbottom = texbottom;

	if(newleft < crop[0])
	{
		newtexleft = texleft+(crop[0]-newleft)*(texright-texleft)/(right-left);
		newleft = crop[0];
	}
	else if(newleft > crop[2])
		return;

	if(newright < crop[0])
		return;
	else if(newright > crop[2])
	{
		newtexright = texleft+(crop[2]-newleft)*(texright-texleft)/(right-left);
		newright = crop[2];
	}

	if(newtop < crop[1])
	{
		newtextop = textop+(crop[1]-newtop)*(texbottom-textop)/(bottom-top);
		newtop = crop[1];
	}
	else if(newtop > crop[3])
		return;

	if(newbottom < crop[1])
		return;
	else if(newbottom > crop[3])
	{
		newtexbottom = textop+(crop[3]-newtop)*(texbottom-textop)/(bottom-top);
		newbottom = crop[3];
	}

	vertices =
	{
		//posx, posy    texx, texy
		newleft, newtop,          newtexleft, newtextop,
		newright, newtop,         newtexright, newtextop,
		newright, newbottom,      newtexright, newtexbottom,

		newright, newbottom,      newtexright, newtexbottom,
		newleft, newbottom,       newtexleft, newtexbottom,
		newleft, newtop,          newtexleft, newtextop
	};
    
    s = &g_shader[g_curS];

	glVertexPointer(2, GL_FLOAT, sizeof(float)*4, &vertices[0]);
	glTexCoordPointer(2, GL_FLOAT, sizeof(float)*4, &vertices[2]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void HighlGlyph2(float left, float top, float right, float bottom)
{
	float vertices[2*6], newleft, newtop, newright, newbottom;
	Shader *s;
	
	newleft = left;
	newtop = top;
	newright = right;
	newbottom = bottom;

	if(newleft < crop[0])
		newleft = crop[0];
	else if(newleft > crop[2])
		newleft = crop[2];

	if(newright < crop[0])
		newright = crop[0];
	else if(newright > crop[2])
		newright = crop[2];

	if(newtop < crop[1])
		newtop = crop[1];
	else if(newtop > crop[3])
		newtop = crop[3];

	if(newbottom < crop[1])
		newbottom = crop[1];
	else if(newbottom > crop[3])
		newbottom = crop[3];

	vertices =
	{
		//posx, posy
		newleft, newtop,
		newright, newtop,
		newright, newbottom,

		newright, newbottom,
		newleft, newbottom,
		newleft, newtop
	};
    
    s = &g_shader[g_curS];
	
	glVertexPointer(2, GL_FLOAT, 0, &vertices[0]);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void SubDrawTextLine(int caret)
{
	int adv;
	
	if(caret == 0)
		DrawCaret();
	for(; g_rtext[i]; i+=adv)
	{
		if(caret == i)
			DrawCaret();
		DrawGlyph();
		adv = 0;
		ToGlyph(&g_rtext[i], &adv);
		AdvGlyph();
	}
	if(caret == i)
		DrawCaret();
}

void DrawTxLine(int fnt, float *inframe, float *incrop,  char *text, float *color, int caret)
{
	unsigned char c;
	Shader *s;
	
	s = &g_shader[g_curS];
	
	if(color == NULL)
	{
		glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);
		for(c=0; c<4; c++) currcolor[c] = 1;
	}
	else
	{
		glUniform4f(s->slot[SSLOT_COLOR], color[0], color[1], color[2], color[3]);
		for(c=0; c<4; c++) currcolor[c] = color[c];
	}

	StartText(text, fnt, 0, inframe[0], inframe, incrop);
	UseFontTex();
	TextLayer(inframe[0], inframe[1]);
	DrawLine(caret);
}

void DrawShadowedText(int fnt, float *inframe, float *incrop, char *text, const float *color, int caret)
{
	unsigned char c;
	Shader *s;
	
	s = &g_shader[g_curS];
	
	currcolor[0] = 0;
	currcolor[1] = 0;
	currcolor[2] = 0;
	currcolor[3] = color != NULL ? color[3] : 1;
	glUniform4f(s->slot[SSLOT_COLOR], currcolor[0], currcolor[1], currcolor[2], currcolor[3]);

	StartText(text, fnt, 0, inframe[0], inframe, incrop);

	UseFontTex();
	TextLayer(inframe[0]+1, inframe[1]);

	SubDrawTextLine(caret);
	TextLayer(inframe[0], inframe[1]+1);
	SubDrawTextLine(caret);
	TextLayer(inframe[0]+1, inframe[1]+1);
	SubDrawTextLine(caret);

	if(color == NULL)
	{
		glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);
		for(c=0; c<4; c++) currcolor[c] = 1;
	}
	else
	{
		glUniform4f(s->slot[SSLOT_COLOR], color[0], color[1], color[2], color[3]);
		for(c=0; c<4; c++) currcolor[c] = color[c];
	}

	TextLayer(inframe[0], inframe[1]);
	SubDrawTextLine(caret);
	glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);
}

void Highlight(int fnt, float *inframe, float *incrop, char *text, int highlstarti, int highlendi, ecbool multiline)
{
	Shader *s;
	
	EndS();
	UseS(SH_COLOR2D);
	s = &g_shader[g_curS];
	glUniform1f(s->slot[SSLOT_WIDTH], (float)g_currw);
	glUniform1f(s->slot[SSLOT_HEIGHT], (float)g_currh);
	glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 0.5f);

	currcolor[0] = 1;
	currcolor[1] = 1;
	currcolor[2] = 1;
	currcolor[3] = 0.5f;

	StartText(text, fnt, 0, inframe[0], inframe, incrop);
	TextLayer(inframe[0], inframe[1]);

	for(i=0; i<highlstarti; i+=adv)
	{
		if(multiline && i == nextlb)
			NextLineBreak();
		adv = 0;
		ToGlyph(&g_rtext[i], &adv);
		AdvGlyph();
	}
	for(; i<highlendi; i+=adv)
	{
		if(multiline && i == nextlb)
			NextLineBreak();
		HighlGlyph();
		adv = 0;
		ToGlyph(&g_rtext[i], &adv);
		AdvGlyph();
	}

	EndS();
	Ortho(g_currw, g_currh, 1, 1, 1, 1);
}

void DrawCenterShadText(int fnt, float *inframe, float *incrop, char *text, float *color, int caret)
{
	float linew, hscale, a;
	int len, adv;
	Font *f;
	short iconi;
	Icon *icon;
	Shader *s;
	unsigned char c;
 
	s = &g_shader[g_curS];
	linew = 0;
	len = RichText_texlen(text);
	f = &g_font[fnt];
	
	for(i=0; text[i]; i+=adv)
	{
		adv = 0;
		k = ToGlyph(&text[i], &adv);
		if(k <= RICH_GLYPH_END)
		{
			linew += f->glyph[k].origsize[0];
		}
		else if(k >= RICH_ICON_START &&
				k <= RICH_ICON_END)
		{
			iconi = k - RICH_ICON_START;
			icon = &g_icon[iconi];
			hscale = f->gheight / (float)icon->height;
			linew += (float)icon->width * hscale;
		}
	}

	startx -= linew/2;

	a = 1;
	if(color != NULL)
		a = color[3];
	
	currcolor[0] = 0.0f;
	currcolor[1] = 0.0f;
	currcolor[2] = 0.0f;
	currcolor[3] = color != NULL ? color[3] : 1;
	glUniform4f(s->slot[SSLOT_COLOR], currcolor[0], currcolor[1], currcolor[2], currcolor[3]);

	StartText(text, fnt, 0, inframe[0]);
	UseFontTex();
	TextLayer(inframe[0]+1, inframe[1]);
	SubDrawTextLine(caret);
	TextLayer(inframe[0], inframe[1]+1);
	SubDrawTextLine(caret);
	TextLayer(inframe[0]+1, inframe[1]+1);
	SubDrawTextLine(caret);

	if(color == NULL)
	{
		glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);
		for(c=0; c<4; c++) currcolor[c] = 1;
	}
	else
	{
		glUniform4f(s->slot[SSLOT_COLOR], color[0], color[1], color[2], color[3]);
		for(c=0; c<4; c++) currcolor[c] = color[c];
	}

	TextLayer(inframe[0], inframe[1]);
	for(; g_rtext[i]; i+=adv)
	{
		DrawGlyph();
		adv = 0;
		ToGlyph(&g_rtext[i], &adv);
		AdvGlyph();
	}

	glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);
}

void DrawTx(int fnt, float *inframe, float *incrop, char *text, float *color, int ln, int caret, ecbool shadow, ecbool multiline)
{
	Shader *s;
	unsigned char c;
	int adv;
	
	s = &g_shader[g_curS];
	
	glUniform4f(s->slot[SSLOT_COLOR], 0.3f, 0.3f, 0.3f, color ? color[3] : 1);
	currcolor[0] = 0.3f;
	currcolor[1] = 0.3f;
	currcolor[2] = 0.3f;
	currcolor[3] = color != NULL ? color[3] : 1;

	StartText(text, fnt, ln, inframe[0], inframe, incrop);
	UseFontTex();
	
	if(shadow)
	{
		TextLayer(inframe[0]+1, inframe[1]);
		if(caret == 0)
			DrawCaret();
		for(; g_rtext[i]; i+=adv)
		{
			if(multiline && i == nextlb)
				NextLineBreak();
			
			if(caret == i)
				DrawCaret();
			
			DrawGlyph();
			
			adv = 0;
			ToGlyph(&g_rtext[i], &adv);
			
			AdvGlyph();
		}
		if(caret == i)
			DrawCaret();
		
		TextLayer(inframe[0], inframe[1]+1);
		if(caret == 0)
			DrawCaret();
		for(; g_rtext[i]; i+=adv)
		{
			if(multiline && i == nextlb)
				NextLineBreak();
			
			if(caret == i)
				DrawCaret();
			
			DrawGlyph();
			
			adv = 0;
			ToGlyph(&g_rtext[i], &adv);
			
			AdvGlyph();
		}
		if(caret == i)
			DrawCaret();
		
		TextLayer(inframe[0]+1, inframe[1]+1);
		if(caret == 0)
			DrawCaret();
		for(; g_rtext[i]; i+=adv)
		{
			if(multiline && i == nextlb)
				NextLineBreak();
			
			if(caret == i)
				DrawCaret();
			
			DrawGlyph();
			
			adv = 0;
			ToGlyph(&g_rtext[i], &adv);
			
			AdvGlyph();
		}
		if(caret == i)
			DrawCaret();
	}

	if(color == NULL)
	{
		glUniform4f(s->slot[SSLOT_COLOR], 1, 1, 1, 1);
		for(c=0; c<4; c++) currcolor[c] = 1;
	}
	else
	{
		glUniform4f(s->slot[SSLOT_COLOR], color[0], color[1], color[2], color[3]);
		for(c=0; c<4; c++) currcolor[c] = color[c];
	}

	TextLayer(inframe[0], inframe[1]);
	if(caret == 0)
		DrawCaret();
	for(; g_rtext[i]; i+=adv)
	{
		if(multiline && i == nextlb)
			NextLineBreak();

		if(caret == i)
			DrawCaret();

		DrawGlyph();
		
		adv = 0;
		ToGlyph(&g_rtext[i], &adv);
		
		AdvGlyph();
	}
	if(caret == i)
		DrawCaret();
}

int CountLines(char *text, int fnt, float *inframe)
{
	int adv;
	
	StartText(text, fnt, 0, inframe[0], inframe, inframe);
	TextLayer(startx, starty);

	for(i=0; g_rtext[i]; i+=adv)
	{
		if(i == nextlb)
			NextLineBreak();
		
		adv = 0;
		ToGlyph(&g_rtext[i], &adv);

		AdvGlyph();
	}

	return line+1;	//add 1 since line starts count from 0
}

int GetLineStart(char *text, int fnt, float *inframe, int getline)
{
	int adv;
	
	StartText(text, fnt, 0, inframe[0], inframe, inframe);
	TextLayer(startx, starty);

	for(i=0; g_rtext[i]; i+=adv)
	{
		if(i == nextlb)
			NextLineBreak();

		if(line == getline)
			return i;
		
		adv = 0;
		ToGlyph(&g_rtext[i], &adv);

		AdvGlyph();
	}
	
	if(line == getline)
		return i;

	return i;	//return last glyph anyway
}

int EndX(char *text, int lastg, int fnt, float *inframe, ecbool multiline)
{
	int highx, adv;
	
	StartText(text, fnt, 0, inframe[0], inframe, inframe);
	TextLayer(startx, starty);

	highx = (int)startx;

	for(i=0; g_rtext[i] && glyphi<lastg; i+=adv)
	{
		if(multiline && i == nextlb)
			NextLineBreak();
		
		adv = 0;
		ToGlyph(&g_rtext[i], &adv);

		AdvGlyph();

		if(x > highx)
			highx = x;
	}

	return highx;
}

int MatchGlyph(char *text, int fnt, float *inframe, int matchx, int matchy, ecbool multiline)
{
	int lastclose, lastx, lasty, adv;
	Font *f;
	float gw;
	
	lastclose = 0;
	f = &g_font[fnt];

	StartText(text, fnt, 0, inframe[0], inframe, inframe);
	TextLayer(inframe[0], inframe[1]);

	if(x >= matchx ||
	   y >= matchy ||
	   size <= 0)
		return lastclose;

	lastx = x;
	lasty = y;

	for(i=0; g_rtext[i]; i+=adv)
	{
		if(multiline && i == nextlb)
			NextLineBreak();
		
		adv = 0;
		k = ToGlyph(&g_rtext[i], &adv);
		
		if(k <= RICH_GLYPH_END)
		{
			gw = f->glyph[k].origsize[0];
			
			if(matchx >= x+gw/2.0f)
			{
				if(!multiline)
					goto match;
				else if(matchy >= y &&
						matchy <= y+f->gheight)
					goto match;
			}
			
			continue;
			
		match:
			lastclose = i;
			lastx = x;
			lasty = y;
		}
		
		AdvGlyph();
		
		if((float)x >= matchx &&
		   (float)y >= matchy)
			return lastclose;
	}

	return lastclose;
}

int TextWidth(int fnt, float *inframe, const char *text)
{
	return EndX(text, Rich_len(text), fnt, inframe, ecfalse) - inframe[0];
}

void LoadFonts()
{
	LoadFont(FONT_EUROSTILE32, "fonts/eurostile32");
	LoadFont(FONT_MSUIGOTHIC16, "fonts/msuigothic16");
	LoadFont(FONT_MSUIGOTHIC10, "fonts/msuigothic10");
	LoadFont(FONT_SMALLFONTS8, "fonts/smallfonts8");
	LoadFont(FONT_SMALLFONTS10, "fonts/smallfonts10");
	LoadFont(FONT_ARIAL10, "fonts/arial10s");
	LoadFont(FONT_GULIM32, "fonts/gulim32");
	LoadFont(FONT_EUROSTILE16, "fonts/eurostile16");
	LoadFont(FONT_CALIBRILIGHT16, "fonts/calibrilight16s-full");
	//LoadFont(FONT_CALIBRILIGHT16, "fonts/gulim16");
	LoadFont(FONT_TERMINAL10, "fonts/terminal10");
}
