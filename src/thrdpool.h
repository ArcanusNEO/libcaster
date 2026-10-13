#pragma once
#ifndef _H_THRDPOOL_
#define _H_THRDPOOL_ 1

#include "cmacs.h"
#include "blocking_deque.h"

class$ (thrdpool, {
  struct blocking_deque mbr$ (exec_deque);
  struct blocking_deque mbr$ (cancel_deque);
  pthread_t mbr$ (cancel_thread);
  pthread_t *mbr$ (tid);
  xapr mbr$ (tail);
  unsigned size;
});

#define THRDPOOL_TASK_ATTR_DYNAMIC 1
struct thrdtask
{
  xapr list_entry;
  word attr;
  void *arg;
  void (*fn) (void *);
};

int $fx (thrdpool, exec, void (*fn) (void *), void *arg);
#define thrdpool_exec $fn (thrdpool, exec)
int $fx (thrdpool, preempt, void (*fn) (void *), void *arg);
#define thrdpool_preempt $fn (thrdpool, preempt)
unsigned $fx (thrdpool, resize, unsigned size);
#define thrdpool_resize $fn (thrdpool, resize)

static inline struct
{
  int fx$ (exec, void (*fn) (void *), void *arg);
  int fx$ (preempt, void (*fn) (void *), void *arg);
  unsigned fx$ (resize, unsigned size);
} *
$fn (thrdpool, vtab) ()
{
  static typeof (*$fn (thrdpool, vtab) ()) vtab = {
    .fn$ (exec) = (void *)$fn (thrdpool, exec),
    .fn$ (preempt) = (void *)$fn (thrdpool, preempt),
    .fn$ (resize) = (void *)$fn (thrdpool, resize),
  };
  return &vtab;
}
#define thrdpool_vtab $fn (thrdpool, vtab)

struct thrdpool *$fx (thrdpool, init);
#define thrdpool_init $fn (thrdpool, init)

#endif /* _H_THRDPOOL_ */
