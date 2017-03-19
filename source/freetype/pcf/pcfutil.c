/*

Copyright 1990, 1994, 1998  The Open Group

Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
OPEN GROUP BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of The Open Group shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from The Open Group.

*/
/* $XFree86: xc/lib/font/util/utilbitmap.c,v 1.3 1999/08/22 08:58:58 dawes Exp $ */

/*
 * Author:  Keith Packard, MIT X Consortium
 */

/* Modified for use with FreeType */


#include "../ft2build.h"
#include "pcfutil.h"


  /*
   *  Invert bit order within each BYTE of an array.
   */

  FT_LOCAL_DEF( void )
  BitOrderInvert( unsigned char*  buf,
                  size_t          nbytes )
  {
    for ( ; nbytes > 0; nbytes--, buf++ )
    {
      unsigned int  val = *buf;


      val = ( ( val >> 1 ) & 0x55 ) | ( ( val << 1 ) & 0xAA );
      val = ( ( val >> 2 ) & 0x33 ) | ( ( val << 2 ) & 0xCC );
      val = ( ( val >> 4 ) & 0x0F ) | ( ( val << 4 ) & 0xF0 );

      *buf = (unsigned char)val;
    }
  }


  /*
   *  Invert byte order within each 16-bits of an array.
   */

  FT_LOCAL_DEF( void )
  TwoByteSwap( unsigned char*  buf,
               size_t          nbytes )
  {
    unsigned char  c;


    for ( ; nbytes >= 2; nbytes -= 2, buf += 2 )
    {
      c      = buf[0];
      buf[0] = buf[1];
      buf[1] = c;
    }
  }

  /*
   *  Invert byte order within each 32-bits of an array.
   */

  FT_LOCAL_DEF( void )
  FourByteSwap( unsigned char*  buf,
                size_t          nbytes )
  {
    unsigned char  c;


    for ( ; nbytes >= 4; nbytes -= 4, buf += 4 )
    {
      c      = buf[0];
      buf[0] = buf[3];
      buf[3] = c;

      c      = buf[1];
      buf[1] = buf[2];
      buf[2] = c;
    }
  }


/* END */
