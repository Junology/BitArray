#include <iostream>
#include <bitset>

#include "LA/traits.hpp"
#include "LA/vector.hpp"
#include "BitArray.hpp"

constexpr std::size_t num_bits = 617;
constexpr std::size_t num_loop = 0x8000;

template <class T>
T quasi_xorshift(T x) {
    x = x ^ (x << 13);
    x = x ^ (x >> 7);
    x = x ^ (x << 17);
    return x;
}

template <class T>
bool test_traverse() noexcept
{
    T bset{123456789ull};

    for(std::size_t i = 0; i < num_loop; ++i) {
        std::string bs(num_bits, '0');

        BitLA::PopTraversable<T>::traverse(
            bset,
            [&bs](int i) { bs[num_bits - i - 1] = '1'; }
            );

        if(bs != bset.to_string()) {
            std::cerr << __func__ << " has failed:" << std::endl;
            std::cerr << bs << std::endl;
            std::cerr << bset << std::endl;
            return false;
        }

        bset = quasi_xorshift(bset);
    }

    return true;
}

template <class T>
bool test_range() noexcept
{
    T bits{123456789ull};

    for(std::size_t i = 0; i < num_loop; ++i) {
        std::string bs(num_bits, '0');

        for(auto i : BitLA::PopTraversable<T>::range(bits)) {
            bs[num_bits - i - 1] = '1';
        }

        if(bs != bits.to_string()) {
            std::cerr << __func__ << " has failed:" << std::endl;
            std::cerr << bs << std::endl;
            std::cerr << bits << std::endl;
            return false;
        }

        bits = quasi_xorshift(bits);
    }

    return true;
}

int main(int, char**)
{
    std::cout << "\e[34;1m---\nTest traverse()\n---\e[m" << std::endl;
    std::cout << "std::bitset" << std::endl;
    if (!test_traverse<std::bitset<num_bits>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 8bit chunks" << std::endl;
    if (!test_traverse<BitArray::BitArray<num_bits,uint8_t>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks" << std::endl;
    if (!test_traverse<BitArray::BitArray<num_bits,uint16_t>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks" << std::endl;
    if (!test_traverse<BitArray::BitArray<num_bits,uint32_t>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks" << std::endl;
    if (!test_traverse<BitArray::BitArray<num_bits,uint64_t>>())
        return EXIT_FAILURE;
    std::cout << "Passed." << std::endl;

    std::cout << "\e[34;1m---\nTest range()\n---\e[m" << std::endl;
    std::cout << "std::bitset" << std::endl;
    if (!test_range<std::bitset<num_bits>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 8bit chunks" << std::endl;
    if (!test_range<BitArray::BitArray<num_bits,uint8_t>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks" << std::endl;
    if (!test_range<BitArray::BitArray<num_bits,uint16_t>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks" << std::endl;
    if (!test_range<BitArray::BitArray<num_bits,uint32_t>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks" << std::endl;
    if (!test_range<BitArray::BitArray<num_bits,uint64_t>>())
        return EXIT_FAILURE;
    std::cout << "Passed." << std::endl;

    return EXIT_SUCCESS;
}
