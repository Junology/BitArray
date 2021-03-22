#include <iostream>

#include "BitArray.hpp"
#include "BitArray/manipulation.hpp"

constexpr std::size_t num_bits = 19;
constexpr std::size_t maxpop = 13;

//! Binomial coefficients
template<
    class T = std::size_t,
    class = std::enable_if_t<
        std::is_integral<T>::value && !std::is_same<T,bool>::value
        >
    >
constexpr T binom(T n, T k) noexcept
{
    if (k < 0 || k > n)
        return 0;

    if (k > n-k)
        k = n-k;

    if (k == 0)
        return 1;

    T result = 1;

    for (T i = 0; i < k; ++i)
    {
        result *= n-i;
        result /= (i+1);
    }

    return result;
}

template <std::size_t n, std::size_t p, class T>
bool test_perms() noexcept
{
    for(std::size_t m = 1; m <= p; ++m) {
        auto barr = BitArray::BitArray<n,T>{}.flip().lowpass(m);
        std::size_t c = binom(n,m);

        for(std::size_t cnt = 1; cnt < c; ++cnt) {
            auto aux = barr;
            BitArray::nextperm(aux);
            if(aux.count() != m) {
                std::cerr << __func__ << "@" << __LINE__ << std::endl;
                std::cerr << barr << std::endl;
                std::cerr << aux << std::endl;
                return false;
            }
            if(!(aux > barr)) {
                std::cerr << __func__ << "@" << __LINE__ << std::endl;
                std::cerr << barr << std::endl;
                std::cerr << aux << std::endl;
                return false;
            }
            barr = aux;
        }
    }
    return true;
}

template <std::size_t n, std::size_t p, class T>
bool test_index() noexcept
{
    for(std::size_t m = 1; m <= p; ++m) {
        auto barr = BitArray::BitArray<n,T>{}.flip().lowpass(m);
        std::size_t c = binom(n,m);

        for(std::size_t cnt = 0; cnt < c; ++cnt) {
            if(cnt)
                BitArray::nextperm(barr);

            if (BitArray::permindex(barr) != cnt) {
                std::cerr << __func__ << "@" << __LINE__ << std::endl;
                std::cerr << barr << std::endl;
                std::cerr << BitArray::permindex(barr)
                          << " != " << cnt << std::endl;
                return false;
            }
        }
    }

    return true;
}

int main(int, char**)
{
    std::cout << "\e[34;1m---\nTest iteration on bit-permutations\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!test_perms<num_bits,maxpop,std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!test_perms<num_bits,maxpop,std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!test_perms<num_bits,maxpop,std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!test_perms<num_bits,maxpop,std::uint64_t>())
        return EXIT_FAILURE;
    std::cout << "Passed." << std::endl;

    std::cout << "\e[34;1m---\nTest bit-permutation indices\n---\e[m" << std::endl;
    std::cout << "8bit chunks:" << std::endl;
    if (!test_index<num_bits,maxpop,std::uint8_t>())
        return EXIT_FAILURE;
    std::cout << "16bit chunks:" << std::endl;
    if (!test_index<num_bits,maxpop,std::uint16_t>())
        return EXIT_FAILURE;
    std::cout << "32bit chunks:" << std::endl;
    if (!test_index<num_bits,maxpop,std::uint32_t>())
        return EXIT_FAILURE;
    std::cout << "64bit chunks:" << std::endl;
    if (!test_index<num_bits,maxpop,std::uint64_t>())
        return EXIT_FAILURE;
    std::cout << "Passed." << std::endl;

    return EXIT_SUCCESS;
}

