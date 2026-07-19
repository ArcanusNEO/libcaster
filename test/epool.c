#include "ctest.h"

#if (EPOOL_DRIVER + 0) > 0

#include <unistd.h>

void
epool_basic ()
{
  suite$ ("epool: throw/catch/finally on a pipe");
  struct epool *ep = epool ();
  check$ (ep != null);
  if (!ep)
    return;

  int fds[2];
  check$ (pipe (fds) == 0);

  /* Variadic ethrow form: first arg is the epool, which is stored into the
     epost's instance field so ecatch/efinally can find it. Returns a heap
     epost owned by the caller. */
  struct epost *post = ethrow (ep, .ident = fds[0], .filter = EPFILT_READ);
  check$ (post != null);
  if (!post)
    {
      close (fds[0]);
      close (fds[1]);
      delete$ (ep);
      return;
    }

  /* make it readable */
  check$ (write (fds[1], "x", 1) == 1);

  struct epost *got = ecatch (ep);
  check$ (got == post);

  /* drain and deregister */
  char buf[8];
  check$ (read (fds[0], buf, sizeof buf) == 1);
  check$ (efinally (post) == 1);
  /* second finally is a no-op (not registered) */
  check$ (efinally (post) == 0);

  free (post);
  close (fds[0]);
  close (fds[1]);
  delete$ (ep);
}

#else

void
epool_basic ()
{
}

#endif
