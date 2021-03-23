/*!
 * \file trails_test.cpp
 * \author Jun Yoshida
 * \copyright (c) 2019-2021 Jun Yoshida.
 * The project is released under the MIT License.
 * \date January, 13 2021: created
 */

#include <iostream>
#include <limits>
#include <tuple>
#include <bitset>

#include "detail/utils.hpp"

template <class T>
T naive_trail0(T x) noexcept
{
    T result = T{0u};
    while(!(x & 0b1u) && result < std::numeric_limits<T>::digits) {
        ++result;
        x >>= 1;
    }

    return result;
}

template <class T>
T naive_trail1(T x) noexcept
{
    T result = T{0u};
    while(x & 0b1u) {
        ++result;
        x >>= 1;
    }

    return result;
}

template <class T, T max>
bool test0() {
    for(T x = T{0u}; x < max; ++x) {
        T cnt = herring::counttrail0(x);
        T cnt_naive = naive_trail0(x);
        if(cnt != cnt_naive) {
            std::cerr << "Counts of trail0 disagree:" << std::endl;
            std::cerr << "countrail0(" << x << ")==" << cnt << std::endl;
            std::cerr << "naive_trail0(" << x << ")==" << cnt_naive << std::endl;
            return false;
        }
    }
    return true;
}

template <class T, T max>
bool test1() {
    for(T x = T{0u}; x < max; ++x) {
        T cnt = herring::counttrail1(x);
        T cnt_naive = naive_trail1(x);
        if(cnt != cnt_naive) {
            std::cerr << "Counts of trail1 disagree:" << std::endl;
            std::cerr << "countrail1(" << x << ")==" << cnt << std::endl;
            std::cerr << "naive_trail1(" << x << ")==" << cnt_naive << std::endl;
            return false;
        }
    }
    return true;
}

template <class T, T max = std::numeric_limits<T>::max()>
bool test_msb() noexcept
{
    for(T x = T{1u}; x < max; ++x) {
        if((x >> herring::msb(x)) != 1u) {
            std::cerr << "Wrong MSB:" << std::endl;
            std::cerr << herring::msb(x) << "@"
                      << std::bitset<std::numeric_limits<T>::digits>(x)
                      << std::endl;
            return false;
        }
    }
    return true;
}

int main(int, char**)
{
    std::cout << "\e[34;1m---\nmake_from_tuple\n---\e[m" << std::endl;
    auto tup0 = std::make_tuple(1,3.14,'c');
    auto tup = herring::make_from_tuple<std::tuple<int,double,char>>(tup0);
    if(std::get<0>(tup) != std::get<0>(tup0)) {
        std::cerr << "The first component of tup: " << std::get<0>(tup) << std::endl;
        std::cerr << "Should be: " << std::get<0>(tup0) << std::endl;
    }
    if(std::get<1>(tup) != std::get<1>(tup0)) {
        std::cerr << "The first component of tup: " << std::get<1>(tup) << std::endl;
        std::cerr << "Should be: " << std::get<1>(tup0) << std::endl;
    }
    if(std::get<2>(tup) != std::get<2>(tup0)) {
        std::cerr << "The first component of tup: " << std::get<2>(tup) << std::endl;
        std::cerr << "Should be: " << std::get<2>(tup0) << std::endl;
    }

    std::cout << "\e[34;1m---\nTest for 8 bits\n---\e[m" << std::endl;
    std::cout << "Count of trailing 0:" << std::endl;
    if(!test0<uint8_t,0xFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Count of trailing 1:" << std::endl;
    if(!test1<uint8_t,0xFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Most Significant Bit:" << std::endl;
    if(!test_msb<uint8_t,0xFFu>()) {
        return EXIT_FAILURE;
    }
    std::cout << "Pass." << std::endl;

    std::cout << "\e[34;1m---\nTest for 16 bits\n---\e[m" << std::endl;
    std::cout << "Count of trailing 0:" << std::endl;
    if(!test0<uint16_t,0xFFFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Count of trailing 1:" << std::endl;
    if(!test1<uint16_t,0xFFFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Most Significant Bit:" << std::endl;
    if(!test_msb<uint16_t>()) {
        return EXIT_FAILURE;
    }
    std::cout << "Pass." << std::endl;

    std::cout << "\e[34;1m---\nTest for 32 bits\n---\e[m" << std::endl;
    std::cout << "Count of trailing 0:" << std::endl;
    if(!test0<uint32_t,0xFFFFFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Count of trailing 1:" << std::endl;
    if(!test1<uint32_t,0xFFFFFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Most Significant Bit:" << std::endl;
    if(!test_msb<uint32_t,0xFFFFFFu>()) {
        return EXIT_FAILURE;
    }
    std::cout << "Pass." << std::endl;

#ifdef UINT64_MAX
    std::cout << "\e[34;1m---\nTest for 64 bits\n---\e[m" << std::endl;
    std::cout << "Count of trailing 0:" << std::endl;
    if(!test0<uint64_t,0xFFFFFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Count of trailing 1:" << std::endl;
    if(!test1<uint64_t,0xFFFFFFu>()) {
        std::cerr << "Test fails." << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Most Significant Bit:" << std::endl;
    if(!test_msb<uint64_t,0xFFFFFFu>()) {
        return EXIT_FAILURE;
    }
    std::cout << "Pass." << std::endl;
#endif

    return EXIT_SUCCESS;
}
