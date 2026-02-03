#include "cmacs.h"
#include "epool.h"
#include <unistd.h>
#include <sys/event.h>
#include <fcntl.h>

#define EPOST_REGISTERED 1

#ifndef EPOOL_CATCH_BUFFER_SIZE
/* default 512 KiB */
#define EPOOL_CATCH_BUFFER_SIZE 524288
#endif

class$ (epool, {
  int ident;
  usz cbc;
  usz cbi;
  struct kevent catchbuf[EPOOL_CATCH_BUFFER_SIZE / sizeof (struct kevent)];
});

static struct epool *
$fx (epool, deinit)
{
  close (this->ident);
  return this;
}

int
$fx (epool, ethrow, struct epost *epost)
{
  if (!epost)
    return 0;

  auto registered = epost->reserved.udata & EPOST_REGISTERED;
  struct kevent ev;
  EV_SET (&ev, epost->ident, epost->filter, EV_ADD | EV_ENABLE | EV_DISPATCH,
          0, 0, epost);
  epost->reserved.udata |= EPOST_REGISTERED;
  if (unlikely (kevent (this->ident, &ev, 1, null, 0, null) < 0))
    {
      epost->reserved.udata &= ~(EPOST_REGISTERED ^ registered);
      return -1;
    }

  return 1;
}

struct epost *
$fx (epool, ecatch)
{
  if (this->cbi >= this->cbc)
    {
      this->cbi = 0;
      if (unlikely ((this->cbc = kevent (this->ident, null, 0, this->catchbuf,
                                         countof (this->catchbuf), null))
                    <= 0))
        return null;
    }
  auto ev = this->catchbuf + this->cbi++;
  struct epost *ep = ev->udata;
  *(void **)ep = this;
  ep->data.udata = ev->flags;
  return ep;
}

int
$fx (epool, efinally, struct epost *epost)
{
  if (!epost || !(epost->reserved.udata & EPOST_REGISTERED))
    return 0;

  struct kevent ev;
  EV_SET (&ev, epost->ident, epost->filter, EV_DELETE, 0, 0, epost);
  if (unlikely (kevent (this->ident, &ev, 1, null, 0, null) < 0))
    return -1;

  epost->reserved.udata &= ~EPOST_REGISTERED;
  return 1;
}

struct epool *
$fx (epool, init)
{
  this->fn$ (deinit) = (void *)$fn (epool, deinit);
  this->cbc = this->cbi = 0;

  int kqfd = kqueue ();
  if (unlikely (kqfd < 0))
    return null;
  int flag = max (0, fcntl (kqfd, F_GETFD));
  if (!(flag & FD_CLOEXEC))
    fcntl (kqfd, F_SETFD, flag | FD_CLOEXEC);
  this->ident = kqfd;
  return this;
}

struct epool *
epool ()
{
  return new$ (epool);
}
