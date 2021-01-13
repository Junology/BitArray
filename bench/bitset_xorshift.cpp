#include <iostream>
#include <chrono>

#include <bitset>

bool quasi_xorshift() {
    static std::bitset<256> x{123456789ull};
    x = x ^ (x << 13);
    x = x ^ (x >> 7);
    x = x ^ (x << 17);
    return x.test(0);
}

int main(int, char**)
{
    volatile bool i;

    std::cout << "Bench mark of the xorshift algorithm implemented with std::bitset." << std::endl;

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
