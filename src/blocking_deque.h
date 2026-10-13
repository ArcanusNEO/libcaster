#pragma once
#ifndef _H_BLOCKING_DEQUE_
#define _H_BLOCKING_DEQUE_ 1

#include "cmacs.h"
#include <pthread.h>

class$ (blocking_deque, {
  xapr *mbr$ (head);
  xapr *mbr$ (tail);
  pthread_mutex_t mbr$ (lock);
  pthread_cond_t mbr$ (cond);
});

xapr *$fx (blocking_deque, shift);
#define blocking_deque_shift $fn (blocking_deque, shift)
xapr *$fx (blocking_deque, pop);
#define blocking_deque_pop $fn (blocking_deque, pop)
void $fx (blocking_deque, unshift, xapr *node);
#define blocking_deque_unshift $fn (blocking_deque, unshift)
void $fx (blocking_deque, push, xapr *node);
#define blocking_deque_push $fn (blocking_deque, push)

static inline struct
{
  xapr *fx$ (shift);
  xapr *fx$ (pop);
  void fx$ (unshift, xapr *node);
  void fx$ (push, xapr *node);
} *
$fn (blocking_deque, vtab) ()
{
  static typeof (*$fn (blocking_deque, vtab) ()) vtab = {
    .fn$ (shift) = (void *)$fn (blocking_deque, shift),
    .fn$ (pop) = (void *)$fn (blocking_deque, pop),
    .fn$ (unshift) = (void *)$fn (blocking_deque, unshift),
    .fn$ (push) = (void *)$fn (blocking_deque, push),
  };
  return &vtab;
}
#define blocking_deque_vtab $fn (blocking_deque, vtab)

struct blocking_deque *$fx (blocking_deque, init);
#define blocking_deque_init $fn (blocking_deque, init)

#endif /* _H_BLOCKING_DEQUE_ */
