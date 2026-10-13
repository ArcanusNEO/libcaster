#include "ctest.h"

#ifdef _REENTRANT

#include <pthread.h>
#include <stdatomic.h>

void
deque_order ()
{
  suite$ ("deque: fifo/lifo ordering");
  struct blocking_deque *dq = new$ (blocking_deque);
  check$ (dq != null);
  if (!dq)
    return;

  xapr a = 0, b = 0, c = 0;
  /* push appends to tail, unshift prepends to head. */
  $fn (blocking_deque, push) (dq, &a);    /* [a]     */
  $fn (blocking_deque, push) (dq, &b);    /* [a b]   */
  $fn (blocking_deque, unshift) (dq, &c); /* [c a b] */

  /* shift takes from head, pop takes from tail. */
  check$ ($fn (blocking_deque, shift) (dq) == &c);
  check$ ($fn (blocking_deque, pop) (dq) == &b);
  check$ ($fn (blocking_deque, shift) (dq) == &a);

  delete$ (dq);
}

#define NPROD 4
#define NCONS 4
#define PER_PROD 2000
#define TOTAL (NPROD * PER_PROD)

struct dqnode
{
  xapr entry;
  int val;
};

static struct blocking_deque *static$ (dq);
static atomic_int static$ (produced);
static atomic_long static$ (sum);
static atomic_int static$ (consumed);

static void *
producer (void *arg)
{
  (void)arg;
  for (int i = 0; i < PER_PROD; ++i)
    {
      struct dqnode *n = malloc (sizeof (struct dqnode));
      n->val = 1;
      atomic_fetch_add (&static$ (produced), 1);
      $fn (blocking_deque, push) (static$ (dq), &n->entry);
    }
  return null;
}

static void *
consumer (void *arg)
{
  (void)arg;
  for (;;)
    {
      xapr *e = $fn (blocking_deque, shift) (static$ (dq));
      struct dqnode *n = container_of (e, struct dqnode, entry);
      if (n->val == -1) /* poison pill */
        {
          free (n);
          break;
        }
      atomic_fetch_add (&static$ (sum), n->val);
      atomic_fetch_add (&static$ (consumed), 1);
      free (n);
    }
  return null;
}

void
deque_concurrent ()
{
  suite$ ("deque: multi-producer/consumer");
  static$ (dq) = new$ (blocking_deque);
  check$ (static$ (dq) != null);
  if (!static$ (dq))
    return;
  atomic_store (&static$ (produced), 0);
  atomic_store (&static$ (sum), 0);
  atomic_store (&static$ (consumed), 0);

  pthread_t prod[NPROD], cons[NCONS];
  for (int i = 0; i < NCONS; ++i)
    pthread_create (&cons[i], null, consumer, null);
  for (int i = 0; i < NPROD; ++i)
    pthread_create (&prod[i], null, producer, null);

  for (int i = 0; i < NPROD; ++i)
    pthread_join (prod[i], null);

  /* one poison pill per consumer. */
  for (int i = 0; i < NCONS; ++i)
    {
      struct dqnode *p = malloc (sizeof (struct dqnode));
      p->val = -1;
      $fn (blocking_deque, push) (static$ (dq), &p->entry);
    }
  for (int i = 0; i < NCONS; ++i)
    pthread_join (cons[i], null);

  check$ (atomic_load (&static$ (produced)) == TOTAL);
  check$ (atomic_load (&static$ (consumed)) == TOTAL);
  check$ (atomic_load (&static$ (sum)) == TOTAL);

  delete$ (static$ (dq));
}

#else /* !_REENTRANT */

void
deque_order ()
{
}
void
deque_concurrent ()
{
}

#endif
