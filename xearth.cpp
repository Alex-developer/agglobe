

#include "xearth.h"
#include "kljcpyrt.h"
extern int errno;

#ifndef NO_SETPRIORITY
#ifndef WIN32
/* apparently some systems (possibly mt.xinu 4.3bsd?) may need 
 * <sys/time.h> to be included here before <sys/resource.h>.
 */
#include <sys/resource.h>	/* for setpriority() */
#endif
#endif

#define ModeX    (0)		/* possible output_mode values */
#define ModePPM  (1)
#define ModeGIF  (2)
#define ModeBMP  (3)
#define ModeTest (4)

/* tokens in specifiers are delimited by spaces, tabs, commas, and
 * forward slashes
 */
#define IsTokenDelim(x)  (((x)==' ')||((x)=='\t')||((x)==',')||((x)=='/'))
#define NotTokenDelim(x) (!IsTokenDelim(x))

int  main(int, char *[]);
void set_priority(int);
void output(void);
void test_mode(void);
void sun_relative_position(double *, double *);
void simple_orbit(time_t, double *, double *);
void pick_random_position(double *, double *);
void set_defaults(void);
int  using_x(int, char *[]);
void command_line(int, char *[]);

char    *progname;              /* program name                */
int      proj_type;		/* projection type             */
int      output_mode;           /* output mode (X, PPM, ...)   */
double   view_lon;              /* viewing position longitude  */
double   view_lat;              /* viewing position latitude   */
double   view_rot;		/* viewing rotation (degrees)  */
int      view_pos_type;		/* type of viewing position    */
double   sun_rel_lon;		/* view lon, relative to sun   */
double   sun_rel_lat;		/* view lat, relative to sun   */
double   orbit_period;		/* orbit period (seconds)      */
double   orbit_inclin;		/* orbit inclination (degrees) */
double   view_mag;		/* viewing magnification       */
int      do_shade;		/* render with shading?        */
double   sun_lon;               /* sun position longitude      */
double   sun_lat;               /* sun position latitude       */
int      compute_sun_pos;       /* compute sun's position?     */
int      wdth;                  /* image width (pixels)        */
int      hght;                  /* image height (pixels)       */
int      shift_x;		/* image shift (x, pixels)     */
int      shift_y;		/* image shift (y, pixels)     */
int      do_stars;		/* show stars in background?   */
double   star_freq;		/* frequency of stars          */
int      big_stars;		/* percent of doublewide stars */
int      do_grid;		/* show lon/lat grid?          */
int      grid_big;		/* lon/lat grid line spacing   */
int      grid_small;		/* dot spacing along grids     */
int      do_label;              /* label image                 */
int      labelpos;              /* label position              */
int      do_markers;		/* display markers (X only)    */
char    *markerfile;		/* for user-spec. marker info  */
int      wait_time;             /* wait time between redraw    */
double   time_warp;		/* passage of time multiplier  */
int      fixed_time;		/* fixed viewing time (ssue)   */
int      day;                   /* day side brightness (%)     */
int      night;                 /* night side brightness (%)   */
int      terminator;		/* terminator discontinuity, % */
double   xgamma;		/* gamma correction (X only)   */
int      use_two_pixmaps;	/* use two pixmaps? (X only)   */
int      num_colors;		/* number of colors to use     */
int      do_fork;		/* fork child process?         */
int      priority;		/* desired process priority    */

time_t start_time = 0;
time_t current_time;
char   errmsgbuf[1024];		/* for formatting warning/error msgs */

jmp_buf fataljmp;

int main(int argc, char *argv[])
{
	
	set_defaults();

	command_line(argc, argv);

	srandom(((int) time(NULL)) + ((int) getpid()));

	output();

  return 0;
}

void output()
{
    bmp_output();
}


void test_mode()
{
}


void compute_positions()
{
  /* determine "current" time
   */
  if (fixed_time == 0)
  {
    current_time = time(NULL);
    if (start_time == 0)
      start_time = current_time;
    else
      current_time = start_time + (current_time - start_time) * time_warp;
  }
  else
  {
    current_time = fixed_time;
  }

  /* determine position on earth's surface where sun is directly
   * overhead
   */
  if (compute_sun_pos)
    sun_position(current_time, &sun_lat, &sun_lon);

  /* determine viewing position
   */
  if (view_pos_type == ViewPosTypeSun)
  {
    sun_relative_position(&view_lat, &view_lon);
  }
  else if (view_pos_type == ViewPosTypeOrbit)
  {
    simple_orbit(current_time, &view_lat, &view_lon);
  }
  else if (view_pos_type == ViewPosTypeRandom)
  {
    pick_random_position(&view_lat, &view_lon);
  }
}


void sun_relative_position(double *lat_ret, double *lon_ret)
     //double *lat_ret;		/* (return) latitude  */
     //double *lon_ret;		/* (return) longitude */
{
  double lat, lon;

  lat = sun_lat + sun_rel_lat;
  lon = sun_lon + sun_rel_lon;

  /* sanity check */
  assert((lat >= -180) && (lat <= 180));
  assert((lon >= -360) && (lon <= 360));

  if (lat > 90)
  {
    lat  = 180 - lat;
    lon += 180;
  }
  else if (lat < -90)
  {
    lat  = -180 - lat;
    lon += 180;
  }

  if (lon > 180)
  {
    do
      lon -= 360;
    while (lon > 180);
  }
  else if (lon < -180)
  {
    do
      lon += 360;
    while (lon < -180);
  }

  *lat_ret = lat;
  *lon_ret = lon;
}


void simple_orbit(time_t ssue, double *lat, double *lon)
     //time_t  ssue;		/* seconds since unix epoch */
     //double *lat;		/* (return) latitude        */
     //double *lon;		/* (return) longitude       */
{
  double x, y, z;
  double a, c, s;
  double t1, t2;

  /* start at 0 N 0 E */
  x = 0;
  y = 0;
  z = 1;

  /* rotate in about y axis (from z towards x) according to the number
   * of orbits we've completed
   */
  a  = ((double) ssue / orbit_period) * (2*M_PI);
  c  = cos(a);
  s  = sin(a);
  t1 = c*z - s*x;
  t2 = s*z + c*x;
  z  = t1;
  x  = t2;

  /* rotate about z axis (from x towards y) according to the
   * inclination of the orbit
   */
  a  = orbit_inclin * (M_PI/180);
  c  = cos(a);
  s  = sin(a);
  t1 = c*x - s*y;
  t2 = s*x + c*y;
  x  = t1;
  y  = t2;

  /* rotate about y axis (from x towards z) according to the number of
   * rotations the earth has made
   */
  a  = ((double) ssue / EarthPeriod) * (2*M_PI);
  c  = cos(a);
  s  = sin(a);
  t1 = c*x - s*z;
  t2 = s*x + c*z;
  x  = t1;
  z  = t2;

  *lat = asin(y) * (180/M_PI);
  *lon = atan2(x, z) * (180/M_PI);
}


/* pick a position (lat, lon) at random
 */
void pick_random_position(double *lat_ret, double *lon_ret)
     //double *lat_ret;		/* (return) latitude  */
     //double *lon_ret;		/* (return) longitude */
{
  int    i;
  double pos[3];
  double mag;
  double s_lat, c_lat;
  double s_lon, c_lon;

  /* select a vector at random */
  do
  {
    mag = 0;
    for (i=0; i<3; i++)
    {
      pos[i] = ((random() % 20001) * 1e-4) - 1;
      mag   += pos[i] * pos[i];
    }
  } while ((mag > 1.0) || (mag < 0.01));

  /* normalize the vector */
  mag = sqrt(mag);
  for (i=0; i<3; i++)
    pos[i] /= mag;

  /* convert to (lat, lon) */
  s_lat = pos[1];
  c_lat = sqrt(1 - s_lat*s_lat);
  s_lon = pos[0] / c_lat;
  c_lon = pos[2] / c_lat;

  *lat_ret = atan2(s_lat, c_lat) * (180/M_PI);
  *lon_ret = atan2(s_lon, c_lon) * (180/M_PI);
}


/* look through the command line arguments to figure out if we're
 * using X or not (if "-ppm", "-gif", or "-test" is found, we're not
 * using X, otherwise we are).
 */
int using_x(int argc, char *argv[])
{
  int i;

  /* loop through the args, break if we find "-ppm", "-gif", or
   * "-test"
   */
  for (i=1; i<argc; i++)
    if ((strcmp(argv[i], "-ppm") == 0) ||
	(strcmp(argv[i], "-gif") == 0) ||
	(strcmp(argv[i], "-test") == 0))
      break;

  /* if we made it through the loop without finding "-ppm", "-gif", or
   * "-test" (and breaking out), assume we're using X.
   */
  return (i == argc);
}


/* set_defaults() gets called at xearth startup (before command line
 * arguments are handled), regardless of what output mode (x, ppm,
 * gif) is being used.
 */
void set_defaults()
{
  output_mode      = ModeBMP;
  proj_type        = ProjTypeOrthographic;
  view_pos_type    = ViewPosTypeFixed;
  sun_rel_lat      = 0;
  sun_rel_lon      = 0;
  compute_sun_pos  = 1;
  view_rot         = 0;
  wdth             = 512;
  hght             = 512;
  shift_x          = 0;
  shift_y          = 0;
  view_mag         = 1.0;
  do_shade         = 1;
  do_label         = 0;
  do_markers       = 1;
  wait_time        = 300;
  time_warp        = 1;
  day              = 100;
  night            = 5;
  terminator       = 1;
  use_two_pixmaps  = 1;
  num_colors       = 64;
  do_fork          = 0;
  priority         = 0;
  do_stars         = 1;
  star_freq        = 0.002;
  big_stars        = 0;
  do_grid          = 1;
  grid_big         = 6;
  grid_small       = 15;
  fixed_time       = 0;
  xgamma           = 1.0;
}


/* procedure for interpreting command line arguments when we're not
 * using X; command_line_x() is used when we are.
 */
void command_line(int argc, char *argv[])
{
  int i;

  progname = argv[0];

  for (i=1; i<argc; i++)
  {
    if (strcmp(argv[i], "-proj") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -proj");
      decode_proj_type(argv[i]);
    }
    else if (strcmp(argv[i], "-pos") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -pos");
      decode_viewing_pos(argv[i]);
    }
    else if (strcmp(argv[i], "-rot") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -rot");
      sscanf(argv[i], "%lf", &view_rot);
      if ((view_rot < -180) || (view_rot > 360))
	fatal("viewing rotation must be between -180 and 360");
    }
    else if (strcmp(argv[i], "-sunpos") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -sunpos");
      decode_sun_pos(argv[i]);
    }
    else if (strcmp(argv[i], "-mag") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -mag");
      sscanf(argv[i], "%lf", &view_mag);
      if (view_mag <= 0)
        fatal("viewing magnification must be positive");
    }
    else if (strcmp(argv[i], "-size") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -size");
      decode_size(argv[i]);
    }
    else if (strcmp(argv[i], "-shift") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -shift");
      decode_shift(argv[i]);
    }
    else if (strcmp(argv[i], "-shade") == 0)
    {
      do_shade = 1;
    }
    else if (strcmp(argv[i], "-noshade") == 0)
    {
      do_shade = 0;
    }
    else if (strcmp(argv[i], "-label") == 0)
    {
      do_label = 1;
      warning("labeling not supported with GIF and PPM output");
    }
    else if (strcmp(argv[i], "-nolabel") == 0)
    {
      do_label = 0;
    }
    else if (strcmp(argv[i], "-labelpos") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -labelpos");
      sscanf(argv[i], "%d", &labelpos);
      if ((labelpos > 3) || (labelpos < 0))
        fatal("arg to -labelpos must be between 0 and 3");
      warning("-labelpos not relevant for GIF or PPM output");
    }
    else if (strcmp(argv[i], "-markers") == 0)
    {
      do_markers = 1;
      warning("markers not supported with GIF and PPM output");
    }
    else if (strcmp(argv[i], "-nomarkers") == 0)
    {
      do_markers = 0;
    }
    else if (strcmp(argv[i], "-markerfile") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -markerfile");
      warning("-markerfile not relevant for GIF or PPM output");
    }
    else if (strcmp(argv[i], "-showmarkers") == 0)
    {
      warning("-showmarkers not relevant for GIF or PPM output");
    }
    else if (strcmp(argv[i], "-stars") == 0)
    {
      do_stars = 1;
    }
    else if (strcmp(argv[i], "-nostars") == 0)
    {
      do_stars = 0;
    }
    else if (strcmp(argv[i], "-starfreq") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -starfreq");
      sscanf(argv[i], "%lf", &star_freq);
      if ((star_freq < 0) || (star_freq > 1))
	fatal("arg to -starfreq must be between 0 and 1");
    }
    else if (strcmp(argv[i], "-bigstars") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -bigstars");
      sscanf(argv[i], "%d", &big_stars);
      if ((big_stars < 0) || (big_stars > 100))
	fatal("arg to -bigstars must be between 0 and 100");
    }
    else if (strcmp(argv[i], "-grid") == 0)
    {
      do_grid = 1;
    }
    else if (strcmp(argv[i], "-nogrid") == 0)
    {
      do_grid = 0;
    }
    else if (strcmp(argv[i], "-grid1") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -grid1");
      sscanf(argv[i], "%d", &grid_big);
      if (grid_big <= 0)
	fatal("arg to -grid1 must be positive");
    }
    else if (strcmp(argv[i], "-grid2") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -grid2");
      sscanf(argv[i], "%d", &grid_small);
      if (grid_small <= 0)
	fatal("arg to -grid2 must be positive");
    }
    else if (strcmp(argv[i], "-day") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -day");
      sscanf(argv[i], "%d", &day);
      if ((day > 100) || (day < 0))
        fatal("arg to -day must be between 0 and 100");
    }
    else if (strcmp(argv[i], "-night") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -night");
      sscanf(argv[i], "%d", &night);
      if ((night > 100) || (night < 0))
        fatal("arg to -night must be between 0 and 100");
    }
    else if (strcmp(argv[i], "-term") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -term");
      sscanf(argv[i], "%d", &terminator);
      if ((terminator > 100) || (terminator < 0))
        fatal("arg to -term must be between 0 and 100");
    }
    else if (strcmp(argv[i], "-gamma") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -gamma");
      sscanf(argv[i], "%lf", &xgamma);
      if (xgamma <= 0)
        fatal("arg to -gamma must be positive");
      warning("gamma correction not supported with GIF and PPM output");
    }
    else if (strcmp(argv[i], "-wait") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -wait");
      sscanf(argv[i], "%d", &wait_time);
      if (wait_time < 0)
        fatal("arg to -wait must be non-negative");
    }
    else if (strcmp(argv[i], "-timewarp") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -timewarp");
      sscanf(argv[i], "%lf", &time_warp);
      if (time_warp <= 0)
	fatal("arg to -timewarp must be positive");
    }
    else if (strcmp(argv[i], "-time") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -time");
      sscanf(argv[i], "%d", &fixed_time);
    }
    else if (strcmp(argv[i], "-onepix") == 0)
    {
      use_two_pixmaps = 0;
      warning("-onepix not relevant for GIF or PPM output");
    }
    else if (strcmp(argv[i], "-twopix") == 0)
    {
      use_two_pixmaps = 1;
      warning("-twopix not relevant for GIF or PPM output");
    }
    else if (strcmp(argv[i], "-mono") == 0)
    {
      warning("monochrome mode not supported with GIF and PPM output");
    }
    else if (strcmp(argv[i], "-nomono") == 0)
    {
      warning("monochrome mode not supported with GIF and PPM output");
    }
    else if (strcmp(argv[i], "-ncolors") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -ncolors");
      sscanf(argv[i], "%d", &num_colors);
      if ((num_colors < 3) || (num_colors > 1024))
        fatal("arg to -ncolors must be between 3 and 1024");
    }
    else if (strcmp(argv[i], "-font") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -font");
      warning("-font not relevant for GIF or PPM output");
    }
    else if (strcmp(argv[i], "-fork") == 0)
    {
      do_fork = 1;
    }
    else if (strcmp(argv[i], "-nofork") == 0)
    {
      do_fork = 0;
    }
    else if (strcmp(argv[i], "-once") == 0)
    {
      warning("-once not relevant for GIF or PPM output");
    }
    else if (strcmp(argv[i], "-noonce") == 0)
    {
      warning("-once not relevant for GIF or PPM output");
    }
    else if (strcmp(argv[i], "-nice") == 0)
    {
      i += 1;
      if (i >= argc) usage("missing arg to -nice");
      sscanf(argv[i], "%d", &priority);
    }
    else if (strcmp(argv[i], "-version") == 0)
    {
      version_info();
    }
    else if (strcmp(argv[i], "-ppm") == 0)
    {
      output_mode = ModePPM;
    }
    else if (strcmp(argv[i], "-gif") == 0)
    {
      output_mode = ModeGIF;
    }
    else if (strcmp(argv[i], "-bmp") == 0)
    {
      output_mode = ModeBMP;
    }
    else if (strcmp(argv[i], "-test") == 0)
    {
      output_mode = ModeTest;
    }
    else if (strcmp(argv[i], "-display") == 0)
    {
      warning("-display not relevant for GIF or PPM output");
    }
    else
    {
      usage(NULL);
    }
  }
}


/* if efficiency really mattered here (which it doesn't), this code
 * could definitely be made quite a bit more efficient.
 */
char **tokenize(char *s, int *argc_ret, const char **msg)
{
  int    lim;
  int    argc;
  char **argv;

  *msg = NULL;
  lim  = 4;
  argc = 0;
  argv = (char **) malloc((unsigned) sizeof(char *) * lim);
  assert(argv != NULL);

  while (1)
  {
    /* skip delimiters (space, tab, comma, forward slash)
     */
    while (1)
    {
      if (NotTokenDelim(*s)) break;
      *s = '\0';
      s += 1;
    }

    if ((*s) == '\0')
    {
      /* if we're at the end of s, no more tokens
       */
      break;
    }
    else if ((*s) == '#')
    {
      /* if we find a comment character (#), replace it
       * with a NUL and act like we found the end s
       */
      *s = '\0';
      break;
    }

    /* make sure there's room for another token
     */
    if (argc == lim)
    {
      lim *= 2;
      argv = (char **) realloc(argv, (unsigned) sizeof(char *) * lim);
      assert(argv != NULL);
    }

    /* get the next token
     */
    if ((*s) == '"')
    {
      /* string token
       */
      *s = '\0';
      s += 1;
      argv[argc++] = s;

      while (((*s) != '"') && ((*s) != '\0'))
	s += 1;

      if ((*s) == '\0')
      {
	/* oops, never found closing double quote
	 */
	*msg = "unterminated string (missing \")";
      }
      else
      {
	/* string token terminated normally
	 */
	*s = '\0';
	s += 1;
      }
    }
    else
    {
      /* normal token
       */
      argv[argc++] = s;

      while (NotTokenDelim(*s) &&
	     ((*s) != '#') && ((*s) != '"') && ((*s) != '\0'))
	s += 1;
    }
  }

  *argc_ret = argc;
  return argv;
}


/* decode projection type; two possibilities:
 *  orthographic - orthographic projection (short form: orth)
 *  mercator     - mercator projection (short form: merc)
 */
void decode_proj_type(char *s)
{
  if ((strcmp(s, "orthographic") == 0) || (strcmp(s, "orth") == 0))
  {
    proj_type = ProjTypeOrthographic;
  }
  else if ((strcmp(s, "mercator") == 0) || (strcmp(s, "merc") == 0))
  {
    proj_type = ProjTypeMercator;
  }
  else
  {
    sprintf(errmsgbuf, "unknown projection type (%s)", s);
    fatal(errmsgbuf);
  }
}


/* decode viewing position specifier; four possibilities:
 *
 *  fixed lat lon  - viewing position fixed wrt earth at (lat, lon)
 *
 *  sunrel lat lon - viewing position fixed wrt sun at (lat, lon)
 *                   [position interpreted as if sun was at (0, 0)]
 *
 *  orbit per inc  - moving viewing position following simple orbit
 *                   with period per and inclination inc
 *
 *  random         - random viewing position
 *
 * fields can be separated with either spaces, commas, or slashes.
 */
void decode_viewing_pos(char *s)
{
  int         argc;
  char      **argv;
  const char *msg;
  double      arg1;
  double      arg2;

  argv = tokenize(s, &argc, &msg);
  if (msg != NULL)
  {
    sprintf(errmsgbuf, "viewing position specifier: %s", msg);
    warning(errmsgbuf);
  }

  if (argc == 3)
  {
    arg1 = 0;
    arg2 = 0;
    sscanf(argv[1], "%lf", &arg1);
    sscanf(argv[2], "%lf", &arg2);
  }

  if (strcmp(argv[0], "fixed") == 0)
  {
    if (argc != 3)
      fatal("wrong number of args in viewing position specifier");

    view_lat      = arg1;
    view_lon      = arg2;
    view_pos_type = ViewPosTypeFixed;

    if ((view_lat > 90) || (view_lat < -90))
      fatal("viewing latitude must be between -90 and 90");
    if ((view_lon > 180) || (view_lon < -180))
      fatal("viewing longitude must be between -180 and 180");
  }
  else if (strcmp(argv[0], "sunrel") == 0)
  {
    if (argc != 3)
      fatal("wrong number of args in viewing position specifier");

    sun_rel_lat   = arg1;
    sun_rel_lon   = arg2;
    view_pos_type = ViewPosTypeSun;

    if ((sun_rel_lat > 90) || (sun_rel_lat < -90))
      fatal("latitude relative to sun must be between -90 and 90");
    if ((sun_rel_lon > 180) || (sun_rel_lon < -180))
      fatal("longitude relative to sun must be between -180 and 180");
  }
  else if (strcmp(argv[0], "orbit") == 0)
  {
    if (argc != 3)
      fatal("wrong number of args in viewing position specifier");

    orbit_period  = arg1 * 3600;
    orbit_inclin  = arg2;
    view_pos_type = ViewPosTypeOrbit;

    if (orbit_period <= 0)
      fatal("orbital period must be positive");
    if ((orbit_inclin > 90) || (orbit_inclin < -90))
      fatal("orbital inclination must be between -90 and 90");
  }
  else if (strcmp(argv[0], "random") == 0)
  {
    if (argc != 1)
      fatal("wrong number of args in viewing position specifier");
    
    view_pos_type = ViewPosTypeRandom;
  }
  else
  {
    fprintf(stderr, "`%s'\n", argv[0]);
    fatal("unrecognized keyword in viewing position specifier");
  }

  free(argv);
}


/* decode sun position specifier:
 *
 *  lat lon - sun position fixed wrt earth at (lat, lon)
 *
 * fields can be separated with either spaces, commas, or slashes.
 */
void decode_sun_pos(char *s)
{
  int         argc;
  char      **argv;
  const char *msg;

  argv = tokenize(s, &argc, &msg);
  if (msg != NULL)
  {
    sprintf(errmsgbuf, "sun position specifier: %s", msg);
    warning(errmsgbuf);
  }

  if (argc != 2)
    fatal("wrong number of args in sun position specifier");

  sscanf(argv[0], "%lf", &sun_lat);
  sscanf(argv[1], "%lf", &sun_lon);

  if ((sun_lat > 90) || (sun_lat < -90))
    fatal("sun latitude must be between -90 and 90");
  if ((sun_lon > 180) || (sun_lon < -180))
    fatal("sun longitude must be between -180 and 180");

  compute_sun_pos = 0;

  free(argv);
}


/* decode size specifier:
 *
 *  width height - width and height of image (in pixels)
 *
 * fields can be separated with either spaces, commas, or slashes.
 */
void decode_size(char *s)
{
  int         argc;
  char      **argv;
  const char *msg;

  argv = tokenize(s, &argc, &msg);
  if (msg != NULL)
  {
    sprintf(errmsgbuf, "size specifier: %s", msg);
    warning(errmsgbuf);
  }

  if (argc != 2)
    fatal("wrong number of args in size specifier");

  sscanf(argv[0], "%d", &wdth);
  sscanf(argv[1], "%d", &hght);

  if (wdth <= 0)
    fatal("wdth arg must be positive");
  if (hght <= 0)
    fatal("hght arg must be positive");

  free(argv);
}


/* decode shift specifier:
 *
 *  xofs yofs - offset in x and y dimensions
 *
 * fields can be separated with either spaces, commas, or slashes.
 */
void decode_shift(char *s)
{
  int         argc;
  char      **argv;
  const char *msg;

  argv = tokenize(s, &argc, &msg);
  if (msg != NULL)
  {
    sprintf(errmsgbuf, "shift specifier: %s", msg);
    warning(errmsgbuf);
  }

  if (argc != 2)
    fatal("wrong number of args in shift specifier");

  sscanf(argv[0], "%d", &shift_x);
  sscanf(argv[1], "%d", &shift_y);

  free(argv);
}


void xearth_bzero(char *buf, unsigned len)
{
  int *tmp;

  /* assume sizeof(int) is a power of two
   */
  assert((sizeof(int) == 4) || (sizeof(int) == 8));

  if (len < sizeof(int))
  {
    /* special case small regions
     */
    while (len > 0)
    {
      *buf = 0;
      buf += 1;
      len -= 1;
    }
  }
  else
  {
    /* zero leading non-word-aligned bytes
     */
    while (((long) buf) & (sizeof(int)-1))
    {
      *buf = 0;
      buf += 1;
      len -= 1;
    }

    /* zero trailing non-word-aligned bytes
     */
    while (len & (sizeof(int)-1))
    {
      len     -= 1;
      buf[len] = 0;
    }

    /* convert remaining len to words
     */
    tmp = (int *) buf;
    if (sizeof(int) == 4)
      len >>= 2;
    else if (sizeof(int) == 8)
      len >>= 3;
    else
      assert(0);

    /* zero trailing non-quadword-aligned words
     */
    while (len & 0x03)
    {
      len     -= 1;
      tmp[len] = 0;
    }

    /* zero remaining data four words at a time
     */
    while (len)
    {
      tmp[0] = 0;
      tmp[1] = 0;
      tmp[2] = 0;
      tmp[3] = 0;
      tmp += 4;
      len -= 4;
    }
  }
}


void version_info()
{
  fflush(stdout);
  fprintf(stderr, "\nThis is AgGlobe version %s.\n\n", VersionString);
  exit(0);
}


void usage(const char *msg)
{
  fflush(stdout);
  fprintf(stderr, "\n");
  if (msg != NULL)
    fprintf(stderr, "%s\n", msg);
  fprintf(stderr, "usage: %s\n", progname);
  fprintf(stderr, " [-proj proj_type] [-pos pos_spec] [-rot angle]\n");
  fprintf(stderr, " [-sunpos sun_pos_spec] [-mag factor] [-size size_spec]\n");
  fprintf(stderr, " [-shift shift_spec] [-shade|-noshade] [-label|-nolabel]\n");
  fprintf(stderr, " [-labelpos geom] [-markers|-nomarkers] [-markerfile file]\n");
  fprintf(stderr, " [-showmarkers] [-stars|-nostars] [-starfreq frequency]\n");
  fprintf(stderr, " [-bigstars percent] [-grid|-nogrid] [-grid1 grid1] [-grid2 grid2]\n");
  fprintf(stderr, " [-day pct] [-night pct] [-term pct] [-gamma gamma_value]\n");
  fprintf(stderr, " [-wait secs] [-timewarp factor] [-time fixed_time]\n");
  fprintf(stderr, " [-onepix|-twopix] [-mono|-nomono] [-ncolors num_colors]\n");
  fprintf(stderr, " [-font font_name] [-fork|-nofork] [-once|-noonce]\n");
  fprintf(stderr, " [-nice priority] [-gif] [-ppm] [-bmp] [-display dpyname] [-version]\n");
  fprintf(stderr, "\n");
  exit(1);
}


void warning(const char *msg)
{
  fflush(stdout);
  fprintf(stderr, "AgGlobe %s: warning - %s\n", VersionString, msg);
  fflush(stderr);
}


void fatal(const char *msg)
{
  longjmp(fataljmp, (int)msg);
  fflush(stdout);
  fprintf(stderr, "AgGlobe %s: fatal - %s\n", VersionString, msg);
  fprintf(stderr, "\n");
  exit(1);
}

