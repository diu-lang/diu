// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#include <assert.h>

#include "utf.h"
#include "utfdef.h"

int UtfNLen(const char* s, long m) {
  int n = 0;
  Rune r;
  const char* es = s + m;

  for (; s < es; n++) {
    uint8_t c = *(uint8_t*)s;
    if (c < RuneSelf) {
      if (c == 0) {
        break;
      }
      s++;
      continue;
    }

    if (!FullRune(s, es - s)) {
      break;
    }
    s += CharToRune(&r, s);
  }

  return n;
}