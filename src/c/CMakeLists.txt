# The Diu Programming Language.
# Copyright (c) 2024, Diu
# https://furzoom.com
# https://github.com/diu-lang/diu
#
# MIT License

set(DIU_BISON_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/diu.yy.c)
set(DIU_BISON_HEADER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/diu.yy.h)

add_executable(diu lex.c ${DIU_BISON_OUTPUT})
target_include_directories(diu
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_command(
    OUTPUT ${DIU_BISON_OUTPUT} ${DIU_BISON_HEADER_OUTPUT}
    COMMAND ${BISON} --defines -o ${DIU_BISON_OUTPUT} ${CMAKE_CURRENT_SOURCE_DIR}/diu.y
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/diu.y
    COMMENT "Generating diu.yy.c file"
)
