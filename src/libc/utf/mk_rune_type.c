// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

/**
 * Make Is(Upper|Lower|Title|Space|Alpha)Rune and
 * To(Upper|Lower|Title)Rune from a UnicodeData.txt file.
 * These can be found at https://www.unicode.org.
 *
 * With -c, runs a check of the existing runetype functions vs.
 * those extracted from UnicodeData.
 *
 * With -p, generates tables for pairs of chars, as well as for
 * ranges and singletons.
 *
 * UnicodeData defines 4 fields of interest:
 *  - a category
 *  - an upper case mapping
 *  - a lower case mapping
 *  - a title case mapping
 *
 * ToUpper, ToLower, and ToTitle are defined directly from the mapping.
 *
 * IsAlphaRune(c) is true iff c is a "letter" category.
 * IsUpperRune(c) is true iff c is the target of ToUpperRune or
 *  is in the uppercase letter category.
 * Similarly for IsLowerRune and IsTitleRune.
 * IsSpaceRune(c) is true for space category chars, "C" locale white space
 * chars,
 *  and two additions:
 *      0x0085 "next line" control char
 *      0xfeff "zero-width non-break space"
 * IsDigitRune(c) is true iff c is a numeric-digit category.
 */

#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libc.h"
#include "utf.h"
#include "utfdef.h"

enum {
  // Fields in the unicode data file.
  FIELD_CODE,
  FIELD_NAME,
  FIELD_CATEGORY,
  FIELD_COMBINING,
  FIELD_BIDIR,
  FIELD_DECOMP,
  FIELD_DECIMAL_DIG,
  FIELD_DIG,
  FIELD_NUMERIC_VAL,
  FIELD_MIRRORED,
  FIELD_UNICODE_1_NAME,
  FIELD_COMMENT,
  FIELD_UPPER,
  FIELD_LOWER,
  FIELD_TITLE,
  NFIELDS,

  MAX_LINE = 1024,

  TO_OFFSET = 1 << 20,

  NRUNES = 1 << 21,
};

#define TO_DELTA(xmapped, x) (TO_OFFSET + (int64_t)(xmapped) - (x))

static char myisspace[NRUNES];
static char myisalpha[NRUNES];
static char myisdigit[NRUNES];
static char myisupper[NRUNES];
static char myislower[NRUNES];
static char myistitle[NRUNES];

static Rune mytoupper[NRUNES];
static Rune mytolower[NRUNES];
static Rune mytotitle[NRUNES];

static void check();
static void mktables(char* src, int use_pairs);
static void fatal(const char* fmt, ...);
static int get_fields(char** fields, int nfields, char* str, const char* delim);
static int get_unicode_line(FILE* in, char** fields, char* buf);
static int get_code(char* s);

static void usage(const char* exe) {
  fprintf(stderr, "usage: %s [-cp] <UnicodeData.txt>\n", exe);
  exit(1);
}

int main(int argc, char* argv[]) {
  FILE* in;
  char buf[MAX_LINE], buf2[MAX_LINE];
  char* p;
  char* fields[NFIELDS + 1];
  char* fields2[NFIELDS + 1];
  int do_check = 0, use_pairs = 0;
  int code, last;
  uint32_t i;

  ARGBEGIN {
    case 'c':
      do_check = 1;
      break;
    case 'p':
      use_pairs = 1;
      break;
    default:
      usage(argv0);
  }
  ARGEND

  if (argc != 1) {
    usage(argv0);
  }

  in = fopen(argv[0], "r");
  if (in == NULL) {
    fatal("can't open %s", argv[0]);
  }

  for (i = 0; i < NRUNES; i++) {
    mytoupper[i] = i;
    mytolower[i] = i;
    mytotitle[i] = i;
  }

  // Make sure IsSpace has all the "C" locale whitespace chars.
  myisspace['\t'] = 1;
  myisspace['\n'] = 1;
  myisspace['\r'] = 1;
  myisspace['\f'] = 1;
  myisspace['\v'] = 1;

  // A couple of other extensions.
  myisspace[0x85] = 1;    // Control char, "next line"
  myisspace[0xfeff] = 1;  // Zero-width non-break case

  last = -1;
  while (get_unicode_line(in, fields, buf)) {
    code = get_code(fields[FIELD_CODE]);
    if (code >= NRUNES) {
      fatal("code-point value too big: %x", code);
    }
    if (code <= last) {
      fatal("bad code sequence: %x then %x", last, code);
    }
    last = code;

    // Check for ranges
    p = fields[FIELD_CATEGORY];
    if (strstr(fields[FIELD_NAME], ", First>") != NULL) {
      if (!get_unicode_line(in, fields2, buf2)) {
        fatal("range start at eof");
      }
      if (strstr(fields2[FIELD_NAME], ", Last>") == NULL) {
        fatal("range start not followed by range end");
      }
      last = get_code(fields2[FIELD_CODE]);
      if (last <= code) {
        fatal("range out of sequence: %x then %x", code, last);
      }
      if (strcmp(p, fields2[FIELD_CATEGORY]) != 0) {
        fatal("range with mismatched category");
      }
    }

    // Set properties and conversions.
    for (; code <= last; code++) {
      if (p[0] == 'L') {
        myisalpha[code] = 1;
      }
      if (p[0] == 'Z') {
        myisspace[code] = 1;
      }

      if (strcmp(p, "Lu") == 0) {
        myisupper[code] = 1;
      }
      if (strcmp(p, "Ll") == 0) {
        myislower[code] = 1;
      }
      if (strcmp(p, "Lt") == 0) {
        myistitle[code] = 1;
      }
      if (strcmp(p, "Nd") == 0) {
        myisdigit[code] = 1;
      }

      // When finding conversions, also need to mark upper/lower case,
      // since some chars, like "III" (0x2162), aren't defined as letters
      // but have a lower case mapping ("iii" (0x2172)).
      if (fields[FIELD_UPPER][0] != '\0') {
        mytoupper[code] = get_code(fields[FIELD_UPPER]);
      }
      if (fields[FIELD_LOWER][0] != '\0') {
        mytolower[code] = get_code(fields[FIELD_LOWER]);
      }
      if (fields[FIELD_TITLE][0] != '\0') {
        mytotitle[code] = get_code(fields[FIELD_TITLE]);
      }
    }
  }

  fclose(in);

  // Check for codes with no totitle mapping but a toupper mapping.
  // These appear in UnicodeData-2.0.14.txt, but are almost certainly
  // erroneous.
  for (i = 0; i < NRUNES; i++) {
    if (mytotitle[i] == i && mytoupper[i] != i && !myistitle[i]) {
      fprintf(stderr,
              "warning: code=%.4x not istitle, totitle is same, toupper=%.4x\n",
              i, mytoupper[i]);
    }
  }

  // Make sure isupper[c] is true if for some x toupper[x] == c
  // ditto for islower and istitle
  for (i = 0; i < NRUNES; i++) {
    if (mytoupper[i] != i) {
      myisupper[mytoupper[i]] = 1;
    }
    if (mytolower[i] != i) {
      myislower[mytolower[i]] = 1;
    }
    if (mytotitle[i] != i) {
      myistitle[mytotitle[i]] = 1;
    }
  }

  if (do_check) {
    check();
  } else {
    mktables(argv[0], use_pairs);
  }

  return 0;
}

/**
 * Find differences between the newly generated tables and current
 * runetypes.
 */
void check() {
  int i;

  for (i = 0; i < NRUNES; i++) {
    if (IsDigitRune(i) != myisdigit[i]) {
      fprintf(stderr, "isdigit diff at %x: runetype=%x, unicode=%x\n",
              i, IsDigitRune(i), myisdigit[i]);
    }

    if (IsSpaceRune(i) != myisspace[i]) {
      fprintf(stderr, "isspace diff at %x: runetype=%x, unicode=%x\n",
              i, IsSpaceRune(i), myisspace[i]);
    }

    if (IsUpperRune(i) != myisupper[i]) {
      fprintf(stderr, "isupper diff at %x: runetype=%x, unicode=%x\n",
              i, IsUpperRune(i), myisupper[i]);
    }

    if (IsLowerRune(i) != myislower[i]) {
      fprintf(stderr, "islower diff at %x: runetype=%x, unicode=%x\n",
              i, IsLowerRune(i), myislower[i]);
    }

    if (IsAlphaRune(i) != myisalpha[i]) {
      fprintf(stderr, "isalpha diff at %x: runetype=%x, unicode=%x\n",
              i, IsAlphaRune(i), myisalpha[i]);
    }

    if (ToUpperRune(i) != mytoupper[i]) {
      fprintf(stderr, "toupper diff at %x: runetype=%x, unicode=%x\n",
              i, ToUpperRune(i), mytoupper[i]);
    }

    if (ToLowerRune(i) != mytolower[i]) {
      fprintf(stderr, "tolower diff at %x: runetype=%x, unicode=%x\n",
              i, ToLowerRune(i), mytolower[i]);
    }

    if (IsTitleRune(i) != myistitle[i]) {
      fprintf(stderr, "istitle diff at %x: runetype=%x, unicode=%x\n",
              i, IsTitleRune(i), myistitle[i]);
    }

    if (ToTitleRune(i) != mytotitle[i]) {
      fprintf(stderr, "totitle diff at %x: runetype=%x, unicode=%x\n",
              i, ToTitleRune(i), mytotitle[i]);
    }
  }
}

/**
 * Generate a properties array for ranges, clearing those cases covered.
 * If force, generate one-entry ranges for singletons.
 */
static int mk_is_range(const char* label, char* prop, int force) {
  int start, stop, some;

  // First, the ranges
  some = 0;
  for (start = 0; start < NRUNES;) {
    if (!prop[start]) {
      start++;
      continue;
    }

    for (stop = start + 1; stop < NRUNES; stop++) {
      if (!prop[stop]) {
        break;
      }
      prop[stop] = 0;
    }

    if (force || stop != start + 1) {
      if (!some) {
        printf("static Rune __is%sr[] = {\n", label);
        some = 1;
      }
      prop[start] = 0;
      printf("\t0x%.4x, 0x%.4x,\n", start, stop - 1);
    }

    start = stop;
  }

  if (some) {
    printf("};\n\n");
  }

  return some;
}

/*
 * Generate a mapping array for pairs with a skip between,
 * clearing those entries covered.
 */
static int mk_is_pair(const char* label, char* prop) {
  int start, stop, some;

  some = 0;
  for (start = 0; start + 2 < NRUNES;) {
    if (!prop[start]) {
      start++;
      continue;
    }

    for (stop = start + 2; stop < NRUNES; stop += 2) {
      if (!prop[stop]) {
        break;
      }
      prop[stop] = 0;
    }

    if (stop != start + 2) {
      if (!some) {
        printf("static Rune __is%sp[] = {\n", label);
        some = 1;
      }
      prop[start] = 0;
      printf("\t0x%.4x, 0x%.4x,\n", start, stop - 2);
    }

    start = stop;
  }

  if (some) {
    printf("};\n\n");
  }
  return some;
}

/*
 * Generate a properties array for singletons, clearing those cases covered.
 */
static int mk_is_single(const char* label, char* prop) {
  int start, some;

  some = 0;
  for (start = 0; start < NRUNES; start++) {
    if (!prop[start]) {
      continue;
    }

    if (!some) {
      printf("static Rune __is%ss[] = {\n", label);
      some = 1;
    }

    prop[start] = 0;
    printf("\t0x%.4x,\n", start);
  }

  if (some) {
    printf("};\n\n");
  }

  return some;
}

/*
 * Generate tables and a function for Is<label>Rune.
 */
static void mk_is(const char* label, char* prop, int use_pairs) {
  int isr, isp, iss;

  isr = mk_is_range(label, prop, 0);
  isp = 0;
  if (use_pairs) {
    isp = mk_is_pair(label, prop);
  }
  iss = mk_is_single(label, prop);

  printf("int Is%sRune(Rune c) {\n", label);
  printf("  Rune* p;\n");
  printf("\n");

  if (isr) {
    printf("  p = rbsearch(c, __is%sr, nelem(__is%sr)/2, 2);\n", label, label);
    printf("  if (p && c >= p[0] && c <= p[1]) {\n");
    printf("    return 1;\n");
    printf("  }\n");
    printf("\n");
  }
  if (isp) {
    printf("  p = rbsearch(c, __is%sp, nelem(__is%sp)/2, 2);\n", label, label);
    printf("  if (p && c >= p[0] && c <= p[1] && !((c - p[0]) & 1)) {\n");
    printf("    return 1;\n");
    printf("  }\n");
    printf("\n");
  }
  if (iss) {
    printf("  p = rbsearch(c, __is%ss, nelem(__is%ss), 1);\n", label, label);
    printf("  if (p && c == p[0]) {\n");
    printf("    return 1;\n");
    printf("  }\n");
    printf("\n");
  }
  printf("  return 0;\n");
  printf("}\n\n");
}

/**
 * Generate a mapping array for ranges, clearing those entries covered.
 * If force, generate one-entry ranges for singletons.
 */
static int mk_to_range(const char* label, uint32_t* map, int force) {
  int delta, some;
  uint32_t start, stop;

  some = 0;
  for (start = 0; start < NRUNES;) {
    if (map[start] == start) {
      start++;
      continue;
    }

    delta = TO_DELTA(map[start], start);
    for (stop = start + 1; stop < NRUNES; stop++) {
      if (TO_DELTA(map[stop], stop) != delta) {
        break;
      }
      map[stop] = stop;
    }

    if (stop != start + 1) {
      if (!some) {
        printf("static Rune __to%sr[] = {\n", label);
        some = 1;
      }
      map[start] = start;
      printf("\t0x%.4x, 0x%.4x, %d,\n", start, stop - 1, delta);
    }

    start = stop;
  }

  if (some) {
    printf("};\n\n");
  }

  return some;
}

/**
 * Generate a mapping array for pairs with a skip between,
 * clearing those entries covered.
 */
static int mk_to_pair(const char* label, uint32_t* map) {
  int delta, some;
  uint32_t start, stop;

  some = 0;
  for (start = 0; start + 2 < NRUNES;) {
    if (map[start] == start) {
      start++;
      continue;
    }

    delta = TO_DELTA(map[start], start);
    for (stop = start + 2; stop < NRUNES; stop += 2) {
      if (TO_DELTA(map[stop], stop) != delta) {
        break;
      }
      map[stop] = stop;
    }

    if (stop != start + 2) {
      if (!some) {
        printf("static Rune __to%sp[] = {\n", label);
        some = 1;
      }
      map[start] = start;
      printf("\t0x%.4x, 0x%.4x, %d,\n", start, stop - 2, delta);
    }

    start = stop;
  }

  if (some) {
    printf("};\n\n");
  }

  return some;
}

/**
 * Generate a mapping array for singletons, clearing those entries covered.
 */
static int mk_to_single(const char* label, uint32_t* map) {
  int delta, some;
  uint32_t start;

  some = 0;
  for (start = 0; start < NRUNES; start++) {
    if (map[start] == start) {
      continue;
    }

    delta = TO_DELTA(map[start], start);
    if (!some) {
      printf("static Rune __to%ss[] = {\n", label);
      some = 1;
    }

    map[start] = start;
    printf("\t0x%.4x, %d,\n", start, delta);
  }

  if (some) {
    printf("};\n\n");
  }

  return some;
}

/*
 * Generate tables and a function for To<label>Rune.
 */
static void mk_to(const char* label, uint32_t* map, int user_pairs) {
  int tor, top, tos;

  tor = mk_to_range(label, map, 0);
  top = 0;
  if (user_pairs) {
    top = mk_to_pair(label, map);
  }
  tos = mk_to_single(label, map);

  printf("Rune To%sRune(Rune c) {\n", label);
  printf("  Rune* p;\n");
  printf("\n");

  if (tor) {
    printf("  p = rbsearch(c, __to%sr, nelem(__to%sr)/3, 3);\n", label, label);
    printf("  if (p && c >= p[0] && c <= p[1]) {\n");
    printf("    return c + p[2] - %d;\n", TO_OFFSET);
    printf("  }\n");
    printf("\n");
  }

  if (top) {
    printf("  p = rbsearch(c, __to%sp, nelem(__to%sp)/3, 3);\n", label, label);
    printf("  if (p && c >= p[0] && c <= p[1] && !((c - p[0]) & 1)) {\n");
    printf("    return c + p[2] - %d;\n", TO_OFFSET);
    printf("  }\n");
    printf("\n");
  }

  if (tos) {
    printf("  p = rbsearch(c, __to%ss, nelem(__to%ss)/2, 2);\n", label, label);
    printf("  if (p && c == p[0]) {\n");
    printf("    return c + p[1] - %d;\n", TO_OFFSET);
    printf("  }\n");
    printf("\n");
  }

  printf("  return c;\n");
  printf("}\n\n");
}

static void mk_is_only(const char* label, char* prop) {
  mk_is_range(label, prop, 1);
  printf("int Is%sRune(Rune c) {\n", label);
  printf("  Rune* p;\n");
  printf("\n");
  printf("  p = rbsearch(c, __is%sr, nelem(__is%sr)/2, 2);\n", label, label);
  printf("  if (p && c >= p[0] && c <= p[1]) {\n");
  printf("    return 1;\n");
  printf("  }\n");
  printf("  return 0;\n");
  printf("}\n\n");
}

void mktables(char* src, int use_pairs) {
  printf("/* generated automatically by mk_rune_type from %s */\n\n",
         basename(src));
  printf("#include \"utf.h\"\n");
  printf("#include \"utf-internal.h\"\n");
  printf("#include \"utfdef.h\"\n\n");

  // The special case is the IsSpace and IsTitle tables, since they are
  // assumed to be small with several ranges.
  mk_is_only("Space", myisspace);
  mk_is_only("Digit", myisdigit);

  mk_is("Alpha", myisalpha, 0);
  mk_is("Upper", myisupper, use_pairs);
  mk_is("Lower", myislower, use_pairs);
  mk_is("Title", myistitle, use_pairs);

  mk_to("Upper", mytoupper, use_pairs);
  mk_to("Lower", mytolower, use_pairs);
  mk_to("Title", mytotitle, use_pairs);
}

void fatal(const char* fmt, ...) {
  va_list ap;
  fprintf(stderr, "%s: fatal error: ", argv0);
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  fprintf(stderr, "\n");

  exit(1);
}

int get_fields(char** fields, int nfields, char* str, const char* delim) {
  int nf;

  fields[0] = str;
  nf = 1;
  if (nf >= nfields) {
    return nf;
  }

  for (; *str; str++) {
    if (strchr(delim, *str) != NULL) {
      *str = '\0';
      fields[nf++] = str + 1;
      if (nf >= nfields) {
        break;
      }
    }
  }

  return nf;
}

int get_unicode_line(FILE* in, char** fields, char* buf) {
  char* p;
  if (fgets(buf, MAX_LINE, in) == NULL) {
    return 0;
  }

  p = strchr(buf, '\n');
  if (p == NULL) {
    fatal("line too long");
  }
  *p = '\0';

  if (get_fields(fields, NFIELDS + 1, buf, ";") != NFIELDS) {
    fatal("bad number of fields");
  }

  return 1;
}

int get_code(char* s) {
  int i = 0, code = 0;

  // Parse a hex number
  while (s[i]) {
    code <<= 4;
    if (s[i] >= '0' && s[i] <= '9') {
      code += s[i] - '0';
    } else if (s[i] >= 'A' && s[i] <= 'F') {
      code += s[i] - 'A' + 10;
    } else {
      fatal("bad code char '%c'", s[i]);
    }
    i++;
  }

  return code;
}