#include "grimoire.h"

#define hello "Hello, world!"

char greet[] = hello;
void *ptr = greet;

void
test_hash ()
{
  clogger (ASSERT, hash$ (greet) == hash$ (hello));
  clogger (ASSERT,
           hash$ (greet, sizeof (greet), 69) == hash$ (hello, .salt = 69));
  clogger (ASSERT, hash$ (greet, 1, 7777777) == hash$ (ptr, .salt = 7777777));
  clogger (INFO, "hash$ (" hello "):", hash$ (hello));
  auto foo = htab$ ();
  htab$ (resiz) (foo, 30);
  /* htab$ (find) (foo, null); */
}
