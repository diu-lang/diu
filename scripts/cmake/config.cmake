# The Diu Programming Language.
# Copyright (c) 2024, Diu
# https://furzoom.com
# https://github.com/diu-lang/diu
#
# MIT License

# Find flex & bison
find_program(FLEX flex)
find_program(BISON bison)

if(NOT FLEX)
    message(FATAL_ERROR "Flex not found!")
endif()

if(NOT BISON)
    message(FATAL_ERROR "Bison not found!")
endif()
