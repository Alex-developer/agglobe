
#include "xearth.h"
#include "kljcpyrt.h"

static void dither_row_ltor(u_char *, u16or32 *);
static void dither_row_rtol(u_char *, u16or32 *);
static void mono_dither_row_ltor(u16or32 *);
static void mono_dither_row_rtol(u16or32 *);

static u_char  *level;
static u_short  grn_idx[256];
static u_short  blu_idx[256];
static s16or32 *curr;
static s16or32 *next;
static int      even_row;

int     dither_ncolors;
u_char *dither_colormap;


void dither_setup(int ncolors)
{
  int      i;
  int      val;
  int      half;
  unsigned nbytes;

  half = (ncolors - 2) / 2;
  ncolors = half*2 + 2;
  dither_ncolors = ncolors;

  level = (u_char *) malloc((unsigned) ncolors);
  assert(level != NULL);

  nbytes = ncolors * 3;
  dither_colormap = (u_char *) malloc(nbytes);
  assert(dither_colormap != NULL);
  xearth_bzero((char *) dither_colormap, nbytes);

  level[0] = 0;
  for (i=1; i<=half; i++)
  {
    val = (i * 255) / half;

    dither_colormap[i*3+0] = 0;
    dither_colormap[i*3+1] = val;
    dither_colormap[i*3+2] = 0;
    level[i] = val;

    i += half;
    dither_colormap[i*3+0] = 0;
    dither_colormap[i*3+1] = 0;
    dither_colormap[i*3+2] = val;
    level[i] = val;
    i -= half;
  }

  dither_colormap[(ncolors-1)*3+0] = 255;
  dither_colormap[(ncolors-1)*3+1] = 255;
  dither_colormap[(ncolors-1)*3+2] = 255;
  level[ncolors-1] = 255;

  for (i=0; i<256; i++)
  {
    val = (i * half + 127) / 255;

    grn_idx[i] = val;

    if (val == 0)
      blu_idx[i] = val;
    else
      blu_idx[i] = val + half;
  }

  nbytes = (sizeof(s16or32) * 2) * (wdth+2);

  curr = (s16or32 *) malloc(nbytes);
  assert(curr != NULL);
  xearth_bzero((char *) curr, nbytes);
  curr += 2;

  next = (s16or32 *) malloc(nbytes);
  assert(next != NULL);
  xearth_bzero((char *) next, nbytes);
  next += 2;

  even_row = 1;
}


void dither_row(u_char *row, u16or32 *rslt)
{
  if (even_row)
    dither_row_ltor(row, rslt);
  else
    dither_row_rtol(row, rslt);

  even_row = !even_row;
}


void dither_cleanup()
{
  free(curr-2);
  free(next-2);
  free(dither_colormap);
  free(level);
}


static void dither_row_ltor(u_char *row, u16or32 *rslt)
{
  int      i, i_lim;
  int      grn, g_tmp;
  int      blu, b_tmp;
  int      idx;
  u_char  *rowtmp;
  s16or32 *currtmp;
  s16or32 *nexttmp;

  rowtmp  = row;
  currtmp = curr;
  nexttmp = next;

  /* use i_lim to encourage compilers to register loop limit
   */
  i_lim = wdth;
  for (i=0; i<i_lim; i++)
  {
    grn = rowtmp[1];
    blu = rowtmp[2];

    if ((grn == 0) && (blu == 0))
    {
      rslt[i] = 0;
    }
    else if ((grn == 255) && (blu == 255))
    {
      rslt[i] = dither_ncolors - 1;
    }
    else
    {
      grn += currtmp[0];
      blu += currtmp[1];

      if (grn > blu)
      {
	if (grn < 0)
	  grn = 0;
	else if (grn > 255)
	  grn = 255;

	idx  = grn_idx[grn];
	grn -= level[idx];
      }
      else
      {
	if (blu < 0)
	  blu = 0;
	else if (blu > 255)
	  blu = 255;

	idx  = blu_idx[blu];
	blu -= level[idx];
      }

      rslt[i] = idx;

      /* conceptually, what we want here is something like
       *
       *   g_tmp = (grn<0) ? 7 : 8;
       *   b_tmp = (blu<0) ? 7 : 8;
       *   currtmp[ 2] += ((grn * 7) + g_tmp) >> 4;
       *   currtmp[ 3] += ((blu * 7) + b_tmp) >> 4;
       *   nexttmp[ 2] += ((grn * 1) + g_tmp) >> 4;
       *   nexttmp[ 3] += ((blu * 1) + b_tmp) >> 4;
       *   nexttmp[ 0] += ((grn * 5) + g_tmp) >> 4;
       *   nexttmp[ 1] += ((blu * 5) + b_tmp) >> 4;
       *   nexttmp[-2] += ((grn * 3) + g_tmp) >> 4;
       *   nexttmp[-1] += ((blu * 3) + b_tmp) >> 4;
       *
       * but we can get tighter code by computing the product terms
       * via a sequence of additions into g_tmp and b_tmp
       */
      g_tmp = ((grn<0) ? 7 : 8) + grn;
      b_tmp = ((blu<0) ? 7 : 8) + blu;
      nexttmp[2] += (g_tmp >> 4);
      nexttmp[3] += (b_tmp >> 4);
      grn += grn;
      blu += blu;
      g_tmp += grn;
      b_tmp += blu;
      nexttmp[-2] += (g_tmp >> 4);
      nexttmp[-1] += (b_tmp >> 4);
      g_tmp += grn;
      b_tmp += blu;
      nexttmp[0] += (g_tmp >> 4);
      nexttmp[1] += (b_tmp >> 4);
      g_tmp += grn;
      b_tmp += blu;
      currtmp[2] += (g_tmp >> 4);
      currtmp[3] += (b_tmp >> 4);
    }

    rowtmp  += 3;
    currtmp += 2;
    nexttmp += 2;
  }

  currtmp = curr;
  curr    = next;
  next    = currtmp;
  xearth_bzero((char *) next, (unsigned) ((sizeof(s16or32) * 2) * wdth));
}


static void dither_row_rtol(u_char *row, u16or32 *rslt)
{
  int      i;
  int      grn, g_tmp;
  int      blu, b_tmp;
  int      idx;
  u_char  *rowtmp;
  s16or32 *currtmp;
  s16or32 *nexttmp;

  rowtmp  = row  + 3*(wdth-1);
  currtmp = curr + 2*(wdth-1);
  nexttmp = next + 2*(wdth-1);

  for (i=(wdth-1); i>=0; i--)
  {
    grn = rowtmp[1];
    blu = rowtmp[2];

    if ((grn == 0) && (blu == 0))
    {
      rslt[i] = 0;
    }
    else if ((grn == 255) && (blu == 255))
    {
      rslt[i] = dither_ncolors - 1;
    }
    else
    {
      grn += currtmp[0];
      blu += currtmp[1];

      if (grn > blu)
      {
	if (grn < 0)
	  grn = 0;
	else if (grn > 255)
	  grn = 255;

	idx  = grn_idx[grn];
	grn -= level[idx];
      }
      else
      {
	if (blu < 0)
	  blu = 0;
	else if (blu > 255)
	  blu = 255;

	idx  = blu_idx[blu];
	blu -= level[idx];
      }

      rslt[i] = idx;

      /* conceptually, what we want here is something like
       *
       *   g_tmp = (grn<0) ? 7 : 8;
       *   b_tmp = (blu<0) ? 7 : 8;
       *   currtmp[-2] += ((grn * 7) + g_tmp) >> 4;
       *   currtmp[-1] += ((blu * 7) + b_tmp) >> 4;
       *   nexttmp[-2] += ((grn * 1) + g_tmp) >> 4;
       *   nexttmp[-1] += ((blu * 1) + b_tmp) >> 4;
       *   nexttmp[ 0] += ((grn * 5) + g_tmp) >> 4;
       *   nexttmp[ 1] += ((blu * 5) + b_tmp) >> 4;
       *   nexttmp[ 2] += ((grn * 3) + g_tmp) >> 4;
       *   nexttmp[ 3] += ((blu * 3) + b_tmp) >> 4;
       *
       * but we can get tighter code by computing the product terms
       * via a sequence of additions into g_tmp and b_tmp
       */
      g_tmp = ((grn<0) ? 7 : 8) + grn;
      b_tmp = ((blu<0) ? 7 : 8) + blu;
      nexttmp[-2] += (g_tmp >> 4);
      nexttmp[-1] += (b_tmp >> 4);
      grn += grn;
      blu += blu;
      g_tmp += grn;
      b_tmp += blu;
      nexttmp[2] += (g_tmp >> 4);
      nexttmp[3] += (b_tmp >> 4);
      g_tmp += grn;
      b_tmp += blu;
      nexttmp[0] += (g_tmp >> 4);
      nexttmp[1] += (b_tmp >> 4);
      g_tmp += grn;
      b_tmp += blu;
      currtmp[-2] += (g_tmp >> 4);
      currtmp[-1] += (b_tmp >> 4);
    }

    rowtmp  -= 3;
    currtmp -= 2;
    nexttmp -= 2;
  }

  currtmp = curr;
  curr    = next;
  next    = currtmp;
  xearth_bzero((char *) next, (unsigned) ((sizeof(s16or32) * 2) * wdth));
}


void mono_dither_setup()
{
  int      i;
  unsigned nbytes;

  nbytes = sizeof(s16or32) * (wdth+2);

  curr = (s16or32 *) malloc(nbytes);
  assert(curr != NULL);
  for (i=0; i<(wdth+2); i++)
    curr[i] = (random() & ((1<<9)-1)) - (1<<8);
  curr += 1;

  next = (s16or32 *) malloc(nbytes);
  assert(next != NULL);
  xearth_bzero((char *) next, nbytes);
  next += 1;

  even_row = 1;
}


void mono_dither_row(u_char *row, u16or32 *rslt)
{
  int i, i_lim;

  /* convert row to gray scale (could save a few instructions per
   * pixel by integrating this into the mono_dither_row_* functions)
   */
  i_lim = wdth;
  for (i=0; i<i_lim; i++)
  {
    rslt[i] = (2 * row[0]) + (5 * row[1]) + row[2];
    row += 3;
  }

  /* dither to 0s (black) and 1s (white)
   */
  if (even_row)
    mono_dither_row_ltor(rslt);
  else
    mono_dither_row_rtol(rslt);

  even_row = !even_row;
}


void mono_dither_cleanup()
{
  free(curr-1);
  free(next-1);
}


static void mono_dither_row_ltor(u16or32 *row)
{
  int      i, i_lim;
  int      val, tmp;
  u16or32 *rowtmp;
  s16or32 *currtmp;
  s16or32 *nexttmp;

  rowtmp  = row;
  currtmp = curr;
  nexttmp = next;

  /* use i_lim to encourage compilers to register loop limit
   */
  i_lim = wdth;
  for (i=0; i<i_lim; i++)
  {
    val = rowtmp[0];

    if (val == 0)
    {
      rowtmp[0] = 0;
    }
    else if (val == 2040)
    {
      rowtmp[0] = 1;
    }
    else
    {
      val += currtmp[0];

      if (val > 1020)
      {
	rowtmp[0] = 1;
	val -= 2040;
      }
      else
      {
	rowtmp[0] = 0;
      }

      /* conceptually, what we want here is something like
       *
       *   tmp = (val < 0) ? 7 : 8;
       *   currtmp[ 1] += ((val * 7) + tmp) >> 4;
       *   nexttmp[ 1] += ((val * 1) + tmp) >> 4;
       *   nexttmp[ 0] += ((val * 5) + tmp) >> 4;
       *   nexttmp[-1] += ((val * 3) + tmp) >> 4;
       *
       * but we can get tighter code by computing the product terms
       * via a sequence of additions into tmp
       */
      tmp = ((val < 0) ? 7 : 8) + val;
      nexttmp[1] += (tmp >> 4);
      val += val;
      tmp += val;
      nexttmp[-1] += (tmp >> 4);
      tmp += val;
      nexttmp[0] += (tmp >> 4);
      tmp += val;
      currtmp[1] += (tmp >> 4);
    }

    rowtmp  += 1;
    currtmp += 1;
    nexttmp += 1;
  }

  currtmp = curr;
  curr    = next;
  next    = currtmp;
  xearth_bzero((char *) next, (unsigned) (sizeof(s16or32) * wdth));
}


static void mono_dither_row_rtol(u16or32 *row)
{
  int      i;
  int      val, tmp;
  u16or32 *rowtmp;
  s16or32 *currtmp;
  s16or32 *nexttmp;

  rowtmp  = row  + (wdth-1);
  currtmp = curr + (wdth-1);
  nexttmp = next + (wdth-1);

  for (i=(wdth-1); i>=0; i--)
  {
    val = rowtmp[0];

    if (val == 0)
    {
      rowtmp[0] = 0;
    }
    else if (val == 2040)
    {
      rowtmp[0] = 1;
    }
    else
    {
      val += currtmp[0];

      if (val > 1020)
      {
	rowtmp[0] = 1;
	val -= 2040;
      }
      else
      {
	rowtmp[0] = 0;
      }

      /* conceptually, what we want here is something like
       *
       *   tmp = (val < 0) ? 7 : 8;
       *   currtmp[-1] += ((val * 7) + tmp) >> 4;
       *   nexttmp[-1] += ((val * 1) + tmp) >> 4;
       *   nexttmp[ 0] += ((val * 5) + tmp) >> 4;
       *   nexttmp[ 1] += ((val * 3) + tmp) >> 4;
       *
       * but we can get tighter code by computing the product terms
       * via a sequence of additions into tmp
       */
      tmp = ((val < 0) ? 7 : 8) + val;
      nexttmp[-1] += (tmp >> 4);
      val += val;
      tmp += val;
      nexttmp[1] += (tmp >> 4);
      tmp += val;
      nexttmp[0] += (tmp >> 4);
      tmp += val;
      currtmp[-1] += (tmp >> 4);
    }

    rowtmp  -= 1;
    currtmp -= 1;
    nexttmp -= 1;
  }

  currtmp = curr;
  curr    = next;
  next    = currtmp;
  xearth_bzero((char *) next, (unsigned) (sizeof(s16or32) * wdth));
}
