#pragma once
#ifndef _H_GRIMOIRE_
#define _H_GRIMOIRE_ 1

#include <cmacs.h>
#include <unistd.h>
#include <pthread.h>

#ifdef _REENTRANT

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

#endif /* _REENTRANT */

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

#endif /* (EPOOL_DRIVER + 0) > 0 */

/******************************** event pool *********************************/

/********************************** clogger **********************************/

enum clogger_level
{
  TRACE,
  DEBUG,
  INFO,
  WARN,
  ERROR,
  ASSERT,
  FATAL,
  NONE,
};

extern struct clogger_level_config
{
  FILE *mbr$ (stream);
  pthread_mutex_t *mbr$ (lock);
} static$ (clogger_level_config)[];

#define _P_clogger_(a)                                                        \
  choose$ (ipe$ (a), ({                                                       \
             puts (choose$ (ipe$ (a), a, ""), _M_ustream_);                   \
             (void)0;                                                         \
           }),                                                                \
           ({                                                                 \
             puti (choose$ (ipe$ (a), 0, a), _M_ustream_);                    \
             (void)0;                                                         \
           }))
#define _P_clogger_helper_(a, b)                                              \
  a;                                                                          \
  putchar (' ', _M_ustream_);                                                 \
  _P_clogger_ (b);
#define _P_clogger_n_(a, ...)                                                 \
  do                                                                          \
    {                                                                         \
      __VA_OPT__ ($reduce)                                                    \
      (__VA_OPT__ (_P_clogger_helper_, ) _P_clogger_ (a) __VA_OPT__ (, )      \
           __VA_ARGS__);                                                      \
    }                                                                         \
  while (0)
#define _P_clogger_do_log_(ustream, level, ...)                               \
  do                                                                          \
    {                                                                         \
      puts ("[" quote$ (level) "] " __FILE__ ":" quote$ (__LINE__) ":",       \
            ustream);                                                         \
      puts (__func__, ustream);                                               \
      puts (": ", ustream);                                                   \
      __VA_OPT__ (auto _M_ustream_ = ustream);                                \
      __VA_OPT__ (_P_clogger_n_ (__VA_ARGS__));                               \
      putchar (EOL, ustream);                                                 \
    }                                                                         \
  while (0)

int $fn (clogger, update_level_lock) (enum clogger_level level);

#ifdef _REENTRANT
#define _P_clogger_lock_(level)                                               \
  pthread_mutex_lock (static$ (clogger_level_config)[level].mbr$ (lock))
#define _P_clogger_unlock_(level)                                             \
  pthread_mutex_unlock (static$ (clogger_level_config)[level].mbr$ (lock))
#define _P_pthread_mutex_lock_(mutex) pthread_mutex_lock (mutex)
#define _P_pthread_mutex_unlock_(mutex) pthread_mutex_unlock (mutex)
#define _P_clogger_update_lock_(level) $fn (clogger, update_level_lock) (level)
#else
#define _P_clogger_lock_(level)                                               \
  ({                                                                          \
    (void)(level);                                                            \
    0;                                                                        \
  })
#define _P_clogger_unlock_(level)                                             \
  ({                                                                          \
    (void)(level);                                                            \
    0;                                                                        \
  })
#define _P_pthread_mutex_lock_(mutex)                                         \
  ({                                                                          \
    (void)(mutex);                                                            \
    0;                                                                        \
  })
#define _P_pthread_mutex_unlock_(mutex)                                       \
  ({                                                                          \
    (void)(mutex);                                                            \
    0;                                                                        \
  })
#define _P_clogger_update_lock_(level)                                        \
  ({                                                                          \
    (void)(level);                                                            \
    0;                                                                        \
  })

#endif

#define _P_clogger_worker_(ust, level, ...)                                   \
  do                                                                          \
    {                                                                         \
      if (level == NONE)                                                      \
        break;                                                                \
      _P_clogger_lock_ (level);                                               \
      void *ust = static$ (clogger_level_config)[level].mbr$ (stream);        \
      if (ust == &static$ (clogger_level_config)[level])                      \
        {                                                                     \
          _P_clogger_unlock_ (level);                                         \
          break;                                                              \
        }                                                                     \
      if (!ust)                                                               \
        ust = static$ (clogger_level_config)[level].mbr$ (stream) = stderr;   \
      if (level == ASSERT)                                                    \
        {                                                                     \
          if (likely (head$ (__VA_ARGS__ __VA_OPT__ (, ) 1)))                 \
            {                                                                 \
              _P_clogger_do_log_ (                                            \
                  ust, level,                                                 \
                  "Assertion `" quote$ (head$ (__VA_ARGS__)) "' passed");     \
            }                                                                 \
          else                                                                \
            {                                                                 \
              _P_clogger_do_log_ (                                            \
                  ust, level,                                                 \
                  "Assertion `" quote$ (head$ (__VA_ARGS__)) "' failed");     \
              _P_clogger_unlock_ (level);                                     \
              abort ();                                                       \
            }                                                                 \
        }                                                                     \
      else                                                                    \
        _P_clogger_do_log_ (ust, level __VA_OPT__ (, ) __VA_ARGS__);          \
      _P_clogger_unlock_ (level);                                             \
      if (unlikely (level == FATAL))                                          \
        abort ();                                                             \
    }                                                                         \
  while (0)
#define clogger(level, ...)                                                   \
  ({                                                                          \
    _P_clogger_worker_ (uniq$ (ust), level __VA_OPT__ (, ) __VA_ARGS__);      \
    (void)0;                                                                  \
  })

#define _P_clgredir_(us, ulock, level, astream)                               \
  do                                                                          \
    {                                                                         \
      if (level == NONE)                                                      \
        break;                                                                \
      void *us = astream;                                                     \
      if (!us)                                                                \
        us = &static$ (clogger_level_config)[level];                          \
      auto ulock = static$ (clogger_level_config)[level].mbr$ (lock);         \
      _P_pthread_mutex_lock_ (ulock);                                         \
      static$ (clogger_level_config)[level].mbr$ (stream) = us;               \
      _P_clogger_update_lock_ (level);                                        \
      _P_pthread_mutex_unlock_ (ulock);                                       \
    }                                                                         \
  while (0)
#define clgredir(level, stream)                                               \
  _P_clgredir_ (uniq$ (us), uniq$ (ulock), level, stream)

/********************************** clogger **********************************/

/************************************ sys ************************************/

unsigned nproc ();

void *static$ (close) (void *);
#define autofd defer$ (static$ (close))

/************************************ sys ************************************/

/********************************* binstore **********************************/

typedef struct binstore
{
  usz size;
  usz capacity;
  byte store[0];
} bsto;

struct binstore *$fx (binstore, resize, usz size);

/********************************* binstore **********************************/

#endif /* _H_GRIMOIRE_ */
