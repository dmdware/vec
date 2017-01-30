#include "richtext.h"
#include "icon.h"
#include "../utils.h"
#include "../sys/utf8.h"

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


int Rich_len(char *in)
{
	unsigned int out;
	unsigned char *p, *lim;
	unsigned int *wlim, high;
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
			if ((flags & UTF8_IGNORE_ERROR) == 0)
				return (0);	/*forbitten character */
			else {
				total--;
				//out--;
			}
		} else if (out == _BOM && (flags & UTF8_SKIP_BOM) != 0) {
			total--;
			//out--;
		}

		//out++;
	}

	return (total);
}