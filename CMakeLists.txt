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

find_package(Threads REQUIRED)


## Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


## Add source directory
include_directories(${CMAKE_SOURCE_DIR}/include)


## Tests
enable_testing()

macro(mk_test name src)
  add_executable(${name} ${src})
  target_compile_options(${name} PRIVATE
	${CMAKE_CXX_FLAGS_DEBUG}
	$<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
	$<$<NOT:$<CXX_COMPILER_ID:MSVC>>: -Wall -Wextra>
	)
  add_test(NAME ${name} COMMAND $<TARGET_FILE:${name}>)
endmacro(mk_test)

mk_test(popcount_test "test/popcount_test.cpp")
mk_test(trails_test "test/trails_test.cpp")
mk_test(convert_test "test/convert_test.cpp")
mk_test(bitop_test "test/bitop_test.cpp")
target_link_libraries(bitop_test Threads::Threads)
mk_test(iterator_test "test/iterator_test.cpp")
mk_test(latraits_test "test/latraits_test.cpp")
mk_test(vector_test "test/vector_test.cpp")
mk_test(matrix_test "test/matrix_test.cpp")
mk_test(matrixop_test "test/matrixop_test.cpp")


## Benchmarks
# pop-count
foreach(BITS 8 16 32 64)
  add_executable("popcount_bench${BITS}" "bench/bitarray_popcnt.cpp")
  target_compile_definitions("popcount_bench${BITS}" PRIVATE NBITS=${BITS})
  target_compile_options("popcount_bench${BITS}" PRIVATE "SHELL:${CMAKE_CXX_FLAGS_RELEASE}")
endforeach(BITS)

add_executable(popcount_benchstd "bench/bitset_popcnt.cpp")
target_compile_options(popcount_benchstd PRIVATE "SHELL:${CMAKE_CXX_FLAGS_RELEASE}")
# xorshift
foreach(BITS 8 16 32 64)
add_executable("xorshift_bench${BITS}" "bench/bitarray_xorshift.cpp")
target_compile_definitions("xorshift_bench${BITS}" PRIVATE NBITS=${BITS})
target_compile_options("xorshift_bench${BITS}" PRIVATE "SHELL:${CMAKE_CXX_FLAGS_RELEASE}")
endforeach(BITS)

add_executable(xorshift_benchstd "bench/bitset_xorshift.cpp")
target_compile_options(xorshift_benchstd PRIVATE "SHELL:${CMAKE_CXX_FLAGS_RELEASE}")
