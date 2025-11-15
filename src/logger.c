#include "logger.h"

pthread_mutex_t stdout_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t stderr_lock = PTHREAD_MUTEX_INITIALIZER;
