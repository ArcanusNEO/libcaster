#pragma once
#ifndef _H_GRIMOIRE_
#define _H_GRIMOIRE_ 1

#include <cmacs.h>
#include <pthread.h>
#include <unistd.h>

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

#if (EPOOL_DRIVER + 0) == 1 || !defined EPOOL_DRIVER && defined __linux__
#undef EPOOL_DRIVER
#define EPOOL_DRIVER 1

#include <sys/epoll.h>
#define EPFILT_READ EPOLLIN
#define EPFILT_WRITE EPOLLOUT

#elif (EPOOL_DRIVER + 0) == 2                                                 \
    || !defined EPOOL_DRIVER                                                  \
           && (defined __FreeBSD__ || defined __NetBSD__                      \
               || defined __OpenBSD__ || defined __DragonFly__                \
               || defined __APPLE__)
#undef EPOOL_DRIVER
#define EPOOL_DRIVER 2

#include <sys/event.h>
#define EPFILT_READ EVFILT_READ
#define EPFILT_WRITE EVFILT_WRITE

#elif (EPOOL_DRIVER + 0) == 3 || !defined EPOOL_DRIVER && defined _WIN32
#undef EPOOL_DRIVER
#define EPOOL_DRIVER 0

#elif (EPOOL_DRIVER + 0) == 4 || !defined EPOOL_DRIVER && defined __sun
#undef EPOOL_DRIVER
#define EPOOL_DRIVER 0

#else
#undef EPOOL_DRIVER
#define EPOOL_DRIVER -1

#endif

#if (EPOOL_DRIVER + 0) > 0

struct epool;

union endex
{
  int ident;
  void *ptr;

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
int $fx (epool, ethrow, struct epost *epost);
struct epost *$fx (epool, ecatch);
int $fx (epool, efinally, struct epost *epost);

static inline$ struct epost *
_S_ethrow_caller_ (struct epost *epost)
{
  if (unlikely ($fn (epool, ethrow) (*(void **)epost, epost) < 0))
    return null;
  return epost;
}

static inline$ struct epost *
_S_ethrow_helper_ (struct epost epost)
{
  void *ret = malloc (sizeof (struct epost));
  if (unlikely (!ret))
    return null;
  memcpy (ret, &epost, sizeof (struct epost));
  if (unlikely (!_S_ethrow_caller_ (ret)))
    {
      free (ret);
      return null;
    }
  return ret;
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

#endif

/******************************** event pool *********************************/

/********************************** logger ***********************************/

extern pthread_mutex_t logger_info_lock;
extern pthread_mutex_t logger_error_lock;

#define log_info(...)                                                         \
  do                                                                          \
    {                                                                         \
      pthread_mutex_lock (&logger_info_lock);                                 \
      puts (__FILE__ ":");                                                    \
      puts (__func__);                                                        \
      puts (":" quote$ (__LINE__) ": ");                                      \
      cout (__VA_ARGS__);                                                     \
      pthread_mutex_unlock (&logger_info_lock);                               \
    }                                                                         \
  while (0)

#define log_error(...)                                                        \
  do                                                                          \
    {                                                                         \
      pthread_mutex_lock (&logger_error_lock);                                \
      puts (__FILE__ ":", stderr);                                            \
      puts (__func__, stderr);                                                \
      puts (":" quote$ (__LINE__) ": ", stderr);                              \
      cerr (__VA_ARGS__);                                                     \
      pthread_mutex_unlock (&logger_error_lock);                              \
    }                                                                         \
  while (0)

#define log_fatal(...)                                                        \
  do                                                                          \
    {                                                                         \
      log_error (__VA_ARGS__);                                                \
      abort ();                                                               \
    }                                                                         \
  while (0)

#define log_assert(...)                                                       \
  do                                                                          \
    {                                                                         \
      if (unlikely (!(__VA_ARGS__)))                                          \
        log_fatal ("Assertion `" quote$ (__VA_ARGS__) "' failed");            \
    }                                                                         \
  while (0)

/********************************** logger ***********************************/

/************************************ sys ************************************/

unsigned nproc ();

void *static$ (close) (void *);
#define autofd defer$ (static$ (close))

/************************************ sys ************************************/

/********************************* binstore **********************************/

struct binstore
{
  usz size;
  usz capacity;
  byte store[0];
};

struct binstore *$fx (binstore, resize, usz size);

/********************************* binstore **********************************/

#endif /* _H_GRIMOIRE_ */
