#include <iostream>
#include <bitset>

#include "LA/traits.hpp"
#include "LA/vector.hpp"
#include "LA/matrix.hpp"
#include "LA/decompose.hpp"
#include "BitArray.hpp"

constexpr std::size_t nrows = 137;
constexpr std::size_t ncols = 59;
constexpr std::size_t num_loop = 0x100;

template <class T, std::size_t r, std::size_t c>
void show_colmat(BitLA::Matrix<herring::BitArray<r,T>,c,BitLA::MatrixMode::ColumnMajor> const& mat)
{
    for(std::size_t i = 0; i < r; ++i) {
        std::cout << "│";
        for(std::size_t j = 0; j < c; ++j) {
            std::cout << mat.coeff(i,j);
        }
        std::cout << "│" << std::endl;
    }
}

template <class T>
T quasi_xorshift(T x) {
    x = x ^ (x << 13);
    x = x ^ (x >> 7);
    x = x ^ (x << 17);
    return x;
}

template <BitLA::MatrixMode mode,class T, std::size_t...Is>
constexpr
BitLA::Matrix<T,sizeof...(Is),mode>
random_mat(T seed, std::index_sequence<Is...>) noexcept
{
    struct _lambda {
        static constexpr T lp(std::size_t i, T init) noexcept {
            for(std::size_t j = 0; j < i; ++j) {
                init = init ^ (init << 49);
                init = init ^ (init >> 15);
                init = init ^ (init << 61);
            }
            return init;
        }
    };

    return BitLA::Matrix<T,sizeof...(Is),mode>(_lambda::lp(Is,seed)...);
}

template <class T, std::size_t r, std::size_t c>
constexpr bool is_upper_triangular(
    BitLA::Matrix<herring::BitArray<r,T>,c,BitLA::MatrixMode::ColumnMajor> const& mat
    ) noexcept
{
    for(std::size_t i = 1; i < r; ++i) {
        for(std::size_t j = 0; j < i; ++j) {
            if(mat.coeff(i,j))
                return false;
        }
    }
    return true;
}

template <class T, std::size_t r, std::size_t c>
constexpr bool test_plu() noexcept
{
    using vec_t = herring::BitArray<r,T>;

    constexpr auto ident = BitLA::Matrix<herring::BitArray<c,T>,c,BitLA::MatrixMode::ColumnMajor>::diagonal(herring::BitArray<c,T>{}.flip());

    auto mat = random_mat<BitLA::MatrixMode::ColumnMajor>(
        vec_t{314159265358979ull},
        std::make_index_sequence<c>()
        );

    for(std::size_t cnt = 0; cnt < num_loop; ++cnt) {
        auto aux = mat;
        auto transf = BitLA::decomp::plu(aux);

        // Check that the return value is an inverse pair.
        if(transf[0] * transf[1] != ident
           || transf[1] * transf[0] != ident)
        {
            std::cerr << __func__ << "@" << __LINE__ << std::endl;
            return false;
        }

        // Check that the return value contains the transformation matrix
        if(aux * transf[0] != mat) {
            std::cerr << __func__ << "@" << __LINE__ << std::endl;
            return false;
        }

        // Check that the transformation matrix is upper-triangular
        if(!is_upper_triangular(transf[0])) {
            std::cerr << __func__ << "@" << __LINE__ << std::endl;
            return false;
        }

        // Check that the result is lower-triangular up to permutation.
        for(std::size_t i = 0; i+1 < mat.cols(); ++i) {
            auto piv = aux.vec(i).countTrail0();

            if(piv == mat.rows())
                continue;

            for(std::size_t j = i+1; j < mat.cols(); ++j) {
                if(aux.coeff(piv,j)) {
                    std::cerr << __func__ << "@" << __LINE__ << std::endl;
                    return false;
                }
            }
        }

        // Update
        for(std::size_t i = 0; i < mat.cols(); ++i) {
            mat.vec(i) = quasi_xorshift(mat.vec(i));
        }
    }

    return true;
}

int main(int, char**)
{
    // Demo
    {
        constexpr std::size_t dr = 7;
        constexpr std::size_t dc = 9;
        using vec_t = herring::BitArray<dr,uint64_t>;
        constexpr BitLA::Matrix<vec_t,dc,BitLA::MatrixMode::ColumnMajor> mat{
            vec_t{1415926535ull},
            vec_t{8979323846ull},
            vec_t{2643383279ull},
            vec_t{5028841971ull},
            vec_t{6939937510ull},
            vec_t{5820974944ull},
            vec_t{5923078164ull},
            vec_t{628620899ull},
            vec_t{8628034825ull}
        };
        show_colmat(mat);
        std::cout << "=" << std::endl;
        auto aux = mat;
        auto transf = BitLA::decomp::plu(aux);
        show_colmat(aux);
        std::cout << "*" << std::endl;
        show_colmat(transf[0]);
        std::cout << "=" << std::endl;
        show_colmat(aux * transf[0]);
    }
    std::cout << "\e[34;1m---\nTest PLU decomposition\n---\e[m" << std::endl;
    std::cout << "BitArray with 8bit chunks@RowMajor" << std::endl;
    if (!test_plu<uint8_t,nrows,ncols>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks@RowMajor" << std::endl;
    if (!test_plu<uint16_t,nrows,ncols>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks@RowMajor" << std::endl;
    if (!test_plu<uint32_t,nrows,ncols>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks@RowMajor" << std::endl;
    if (!test_plu<uint64_t,nrows,ncols>())
        return EXIT_FAILURE;

    std::cout << "Passed" << std::endl;
    return EXIT_SUCCESS;
}
