#include "binstore.h"

struct binstore *
$fx (binstore, resize, usz size)
{
  if (size == this->size)
    return this;
  auto capacity = dynarr$ (this->capacity, size);
  if (capacity != this->capacity)
    {
      struct binstore *that
          = realloc (this, sizeof (struct binstore) + capacity);
      if (unlikely (!that))
        {
          this->size = min (size, this->capacity);
          return this;
        }
      this = that;
      that->capacity = capacity;
    }
  this->size = size;
  return this;
}
