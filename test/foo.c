#include "grimoire.h"

void
foo ()
{
  clogger (TRACE, (uptr)static$ (clogger_level_config));
  clogger (WARN, "Warning!");
  clogger (ASSERT, (uptr)&foo);
  /* clogger (ASSERT, 0); */
  /* clogger (FATAL, (uptr)static$ (clogger_level_config)); */
}
