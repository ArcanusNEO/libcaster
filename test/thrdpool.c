#include "ctest.h"

#ifdef _REENTRANT

#include <unistd.h>
#include <stdatomic.h>

static atomic_int static$ (counter);
static atomic_int static$ (marker);

static void
inc_job (void *arg)
{
  (void)arg;
  atomic_fetch_add (&static$ (counter), 1);
}

static void
slow_job (void *arg)
{
  (void)arg;
  usleep (20 * 1000);
}

static void
mark_job (void *arg)
{
  atomic_int *slot = arg;
  atomic_store (slot, atomic_fetch_add (&static$ (marker), 1));
}

/* Wait until `expected` tasks have run, or timeout (ms). */
static int
wait_count (atomic_int *c, int expected, int timeout_ms)
{
  for (int waited = 0; waited < timeout_ms; waited += 5)
    {
      if (atomic_load (c) >= expected)
        return 1;
      usleep (5 * 1000);
    }
  return atomic_load (c) >= expected;
}

void
thrdpool_exec ()
{
  suite$ ("thrdpool: exec runs all tasks");
  struct thrdpool *tp = new$ (thrdpool);
  check$ (tp != null);
  if (!tp)
    return;
  atomic_store (&static$ (counter), 0);

  check$ ($fn (thrdpool, resize) (tp, 4) == 4);
  int all_ok = 1;
  for (int i = 0; i < 1000; ++i)
    if ($fn (thrdpool, exec) (tp, inc_job, null) != 0)
      all_ok = 0;
  check$ (all_ok);
  check$ (wait_count (&static$ (counter), 1000, 5000));
  check$ (atomic_load (&static$ (counter)) == 1000);

  delete$ (tp);
}

void
thrdpool_resize ()
{
  suite$ ("thrdpool: resize up/down/zero");
  struct thrdpool *tp = new$ (thrdpool);
  check$ (tp != null);
  if (!tp)
    return;
  atomic_store (&static$ (counter), 0);

  check$ ($fn (thrdpool, resize) (tp, 8) == 8);
  check$ ($fn (thrdpool, resize) (tp, 8) == 8); /* no-op */
  check$ ($fn (thrdpool, resize) (tp, 3) == 3); /* shrink */

  /* Pool still works after shrink. */
  for (int i = 0; i < 500; ++i)
    $fn (thrdpool, exec) (tp, inc_job, null);
  check$ (wait_count (&static$ (counter), 500, 5000));
  check$ (atomic_load (&static$ (counter)) == 500);

  check$ ($fn (thrdpool, resize) (tp, 0) == 0); /* drain to empty */

  delete$ (tp);
}

void
thrdpool_reject_null ()
{
  suite$ ("thrdpool: reject null fn");
  struct thrdpool *tp = new$ (thrdpool);
  check$ (tp != null);
  if (!tp)
    return;
  $fn (thrdpool, resize) (tp, 2);
  check$ ($fn (thrdpool, exec) (tp, null, null) == -1);
  check$ ($fn (thrdpool, preempt) (tp, null, null) == -1);
  delete$ (tp);
}

void
thrdpool_preempt ()
{
  suite$ ("thrdpool: preempt jumps ahead of queued work");
  struct thrdpool *tp = new$ (thrdpool);
  check$ (tp != null);
  if (!tp)
    return;
  atomic_store (&static$ (marker), 0);

  /* Single worker so ordering is deterministic. */
  check$ ($fn (thrdpool, resize) (tp, 1) == 1);

  /* Occupy the worker so the queue builds up behind it. */
  $fn (thrdpool, exec) (tp, slow_job, null);

  atomic_int a_first = -1, a_second = -1, a_jumper = -1;
  $fn (thrdpool, exec) (tp, mark_job, &a_first);
  $fn (thrdpool, exec) (tp, mark_job, &a_second);
  /* preempt should be pulled before the two queued exec tasks. */
  $fn (thrdpool, preempt) (tp, mark_job, &a_jumper);

  usleep (200 * 1000);
  int first = atomic_load (&a_first);
  int second = atomic_load (&a_second);
  int jumper = atomic_load (&a_jumper);
  check$ (jumper >= 0 && first >= 0 && second >= 0);
  check$ (jumper < first); /* preempted task ran before earlier-queued exec */

  delete$ (tp);
}

/* Teardown must reclaim tasks still queued in exec_deque that are flagged
   THRDPOOL_TASK_ATTR_DYNAMIC (the pool owns them). Run under ASan/LSan to
   catch a leak. A size-0 pool has no workers, so every enqueued task stays
   pending until delete$. */
void
thrdpool_teardown_reclaims_dynamic ()
{
  suite$ ("thrdpool: teardown reclaims pending dynamic tasks");
  struct thrdpool *tp = new$ (thrdpool);
  check$ (tp != null);
  if (!tp)
    return;

  int enq = 0;
  for (int i = 0; i < 64; ++i)
    if ($fn (thrdpool, exec) (tp, inc_job, null) == 0)
      ++enq;
  check$ (enq == 64);

  /* No workers were ever created, so none of these can have run. */
  delete$ (tp); /* dynamic tasks must be freed here, not leaked */
  check$ (1);
}

/* Teardown must NOT free caller-owned (non-dynamic) tasks that were enqueued
   directly into the public exec_deque. Freeing them is a bad free on a
   non-heap pointer. Run under ASan to catch it. */
void
thrdpool_teardown_spares_static ()
{
  suite$ ("thrdpool: teardown spares non-dynamic tasks");
  struct thrdpool *tp = new$ (thrdpool);
  check$ (tp != null);
  if (!tp)
    return;

  /* Static storage, attr has no DYNAMIC bit: the pool must leave it alone. */
  static struct thrdtask st;
  st.fn = inc_job;
  st.arg = null;
  st.attr = 0;
  $fn (blocking_deque, push) (&tp->mbr$ (exec_deque), &st.list_entry);

  delete$ (tp); /* must not free &st */
  check$ (1);
}

#else /* !_REENTRANT */

void
thrdpool_exec ()
{
}
void
thrdpool_resize ()
{
}
void
thrdpool_reject_null ()
{
}
void
thrdpool_preempt ()
{
}
void
thrdpool_teardown_reclaims_dynamic ()
{
}
void
thrdpool_teardown_spares_static ()
{
}

#endif
