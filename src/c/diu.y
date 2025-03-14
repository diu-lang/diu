// The Programming Language Nua.
// Copyright (c) 2024, Nua
// https://furzoom.com
// https://github.com/nua-lang/nua
//
// MIT License

%{
#include "diu.h"
%}

%union {
  char *str;
}


%token <str> LNAME

%%

file: LNAME
    {
      print_msg($1);
    }


