#include "sys.h"

unsigned
nproc ()
{
  auto np =
#if defined _WIN32
      GetActiveProcessorCount (ALL_PROCESSOR_GROUPS);
#else
      sysconf (_SC_NPROCESSORS_ONLN);
#endif
  return max (1, np);
}
