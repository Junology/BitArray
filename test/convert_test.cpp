#include <iostream>

#include "BitArray.hpp"
#include "transforms.hpp"

constexpr std::size_t num_bits = 317;

template <class T>
T quasi_xorshift(T x) {
    x = x ^ (x << 13);
    x = x ^ (x >> 7);
    x = x ^ (x << 17);
    return x;
}

template <class T>
bool test_from_bitset() noexcept
{
    std::bitset<num_bits> bset{123456789ull};

    for(std::size_t i = 0; i < 0x10000; ++i) {
        auto barr = BitArray::from::bitset<T>(bset);
        for(std::size_t j = 0; j < num_bits; ++j) {
            if (barr.test(j) != bset.test(j)) {
                std::cerr << "Mismatch:" << std::endl;
                std::cerr << "std::bitset:\n" << bset.to_string() << std::endl;
                std::cerr << "BitArray:\n" << barr << std::endl;
                return false;
            }
        }
        bset = quasi_xorshift(bset);
    }

    return true;
}

template <class T>
bool test_to_bitset() noexcept
{
    BitArray::BitArray<num_bits,T> barr{123456789ull};

    for(std::size_t i = 0; i < 0x10000; ++i) {
        auto bset = BitArray::to::bitset(barr);
        for(std::size_t j = 0; j < num_bits; ++j) {
            if (barr.test(j) != bset.test(j)) {
                std::cerr << "Mismatch:" << std::endl;
                std::cerr << "std::bitset:\n" << bset.to_string() << std::endl;
                std::cerr << "BitArray:\n" << barr << std::endl;
                return false;
            }
        }
        barr = quasi_xorshift(barr);
    }

    return true;
}

int main(int, char**)
{
    std::cout << "\e[34;1m---\nTest std::bitset -> BitArray\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!test_from_bitset<std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!test_from_bitset<std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!test_from_bitset<std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!test_from_bitset<std::uint64_t>())
        return EXIT_FAILURE;

    std::cout << "\e[34;1m---\nTest BitArray -> std::bitset\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!test_to_bitset<std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!test_to_bitset<std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!test_to_bitset<std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!test_to_bitset<std::uint64_t>())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
