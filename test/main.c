#include "ctest.h"

int static$ (ctest_fails) = 0;

/* sys */
extern void sys_nproc ();

/* deque (no-op in single-threaded build) */
extern void deque_order ();
extern void deque_concurrent ();

/* thrdpool (no-op in single-threaded build) */
extern void thrdpool_exec ();
extern void thrdpool_resize ();
extern void thrdpool_reject_null ();
extern void thrdpool_preempt ();
extern void thrdpool_teardown_reclaims_dynamic ();
extern void thrdpool_teardown_spares_static ();

/* epool (no-op on unsupported platforms) */
extern void epool_basic ();

int
main (int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  sys_nproc ();

  deque_order ();
  deque_concurrent ();

  thrdpool_exec ();
  thrdpool_resize ();
  thrdpool_reject_null ();
  thrdpool_preempt ();
  thrdpool_teardown_reclaims_dynamic ();
  thrdpool_teardown_spares_static ();

  epool_basic ();

  if (static$ (ctest_fails))
    {
      clogger (ERROR, "checks failed:", static$ (ctest_fails));
      return 1;
    }
  clogger (INFO, "all checks passed");
}
