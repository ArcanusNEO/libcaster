#pragma once
#ifndef _H_CTEST_
#define _H_CTEST_ 1
#include "cmacs.h"
#include "grimoire.h"

extern int static$ (ctest_fails);

#define suite$(name) clogger (INFO, "suite:", name)

#define _P_check_worker_(cond, expr)                                          \
  do                                                                          \
    {                                                                         \
      if (likely (cond))                                                      \
        clogger (TRACE, "[PASS]", expr);                                      \
      else                                                                    \
        {                                                                     \
          ++static$ (ctest_fails);                                            \
          clogger (ERROR, "[FAIL]", expr);                                    \
        }                                                                     \
    }                                                                         \
  while (0)
#define check$(...) _P_check_worker_ ((__VA_ARGS__), #__VA_ARGS__)

#endif /* _H_CTEST_ */
