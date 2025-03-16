// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#include <assert.h>
#include <string.h>

#include "utf.h"
#include "utfdef.h"

char* UtfRune(const char* s, Rune r) {
  Rune r1;
  uint8_t c;
  int n;

  if (r < RuneSync) {
    // not part of utf sequence
    return strchr(s, r);
  }

  for (;;) {
    c = *(uint8_t*)s;
    if (c < RuneSelf) {
      if (c == 0) {
        return 0;
      }
      if (c == r) {
        return (char*)s;
      }
      s++;
      continue;
    }
    n = CharToRune(&r1, s);
    if (r1 == r) {
      return (char*)s;
    }
    s += n;
  }
}
