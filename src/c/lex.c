// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

#include "diu.h"
#include "diu.yy.h"

int yyparse();

void yyerror(char *fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  fprintf(stderr, "failed: ");
  vfprintf(stderr, fmt, arg);
  va_end(arg);
  exit(1);
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  yyparse();

  return 0;
}

long yylex() {
  long c;
  char buf[128];
  char *cp;

L0:
  c = fgetc(stdin);
  if (isspace(c)) {
    goto L0;
  }

  if (isalpha(c)) {
    cp = buf;
    goto talph;
  }

  switch (c) {
  case EOF:
    ungetc(EOF, stdin);
    return -1;
  default:
    fprintf(stderr, "unexpected '%c'\n", (char)c);
    return -1;
  }

talph:
  for (;;) {
    if (isspace(c) || c == EOF) {
      break;
    }
    *cp++ = c;
    c = fgetc(stdin);
  }
  *cp = 0;
  ungetc(c, stdin);

  yylval.str = strdup(buf);
  return LNAME;
}

void print_msg(char *msg) { printf("debug: %s\n", msg); }
