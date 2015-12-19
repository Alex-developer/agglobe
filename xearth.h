/*
 * xearth.h
 * kirk johnson
 * july 1993
 *
 * RCS $Id: xearth.h,v 1.28 1995/09/29 18:12:30 tuna Exp $
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

#ifndef _XEARTH_H_
#define _XEARTH_H_

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <X11/Xos.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <setjmp.h>
#include "port.h"
#include "extarr.h"
#include "kljcpyrt.h"

#define VersionString "1.0"

/* if NO_RANDOM is defined, use lrand48() and srand48() 
 * instead of random() and srandom()
 */
#ifdef NO_RANDOM
#define random()   lrand48()
#define srandom(x) srand48((long) (x))
#endif /* NO_RANDOM */

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif /* !M_PI */

/* a particularly large number
 */
#define BigNumber (1e6)

/* rotational period of the earth (seconds)
 */
#define EarthPeriod (86400)

/* types of pixels
 */
#define PixTypeSpace     (0)
#define PixTypeLand      (1)
#define PixTypeWater     (2)
#define PixTypeStar      (3)
#define PixTypeGridLand  (4)
#define PixTypeGridWater (5)

/* types of dots
 */
#define DotTypeStar (0)
#define DotTypeGrid (1)

/* types of viewing positions
 */
#define ViewPosTypeFixed  (0)
#define ViewPosTypeSun    (1)
#define ViewPosTypeOrbit  (2)
#define ViewPosTypeRandom (3)

/* types of projections
 */
#define ProjTypeOrthographic (0)
#define ProjTypeMercator     (1)

/* types of marker label alignment
 */
#define MarkerAlignDefault (0)
#define MarkerAlignLeft    (1)
#define MarkerAlignRight   (2)
#define MarkerAlignAbove   (3)
#define MarkerAlignBelow   (4)

/* vector rotation
 */
#define XFORM_ROTATE(p,vpi)             \
 do {                                   \
  double _p0_, _p1_, _p2_;              \
  double _c_, _s_, _t_;                 \
  _p0_ = p[0];                          \
  _p1_ = p[1];                          \
  _p2_ = p[2];                          \
  _c_  = vpi.cos_lon;                   \
  _s_  = vpi.sin_lon;                   \
  _t_  = (_c_ * _p0_) - (_s_ * _p2_);   \
  _p2_ = (_s_ * _p0_) + (_c_ * _p2_);   \
  _p0_ = _t_;                           \
  _c_  = vpi.cos_lat;                   \
  _s_  = vpi.sin_lat;                   \
  _t_  = (_c_ * _p1_) - (_s_ * _p2_);   \
  _p2_ = (_s_ * _p1_) + (_c_ * _p2_);   \
  _p1_ = _t_;                           \
  _c_  = vpi.cos_rot;                   \
  _s_  = vpi.sin_rot;                   \
  _t_  = (_c_ * _p0_) - (_s_ * _p1_);   \
  _p1_ = (_s_ * _p0_) + (_c_ * _p1_);   \
  _p0_ = _t_;                           \
  p[0] = _p0_;                          \
  p[1] = _p1_;                          \
  p[2] = _p2_;                          \
 } while (0)

/* mercator projection (xyz->xy)
 */
#define MERCATOR_X(x, z)  (atan2((x), (z)))
#define MERCATOR_Y(y)     (((y) >= 1) ? (BigNumber)      \
			   : (((y) <= -1) ? (-BigNumber) \
			      : log(tan((asin(y)/2) + (M_PI/4)))))
#define INV_MERCATOR_Y(y) (sin(2 * (atan(exp(y)) - (M_PI/4))))

/* xy->screen projections
 */
#define XPROJECT(x)     ((proj_scale*(x))+proj_xofs)
#define YPROJECT(y)     (proj_yofs-(proj_scale*(y)))
#define INV_XPROJECT(x) (((x)-proj_xofs)*inv_proj_scale)
#define INV_YPROJECT(y) ((proj_yofs-(y))*inv_proj_scale)

#ifdef __alpha
/* on alpha systems, trade off space for more efficient access
 */
typedef int      s8or32;
typedef unsigned u8or32;
typedef int      s16or32;
typedef unsigned u16or32;
#else
/* other systems can access 8- and 16-bit items efficiently
 */
typedef char           s8or32;
typedef unsigned char  u8or32;
typedef short          s16or32;
typedef unsigned short u16or32;
#endif

typedef struct
{
  double cos_lat, sin_lat;	/* cos/sin of view_lat */
  double cos_lon, sin_lon;	/* cos/sin of view_lon */
  double cos_rot, sin_rot;	/* cos/sin of view_rot */
} ViewPosInfo;

typedef struct
{
  short y;
  short lo_x;
  short hi_x;
  short val;
} ScanBit;

typedef struct
{
  short  x;
  short  y;
  u_char type;
} ScanDot;

typedef struct
{
  float lat;
  float lon;
  char *label;
  int   align;
  COLORREF	lColour;
} MarkerInfo;

/* bmp.c */
extern void bmp_output(void);

/* dither.c */
extern int     dither_ncolors;
extern u_char *dither_colormap;
extern void    dither_setup(int);
extern void    dither_row(u_char *, u16or32 *);
extern void    dither_cleanup(void);
extern void    mono_dither_setup(void);
extern void    mono_dither_row(u_char *, u16or32 *);
extern void    mono_dither_cleanup(void);

/* gif.c */
extern void gif_output(void);

/* mapdata.c */
extern short map_data[];

/* markers.c */
extern MarkerInfo *marker_info;
extern void        load_marker_info(char *);
extern void        show_marker_info(char *);

/* ppm.c */
extern void ppm_output(void);

/* render.c */
extern void render(void (*)(u_char *));
extern void do_dots(void);

/* resources.c */
extern char        *get_string_resource(const char *, const char *);
extern int          get_boolean_resource(const char *, const char *);
extern int          get_integer_resource(const char *, const char *);
extern double       get_float_resource(const char *, const char *);
extern unsigned int get_pixel_resource(const char *, const char *);

/* scan.c */
extern ViewPosInfo view_pos_info;
extern double      proj_scale;
extern double      proj_xofs;
extern double      proj_yofs;
extern double      inv_proj_scale;
extern ExtArr      scanbits;
extern void        scan_map(void);

/* sunpos.c */
extern void sun_position(time_t, double *, double *);

/* x11.c */
extern void command_line_x(int, char *[]);
extern void x11_output(void);

/* xearth.c */
extern char  *progname;
extern int    proj_type;
extern double view_lon;
extern double view_lat;
extern double view_rot;
extern double view_mag;
extern int    do_shade;
extern double sun_lon;
extern double sun_lat;
extern int    wdth;
extern int    hght;
extern int    shift_x;
extern int    shift_y;
extern int    do_stars;
extern double star_freq;
extern int    big_stars;
extern int    do_grid;
extern int    grid_big;
extern int    grid_small;
extern int    do_label;
extern int    labelpos;
extern int    do_markers;
extern char  *markerfile;
extern int    wait_time;
extern double time_warp;
extern int    fixed_time;
extern int    day;
extern int    night;
extern int    terminator;
extern double xgamma;
extern int    use_two_pixmaps;
extern int    num_colors;
extern int    do_fork;
extern int    priority;
extern time_t current_time;

extern void   compute_positions(void);
extern char **tokenize(char *, int *, const char **);
extern void   decode_proj_type(char *);
extern void   decode_viewing_pos(char *);
extern void   decode_sun_pos(char *);
extern void   decode_size(char *);
extern void   decode_shift(char *);
extern void   xearth_bzero(char *, unsigned);
extern void   version_info(void) _noreturn;
extern void   usage(const char *) _noreturn;
extern void   warning(const char *);
extern void   fatal(const char *) _noreturn;

#ifdef USE_EXACT_SQRT

#define SQRT(x) (((x) <= 0.0) ? (0.0) : (sqrt(x)))

#else

/*
 * brute force approximation for sqrt() over [0,1]
 *  - two quadratic regions
 *  - returns zero for args less than zero
 */
#define SQRT(x)                                             \
  (((x) > 0.13)                                             \
   ? ((((-0.3751672414*(x))+1.153263483)*(x))+0.2219037586) \
   : (((x) > 0.0)                                           \
      ? ((((-9.637346154*(x))+3.56143)*(x))+0.065372935)    \
      : (0.0)))

#endif /* USE_EXACT_SQRT */

#ifndef isupper
# define isupper(c)  ((c) >= 'A' && (c) <= 'Z')
#endif
#ifndef _tolower
# define _tolower(c)  ((c) - 'A' + 'a')
#endif

#endif
