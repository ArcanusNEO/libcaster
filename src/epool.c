#include "epool.h"

#if (EPOOL_DRIVER + 0) == 0
#warning epool: function not implemented
#elif (EPOOL_DRIVER + 0) < 0
#warning epool: unknown platform
#elif (EPOOL_DRIVER + 0) == 1
#include "epoll.inc"
#elif (EPOOL_DRIVER + 0) == 2
#include "kqueue.inc"
#elif (EPOOL_DRIVER + 0) == 3
/* Reserved for Windows */
#elif (EPOOL_DRIVER + 0) == 4
/* Reserved for Solaris */
#endif
