#include "ctest.h"

void
sys_nproc ()
{
  suite$ ("sys: nproc");
  unsigned n = nproc ();
  clogger (INFO, "processors:", n);
  check$ (n >= 1); /* contract: always at least 1 */
}
