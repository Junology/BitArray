#include <iostream>

#include "BitArray.hpp"
#include "BitArray/iterators.hpp"

constexpr std::size_t num_bits = 317;
constexpr std::size_t num_bits_small = 13;
constexpr std::size_t num_loops = 0x1000;

template <class T>
T quasi_xorshift(T x) {
    x = x ^ (x << 13);
    x = x ^ (x >> 7);
    x = x ^ (x << 17);
    return x;
}

template <std::size_t n, class T>
constexpr BitArray::BitArray<n,T> mkFizzBuzzBits() noexcept {
    BitArray::BitArray<n,T> result{0u};

    // Set bitarr to be "fizz-buzz" bits (or-mixed).
    for(std::size_t i = 0; i < n; ++i) {
        if (i % 11 == 0 || i % 17 == 0) {
            result.set(i);
        }
    }

    return result;
}

template <std::size_t n, class T>
bool fizzbuzz_test() {
    constexpr BitArray::BitArray<n,T> bitarr = mkFizzBuzzBits<n,T>();
    auto tester = bitarr;

    auto rng = BitArray::range<BitArray::PopIterator>(bitarr);
    for(auto itr = rng.begin(); itr != rng.end(); ++itr) {
        if (*itr >= n || !bitarr.test(*itr))
            return false;
        tester.reset(*itr);
    }

    if(tester)
        return false;
    else
        return true;
}

template <std::size_t n, class T>
bool xorshift_test() {
    constexpr BitArray::BitArray<n,T> bitarr{314159265358979ull};
    auto tester = bitarr;

    for(std::size_t cnt = 0; cnt < num_loops; ++cnt) {
        decltype(tester) aux{};
        for(auto i : BitArray::range<BitArray::PopIterator>(tester)) {
            if (i >= n) {
                std::cerr << __func__ << "@" << __LINE__ << ":" << std::endl;
                std::cerr << tester << std::endl;
                std::cerr << "i" << std::endl;
                return false;
            }
            aux.set(i);
        }

        if(aux != tester) {
            std::cerr << __func__ << "@" << __LINE__ << ":" << std::endl;
            std::cerr << tester << std::endl;
            std::cerr << aux << std::endl;
            return false;
        }

        tester = quasi_xorshift(tester);
    }

    return true;
}

int main(int, char**)
{
    std::cout << "\e[34;1m---\nTest FizzBuzz\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!fizzbuzz_test<num_bits,std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!fizzbuzz_test<num_bits,std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!fizzbuzz_test<num_bits,std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!fizzbuzz_test<num_bits,std::uint64_t>())
        return EXIT_FAILURE;
    std::cout << "Passed." << std::endl;

    std::cout << "\e[34;1m---\nTest FizzBuzz with a small number of bits\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!fizzbuzz_test<num_bits_small,std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!fizzbuzz_test<num_bits_small,std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!fizzbuzz_test<num_bits_small,std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!fizzbuzz_test<num_bits_small,std::uint64_t>())
        return EXIT_FAILURE;
    std::cout << "Passed." << std::endl;

    std::cout << "\e[34;1m---\nTest random arrays\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!xorshift_test<num_bits,std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!xorshift_test<num_bits,std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!xorshift_test<num_bits,std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!xorshift_test<num_bits,std::uint64_t>())
        return EXIT_FAILURE;
    std::cout << "Passed." << std::endl;

    std::cout << "\e[34;1m---\nTest random arrays with a small number of bits\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!xorshift_test<num_bits_small,std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!xorshift_test<num_bits_small,std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!xorshift_test<num_bits_small,std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!xorshift_test<num_bits_small,std::uint64_t>())
        return EXIT_FAILURE;
    std::cout << "Passed." << std::endl;

    return EXIT_SUCCESS;
}

