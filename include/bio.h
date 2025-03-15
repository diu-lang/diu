// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#ifndef BIO_H_
#define BIO_H_ 1

#if defined(__cplusplus)
extern "C" {
#endif

#include <fcntl.h> /* for O_RDONLY, O_WRONLY */

typedef struct Biobuf Biobuf;

enum {
  Bsize       = 8 * 1024,
  Bungetsize  = 4,        /* space for ungetc */
  Bmagic      = 0x314159,
  Beof        = -1,
  Bbad        = -2,

  Binactive   = 0,        /* states */
  Bractive,
  Bwactive,
  Bracteof,

  Bend,
};

struct Biobuf {
  int icount;   /* neg num of bytes at eob */
  int ocount;   /* num of bytes at bob */
  int rdline;   /* num of bytes after rdline */
  int runesize; /* num of bytes of last getrune */
  int state;    /* r/w/inactive */
  int fd;       /* open file */
  int flag;     /* magic if malloc'ed */

};

int Bflush(Biobuf *bp);

#if defined(__cplusplus)
}
#endif

#endif
