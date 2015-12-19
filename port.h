/*
 * port.h
 * kirk johnson
 * september 1995
 *
 * RCS $Id: port.h,v 1.2 1995/09/24 00:45:47 tuna Exp $
 *
 * Copyright (C) 1989, 1990, 1993, 1994, 1995 Kirk Lauritz Johnson
 *
 * Parts of the source code (as marked) are:
 *   Copyright (C) 1989, 1990, 1991 by Jim Frost
 *   Copyright (C) 1992 by Jamie Zawinski <jwz@lucid.com>
 *
 * Permission to use, copy, modify and freely distribute xearth for
 * non-commercial and not-for-profit purposes is hereby granted
 * without fee, provided that both the above copyright notice and this
 * permission notice appear in all copies and in supporting
 * documentation.
 *
 * Unisys Corporation holds worldwide patent rights on the Lempel Zev
 * Welch (LZW) compression technique employed in the CompuServe GIF
 * image file format as well as in other formats. Unisys has made it
 * clear, however, that it does not require licensing or fees to be
 * paid for freely distributed, non-commercial applications (such as
 * xearth) that employ LZW/GIF technology. Those wishing further
 * information about licensing the LZW patent should contact Unisys
 * directly at (lzw_info@unisys.com) or by writing to
 *
 *   Unisys Corporation
 *   Welch Licensing Department
 *   M/S-C1SW19
 *   P.O. Box 500
 *   Blue Bell, PA 19424
 *
 * The author makes no representations about the suitability of this
 * software for any purpose. It is provided "as is" without express or
 * implied warranty.
 *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _PORT_H_
#define _PORT_H_

/* Allow prototypes throughout */
//#if __STDC__
//#define _P(x) x
//#else
//#ifdef __cplusplus
//#define _P(x) x
//#else
//#define _P(x) ()
//#endif
//#endif

/* Allow use of "const" */
//#if !defined(__GNUC__)
//#ifndef const
//#define const
//#endif
//#endif

/* Allow use of GNU's __attribute__((noreturn)) */
#if __GNUC__ > 1
#define _noreturn __attribute__((noreturn))
#else
#define _noreturn
#endif

/* typedefs and defines for Win32 systems */
#ifdef WIN32
#include <process.h>
typedef unsigned char u_char;
typedef unsigned short u_short;
#define srandom(x) srand(x)
#define random() rand()
#define setpriority(a,b,c) (0)
#endif /* WIN32 */

#endif
