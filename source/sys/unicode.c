#include "unicode.h"
#include "../utils.h"
#include "utf8.h"


#define ERR(x) {g_applog<<x<<std::endl; g_applog.flush(); exit(1);}

int ValidateUTF8(char unsigned *buff, int len)
{
	int x;
	for (x = 0; x < len; x++)
	{
		if ((unsigned char)(buff[x]) > 0xfd)
		{
			char msg[128];
			sprintf(msg, "Byte %i is invalid\n", x);
			InfoMess(msg, msg);
			return 0;
		}
	}
	return 1;
}

int UTF8Len(unsigned char ch)
{
	int l;
	unsigned char c = ch;
	c >>= 3;
	// 6 => 0x7e
	// 5 => 0x3e
	if (c == 0x1e)
	{
		l = 4;
	}
	else
	{
		c >>= 1;
		if (c == 0xe)
		{
			l = 3;
		}
		else
		{
			c >>= 1;
			if (c == 0x6)
			{
				l = 2;
			}
			else
			{
				l = 1;
			}
		}
	}
	return l;
}

int UTF32Len(const unsigned int *ustr)
{
	int i=0;

	for(; ustr[i]; i++)
		;

	return i;
}

unsigned char *ToUTF8(const unsigned int *unicode)
{
	int utf32len = UTF32Len(unicode);
	int utf8len = utf32len * 4;
	unsigned char *utf8 = new unsigned char [ utf8len + 1 ];
	utf8len = wchar_to_utf8(unicode, utf32len, (char*)utf8, utf8len, 0);
	utf8[utf8len] = 0;
	return utf8;
}

unsigned int *ToUTF32(const unsigned char *utf8)
{
	int utf8len = strlen((char*)utf8);
	unsigned int *utf32 = new unsigned int [ utf8len + 1 ];
	int utf32len = utf8_to_wchar((char*)utf8, utf8len, utf32, utf8len, 0);
	utf32[utf32len] = 0;
	return utf32;
}

static char temputf8[7];

char *FromGlyph(const unsigned int in)
{
	unsigned int ch;
	unsigned char *p, *oc;
	int n;
	
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
	
	return temputf8;
}

unsigned int ToGlyph(const char *in, int *index)
{
	unsigned char *p, *lim;
	unsigned int high;
	int n, total, i, n_bits;
	
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

