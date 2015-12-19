
#include "xearth.h"
#include "kljcpyrt.h"


ExtArr extarr_alloc(unsigned eltsize)
{
  ExtArr rslt;

  rslt = (ExtArr) malloc((unsigned) sizeof(struct extarr));
  assert(rslt != NULL);

  rslt->eltsize = eltsize;
  rslt->limit   = 1;
  rslt->count   = 0;

  rslt->body = (void *) malloc((unsigned) eltsize*rslt->limit);
  assert(rslt->body != NULL);

  return rslt;
}


void extarr_free(ExtArr x)
{
  free(x->body);
  free(x);
}


void *extarr_next(ExtArr x)
{
  unsigned eltsize;
  unsigned limit;
  unsigned count;
  void    *body;
  void    *rslt;

  eltsize = x->eltsize;
  limit   = x->limit;
  count   = x->count;
  body    = x->body;

  if (count == limit)
  {
    limit *= 2;
    body   = (void *) realloc(body, (unsigned) eltsize*limit);
    assert(body != NULL);

    x->limit = limit;
    x->body  = body;
  }

  rslt = (void *) ((char *) body + (count * eltsize));
  x->count = count + 1;

  return rslt;
}
