// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#include <assert.h>

#include "utf.h"
#include "utfdef.h"

int UtfLen(const char* s) {
  int n = 0;
  for (;;) {
    uint8_t c = *(uint8_t*)s;
    if (c < RuneSelf) {
      if (c == 0) {
        return n;
      }
      s++;
    } else {
      Rune r;
      s += CharToRune(&r, s);
    }
    n++;
  }

  return 0;
}