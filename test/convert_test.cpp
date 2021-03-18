#include <iostream>

#include "BitArray.hpp"
#include "BitArray/transforms.hpp"

constexpr std::size_t num_bits = 317;

template <class T>
T quasi_xorshift(T x) {
    x = x ^ (x << 13);
    x = x ^ (x >> 7);
    x = x ^ (x << 17);
    return x;
}

template <class T>
bool test_bindigits() noexcept
{
    using bset_t = std::bitset<std::numeric_limits<T>::digits>;

    constexpr T x0 = static_cast<T>(123456789ull);

    // bindigits() can be computed in compile-time.
    constexpr auto x0_bin = BitArray::bindigits(x0);
    std::string str0(x0_bin.data(), x0_bin.size());

    if(bset_t(x0).to_string() != str0) {
        std::cerr << "Wrong binary representation:" << std::endl;
        std::cerr << str0 << std::endl;
        std::cerr << bset_t(x0) << std::endl;
        return false;
    }

    auto x = x0;
    for(std::size_t i = 0; i < 0x10000; ++i) {
        x = quasi_xorshift(x);
        auto x_bin = BitArray::bindigits(x,'0','1');
        std::string str(x_bin.data(), x_bin.size());
        if(bset_t(x).to_string() != str) {
            std::cerr << "Wrong binary representation:" << std::endl;
            std::cerr << str << std::endl;
            std::cerr << bset_t(x) << std::endl;
            return false;
        }
    }

    return true;
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
    std::cout << "\e[34;1m---\nTest binary digit representations\n---\e[m" << std::endl;
    std::cout << "8bit binary representations:" << std::endl;
    if(!test_bindigits<std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit binary representations:" << std::endl;
    if(!test_bindigits<std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit binary representations:" << std::endl;
    if(!test_bindigits<std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit binary representations:" << std::endl;
    if(!test_bindigits<std::uint64_t>())
        return EXIT_FAILURE;
    std::cout << "Passed." << std::endl;

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
