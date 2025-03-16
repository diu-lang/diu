// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#include <assert.h>

#include "utf.h"
#include "utfdef.h"

enum {
  Bit1 = 7,
  Bitx = 6,
  Bit2 = 5,
  Bit3 = 4,
  Bit4 = 3,
  Bit5 = 2,

  T1 = ((1 << (Bit1 + 1)) - 1) ^ 0xFF,  /* 0000 0000 */
  Tx = ((1 << (Bitx + 1)) - 1) ^ 0xFF,  /* 1000 0000 */
  T2 = ((1 << (Bit2 + 1)) - 1) ^ 0xFF,  /* 1100 0000 */
  T3 = ((1 << (Bit3 + 1)) - 1) ^ 0xFF,  /* 1110 0000 */
  T4 = ((1 << (Bit4 + 1)) - 1) ^ 0xFF,  /* 1111 0000 */
  T5 = ((1 << (Bit5 + 1)) - 1) ^ 0xFF,  /* 1111 1000 */

  Rune1 = (1 << (Bit1 + 0 * Bitx)) - 1, /* 0000 0000 0000 0000 0111 1111 */
  Rune2 = (1 << (Bit2 + 1 * Bitx)) - 1, /* 0000 0000 0000 0111 1111 1111 */
  Rune3 = (1 << (Bit3 + 2 * Bitx)) - 1, /* 0000 0000 1111 1111 1111 1111 */
  Rune4 = (1 << (Bit4 + 3 * Bitx)) - 1, /* 0001 1111 1111 1111 1111 1111 */

  Maskx = (1 << Bitx) - 1,              /* 0011 1111 */
  Testx = Maskx ^ 0xFF,                 /* 1100 0000 */

  Bad = RuneError,
};

int RuneToChar(char* s, const Rune* r) {
  Rune c = *r;

  // One character sequence
  //  0000-007F ==> 00-7F
  if (c <= Rune1) {
    s[0] = c;
    return 1;
  }

  // Two characters sequence
  //  0080-07FF ==> T2 Tx
  if (c <= Rune2) {
    s[0] = T2 | (c >> 1 * Bitx);
    s[1] = Tx | (c & Maskx);
    return 2;
  }

  // If the Rune is out of range, convert it to the error rune.
  // Do this test here because the error rune encodes to three bytes.
  // Doing it earlier would duplicate work, since an out of range
  // Rune wouldn't have.
  if (c > RuneMax) {
    c = RuneError;
  }

  // Three characters sequence
  //  0800-FFFF ==> T3 Tx Tx
  if (c <= Rune3) {
    s[0] = T3 | (c >> 2 * Bitx);
    s[1] = Tx | ((c >> 1 * Bitx) & Maskx);
    s[2] = Tx | (c & Maskx);
    return 3;
  }

  // Four characters sequence (21-bit value)
  //  10000-10FFFF ==> T4 Tx Tx Tx
  s[0] = T4 | (c >> 3 * Bitx);
  s[1] = Tx | ((c >> 2 * Bitx) & Maskx);
  s[2] = Tx | ((c >> 1 * Bitx) & Maskx);
  s[3] = Tx | (c & Maskx);
  return 4;
}

/**
 * This is the older "unsafe" version, which works fine on
 * null-terminated strings.
 */
int CharToRune(Rune* r, const char* s) {
  Rune l;
  uint8_t c, c1, c2, c3;

  // One character sequence
  //  0000-007F ==> T1
  c = *(uint8_t*)s;
  if (c < Tx) {
    *r = c;
    return 1;
  }

  // Two characters sequence
  //  0080-07FF ==> T2 Tx
  c1 = *(uint8_t*)(s + 1) ^ Tx;
  if (c1 & Testx) {
    goto bad;
  }
  if (c < T3) {
    if (c < T2) {
      goto bad;
    }
    l = ((c << Bitx) | c1) & Rune2;
    if (l <= Rune1) {
      goto bad;
    }
    *r = l;
    return 2;
  }

  // Three characters sequence
  //  0800-FFFF ==> T3 Tx Tx
  c2 = *(uint8_t*)(s + 2) ^ Tx;
  if (c2 & Testx) {
    goto bad;
  }
  if (c < T4) {
    l = ((((c << Bitx) | c1) << Bitx) | c2) & Rune3;
    if (l <= Rune2) {
      goto bad;
    }
    *r = l;
    return 3;
  }

  // Four characters sequence
  //  10000->10FFFF ==> T4 Tx Tx Tx
  c3 = *(uint8_t*)(s + 3) ^ Tx;
  if (c3 & Testx) {
    goto bad;
  }
  if (c < T5) {
    l = ((((((c << Bitx) | c1) << Bitx) | c2) << Bitx) | c3) & Rune4;
    if (l <= Rune3) {
      goto bad;
    }
    *r = l;
    return 4;
  }

  // Support for 5-byte or longer UTF-8 would go here, but
  // since we don't have that, we'll just fall through to bad.

  // Bad decoding.
bad:
  *r = Bad;
  return 1;
}

/**
 * This is a slower but "safe" version of old CharToRune that
 * works on strings that are not necessarily null-terminated.
 *
 * If you know for sure that your string is null-terminated,
 * CharToRune will be a bit faster.
 *
 * It is guaranteed not to attempt to access "length" past
 * the incoming pointer. This is to avoid possible access
 * violations.  If the string appears to be well-formed but
 * incomplete (i.e., to get the whole Rune we'd need to read
 * past s+n) then we'll set the Rune to Bad and return 0.
 *
 * Note: If we have decoding problems for other reasons, we
 * return 1 instead of 0.
 */
int CharNToRune(Rune* r, const char* s, int n) {
  Rune l;
  uint8_t c, c1, c2, c3;

  // When we're not allowed to read anything.
  if (n <= 0) {
    goto badlen;
  }

  // One character sequence
  //  0000-007F ==> T1
  c = *(uint8_t*)s;
  if (c < Tx) {
    *r = c;
    return 1;
  }

  // If we can't read more than one character, we must stop.
  if (n <= 1) {
    goto badlen;
  }

  // Two characters sequence
  //  0080-07FF ==> T2 Tx
  c1 = *(uint8_t*)(s + 1) ^ Tx;
  if (c1 & Testx) {
    goto bad;
  }
  if (c < T3) {
    if (c < T2) {
      goto bad;
    }
    l = ((c << Bitx) | c1) & Rune2;
    if (l <= Rune1) {
      goto bad;
    }
    *r = l;
    return 2;
  }

  // If we can't read more than one character, we must stop.
  if (n <= 2) {
    goto badlen;
  }

  // Three characters sequence
  //  0800-FFFF ==> T3 Tx Tx
  c2 = *(uint8_t*)(s + 2) ^ Tx;
  if (c2 & Testx) {
    goto bad;
  }
  if (c < T4) {
    l = ((((c << Bitx) | c1) << Bitx) | c2) & Rune3;
    if (l <= Rune2) {
      goto bad;
    }
    *r = l;
    return 3;
  }

  // If we can't read more than one character, we must stop.
  if (n <= 3) {
    goto badlen;
  }

  // Four characters sequence
  //  10000->10FFFF ==> T4 Tx Tx Tx
  c3 = *(uint8_t*)(s + 3) ^ Tx;
  if (c3 & Testx) {
    goto bad;
  }
  if (c < T5) {
    l = ((((((c << Bitx) | c1) << Bitx) | c2) << Bitx) | c3) & Rune4;
    if (l <= Rune3) {
      goto bad;
    }
    *r = l;
    return 4;
  }

  // Support for 5-byte or longer UTF-8 would go here, but
  // since we don't have that, we'll just fall through to bad.

  // Bad decoding.
bad:
  *r = Bad;
  return 1;

badlen:
  *r = Bad;
  return 0;
}

int IsValidCharNToRune(const char* str, int n, Rune* r, int* consumed) {
  *consumed = CharNToRune(r, str, n);
  return *r != RuneError;
}

int RuneLen(Rune r) {
  char str[8];
  return RuneToChar(str, &r);
}

int RuneNLen(const Rune* r, int n) {
  int nb = 0;
  Rune c;

  while (n--) {
    c = *r++;
    if (c <= Rune1) {
      nb++;
    } else if (c <= Rune2) {
      nb += 2;
    } else if (c <= Rune3) {
      nb += 3;
    } else if (c <= Rune4) {
      nb += 4;
    }
  }

  return nb;
}

int FullRune(const char* s, int n) {
  if (n > 0) {
    uint8_t c = *(uint8_t*)s;
    if (c < Tx) {
      return 1;
    }
    if (n > 1) {
      if (c < T3) {
        return 1;
      }
      if (n > 2) {
        if (c < T4 || n > 3) {
          return 1;
        }
      }
    }
  }

  return 0;
}
