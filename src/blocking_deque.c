#include "blocking_deque.h"

static struct blocking_deque *
$fx (blocking_deque, deinit)
{
  pthread_cond_destroy (&this->mbr$ (cond));
  pthread_mutex_destroy (&this->mbr$ (lock));
  xlsfree$ (&this->mbr$ (head), &this->mbr$ (tail));
  return this;
}

static void
mutex_unlock_wrapper (void *mutex)
{
  pthread_mutex_unlock (mutex);
}

#define genbdget(op)                                                          \
  xapr *$fx (blocking_deque, op)                                              \
  {                                                                           \
    xapr *ret;                                                                \
    do                                                                        \
      pthread_testcancel ();                                                  \
    while (unlikely (pthread_mutex_lock (&this->mbr$ (lock))));               \
    pthread_cleanup_push (mutex_unlock_wrapper, &this->mbr$ (lock));          \
    while (!this->mbr$ (head))                                                \
      pthread_cond_wait (&this->mbr$ (cond), &this->mbr$ (lock));             \
    ret = xlist$ (op) (&this->mbr$ (head), &this->mbr$ (tail));               \
    pthread_cleanup_pop (1);                                                  \
    return ret;                                                               \
  }
genbdget (shift);
genbdget (pop);
#undef genbdget

#define genbdput(op)                                                          \
  int $fx (blocking_deque, op, xapr *node)                                    \
  {                                                                           \
    if (unlikely (pthread_mutex_lock (&this->mbr$ (lock))))                   \
      return -1;                                                              \
    xlist$ (op) (node, &this->mbr$ (head), &this->mbr$ (tail));               \
    pthread_mutex_unlock (&this->mbr$ (lock));                                \
    pthread_cond_signal (&this->mbr$ (cond));                                 \
    return 0;                                                                 \
  }
genbdput (unshift);
genbdput (push);
#undef genbdput

struct blocking_deque *
$fx (blocking_deque, init)
{
  this->fn$ (deinit) = (void *)$fn (blocking_deque, deinit);
  this->mbr$ (head) = this->mbr$ (tail) = null;

  if (unlikely (pthread_mutex_init (&this->mbr$ (lock), null)))
    goto LOCK_INIT_FAILURE;
  if (unlikely (pthread_cond_init (&this->mbr$ (cond), null)))
    goto COND_INIT_FAILURE;

  return this;

  pthread_cond_destroy (&this->mbr$ (cond));
COND_INIT_FAILURE:
  pthread_mutex_destroy (&this->mbr$ (lock));
LOCK_INIT_FAILURE:
  return null;
}
