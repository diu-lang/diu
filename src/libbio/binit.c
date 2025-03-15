// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#include <bio.h>

#include <stddef.h>
#include <stdlib.h>

enum {
  MAXBUFS = 20,
};

static Biobuf *wbufs[MAXBUFS];
static int at_exit_flag;

static void batexit() {
  Biobuf *bp;
  int i;

  for (i = 0; i < MAXBUFS; i++) {
    bp = wbufs[i];
    if (bp != NULL) {
      wbufs[i] = NULL;
      Bflush(bp);
    }
  }
}

static void uninstall(Biobuf *bp) {
  int i;

  for (i = 0; i < MAXBUFS; i++) {
    if (wbufs[i] == bp) {
      wbufs[i] = NULL;
    }
  }
}

static void install(Biobuf *bp) {
  int i;

  uninstall(bp);
  for (i = 0; i < MAXBUFS; i++) {
    if (wbufs[i] == NULL) {
      wbufs[i] = bp;
      break;
    }
  }

  if (at_exit_flag == 0) {
    at_exit_flag = 1;
    atexit(batexit);
  }
}


