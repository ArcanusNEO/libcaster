#include "sys.h"

void *
static$ (close) (void *pfd)
{
  if (!pfd || *(int *)pfd < 0)
    return null;
  close (*(int *)pfd);
  return pfd;
}

unsigned
nproc ()
{
  auto np = sysconf (_SC_NPROCESSORS_ONLN);
  return max (1, np);
}
