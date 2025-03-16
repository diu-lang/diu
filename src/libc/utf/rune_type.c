// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#include "utf.h"
#include "utf-internal.h"
#include "utfdef.h"

Rune* rbsearch(Rune c, Rune* t, int n, int ne) {
  Rune* p;
  int m;

  while (n > 1) {
    m = n >> 1;
    p = t + m * ne;
    if (c >= p[0]) {
      t = p;
      n = n - m;
    } else {
      n = m;
    }
  }

  if (n && c >= t[0]) {
    return t;
  }
  return 0;
}

/**
 * The "ideographic" property is hard to extract from UnicodeData.txt,
 * so it is hard coded here.
 *
 * It is defined in the Unicode PropList.txt file, for example
 * PropList-4.1.0.txt. Unlike the UnicodeData.txt file, the format of
 * PropList changes between versions. This property appears relatively
 * static.
 *
 * It is the same in version 4.0.1, except that version defines some >16
 * bit chars as ideographic as well: 20000..2a6d6, and 2f800..2fa1d.
 */
static Rune __isideographicr[] = {
    0x3006, 0x3007,   // 3006 not in Unicode 2, in 2.1
    0x3021, 0x3029,
    0x3038, 0x303a,   // not in Unicode 2 or 2.1
    0x3400, 0x4db5,   // not in Unicode 2 or 2.1
    0x4e00, 0x9fbb,   // 0x9FA6..0x9FBB added for 4.1.0?
    0xf900, 0xfa2d,
    0x20000, 0x2a6d6,
    0x2f800, 0x2fa1d,
};

int IsIdeographicRune(Rune c) {
  Rune* p = rbsearch(c, __isideographicr, nelem(__isideographicr) / 2, 2);
  if (p && c >= p[0] && c <= p[1]) {
    return 1;
  }
  return 0;
}