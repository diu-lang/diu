// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#ifndef UTF_H_
#define UTF_H_

#include <stdint.h>

/* Code-point values in Unicode 4.0 are 21 bits width. */
typedef uint32_t Rune;

enum {
  UtfMax = 4,          // Maximum bytes per rune
  RuneSync = 0x80,     // Cannot represent part of a UTF sequence (<)
  RuneSelf = 0x80,     // Rune and UTF sequences are the same (<)
  RuneError = 0xFFFD,  // Decoding error in UTF
  RuneMax = 0x10FFFF,  // Maximum rune value
};

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Rune routines.
 */

/**
 * RuneToChar copies (encodes) one rune, pointed to by r, to at most
 * UtfMax bytes starting as s and returns the number of bytes generated.
 */
int RuneToChar(char* s, const Rune* r);

/**
 * CharToRune copies (decodes) at most UtfMax bytes starting at s to
 * one rune, pointed to by r, and returns the number of bytes consumed.
 * If the input is not exactly in UTF format, CharToRune will set *r
 * to RuneError and return 1.
 *
 * Note: There is no special case for a "null-terminated" string. A
 * string whose first byte has the value 0 is the UTF8 encoding of the
 * Unicode value 0 (i.e., ASCII NULL). A byte value of 0 is illegal
 * anywhere else in a UTF sequence.
 */
int CharToRune(Rune* r, const char* s);

/**
 * CharNToRune is like CharToRune, except that it will access at most
 * n bytes of s. If the UTF sequence is incomplete within n bytes,
 * CharNToRune will set *r to RuneError and return 0. If it is complete
 * but not in UTF format, it will set *r to RuneError and return 1.
 */
int CharNToRune(Rune* r, const char* s, int n);

/**
 * IsValidCharNToRune(str, n, r, consumed) is a convenience function
 * that calls "*consumed = CharNToRune(r, str, n)" and returns an int
 * (logically boolean) indicating whether the first n bytes of str was
 * a valid and complete UTF sequence.
 */
int IsValidCharNToRune(const char* str, int n, Rune* r, int* consumed);

/**
 * RuneLen returns the number of bytes required to convert r into UTF.
 */
int RuneLen(Rune r);

/**
 * RuneNLen returns the number of bytes required to convert the n
 * runes pointed to by r into UTF.
 */
int RuneNLen(const Rune* r, int n);

/**
 * FullRune returns 1 if the string s of length n is long enough to
 * be decoded by CharToRune, and returns 0 otherwise. This does not
 * guarantee that the string contains a legal UTF encoding. This
 * routine is used by programs that obtain input one byte at a time
 * and need to know when a full rune has arrived.
 */
int FullRune(const char* s, int n);

/**
 * The following routines are analogous to the corresponding string
 * routines with "Utf" substituted for "str", and "Rune" substituted
 * for "chr".
 */

/**
 * UtfLen returns the number of runes that are represented by the UTF
 * string s. (cf. strlen)
 */
int UtfLen(const char* s);

/**
 * UtfNLen returns the number of complete runes that are represented
 * by the first n bytes of the UTF string s. If the last few bytes of
 * the string contain an incompletely coded rune, UtfNLen will not
 * count them; in this way, it differs from UtfLen, which includes
 * every bytes of the string. (cf. strnlen)
 */
int UtfNLen(const char* s, long n);

/**
 * UtfRune returns a pointer to the first occurrence of rune r in the
 * UTF string s, or 0 if r does not occur in the string. The NULL byte
 * terminating a string is considered to be part of the string s.
 * (cf. strchr)
 */
char* UtfRune(const char* s, Rune r);

/**
 * UtfRRune returns a pointer to the last occurrence of rune r in the
 * UTF string s, or 0 if r does not occur in the string. The NULL byte
 * terminating a string is considered to be part of the string s.
 * (cf. strrchr)
 */
char* UtfRRune(const char* s, Rune r);

/**
 * UtfUtf returns a pointer to the first occurrence of the UTF string
 * s2 as a UTF substring of s1, or 0 if there is none. If s2 is the
 * null string, UtfUtf returns s1. (cf. strstr)
 */
const char* UtfUtf(const char* s1, const char* s2);

/**
 * UtfECpy copies UTF sequences until a null sequence has been copied,
 * but write no sequences beyond es1. If any sequences are copied,
 * s1 is terminated by a null sequence, and a pointer to that sequence
 * is returned.  Otherwise, the original s1 is returned. (cf. strecpy)
 */
char* UtfECpy(char* s1, char* es1, const char* s2);

/**
 * These routines are rune-string analogues of the corresponding
 * routines in strcat(3).
 */
Rune* RuneStrcat(Rune* s1, const Rune* s2);
Rune* RuneStrncat(Rune* s1, const Rune* s2, long n);

/**
 * The following routines test types and modify cases for Unicode
 * characters. Unicode defines some characters as letters and
 * specifies three cases: upper, lower, and title. Mappings among the
 * cases are also defined, although they are not exhaustive: some
 * upper case letters have no lower case mapping, and so on. Unicode
 * also defines several character properties, a subset of which are
 * checked by these routines. These routines are based on Unicode
 * version 3.0.0.
 *
 * NOTE: The routines are implemented in C, so the boolean functions
 * (e.g., IsUpperRune) return 0 for false and 1 for true.
 *
 * ToUpperRune, ToLowerRune, and ToTitleRune are the Unicode case
 * mappings. These routines return the character unchanged if it has
 * no defined mapping.
 */
Rune ToUpperRune(Rune r);
Rune ToLowerRune(Rune r);
Rune ToTitleRune(Rune r);

/**
 * IsUpperRune tests for upper case characters, including Unicode * upper case letters and targets of the toupper mapping. IsLowerRune
 * and IsTitleRune are defined analogously.
 */
int IsUpperRune(Rune r);
int IsLowerRune(Rune r);
int IsTitleRune(Rune r);

/**
 * IsAlphaRune tests for Unicode letters; This includes ideographs in
 * addition to alphabetic characters.
 */
int IsAlphaRune(Rune r);

/**
 * IsDigitRune tests for digits. Non-digit numbers, such as Roman
 * numerals, are not included.
 */
int IsDigitRune(Rune r);

/**
 * IsIdeographicRune tests for ideographic characters and numbers,
 * as defined by the Unicode standard.
 */
int IsIdeographicRune(Rune r);

/**
 * IsSpaceRune tests for whitespace characters, including "C" locale
 * whitespace, Unicode defined whitespace, and the "zero-width
 * non-break space" character.
 */
int IsSpaceRune(Rune r);

#ifdef __cplusplus
};
#endif

#endif
