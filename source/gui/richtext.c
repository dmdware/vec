#include "richtext.h"
#include "icon.h"
#include "../utils.h"


void Rich_copy(unsigned int **ndest, unsigned int *src)
{
	int len;
	len = UTF32Len(src) + 1;
	*ndest = (unsigned int*)malloc(sizeof(unsigned int)  *len);
	memcpy(*ndest, src, sizeof(unsigned int)  *len);
}

void Rich_init_i(unsigned int **rich, short i)
{
	*rich = (unsigned int*)malloc(sizeof(unsigned int)  *2);
	(*rich)[0] = RICH_ICON_START + i;
	(*rich)[1] = 0;
}

void Rich_substr(unsigned int **ndest, unsigned int *src, int start, int length)
{
	*ndest = (unsigned int*)malloc(sizeof(unsigned int*)  *(length+1));
	memcpy(*ndest, &src[start], length+1);
	
	//TODO blocks control fonts,colors
}

void Rich_add(unsigned int **ndest, unsigned int *src)
{
	int len1, len2;
	len1 = UTF32Len(*ndest);
	len2 = UTF32Len(src);
	*ndest = (unsigned int*)realloc(*ndest, sizeof(unsigned int)  *(len1 + len2 + 1));
	memcpy(&(*ndest)[len1], src, len2 + 1);
}

//icons counted as 1 chars, control blocks 0, attached to beginning of text they influence and repeated for following text blocks
void Rich_substr(unsigned int **ndest, unsigned int *src, int start, int length)
{
	int i;
	*ndest = (unsigned int*)malloc(sizeof(unsigned int)  *(length + 1));
	for(i=start; i<start+length; ++i)
		(*ndest)[i-start] = src[i];
	(*ndest)[i-start] = 0;
}

void ParseTags(char **ndest, char *src, int *caret)
{
	short icon;
	int len, taglen, srci, desti, iconi, ci, adv;
	ecbool match;
	char *tag, *iconc, *tempdest, tempc[2];
	unsigned int k;
	
	len = strlen(src);
	pstrset(&tempdest, "");
	
	for(srci=0, desti=0; srci<len; )
	{
		for(icon=0; icon<ICONS; ++icon)
		{
			match = ectrue;
			tag = g_icon[icon].tag;
			taglen = strlen(tag);
			if(!taglen)
				continue;
			for(iconi=0; iconi<taglen && iconi+srci<len; ++iconi)
			{
				if(tag[iconi] == src[iconi+srci])
					continue;
				match = ecfalse;
				break;
			}
			if(!match)
				continue;
			k = RICH_ICON_START + icon;
			iconc = FromGlyph(k, &adv);
			pstradd(&tempdest, iconc);
			if(caret && *caret > ci)
				(*caret) -= imin(taglen,*caret-ci);
			srci+=taglen;
			ci+=taglen;
			desti+=strlen(iconc);
			break;
		}
		
		if(match)
			continue;
		
		tempc[0] = src[srci];
		tempc[1] = 0;
		pstradd(&tempdest, tempc);
		++srci;
		//++glyphi;
		++desti;
	}
	
	free(*ndest);
	*ndest = tempdest;
}

void pwver(char **ndest, char *src)	//asterisk-mask password string
{
	int len;
	unsigned int *utf32;
	utf32 = ToUTF32(src);
	len = UTF32Len(utf32);
	free(utf32);
	*ndest = (char*)malloc(len+1);
	(*ndest)[len] = 0;
	for(--len; len>=0; --len)
		(*ndest)[len] = '*';
}
