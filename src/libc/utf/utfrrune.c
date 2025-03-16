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

char* UtfRRune(const char* s, Rune r) {
  Rune r1;
  uint8_t c;
  int n;
  const char* s1;

  if (r < RuneSync) {
    // not part of utf sequence
    return strrchr(s, r);
  }

  s1 = 0;
  for (;;) {
    c = *(uint8_t*)s;
    if (c < RuneSelf) {
      if (c == 0) {
        return (char*)s1;
      }
      if (c == r) {
        s1 = s;
      }
      s++;
      continue;
    }
    n = CharToRune(&r1, s);
    if (r1 == r) {
      s1 = s;
    }
    s += n;
  }

  return 0;
}
