
#include "xearth.h"
#include "kljcpyrt.h"

static void ppm_setup(FILE *);
static void ppm_row(u_char *);

static FILE    *outs;
static unsigned bytes_per_row;


void ppm_output()
{
  compute_positions();
  scan_map();
  do_dots();
  ppm_setup(stdout);
  render(ppm_row);
}


static void ppm_setup(FILE *s)
{
  outs          = s;
  bytes_per_row = wdth * 3;

  fprintf(outs, "P6\n%d %d\n255\n", wdth, hght);
}


static void ppm_row(u_char *row)
{
  assert(fwrite(row, 1, bytes_per_row, outs) == bytes_per_row);
}
