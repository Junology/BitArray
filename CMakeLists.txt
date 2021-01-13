cmake_minimum_required(VERSION 3.3)

## Set project
set(PROJECT_NAME "BitArray")
project(${PROJECT_NAME})
set(PROJECT_VERSION 0.0.1)

set(PROJECT_TARGET_NAME ${PROJECT_NAME})
set(PROJECT_LICENSE_TYPE "MIT License")
file(READ ${CMAKE_SOURCE_DIR}/LICENSE PROJECT_LICENSE_BODY)


## Language setting
enable_language(CXX)

option(USE_CLANG "Use clang instead of GCC" OFF)

if(USE_CLANG)
set(CMAKE_C_COMPILER "/usr/bin/clang" CACHE STRING "clang compiler" FORCE)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE STRING "clang++ compiler" FORCE)
endif(USE_CLANG)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


## Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


## Add source directory
include_directories(${CMAKE_SOURCE_DIR}/include)


## Tests
enable_testing()

macro(mk_test name)
  add_executable(${name} "test/${name}.cpp")
  target_compile_options(${name} PRIVATE
	${CMAKE_CXX_FLAGS_DEBUG}
	$<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -Wall -Wextra>
	)
  add_test(NAME ${name} COMMAND $<TARGET_FILE:${name}>)
endmacro(mk_test)

mk_test(popcount_test)
mk_test(trails_test)

