#include "clogger.h"

char const *static$ (clogger_level_string)[NONE]
    = { "ERROR", "WARN", "INFO", "DEBUG", "TRACE", "FATAL", "ASSERT" };
FILE *atomic static$ (clogger_level_stream)[NONE];

__attribute__ ((__constructor__)) static inline void
init ()
{
  static$ (clogger_level_stream)[FATAL] = stderr;
  static$ (clogger_level_stream)[ASSERT] = stderr;
  static$ (clogger_level_stream)[ERROR] = stderr;
  static$ (clogger_level_stream)[WARN] = stderr;
  static$ (clogger_level_stream)[INFO] = stderr;
  static$ (clogger_level_stream)[DEBUG] = stderr;
  static$ (clogger_level_stream)[TRACE] = stderr;
}
