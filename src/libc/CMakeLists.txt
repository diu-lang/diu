# The Diu Programming Language.
# Copyright (c) 2024, Diu
# https://furzoom.com
# https://github.com/diu-lang/diu
#
# MIT License

set(LIBC_SRCS
    argv0.c

    utf/rune.c
    utf/rune_type.c
    utf/rune_type_body-5.0.0.c
    utf/utfecpy.c
    utf/utflen.c
    utf/utfnlen.c
    utf/utfrune.c
    utf/utfrrune.c
    utf/utfutf.c
)

add_library(c STATIC ${LIBC_SRCS})

add_executable(mk_rune_type utf/mk_rune_type.c)
target_link_libraries(mk_rune_type c)
