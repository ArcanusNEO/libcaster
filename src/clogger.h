#pragma once
#ifndef _H_CLOGGER_
#define _H_CLOGGER_ 1
#include "cmacs.h"

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
extern FILE *atomic static$ (clogger_level_stream)[];

#ifdef _REENTRANT
#define _P_clogger_lock_ flockfile
#define _P_clogger_unlock_ funlockfile
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
      _P_clogger_lock_ (ustream);                                             \
      puts ("[" quote$ (level) "] " __FILE__ ":" quote$ (__LINE__) ":",       \
            ustream);                                                         \
      puts (__func__, ustream);                                               \
      puts (": ", ustream);                                                   \
      __VA_OPT__ (auto _M_ustream_ = ustream);                                \
      __VA_OPT__ (_P_clogger_n_ (__VA_ARGS__));                               \
      putchar (EOL, ustream);                                                 \
      _P_clogger_unlock_ (ustream);                                           \
    }                                                                         \
  while (0)

#define _P_clogger_worker_(ust, level, ...)                                   \
  do                                                                          \
    {                                                                         \
      if (level == NONE)                                                      \
        break;                                                                \
      void *ust = static$ (clogger_level_stream)[level];                      \
      if (!ust)                                                               \
        break;                                                                \
      if (level == ASSERT)                                                    \
        {                                                                     \
          if (unlikely (!(head$ (__VA_ARGS__))))                              \
            {                                                                 \
              _P_clogger_do_log_ (                                            \
                  ust, level,                                                 \
                  "Assertion `" quote$ (head$ (__VA_ARGS__)) "' failed");     \
              abort ();                                                       \
            }                                                                 \
        }                                                                     \
      else                                                                    \
        _P_clogger_do_log_ (ust, level __VA_OPT__ (, ) __VA_ARGS__);          \
      if (unlikely (level == FATAL))                                          \
        abort ();                                                             \
    }                                                                         \
  while (0)

#define clogger(level, ...)                                                   \
  ({                                                                          \
    _P_clogger_worker_ (uniq$ (ust), level __VA_OPT__ (, ) __VA_ARGS__);      \
    (void)0;                                                                  \
  })

#define clgredir(level, stream)                                               \
  do                                                                          \
    {                                                                         \
      if (level == NONE)                                                      \
        break;                                                                \
      static$ (clogger_level_stream)[level] = stream;                         \
    }                                                                         \
  while (0)

#endif /* _H_CLOGGER_ */
