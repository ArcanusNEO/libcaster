#include "binstore.h"

struct binstore *
$fx (binstore, resize, usz size)
{
  if (!this)
    {
      auto capacity = dynarr$ (0, size);
      this = malloc (sizeof (struct binstore) + capacity);
      if (unlikely (!this))
        return null;
      this->capacity = capacity;
      this->size = size;
      return this;
    }

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
      that->capacity = capacity;
      this = that;
    }
  this->size = size;
  return this;
}
