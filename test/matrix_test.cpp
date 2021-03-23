#include <iostream>
#include <bitset>

#include "LA/traits.hpp"
#include "LA/vector.hpp"
#include "LA/matrix.hpp"
#include "BitArray.hpp"
#include "BitArray/manipulation.hpp"

constexpr std::size_t num_bits = 137;
constexpr std::size_t num_loop = 0x20;

template <class T>
T quasi_xorshift(T x) {
    x = x ^ (x << 13);
    x = x ^ (x >> 7);
    x = x ^ (x << 17);
    return x;
}

template <class T>
T quasi_xorshift2(T x) {
    x = x ^ (x << 49);
    x = x ^ (x >> 15);
    x = x ^ (x << 61);
    return x;
}

template <std::size_t n, class T>
constexpr BitLA::Matrix<herring::BitArray<n,T>,n,BitLA::MatrixMode::RowMajor>
xorshift_rowmat() noexcept
{
    using vec_t = herring::BitArray<n,T>;
    using mat_t = BitLA::Matrix<vec_t,n,BitLA::MatrixMode::RowMajor>;

    // 13 left shift
    auto lshift13 = mat_t::diagonal(vec_t{}.flip());
    for(std::size_t i = 0; i+13 < n; ++i)
        lshift13.vec(i).set(i+13);

    // 7 right shift
    auto rshift7 = mat_t::diagonal(vec_t{}.flip());
    for(std::size_t i = 0; i+7 < n; ++i)
        rshift7.vec(i+7).set(i);

    // 17 left shift
    auto lshift17 = mat_t::diagonal(vec_t{}.flip());
    for(std::size_t i = 0; i+17 < n; ++i)
        lshift17.vec(i).set(i+17);

    return lshift13*rshift7*lshift17;
}

template <std::size_t n, class T>
constexpr BitLA::Matrix<herring::BitArray<n,T>,n,BitLA::MatrixMode::ColumnMajor>
xorshift_colmat() noexcept
{
    using vec_t = herring::BitArray<n,T>;
    using mat_t = BitLA::Matrix<vec_t,n,BitLA::MatrixMode::ColumnMajor>;

    // 13 left shift
    auto lshift13 = mat_t::diagonal(vec_t{}.flip());
    for(std::size_t i = 0; i+13 < n; ++i)
        lshift13.vec(i).set(i+13);

    // 7 right shift
    auto rshift7 = mat_t::diagonal(vec_t{}.flip());
    for(std::size_t i = 0; i+7 < n; ++i)
        rshift7.vec(i+7).set(i);

    // 17 left shift
    auto lshift17 = mat_t::diagonal(vec_t{}.flip());
    for(std::size_t i = 0; i+17 < n; ++i)
        lshift17.vec(i).set(i+17);

    return lshift17*rshift7*lshift13;
}

template <std::size_t n,class T>
bool test_xorshift() noexcept
{
    constexpr auto mat_row = xorshift_rowmat<n,T>();
    constexpr auto mat_col = xorshift_colmat<n,T>();
    herring::BitArray<n,T> vec{314159265358979ull};

    for(std::size_t i = 0; i < num_loop; ++i) {
        vec = quasi_xorshift2(vec);
        for(std::size_t j = 0; j < num_loop; ++j) {
            auto aux = vec * mat_row;
            if(aux != quasi_xorshift(vec)) {
                std::cerr << __func__ << "@" << __LINE__ << ":" << std::endl;
                std::cerr << aux << std::endl;
                std::cerr << quasi_xorshift(vec) << std::endl;
                return false;
            }
            if(aux != mat_col*vec) {
                std::cerr << __func__ << "@" << __LINE__ << ":" << std::endl;
                std::cerr << aux << std::endl;
                std::cerr << mat_col*vec << std::endl;
                return false;
            }
            vec = aux;
        }
    }

    return true;
}

template <class T>
bool test_identity() noexcept
{
    auto idmat_row = BitLA::Matrix<T,num_bits,BitLA::MatrixMode::RowMajor>::diagonal(T{}.flip());
    auto idmat_col = BitLA::Matrix<T,num_bits,BitLA::MatrixMode::ColumnMajor>::diagonal(T{}.flip());

    // Check coefficients
    for(std::size_t i = 0; i < num_bits; ++i) {
        for(std::size_t j = 0; j < num_bits; ++j) {
            if(idmat_row.coeff(i,j) != (i==j)) {
                std::cerr << __func__ << " has wrong coeff @" << __LINE__ << ":" << std::endl;
                return false;
            }
            if(idmat_col.coeff(i,j) != (i==j)) {
                std::cerr << __func__ << "has wrong coeff @" << __LINE__ << ":" << std::endl;
                return false;
            }
        }
    }

    // Check unitality
    T bset{123456789ull};

    for(std::size_t i = 0; i < num_loop; ++i) {
        bset = quasi_xorshift(bset);

        auto bset_r = bset * idmat_row;
        auto bset_c = idmat_col * bset;

        if(bset_r != bset) {
            std::cerr << __func__ << " has failed @" << __LINE__ << ":" << std::endl;
            std::cerr << bset_r << std::endl;
            std::cerr << bset << std::endl;
            return false;
        }

        if(bset_c != bset) {
            std::cerr << __func__ << " has failed @" << __LINE__ << ":" << std::endl;
            std::cerr << bset_c << std::endl;
            std::cerr << bset << std::endl;
            return false;
        }
    }

    return true;
}

template <class T,BitLA::MatrixMode mode>
bool test_mult() noexcept
{
    constexpr std::size_t num_vecs = 91;
    using mat_lhs_t = BitLA::Matrix<T,num_vecs,mode>;
    using mat_rhs_t = BitLA::Matrix<std::bitset<num_vecs>,num_bits,mode>;
    T seed{31415926535ull};
    std::bitset<num_vecs> seed2{123456789ull};
    mat_lhs_t lhs;
    mat_rhs_t rhs;

    // initialize test matrices
    for(std::size_t i = 0; i < num_vecs; ++i) {
        lhs.vec_us(i) = seed;
        seed = quasi_xorshift2(seed);
    }
    for(std::size_t i = 0; i < num_bits; ++i) {
        rhs.vec_us(i) = seed2;
        seed2 = quasi_xorshift2(seed2);
    }

    // Test
    for(std::size_t count = 0; count < num_loop; ++count) {
        auto mat_lr = lhs*rhs;
        auto mat_rl = rhs*lhs;

        // test mat_lr
        for(std::size_t i = 0; i < mat_lhs_t::nrows; ++i) {
            for(std::size_t j = 0; j < mat_rhs_t::ncols; ++j) {
                bool b = mat_lr.coeff(i,j);
                for(std::size_t k = 0; k < mat_lhs_t::ncols; ++k)
                    b ^= lhs.coeff(i,k) & rhs.coeff(k,j);

                if(b) {
                    std::cerr << __func__ << "@" << __LINE__ << std::endl;
                    return false;
                }
            }
        }

        // test mat_rl
        for(std::size_t i = 0; i < mat_rhs_t::nrows; ++i) {
            for(std::size_t j = 0; j < mat_lhs_t::ncols; ++j) {
                bool b = mat_rl.coeff(i,j);
                for(std::size_t k = 0; k < mat_rhs_t::ncols; ++k) {
                    b ^= rhs.coeff(i,k) & lhs.coeff(k,j);
                }

                if(b) {
                    std::cerr << __func__ << "@" << __LINE__ << std::endl;
                    return false;
                }
            }
        }

        // Update
        for(std::size_t i = 0; i < num_vecs; ++i)
            lhs.vec(i) = quasi_xorshift(lhs.vec(i));
        for(std::size_t j = 0; j < num_bits; ++j)
            rhs.vec(j) = quasi_xorshift(rhs.vec(j));
    }

    return true;
}


int main(int, char**)
{
    std::cout << "\e[34;1m---\nTest on identity matrices\n---\e[m" << std::endl;
    std::cout << "std::bitset" << std::endl;
    if (!test_identity<std::bitset<num_bits>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 8bit chunks" << std::endl;
    if (!test_identity<herring::BitArray<num_bits,uint8_t>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks" << std::endl;
    if (!test_identity<herring::BitArray<num_bits,uint16_t>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks" << std::endl;
    if (!test_identity<herring::BitArray<num_bits,uint32_t>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks" << std::endl;
    if (!test_identity<herring::BitArray<num_bits,uint64_t>>())
        return EXIT_FAILURE;
    std::cout << "Passed." << std::endl;

    std::cout << "\e[34;1m---\nMatrix xorshift\n---\e[m" << std::endl;
    std::cout << "BitArray with 8bit chunks" << std::endl;
    if (!test_xorshift<num_bits,uint8_t>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks" << std::endl;
    if (!test_identity<herring::BitArray<num_bits,uint16_t>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks" << std::endl;
    if (!test_identity<herring::BitArray<num_bits,uint32_t>>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks" << std::endl;
    if (!test_identity<herring::BitArray<num_bits,uint64_t>>())
        return EXIT_FAILURE;
    std::cout << "Passed." << std::endl;

    std::cout << "\e[34;1m---\nTest multiplications\n---\e[m" << std::endl;
    std::cout << "std::bitset@RowMajor" << std::endl;
    if (!test_mult<std::bitset<num_bits>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "std::bitset@ColumnMajor" << std::endl;
    if (!test_mult<std::bitset<num_bits>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 8bit chunks@RowMajor" << std::endl;
    if (!test_mult<herring::BitArray<num_bits,uint8_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 8bit chunks@ColumnMajor" << std::endl;
    if (!test_mult<herring::BitArray<num_bits,uint8_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks@RowMajor" << std::endl;
    if (!test_mult<herring::BitArray<num_bits,uint16_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks@ColumnMajor" << std::endl;
    if (!test_mult<herring::BitArray<num_bits,uint16_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks@RowMajor" << std::endl;
    if (!test_mult<herring::BitArray<num_bits,uint32_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks@ColumnMajor" << std::endl;
    if (!test_mult<herring::BitArray<num_bits,uint32_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks@RowMajor" << std::endl;
    if (!test_mult<herring::BitArray<num_bits,uint64_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks@ColumnMajor" << std::endl;
    if (!test_mult<herring::BitArray<num_bits,uint64_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
