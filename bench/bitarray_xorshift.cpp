#include <iostream>
#include <chrono>

#include "BitArray.hpp"

#define CHUNK_TYPE_AUX(x) uint ## x ## _t
#define CHUNK_TYPE_AUX2(x) CHUNK_TYPE_AUX(x)
#define CHUNK_TYPE CHUNK_TYPE_AUX2(NBITS)

bool quasi_xorshift() {
    static BitArray::BitArray<256,CHUNK_TYPE> x{123456789ull};
    x = x ^ (x << 13);
    x = x ^ (x >> 7);
    x = x ^ (x << 17);
    return x.test(0);
}

int main(int, char**)
{
    volatile bool i;

    std::cout << "Bench mark of the xorshift algorithm implemented with BitArray with" << NBITS << "bit chunks." << std::endl;

    auto beg_time = std::chrono::system_clock::now();
    for(unsigned long long x = 0ull; x < 0xFFFFFFull; ++x) {
        i = quasi_xorshift();
    }
    auto end_time = std::chrono::system_clock::now();
    std::cout
        << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - beg_time).count()
        << "ms" << std::endl;;

    return EXIT_SUCCESS;
}
