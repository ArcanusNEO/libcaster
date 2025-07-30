#include "epool.h"

#if (EPOOL_BACKEND + 0) == 1 || !defined EPOOL_BACKEND && defined __linux__
#include "epoll.inc"
#elif (EPOOL_BACKEND + 0) == 2                                                \
    || !defined EPOOL_BACKEND                                                 \
           && (defined __FreeBSD__ || defined __NetBSD__                      \
               || defined __OpenBSD__ || defined __DragonFly__                \
               || defined __APPLE__)
#include "kqueue.inc"
#elif (EPOOL_BACKEND + 0) == 3 || !defined EPOOL_BACKEND && defined _WIN32
#error Unsupported platform
#elif (EPOOL_BACKEND + 0) == 4 || !defined EPOOL_BACKEND && defined __sun
#error Unsupported platform
#else
#error Unknown platform
#endif
