#pragma once
#ifndef _H_CLOGGER_
#define _H_CLOGGER_ 1
#include "cmacs.h"
#include <pthread.h>

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

#endif /* _H_CLOGGER_ */
