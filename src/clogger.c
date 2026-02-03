#include "clogger.h"
#include <stdatomic.h>

static pthread_mutex_t clogger_level_lock[] = {
  [TRACE] = PTHREAD_MUTEX_INITIALIZER, [DEBUG] = PTHREAD_MUTEX_INITIALIZER,
  [INFO] = PTHREAD_MUTEX_INITIALIZER,  [WARN] = PTHREAD_MUTEX_INITIALIZER,
  [ERROR] = PTHREAD_MUTEX_INITIALIZER, [ASSERT] = PTHREAD_MUTEX_INITIALIZER,
  [FATAL] = PTHREAD_MUTEX_INITIALIZER, [NONE] = PTHREAD_MUTEX_INITIALIZER,
};

struct clogger_level_config static$ (clogger_level_config)[] = {
  [TRACE] = { .mbr$ (lock) = clogger_level_lock },
  [DEBUG] = { .mbr$ (lock) = clogger_level_lock },
  [INFO] = { .mbr$ (lock) = clogger_level_lock },
  [WARN] = { .mbr$ (lock) = clogger_level_lock },
  [ERROR] = { .mbr$ (lock) = clogger_level_lock },
  [ASSERT] = { .mbr$ (lock) = clogger_level_lock },
  [FATAL] = { .mbr$ (lock) = clogger_level_lock },
  [NONE] = { .mbr$ (stream) = (void *)&static$ (clogger_level_config)[NONE],
             .mbr$ (lock) = &clogger_level_lock[NONE], },
};

int
$fn (clogger, update_level_lock) (enum clogger_level level)
{
  if (level < TRACE || level >= NONE)
    return -1;
  auto st = static$ (clogger_level_config)[level].mbr$ (stream);
  if (!st)
    st = static$ (clogger_level_config)[level].mbr$ (stream) = stderr;
  auto j = level;
  for (int i = TRACE; i < NONE; ++i)
    {
      if (i == level)
        continue;
      if (st == static$ (clogger_level_config)[i].mbr$ (stream)
          || st == stderr && !static$ (clogger_level_config)[i].mbr$ (stream))
        {
          j = i;
          break;
        }
    }
  atomic_thread_fence (memory_order_release);
  static$ (clogger_level_config)[level].mbr$ (lock) = &clogger_level_lock[j];
  return 0;
}
