#pragma once
#ifndef _H_SYS_
#define _H_SYS_ 1
#include "cmacs.h"
#include <unistd.h>

unsigned nproc ();

void *static$ (close) (void *);
#define autofd defer$ (static$ (close))

#endif /* _H_SYS_ */
