#include <iostream>

#include "BitArray.hpp"
#include "BitArray/iterators.hpp"

constexpr std::size_t num_bits = 317;

template <class T>
constexpr BitArray::BitArray<num_bits, T> mkFizzBuzzBits() noexcept {
    BitArray::BitArray<num_bits, T> result{0u};

    // Set bitarr to be "fizz-buzz" bits (or-mixed).
    for(std::size_t i = 0; i < num_bits; ++i) {
        if (i % 11 == 0 || i % 17 == 0) {
            result.set(i);
        }
    }

    return result;
}

template <class T>
bool popiterator_test() {
    constexpr BitArray::BitArray<num_bits, T> bitarr = mkFizzBuzzBits<T>();
    auto tester = bitarr;

    auto rng = BitArray::range<BitArray::PopIterator>(bitarr);
    for(auto itr = rng.begin(); itr != rng.end(); ++itr) {
        if (!bitarr.test(*itr))
            return false;
        tester.reset(*itr);
    }

    if(tester)
        return false;
    else
        return true;
}

int main(int, char**)
{
    std::cout << "\e[34;1m---\nTest PopIterator\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!popiterator_test<std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!popiterator_test<std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!popiterator_test<std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!popiterator_test<std::uint64_t>())
        return EXIT_FAILURE;

    std::cout << "Passed." << std::endl;
    return EXIT_SUCCESS;
}

