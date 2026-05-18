#pragma once
#ifndef _H_HASH_
#define _H_HASH_ 1
#include "cmacs.h"

struct _S_hash_
{
  void const *msg;
  usz len;
  u64 const salt;
};
u64 _S_hash_ (struct _S_hash_ arg);
#define hash$(m, ...)                                                         \
  _S_hash_ ((struct _S_hash_){                                                \
      .salt = 0,                                                              \
      .len = typematch$ (m, &(m)[0], sizeof (*m), sizeof (m)),                \
      .msg = m __VA_OPT__ (, ) __VA_ARGS__ })

#endif /* _H_HASH_ */
