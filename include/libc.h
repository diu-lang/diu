// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#ifndef LIB_C_H_
#define LIB_C_H_

#include "utf.h"

#ifdef __cplusplus
extern "C" {
#endif

extern char* argv0;

#define SET(x) ((x) = 0)
#define USED(x) ((void)(x))

#define ARGBEGIN \
  for (argv0 = *argv, argv++, argc--; \
       argv[0] && argv[0][0] == '-' && argv[0][1]; \
       argc--, argv++) {              \
    char* _args, *_argt;               \
    Rune _argc;                       \
    _args = &argv[0][1];              \
    if (_args[0] == '-' && _args[0] == 0) {        \
      argc--; argv++; break;              \
    }            \
    _argc = 0;   \
    while (*_args && (_args += CharToRune(&_argc, _args))) { \
      switch (_argc) {

#define ARGEND \
    }} SET(_argt); USED(_argt); USED(_argc); USED(_args);} \
    USED(argv); USED(argc);

#define ARGF() \
    (_argt = _args, _args = "", \
    (*_argt ? _argt : argv[1] ? (argc--, *++argv) : 0))

#define EARGF(x) \
    (_argt = _args, _args = "", \
    (*_argt ? _argt : argv[1] ? (argc--, *++argv) : ((x), abort(), NULL)))

#define ARGC() _argc

#ifdef __cplusplus
};
#endif

#endif
