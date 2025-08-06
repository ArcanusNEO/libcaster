#pragma once
#ifndef _H_GRIMOIRE_
#define _H_GRIMOIRE_ 1

#include <cmacs.h>
#include <pthread.h>

/****************************** blocking deque *******************************/

class$ (blocking_deque, {
  xapr *mbr$ (head);
  xapr *mbr$ (tail);
  pthread_mutex_t mbr$ (lock);
  pthread_cond_t mbr$ (cond);
});

xapr *$fx (blocking_deque, shift);
xapr *$fx (blocking_deque, pop);
int $fx (blocking_deque, unshift, xapr *node);
int $fx (blocking_deque, push, xapr *node);

static inline struct
{
  xapr *fx$ (shift);
  xapr *fx$ (pop);
  int fx$ (unshift, xapr *node);
  int fx$ (push, xapr *node);
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

struct blocking_deque *$fx (blocking_deque, init);

/****************************** blocking deque *******************************/

/******************************** thread pool ********************************/

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
int $fx (thrdpool, preempt, void (*fn) (void *), void *arg);
unsigned $fx (thrdpool, resize, unsigned size);

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

struct thrdpool *$fx (thrdpool, init);

/******************************** thread pool ********************************/

/******************************** event pool *********************************/

struct epool;

union endex
{
  void *ptr;
  long ident;

  long long idata;
  unsigned long long udata;
  long double fdata;
};

trait$ (epost, {
  union endex reserved;

  int ident;
  unsigned long long filter;
  union endex data;

  union endex context;
  union endex usr;
});

struct epool *epool ();
struct epost *$fx (epool, ethrow, struct epost *);
struct epost *$fx (epool, ecatch);
int $fx (epool, efinally, struct epost *);

static inline$ struct epost *
_S_ethrow_caller_ (struct epost *epost)
{
  return $fn (epool, ethrow) (*(void **)epost, epost);
}

static inline$ struct epost *
_S_ethrow_helper_ (struct epost epost)
{
  void *ret = malloc (sizeof (struct epost));
  if (unlikely (!ret))
    return null;
  memcpy (ret, &epost, sizeof (struct epost));
  return _S_ethrow_caller_ (ret);
}

#define ethrow(ep, ...)                                                       \
  head$ (__VA_OPT__ (_S_ethrow_helper_ ((struct epost){                       \
                         ep, .reserved = (union endex){ 0 },                  \
                         __VA_ARGS__ }), ) _S_ethrow_caller_ (ep))

static inline$ struct epost *
ecatch (struct epool *epool)
{
  return $fn (epool, ecatch) (epool);
}

static inline$ int
efinally (struct epost *epost)
{
  return $fn (epool, efinally) (*(void **)epost, epost);
}

#if (EPOOL_BACKEND + 0) == 1 || !defined EPOOL_BACKEND && defined __linux__
#include <sys/epoll.h>
#define EPFILT_READ EPOLLIN
#define EPFILT_WRITE EPOLLOUT
#elif (EPOOL_BACKEND + 0) == 2                                                \
    || !defined EPOOL_BACKEND                                                 \
           && (defined __FreeBSD__ || defined __NetBSD__                      \
               || defined __OpenBSD__ || defined __DragonFly__                \
               || defined __APPLE__)
#include <sys/event.h>
#define EPFILT_READ EVFILT_READ
#define EPFILT_WRITE EVFILT_WRITE
#elif (EPOOL_BACKEND + 0) == 3 || !defined EPOOL_BACKEND && defined _WIN32
#error Unsupported platform
#elif (EPOOL_BACKEND + 0) == 4 || !defined EPOOL_BACKEND && defined __sun
#error Unsupported platform
#else
#error Unknown platform
#endif

/******************************** event pool *********************************/

#endif /* _H_GRIMOIRE_ */
