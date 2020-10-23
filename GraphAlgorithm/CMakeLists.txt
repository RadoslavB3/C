cmake_minimum_required(VERSION 3.0)

# Project configuration
project(hw04)
set(SOURCES    main.c)
set(EXECUTABLE dimes)

file(GLOB sources "*.c")
set(sources_lib ${sources})
list(REMOVE_ITEM sources_lib ${CMAKE_CURRENT_SOURCE_DIR}/main.c)

add_library(settle-up-lib ${sources_lib})

add_executable(${EXECUTABLE} ${SOURCES})
target_link_libraries(${EXECUTABLE} PRIVATE settle-up-lib)


# Configure compiler warnings
if (CMAKE_C_COMPILER_ID MATCHES Clang OR ${CMAKE_C_COMPILER_ID} STREQUAL GNU)
  # using regular Clang, AppleClang or GCC
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -Wextra -pedantic")
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
  # using Visual Studio C++
  target_compile_definitions(${EXECUTABLE} PRIVATE _CRT_SECURE_NO_DEPRECATE)
  set(CMAKE_CXX_FLAGS "/permissive- /W4 /EHsc")
endif()

if(MINGW)
    target_compile_definitions(${EXECUTABLE} PRIVATE __USE_MINGW_ANSI_STDIO=1)
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(doc COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile"
        WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
        COMMENT "Building documentation into doxyDoc build dir..."
    )
endif()
