#include "logger.h"

pthread_mutex_t logger_info_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t logger_error_lock = PTHREAD_MUTEX_INITIALIZER;
