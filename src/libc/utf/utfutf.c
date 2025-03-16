// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#include "utf.h"

#include <assert.h>
#include <string.h>

#include "utfdef.h"

/**
 * Return pointer to first occurrence of s2 in s1,
 * 0 if none.
 */
const char* UtfUtf(const char* s1, const char* s2) {
  const char* p;
  Rune r, f, n1, n2;

  n1 = CharToRune(&r, s2);
  f = r;
  if (f <= RuneSync) {  // represents self
    return strstr(s1, s2);
  }

  n2 = strlen(s2);
  for (p = s1; (p = UtfRune(p, f)) != 0; p += n1) {
    if (strncmp(p, s2, n2) == 0) {
      return p;
    }
  }

  return 0;
}
