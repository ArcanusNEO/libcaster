#pragma once
#ifndef _H_BINSTORE_
#define _H_BINSTORE_ 1
#include "cmacs.h"

typedef struct binstore
{
  usz size;
  usz capacity;
  byte store[0];
} bsto;

struct binstore *$fx (binstore, resize, usz size);

#endif /* _H_BINSTORE_ */
