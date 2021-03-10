/*!
 * \file popcount_test.cpp
 * \author Jun Yoshida
 * \copyright (c) 2019-2021 Jun Yoshida.
 * The project is released under the MIT License.
 * \date January, 13 2021: created
 */

#include <iostream>

#include "BitArray/utils.hpp"

template <class T>
T naive_popcnt(T x) noexcept
{
    T result = T{0u};
    while(x) {
        if (x & T{1u}) ++result;
        x >>= 1;
    }

    return result;
}

template <class T, T max>
bool test() {
    for(T x = T{0u}; x < max; ++x) {
        T cnt = BitArray::popcount(x);
        T cnt_naive = naive_popcnt(x);
        if(cnt != cnt_naive) {
            std::cerr << "Pop-counts disagree:" << std::endl;
            std::cerr << "popcount(" << x << ")==" << cnt << std::endl;
            std::cerr << "naive_popcnt(" << x << ")==" << cnt_naive << std::endl;
            return false;
        }
    }
    return true;
}

int main(int, char**)
{
    std::cout << "\e[34;1m---\nTest for 8 bits\n---\e[m" << std::endl;
    if(!test<uint8_t,0xFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Pass." << std::endl;

    std::cout << "\e[34;1m---\nTest for 16 bits\n---\e[m" << std::endl;
    if(!test<uint16_t,0xFFFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Pass." << std::endl;

    std::cout << "\e[34;1m---\nTest for 32 bits\n---\e[m" << std::endl;
    if(!test<uint32_t,0xFFFFFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Pass." << std::endl;

#ifdef UINT64_MAX
    std::cout << "\e[34;1m---\nTest for 64 bits\n---\e[m" << std::endl;
    if(!test<uint64_t,0xFFFFFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Pass." << std::endl;
#endif

    return EXIT_SUCCESS;
}
