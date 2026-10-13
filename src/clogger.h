#pragma once
#ifndef _H_CLOGGER_
#define _H_CLOGGER_ 1
#include "cmacs.h"

#define ERROR 0
#define WARN 1
#define INFO 2
#define DEBUG 3
#define TRACE 4
#define FATAL 5
#define ASSERT 6
#define NONE 7

extern char const *static$ (clogger_level_string)[];
extern FILE *atomic static$ (clogger_level_stream)[];

#ifdef _REENTRANT
#if _P_PLATFORM_ == (_P_WINDOWS_ + 0)
#define _P_clogger_lock_ _lock_file
#define _P_clogger_unlock_ _unlock_file
#else
#define _P_clogger_lock_ flockfile
#define _P_clogger_unlock_ funlockfile
#endif
#else
#define _P_clogger_lock_ (void)
#define _P_clogger_unlock_ (void)
#endif

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
  putc (' ', _M_ustream_);                                                    \
  _P_clogger_ (b);
#define _P_clogger_n_(a, ...)                                                 \
  do                                                                          \
    {                                                                         \
      __VA_OPT__ ($reduce)                                                    \
      (__VA_OPT__ (_P_clogger_helper_, ) _P_clogger_ (a) __VA_OPT__ (, )      \
           __VA_ARGS__);                                                      \
    }                                                                         \
  while (0)
#define _P_clogger_do_log_(stream, level, ...)                                \
  do                                                                          \
    {                                                                         \
      _P_clogger_lock_ (stream);                                              \
      putc ('[', stream);                                                     \
      puts (static$ (clogger_level_string)[level], stream);                   \
      puts ("] " __FILE__ ":" quote$ (__LINE__) ":", stream);                 \
      puts (__func__, stream);                                                \
      putc (':', stream);                                                     \
      puti (timestamp$ (CLOCK_REALTIME), stream);                             \
      puts (": ", stream);                                                    \
      __VA_OPT__ (auto _M_ustream_ = stream);                                 \
      __VA_OPT__ (_P_clogger_n_ (__VA_ARGS__));                               \
      putc (EOL, stream);                                                     \
      _P_clogger_unlock_ (stream);                                            \
    }                                                                         \
  while (0)

#define _P_clogger_worker_(ulv, ust, level, ...)                              \
  do                                                                          \
    {                                                                         \
      auto ulv = level;                                                       \
      if (ulv == NONE)                                                        \
        break;                                                                \
      void *ust = static$ (clogger_level_stream)[ulv];                        \
      if (!ust)                                                               \
        break;                                                                \
      if (ulv == ASSERT)                                                      \
        {                                                                     \
          if (unlikely (!(head$ (__VA_ARGS__))))                              \
            {                                                                 \
              _P_clogger_do_log_ (                                            \
                  ust, ulv,                                                   \
                  "Assertion `" quote$ (head$ (__VA_ARGS__)) "' failed");     \
              abort ();                                                       \
            }                                                                 \
        }                                                                     \
      else                                                                    \
        _P_clogger_do_log_ (ust, ulv __VA_OPT__ (, ) __VA_ARGS__);            \
      if (unlikely (ulv == FATAL))                                            \
        abort ();                                                             \
    }                                                                         \
  while (0)

#define clogger(level, ...)                                                   \
  ({                                                                          \
    _P_clogger_worker_ (uniq$ (ulv), uniq$ (ust),                             \
                        level __VA_OPT__ (, ) __VA_ARGS__);                   \
    (void)0;                                                                  \
  })

#define _P_clgredir_(ulv, level, stream)                                      \
  do                                                                          \
    {                                                                         \
      auto ulv = level;                                                       \
      if (ulv < NONE)                                                         \
        static$ (clogger_level_stream)[ulv] = stream;                         \
    }                                                                         \
  while (0)
#define clgredir(level, stream) _P_clgredir_ (uniq$ (ulv), level, stream)

#endif /* _H_CLOGGER_ */
