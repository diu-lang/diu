// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#ifndef U_H_
#define U_H_ 1

#if defined(__cplusplus)
extern "C" {
#endif

#define _LARGEFILE64_SOURCE 1
#define _FILE_OFFSET_BITS   64

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * OS-specific crap
 */
#if defined(__linux__)
# include <sys/types.h>
# include <pthread.h>
#elif defined(__APPLE__)
# include <sys/types.h>
# include <pthread.h>
# undef _ANSI_SOURCE
# undef _POSIX_C_SOURCE
# undef _XOPEN_SOURCE
# if !defined(NSIG)
#   define NSIG 32
# endif
# define _NEEDLL 1
#else
  /* No idea what system this is -- try some defaults */
# include <pthread.h>
#endif

#if defined(__cplusplus)
}
#endif
#endif

