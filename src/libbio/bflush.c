// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#include <bio.h>

int Bflush(Biobuf *bp) {
  int n, c;

  switch (bp->state) {
    case Bwactive:
      n = bp->bsize + bp->ocount;
      break;
    case Bracteof:
      break;
    case Bractive:
      break;
    default:
      return Beof;
  }
}