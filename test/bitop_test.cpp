#include <iostream>
#include <thread>

#include "BitArray.hpp"
#include "BitArray/transforms.hpp"
#include "BitArray/manipulation.hpp"

constexpr std::size_t num_bits = 317;
constexpr std::size_t num_loop = 0x8000;

template <class T>
T quasi_xorshift(T x) {
    x = x ^ (x << 13);
    x = x ^ (x >> 7);
    x = x ^ (x << 17);
    return x;
}

template <class T, std::size_t N>
bool set_vs_arr(std::bitset<N> const& bset, herring::BitArray<N,T> const& barr)
{
    for(std::size_t i = 0; i < N; ++i) {
        if (barr.test(i) != bset.test(i)) {
            std::cerr << "Mismatch:" << std::endl;
            std::cerr << "std::bitset:\n" << bset.to_string() << std::endl;
            std::cerr << "BitArray:\n" << barr << std::endl;
            return false;
        }
    }

    return true;
}

template <class T>
bool test_qxorshift() noexcept
{
    std::bitset<num_bits> bset{123456789ull};
    herring::BitArray<num_bits,T> barr{123456789ull};

    for(std::size_t i = 0; i < num_loop; ++i) {
        if (!set_vs_arr(bset, barr))
            return false;
        bset = quasi_xorshift(bset);
        barr = quasi_xorshift(barr);
    }

    return true;
}

template <class T>
bool test_or() noexcept
{
    std::bitset<num_bits> bset1{123456789ull}, bset2{0xABCDEFull};
    herring::BitArray<num_bits,T> barr1{123456789ull}, barr2{0xABCDEFull};

    for(std::size_t i = 0; i < num_loop; ++i) {
        if (!set_vs_arr(bset1 | bset2, barr1 | barr2))
            return false;
        bset1 = quasi_xorshift(bset1);
        bset2 = quasi_xorshift(bset2);
        barr1 = quasi_xorshift(barr1);
        barr2 = quasi_xorshift(barr2);
    }

    return true;
}

template <class T>
bool test_and() noexcept
{
    std::bitset<num_bits> bset1{123456789ull}, bset2{0xABCDEFull};
    herring::BitArray<num_bits,T> barr1{123456789ull}, barr2{0xABCDEFull};

    for(std::size_t i = 0; i < num_loop; ++i) {
        if (!set_vs_arr(bset1 & bset2, barr1 & barr2))
            return false;
        bset1 = quasi_xorshift(bset1);
        bset2 = quasi_xorshift(bset2);
        barr1 = quasi_xorshift(barr1);
        barr2 = quasi_xorshift(barr2);
    }

    return true;
}

template <class T>
bool test_not() noexcept
{
    std::bitset<num_bits> bset{123456789ull};
    herring::BitArray<num_bits,T> barr{123456789ull};

    for(std::size_t i = 0; i < num_loop; ++i) {
        if (!set_vs_arr(~bset, ~barr))
            return false;
        bset = quasi_xorshift(bset);
        barr = quasi_xorshift(barr);
    }

    return true;
}

template <class T>
bool test_increment() noexcept
{
    // max + 1 should be 0.
    {
        auto x = herring::BitArray<num_bits>{}.flip();
        ++x;
        if(x) {
            std::cerr << __func__ << "@" << __LINE__ << std::endl;
            std::cerr << x << std::endl;
            return false;
        }
    }

    constexpr unsigned long addee_num = 10000u;
    herring::BitArray<num_bits> bset{123456789ull};
    herring::BitArray<num_bits> addee{addee_num};

    for(std::size_t i = 0; i < num_loop; ++i) {
        herring::BitArray<num_bits> add_result = bset;

        // Adder
        std::thread adder(
            [&addee,&add_result]{
                auto conti = addee;
                while(conti) {
                    auto conti_next = add_result & conti;
                    add_result ^= conti;
                    conti = (conti_next << 1u);
                }
            });

        // Million times increments
        for(std::size_t j = 0; j < addee_num; ++j) {
            ++bset;
        }

        adder.join();

        if (bset != add_result) {
            std::cerr << i << ":" << std::endl;
            std::cerr << bset << std::endl;
            std::cerr << add_result << std::endl;
            return false;
        }
    }

    return true;
}

template <class T>
bool test_msb() noexcept
{
    using barr_t = herring::BitArray<num_bits,T>;
    barr_t barr{314159265358979ull};

    for(std::size_t cnt = 0; cnt < num_loop; ++cnt) {
        if((barr >> barr.msb()) != barr_t{1u}) {
            std::cerr << __func__ << "@" << __LINE__ << std::endl;
            std::cerr << barr.msb() << "@" << barr << std::endl;
            return false;
        }
        barr = quasi_xorshift(barr);
    }
    return true;
}

int main(int, char**)
{
    std::cout << "\e[34;1m---\nTest quasi_xorshift()\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!test_qxorshift<std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!test_qxorshift<std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!test_qxorshift<std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!test_qxorshift<std::uint64_t>())
        return EXIT_FAILURE;

    std::cout << "\e[34;1m---\nTest 'or' operation\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!test_or<std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!test_or<std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!test_or<std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!test_or<std::uint64_t>())
        return EXIT_FAILURE;

    std::cout << "\e[34;1m---\nTest 'and' operation\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!test_and<std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!test_and<std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!test_and<std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!test_and<std::uint64_t>())
        return EXIT_FAILURE;

    std::cout << "\e[34;1m---\nTest 'not' operation\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!test_not<std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!test_not<std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!test_not<std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!test_not<std::uint64_t>())
        return EXIT_FAILURE;

    std::cout << "\e[34;1m---\nTest 'increment' operation\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!test_increment<std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!test_increment<std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!test_increment<std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!test_increment<std::uint64_t>())
        return EXIT_FAILURE;

    std::cout << "\e[34;1m---\nTest Most Significant Bit (MSB).\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!test_msb<std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!test_msb<std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!test_msb<std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!test_msb<std::uint64_t>())
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}
