#pragma once
#ifndef _H_EPOOL_
#define _H_EPOOL_ 1
#include "cmacs.h"

struct epool;

union endex
{
  void *ptr;
  long ident;

  long long idata;
  unsigned long long udata;
  long double fdata;
};

trait$ (epost, {
  union endex reserved;

  int ident;
  unsigned long long filter;
  union endex data;

  union endex context;
  union endex usr;
});

struct epool *epool ();
int $fx (epool, ethrow, struct epost *epost);
struct epost *$fx (epool, ecatch);
int $fx (epool, efinally, struct epost *epost);

static inline$ struct epost *
_S_ethrow_caller_ (struct epost *epost)
{
  if (unlikely ($fn (epool, ethrow) (*(void **)epost, epost) < 0))
    return null;
  return epost;
}

static inline$ struct epost *
_S_ethrow_helper_ (struct epost epost)
{
  void *ret = malloc (sizeof (struct epost));
  if (unlikely (!ret))
    return null;
  memcpy (ret, &epost, sizeof (struct epost));
  if (unlikely (!_S_ethrow_caller_ (ret)))
    {
      free (ret);
      return null;
    }
  return ret;
}

#define ethrow(ep, ...)                                                       \
  head$ (__VA_OPT__ (_S_ethrow_helper_ ((struct epost){                       \
                         ep, .reserved = (union endex){ 0 },                  \
                         __VA_ARGS__ }), ) _S_ethrow_caller_ (ep))

static inline$ struct epost *
ecatch (struct epool *epool)
{
  return $fn (epool, ecatch) (epool);
}

static inline$ int
efinally (struct epost *epost)
{
  return $fn (epool, efinally) (*(void **)epost, epost);
}

#if (EPOOL_BACKEND + 0) == 1 || !defined EPOOL_BACKEND && defined __linux__
#include <sys/epoll.h>
#define EPFILT_READ EPOLLIN
#define EPFILT_WRITE EPOLLOUT
#elif (EPOOL_BACKEND + 0) == 2                                                \
    || !defined EPOOL_BACKEND                                                 \
           && (defined __FreeBSD__ || defined __NetBSD__                      \
               || defined __OpenBSD__ || defined __DragonFly__                \
               || defined __APPLE__)
#include <sys/event.h>
#define EPFILT_READ EVFILT_READ
#define EPFILT_WRITE EVFILT_WRITE
#elif (EPOOL_BACKEND + 0) == 3 || !defined EPOOL_BACKEND && defined _WIN32
#error Unsupported platform
#elif (EPOOL_BACKEND + 0) == 4 || !defined EPOOL_BACKEND && defined __sun
#error Unsupported platform
#else
#error Unknown platform
#endif

#endif /* _H_EPOOL_ */
