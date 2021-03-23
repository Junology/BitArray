#include <iostream>
#include <chrono>

#include "BitArray.hpp"

#ifndef NBITS
#define NBITS 32
#endif

#define CHUNK_TYPE_AUX(x) uint ## x ## _t
#define CHUNK_TYPE_AUX2(x) CHUNK_TYPE_AUX(x)
#define CHUNK_TYPE CHUNK_TYPE_AUX2(NBITS)

int main(int, char**)
{
    using barr_type = herring::BitArray<128,CHUNK_TYPE>;
    volatile std::size_t i;

    std::cout << "Bench mark of count() for " << NBITS << "bit chunks." << std::endl;

    auto beg_time = std::chrono::system_clock::now();
    for(unsigned long long x = 0ull; x < 0xFFFFFFFull; ++x) {
        i = barr_type{x}.count();
    }
    auto end_time = std::chrono::system_clock::now();
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - beg_time).count()
        << "ms" << std::endl;;

    return EXIT_SUCCESS;
}
