#pragma once
#ifndef _H_BINSTORE_
#define _H_BINSTORE_ 1
#include "cmacs.h"

struct binstore
{
  union
  {
    struct
    {
      usz capacity;
      byte *store;
    };
    byte embed[sizeof (usz) + sizeof (byte *)];
  };
  usz size;
};

#endif /* _H_BINSTORE_ */
