#include "hash.h"
#include "a5hash.h"

u64
_S_hash_ (struct _S_hash_ arg)
{
  return a5hash (arg.msg, arg.len, arg.salt);
}
