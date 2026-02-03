#include "cmacs.h"
#include "epool.h"
#include <unistd.h>
#include <sys/epoll.h>

#define EPOST_REGISTERED 1

#ifndef EPOOL_CATCH_BUFFER_SIZE
/* default 512 KiB */
#define EPOOL_CATCH_BUFFER_SIZE 524288
#endif

class$ (epool, {
  int ident;
  usz cbc;
  usz cbi;
  struct epoll_event
      catchbuf[EPOOL_CATCH_BUFFER_SIZE / sizeof (struct epoll_event)];
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
  auto op = registered ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
  struct epoll_event ev
      = { .events = EPOLLONESHOT | epost->filter, .data.ptr = epost };
  epost->reserved.udata |= EPOST_REGISTERED;
  if (unlikely (epoll_ctl (this->ident, op, epost->ident, &ev) < 0))
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
      if (unlikely ((this->cbc = epoll_wait (this->ident, this->catchbuf,
                                             countof (this->catchbuf), -1))
                    <= 0))
        return null;
    }
  auto ev = this->catchbuf + this->cbi++;
  struct epost *ep = ev->data.ptr;
  *(void **)ep = this;
  ep->data.udata = ev->events;
  return ep;
}

int
$fx (epool, efinally, struct epost *epost)
{
  if (!epost || !(epost->reserved.udata & EPOST_REGISTERED))
    return 0;

  if (unlikely (epoll_ctl (this->ident, EPOLL_CTL_DEL, epost->ident, null)
                < 0))
    return -1;

  epost->reserved.udata &= ~EPOST_REGISTERED;
  return 1;
}

struct epool *
$fx (epool, init)
{
  this->fn$ (deinit) = (void *)$fn (epool, deinit);
  this->cbc = this->cbi = 0;

  int epfd = epoll_create1 (EPOLL_CLOEXEC);
  if (unlikely (epfd < 0))
    return null;
  this->ident = epfd;
  return this;
}

struct epool *
epool ()
{
  return new$ (epool);
}
