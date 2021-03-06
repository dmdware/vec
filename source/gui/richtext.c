/*
 /*
 * Copyright (c) 2007 Alexey Vatchenko <av@bsdua.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * utf8: implementation of UTF-8 charset encoding (RFC3629).
 */

#include "richtext.h"
#include "icon.h"
#include "../utils.h"

//replaced wchar_t with unsigned int
//replaced u_char with unsigned char
//replaced size_t with int
//renamed .c to .cpp to get rid of xtgmath.h and cstdio errors

#define _NXT	0x80
#define _SEQ2	0xc0
#define _SEQ3	0xe0
#define _SEQ4	0xf0
#define _SEQ5	0xf8
#define _SEQ6	0xfc

#define _BOM	0xfeff

static int __wchar_forbitten(unsigned int sym);
static int __utf8_forbitten(unsigned char octet);

static int
__wchar_forbitten(unsigned int sym)
{

	/*Surrogate pairs */
	if (sym >= 0xd800 && sym <= 0xdfff)
		return (-1);

	return (0);
}

static int
__utf8_forbitten(unsigned char octet)
{

	switch (octet) {
	case 0xc0:
	case 0xc1:
	case 0xf5:
	case 0xff:
		return (-1);
	}

	return (0);
}

/*
 * DESCRIPTION
 *	This function translates UTF-8 string into UCS-4 string (all symbols
 *	will be in local machine byte order).
 *
 *	It takes the following arguments:
 *	in	- input UTF-8 string. It can be null-terminated.
 *	insize	- size of input string in bytes.
 *	out	- result buffer for UCS-4 string. If out is NULL,
 *		function returns size of result buffer.
 *	outsize - size of out buffer in wide characters.
 *
 * RETURN VALUES
 *	The function returns size of result buffer (in wide characters).
 *	Zero is returned in case of error.
 *
 * CAVEATS
 *	1. If UTF-8 string contains zero symbols, they will be translated
 *	   as regular symbols.
 *	2. If UTF8_IGNORE_ERROR or UTF8_SKIP_BOM flag is set, sizes may vary
 *	   when `out' is NULL and not NULL. It's because of special UTF-8
 *	   sequences which may result in forbitten (by RFC3629) UNICODE
 *	   characters.  So, the caller must check return value every time and
 *	   not prepare buffer in advance (\0 terminate) but after calling this
 *	   function.
 */
int
utf8_to_wchar(const char *in, int insize, unsigned int *out, int outsize,
    int flags)
{
	unsigned char *p, *lim;
	unsigned int *wlim, high;
	int n, total, i, n_bits;

	if (in == NULL || insize == 0 || (outsize == 0 && out != NULL))
		return (0);

	total = 0;
	p = (unsigned char *)in;
	lim = p + insize;
	wlim = out + outsize;

	for (; p < lim; p += n) {
		if (__utf8_forbitten(*p) != 0 &&
		    (flags & UTF8_IGNORE_ERROR) == 0)
			return (0);

		/*
		 * Get number of bytes for one wide character.
		 */
		n = 1;	/*default: 1 byte. Used when skipping bytes. */
		if ((*p & 0x80) == 0)
			high = (unsigned int)*p;
		else if ((*p & 0xe0) == _SEQ2) {
			n = 2;
			high = (unsigned int)(*p & 0x1f);
		} else if ((*p & 0xf0) == _SEQ3) {
			n = 3;
			high = (unsigned int)(*p & 0x0f);
		} else if ((*p & 0xf8) == _SEQ4) {
			n = 4;
			high = (unsigned int)(*p & 0x07);
		} else if ((*p & 0xfc) == _SEQ5) {
			n = 5;
			high = (unsigned int)(*p & 0x03);
		} else if ((*p & 0xfe) == _SEQ6) {
			n = 6;
			high = (unsigned int)(*p & 0x01);
		} else {
			if ((flags & UTF8_IGNORE_ERROR) == 0)
				return (0);
			continue;
		}

		/*does the sequence header tell us truth about length? */
		if (lim - p <= n - 1) {
			if ((flags & UTF8_IGNORE_ERROR) == 0)
				return (0);
			n = 1;
			continue;	/*skip */
		}

		/*
		 * Validate sequence.
		 * All symbols must have higher bits set to 10xxxxxx
		 */
		if (n > 1) {
			for (i = 1; i < n; i++) {
				if ((p[i] & 0xc0) != _NXT)
					break;
			}
			if (i != n) {
				if ((flags & UTF8_IGNORE_ERROR) == 0)
					return (0);
				n = 1;
				continue;	/*skip */
			}
		}

		total++;

		if (out == NULL)
			continue;

		if (out >= wlim)
			return (0);		/*no space left */

		*out = 0;
		n_bits = 0;
		for (i = 1; i < n; i++) {
			*out |= (unsigned int)(p[n - i] & 0x3f) << n_bits;
			n_bits += 6;		/*6 low bits in every byte */
		}
		*out |= high << n_bits;

		if (__wchar_forbitten(*out) != 0) {
			if ((flags & UTF8_IGNORE_ERROR) == 0)
				return (0);	/*forbitten character */
			else {
				total--;
				out--;
			}
		} else if (*out == _BOM && (flags & UTF8_SKIP_BOM) != 0) {
			total--;
			out--;
		}

		out++;
	}

	return (total);
}

/*
 * DESCRIPTION
 *	This function translates UCS-4 symbols (given in local machine
 *	byte order) into UTF-8 string.
 *
 *	It takes the following arguments:
 *	in	- input unicode string. It can be null-terminated.
 *	insize	- size of input string in wide characters.
 *	out	- result buffer for utf8 string. If out is NULL,
 *		function returns size of result buffer.
 *	outsize - size of result buffer.
 *
 * RETURN VALUES
 *	The function returns size of result buffer (in bytes). Zero is returned
 *	in case of error.
 *
 * CAVEATS
 *	If UCS-4 string contains zero symbols, they will be translated
 *	as regular symbols.
 */
int
wchar_to_utf8(const unsigned int *in, int insize, char *out, int outsize,
    int flags)
{
	unsigned int *w, *wlim, ch;
	unsigned char *p, *lim, *oc;
	int total, n;

	if (in == NULL || insize == 0 || (outsize == 0 && out != NULL))
		return (0);

	w = (unsigned int *)in;
	wlim = w + insize;
	p = (unsigned char *)out;
	lim = p + outsize;
	total = 0;
	for (; w < wlim; w++) {
		if (__wchar_forbitten(*w) != 0) {
			if ((flags & UTF8_IGNORE_ERROR) == 0)
				return (0);
			else
				continue;
		}

		if (*w == _BOM && (flags & UTF8_SKIP_BOM) != 0)
			continue;

		if (*w < 0) {
			if ((flags & UTF8_IGNORE_ERROR) == 0)
				return (0);
			continue;
		} else if (*w <= 0x0000007f)
			n = 1;
		else if (*w <= 0x000007ff)
			n = 2;
		else if (*w <= 0x0000ffff)
			n = 3;
		else if (*w <= 0x001fffff)
			n = 4;
		else if (*w <= 0x03ffffff)
			n = 5;
		else /*if (*w <= 0x7fffffff) */
			n = 6;

		total += n;

		if (out == NULL)
			continue;

		if (lim - p <= n - 1)
			return (0);		/*no space left */

		/*make it work under different endians */
		ch = htonl(*w);
		oc = (unsigned char *)&ch;
		switch (n) {
		case 1:
			*p = oc[3];
			break;

		case 2:
			p[1] = _NXT | oc[3] & 0x3f;
			p[0] = _SEQ2 | (oc[3] >> 6) | ((oc[2] & 0x07) << 2);
			break;

		case 3:
			p[2] = _NXT | oc[3] & 0x3f;
			p[1] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[0] = _SEQ3 | ((oc[2] & 0xf0) >> 4);
			break;

		case 4:
			p[3] = _NXT | oc[3] & 0x3f;
			p[2] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[1] = _NXT | ((oc[2] & 0xf0) >> 4) |
			    ((oc[1] & 0x03) << 4);
			p[0] = _SEQ4 | ((oc[1] & 0x1f) >> 2);
			break;

		case 5:
			p[4] = _NXT | oc[3] & 0x3f;
			p[3] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[2] = _NXT | ((oc[2] & 0xf0) >> 4) |
			    ((oc[1] & 0x03) << 4);
			p[1] = _NXT | (oc[1] >> 2);
			p[0] = _SEQ5 | oc[0] & 0x03;
			break;

		case 6:
			p[5] = _NXT | oc[3] & 0x3f;
			p[4] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[3] = _NXT | (oc[2] >> 4) | ((oc[1] & 0x03) << 4);
			p[2] = _NXT | (oc[1] >> 2);
			p[1] = _NXT | oc[0] & 0x3f;
			p[0] = _SEQ6 | ((oc[0] & 0x40) >> 6);
			break;
		}

		/*
		 * NOTE: do not check here for forbitten UTF-8 characters.
		 * They cannot appear here because we do proper convertion.
		 */

		p += n;
	}

	return (total);
}


//edit

static char temputf8[7];

char *FromGlyph(const unsigned int in, int *adv)
{
	const unsigned int *w = &in;
	unsigned int ch;
	unsigned char *p, *oc;
	int n;
	
	*adv = 0;
	
	p = (unsigned char *)temputf8;
	
	if (in == _BOM /*&& (flags & UTF8_SKIP_BOM) != 0 */ )
	{
		p[0] = 0;
		return temputf8;
	}
	
	if (in < 0) {
		/*if ((flags & UTF8_IGNORE_ERROR) == 0)
			return (0); */
		p[0] = 0;
		return temputf8;
	} else if (*w <= 0x0000007f)
		n = 1;
	else if (*w <= 0x000007ff)
		n = 2;
	else if (*w <= 0x0000ffff)
		n = 3;
	else if (*w <= 0x001fffff)
		n = 4;
	else if (*w <= 0x03ffffff)
		n = 5;
	else /*if (*w <= 0x7fffffff) */
		n = 6;
	
	/*make it work under different endians */
	ch = htonl(in);
	oc = (unsigned char *)&ch;
	switch (n) {
		case 1:
			p[1] = 0;
			p[0] = oc[3];
			break;
			
		case 2:
			p[2] = 0;
			p[1] = _NXT | oc[3] & 0x3f;
			p[0] = _SEQ2 | (oc[3] >> 6) | ((oc[2] & 0x07) << 2);
			break;
			
		case 3:
			p[3] = 0;
			p[2] = _NXT | oc[3] & 0x3f;
			p[1] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[0] = _SEQ3 | ((oc[2] & 0xf0) >> 4);
			break;
			
		case 4:
			p[4] = 0;
			p[3] = _NXT | oc[3] & 0x3f;
			p[2] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[1] = _NXT | ((oc[2] & 0xf0) >> 4) |
			((oc[1] & 0x03) << 4);
			p[0] = _SEQ4 | ((oc[1] & 0x1f) >> 2);
			break;
			
		case 5:
			p[5] = 0;
			p[4] = _NXT | oc[3] & 0x3f;
			p[3] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[2] = _NXT | ((oc[2] & 0xf0) >> 4) |
			((oc[1] & 0x03) << 4);
			p[1] = _NXT | (oc[1] >> 2);
			p[0] = _SEQ5 | oc[0] & 0x03;
			break;
			
		case 6:
			p[6] = 0;
			p[5] = _NXT | oc[3] & 0x3f;
			p[4] = _NXT | (oc[3] >> 6) | ((oc[2] & 0x0f) << 2);
			p[3] = _NXT | (oc[2] >> 4) | ((oc[1] & 0x03) << 4);
			p[2] = _NXT | (oc[1] >> 2);
			p[1] = _NXT | oc[0] & 0x3f;
			p[0] = _SEQ6 | ((oc[0] & 0x40) >> 6);
			break;
	}
	
	*adv = n;

	return temputf8;
}

unsigned int ToGlyph(const char *in, int *index)
{
	unsigned char *p;//, *lim;
	unsigned int high;
	int n, 
		//total, 
		i, n_bits;
	
	unsigned int out;
	
	if (in == NULL || *in == 0)
		return 0;
	
	p = (unsigned char *)in;
	
	/*
	 * Get number of bytes for one wide character.
	 */
	n = 1;	/*default: 1 byte. Used when skipping bytes. */
	if ((*p & 0x80) == 0)
		high = (unsigned int)*p;
	else if ((*p & 0xe0) == _SEQ2) {
		n = 2;
		high = (unsigned int)(*p & 0x1f);
	} else if ((*p & 0xf0) == _SEQ3) {
		n = 3;
		high = (unsigned int)(*p & 0x0f);
	} else if ((*p & 0xf8) == _SEQ4) {
		n = 4;
		high = (unsigned int)(*p & 0x07);
	} else if ((*p & 0xfc) == _SEQ5) {
		n = 5;
		high = (unsigned int)(*p & 0x03);
	} else if ((*p & 0xfe) == _SEQ6) {
		n = 6;
		high = (unsigned int)(*p & 0x01);
	} else {
		return 0;
	}
	
	out = 0;
	n_bits = 0;
	for (i = 1; i < n; i++) {
		out |= (unsigned int)(p[n - i] & 0x3f) << n_bits;
		n_bits += 6;		/*6 low bits in every byte */
	}
	out |= high << n_bits;
	
	if(index)
		(*index) += n;
	
	return out;
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
	len = Rich_len(src);
	*ndest = (char*)malloc(len+1);
	(*ndest)[len] = 0;
	for(--len; len>=0; --len)
		(*ndest)[len] = '*';
}


int Rich_len(const char *in)
{
	unsigned int out;
	unsigned char *p, *lim;
	unsigned int //*wlim, 
		high;
	int n, total, i, n_bits;
	int insize;
	int outsize;

	insize = strlen(in);
	outsize = insize;

	if (in == NULL)
		return (0);

	total = 0;
	p = (unsigned char *)in;
	lim = p + insize;
	//wlim = out + outsize;

	for (; p < lim; p += n) {
		//if (__utf8_forbitten(*p) != 0 &&
		  //  (flags & UTF8_IGNORE_ERROR) == 0)
			//return (0);

		/*
		 * Get number of bytes for one wide character.
		 */
		n = 1;	/*default: 1 byte. Used when skipping bytes. */
		if ((*p & 0x80) == 0)
			high = (unsigned int)*p;
		else if ((*p & 0xe0) == _SEQ2) {
			n = 2;
			high = (unsigned int)(*p & 0x1f);
		} else if ((*p & 0xf0) == _SEQ3) {
			n = 3;
			high = (unsigned int)(*p & 0x0f);
		} else if ((*p & 0xf8) == _SEQ4) {
			n = 4;
			high = (unsigned int)(*p & 0x07);
		} else if ((*p & 0xfc) == _SEQ5) {
			n = 5;
			high = (unsigned int)(*p & 0x03);
		} else if ((*p & 0xfe) == _SEQ6) {
			n = 6;
			high = (unsigned int)(*p & 0x01);
		} else {
			//if ((flags & UTF8_IGNORE_ERROR) == 0)
			//	return (0);
			continue;
		}

		/*does the sequence header tell us truth about length? */
		if (lim - p <= n - 1) {
			//if ((flags & UTF8_IGNORE_ERROR) == 0)
			//	return (0);
			n = 1;
			continue;	/*skip */
		}

		/*
		 * Validate sequence.
		 * All symbols must have higher bits set to 10xxxxxx
		 */
		if (n > 1) {
			for (i = 1; i < n; i++) {
				if ((p[i] & 0xc0) != _NXT)
					break;
			}
			if (i != n) {
	//			if ((flags & UTF8_IGNORE_ERROR) == 0)
	//				return (0);
				n = 1;
				continue;	/*skip */
			}
		}

		total++;

		//if (out == NULL)
		//	continue;

		//if (out >= wlim)
		//	return (0);		/*no space left */

		out = 0;
		n_bits = 0;
		for (i = 1; i < n; i++) {
			out |= (unsigned int)(p[n - i] & 0x3f) << n_bits;
			n_bits += 6;		/*6 low bits in every byte */
		}
		out |= high << n_bits;

		if (__wchar_forbitten(out) != 0) {
	//		if ((flags & UTF8_IGNORE_ERROR) == 0)
	//			return (0);	/*forbitten character */
	//		else 
			{
				total--;
				//out--;
			}
		}
		//else if (out == _BOM && (flags & UTF8_SKIP_BOM) != 0) 
		else
		{
			total--;
			//out--;
		}

		//out++;
	}

	return (total);
}


/* only use with dynamic array string */

void pstrset(char **out, const char *in)
{
	int len = strlen(in);
	
	*out = (char*)malloc(len+1);
	memcpy(*out, in, len+1);
}

void pstradd(char **out, const char *in)
{
	int addlen = strlen(in);
	int len = 0;
	
	len = strlen(*out);
	*out = (char*)realloc(*out, len + addlen + 1);
	
	memcpy(&(*out)[len], in, addlen + 1);
}

void psubstr(char **out, const char *in, int beg, int len)
{
	*out = (char*)malloc(len + 1);
	memcpy(*out, &in[beg], len);
	(*out)[len] = 0;
}

void delprev(char **s, int *caret)
{
	int glyphi, ci, adv;
	unsigned int k;
	char *sub1, *sub2;
	
	for(glyphi=0, ci=0; (*s)[ci]; ci+=adv, glyphi++)
	{
		adv = 0;
		k = ToGlyph(&(*s)[ci], &adv);
		
		if(ci > 0 && adv + ci >= *caret)
		{
			psubstr(&sub1, *s, 0, ci);
			psubstr(&sub2, *s, adv+ci, strlen(*s)-adv-ci);
			free(*s);
			pstrset(s, sub1);
			pstradd(s, sub2);
			free(sub1);
			free(sub2);
			(*caret) -= (*caret)-ci;
			return;
		}
	}
}

void delnext(char **s, int *caret)
{
	int glyphi, ci, adv;
	unsigned int k;
	char *sub1, *sub2;
	
	for(glyphi=0, ci=0; (*s)[ci]; ci+=adv, glyphi++)
	{
		adv = 0;
		k = ToGlyph(&(*s)[ci], &adv);
		
		if(adv + ci > *caret)
		{
			psubstr(&sub1, *s, 0, ci);
			psubstr(&sub2, *s, adv+ci, strlen(*s)-adv-ci);
			free(*s);
			pstrset(s, sub1);
			pstradd(s, sub2);
			free(sub1);
			free(sub2);
			(*caret) -= (*caret)-ci;
			return;
		}
	}
}

int prevlen(char *s, int caret)
{
	int glyphi, ci, adv;
	unsigned int k;
	
	for(glyphi=0, ci=0; s[ci]; ci+=adv, glyphi++)
	{
		adv = 0;
		k = ToGlyph(&s[ci], &adv);
		
		if(ci > 0 && adv + ci >= caret)
			return adv;
	}
	
	return 0;
}

int nextlen(char *s, int caret)
{
	int glyphi, ci, adv;
	unsigned int k;
	
	for(glyphi=0, ci=0; s[ci]; ci+=adv, glyphi++)
	{
		adv = 0;
		k = ToGlyph(&s[ci], &adv);
		
		if(adv + ci > caret)
			return adv;
	}
	
	return 0;
}