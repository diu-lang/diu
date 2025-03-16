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

char* UtfECpy(char* to, char* e, const char* from) {
  char* end;

  if (to >= e) {
    return to;
  }
  end = memccpy(to, from, '\0', e - to);
  if (end == nil) {
    end = e - 1;
    while (end > to && (*--end & 0xC0) == 0x80)
      ;
    *end = '\0';
  } else {
    end--;
  }

  return end;
}
