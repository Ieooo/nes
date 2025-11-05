cmake_minimum_required(VERSION 3.10)

project(nes)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB ALL_SRC *.c)
set(COMMON_SRC ${ALL_SRC})
list(FILTER COMMON_SRC EXCLUDE REGEX "main\\.c$|test/test\\.c$")

enable_testing()

add_executable(nes ${COMMON_SRC} main.c)
add_executable(nes_test ${COMMON_SRC} test/test.c)
add_test(NAME TestInstruction COMMAND nes_test)

add_custom_command(TARGET nes_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/test/test.bin
        ${CMAKE_BINARY_DIR}/test.bin
)