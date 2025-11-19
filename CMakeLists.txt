cmake_minimum_required(VERSION 3.10)

project(nes)

set(CMAKE_C_STANDARD 11)
set(CMAKE_BUILD_TYPE Debug)

find_package(SDL2 REQUIRED)

set(NES_SRC cpu.c instruction.c nes.c memory.c mapper.c keyboard.c audio.c video.c apu.c ppu.c rom.c main.c)
set(6502_TEST_SRC test/test.c cpu.c memory.c mapper.c apu.c ppu.c instruction.c)
list(FILTER COMMON_SRC EXCLUDE REGEX "nes\\.c$|test/test\\.c$")

enable_testing()

add_executable(nes ${NES_SRC})
add_executable(6502_test ${6502_TEST_SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 m)

add_custom_command(TARGET 6502_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/test/test.bin
        ${CMAKE_BINARY_DIR}/test.bin
)