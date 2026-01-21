#include "grimoire.h"

extern void foo ();

int
main (int argc, char *argv[])
{
  clogger (INFO, "Test begin:", "Hello, world!");
  clogger (NONE, "foobar");
  clgredir (INFO, stdout);
  foo ();
  clogger (ERROR, "Error message:", "argv[0] =", argv[0]);
  clogger (DEBUG, "Debug begin");
  clogger (ASSERT, !0, !!0, 1, 2);
  clogger (INFO, "ASSERT:", quote$ (!0, !!0, 1, 2));
  clogger (ASSERT);
  clogger (INFO, "ASSERT:", quote$ ());
  clgredir (ASSERT, null);
  /* clogger (FATAL, "FATAL!"); */
  clgredir (FATAL, null);
  clogger (ASSERT, 0);
  clogger (FATAL, "FATAL:", "NOT FATAL!");
  clogger (DEBUG, "argc =", argc, "argv =", (uptr)&argv);
}
