#pragma once
#ifndef _H_LOGGER_
#define _H_LOGGER_ 1
#include "cmacs.h"

#include <pthread.h>
extern pthread_mutex_t logger_info_lock;
extern pthread_mutex_t logger_error_lock;

#define log_info(...)                                                         \
  do                                                                          \
    {                                                                         \
      pthread_mutex_lock (&logger_info_lock);                                 \
      puts (__FILE__ ":");                                                    \
      puts (__func__);                                                        \
      puts (":" quote$ (__LINE__) ": ");                                      \
      cout (__VA_ARGS__);                                                     \
      pthread_mutex_unlock (&logger_info_lock);                               \
    }                                                                         \
  while (0)

#define log_error(...)                                                        \
  do                                                                          \
    {                                                                         \
      pthread_mutex_lock (&logger_error_lock);                                \
      puts (__FILE__ ":", stderr);                                            \
      puts (__func__, stderr);                                                \
      puts (":" quote$ (__LINE__) ": ", stderr);                              \
      cerr (__VA_ARGS__);                                                     \
      pthread_mutex_unlock (&logger_error_lock);                              \
    }                                                                         \
  while (0)

#define log_fatal(...)                                                        \
  do                                                                          \
    {                                                                         \
      log_error (__VA_ARGS__);                                                \
      abort ();                                                               \
    }                                                                         \
  while (0)

#define log_assert(...)                                                       \
  do                                                                          \
    {                                                                         \
      if (unlikely (!(__VA_ARGS__)))                                          \
        log_fatal ("Assertion `" quote$ (__VA_ARGS__) "' failed");            \
    }                                                                         \
  while (0)

#endif /* _H_LOGGER_ */
