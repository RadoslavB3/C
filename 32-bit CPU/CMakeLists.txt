cmake_minimum_required(VERSION 3.0)
project(hw03)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang OR ${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  # using regular Clang or AppleClang or GCC
  set(CMAKE_C_FLAGS "-pedantic -Wall -Wextra -g -O0 -DBONUS_JMP -DBONUS_CALL")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
  set(CMAKE_CXX_FLAGS "/permissive- /W4 /EHsc")
  add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

add_executable(cpu "main.c" "cpu.c")
add_executable(compiler "compiler.c")

# to enable the first bonus (cmp, jmp, ...) uncomment this line:
#target_compile_definitions(cpu PUBLIC -DBONUS_JMP)

# to enable the second bonus (call, ret) uncomment this line:
#target_compile_definitions(cpu PUBLIC -DBONUS_CALL)

target_compile_definitions(compiler PUBLIC -D_POSIX_C_SOURCE=200809L )
