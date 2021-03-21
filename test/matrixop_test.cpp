#include <iostream>
#include <bitset>

#include "LA/traits.hpp"
#include "LA/vector.hpp"
#include "LA/matrix.hpp"
#include "BitArray.hpp"
#include "BitArray/manipulation.hpp"

constexpr std::size_t num_bits = 71;
constexpr std::size_t num_vecs = 39;
constexpr std::size_t num_loop = 0x20;

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

template <class T, BitLA::MatrixMode mode>
bool test_scalar()
{
    auto mat = random_mat<mode>(
        T{314159265358979ull}, std::make_index_sequence<num_vecs>()
        );

    for(std::size_t cnt = 0; cnt < num_loop; ++cnt) {
        // Test scalar row
        for(std::size_t i = 0; i < mat.rows(); ++i) {
            auto aux = mat;

            if(aux.scalar_row(true,i) != mat) {
                std::cerr << __func__ << "@" << __LINE__ << std::endl;
                return false;
            }

            aux.scalar_row(false,i);
            for(std::size_t j = 0; j < mat.rows(); ++j) {
                for(std::size_t k = 0; k < mat.cols(); ++k) {
                    if ((i!=j && aux.coeff(j,k) != mat.coeff(j,k))
                        || (i==j && aux.coeff(j,k))) {
                        std::cerr << __func__ << "@" << __LINE__ << std::endl;
                        return false;
                    }
                }
            }
        }
        // Test scalar col
        for(std::size_t i = 0; i < mat.cols(); ++i) {
            auto aux = mat;

            if(aux.scalar_col(true,i) != mat) {
                std::cerr << __func__ << "@" << __LINE__ << std::endl;
                return false;
            }

            aux.scalar_col(false,i);
            for(std::size_t j = 0; j < mat.rows(); ++j) {
                for(std::size_t k = 0; k < mat.cols(); ++k) {
                    if ((i!=k && aux.coeff(j,k) != mat.coeff(j,k))
                        || (i==k && aux.coeff(j,k))) {
                        std::cerr << __func__ << "@" << __LINE__ << std::endl;
                        return false;
                    }
                }
            }
        }
        // Update
        for(std::size_t i = 0; i < num_vecs; ++i) {
            mat.vec(i) = quasi_xorshift(mat.vec(i));
        }
    }

    return true;
}

template <class T, BitLA::MatrixMode mode>
bool test_swap()
{
    auto mat = random_mat<mode>(
        T{314159265358979ull}, std::make_index_sequence<num_vecs>()
        );

    for(std::size_t cnt = 0; cnt < num_loop; ++cnt) {
        // Test swap rows
        for(std::size_t i = 0; i < mat.rows(); ++i) {
            for(std::size_t j = 0; j < mat.rows(); ++j) {
                auto aux = mat;
                aux.swap_rows(i,j);

                for(std::size_t k = 0; k < mat.rows(); ++k) {
                    for(std::size_t l = 0; l < mat.cols(); ++l) {
                        if(k==i && aux.coeff(i,l) != mat.coeff(j,l)) {
                            std::cerr << __func__ << "@" << __LINE__ << std::endl;
                            std::cerr << "i=" << i << ", j=" << j << std::endl;
                            std::cout << aux.vec(i) << std::endl;
                            std::cout << aux.vec(j) << std::endl;
                            std::cout << mat.vec(i) << std::endl;
                            std::cout << mat.vec(j) << std::endl;
                            return false;
                        }
                        else if(k==j && aux.coeff(j,l) != mat.coeff(i,l)) {
                            std::cerr << __func__ << "@" << __LINE__ << std::endl;
                            std::cerr << "i=" << i << ", j=" << j << std::endl;
                            return false;
                        }
                        else if(k!=i && k!= j && aux.coeff(k,l) != mat.coeff(k,l)) {
                            std::cerr << __func__ << "@" << __LINE__ << std::endl;
                            std::cerr << "i=" << i << ", j=" << j << std::endl;
                            return false;
                        }
                    }
                }
            }
        }
        // Test swap cols
        for(std::size_t i = 0; i < mat.cols(); ++i) {
            for(std::size_t j = 0; j < mat.cols(); ++j) {
                auto aux = mat;
                aux.swap_cols(i,j);

                for(std::size_t k = 0; k < mat.rows(); ++k) {
                    for(std::size_t l = 0; l < mat.cols(); ++l) {
                        if(l==i && aux.coeff(k,i) != mat.coeff(k,j)) {
                            std::cerr << __func__ << "@" << __LINE__ << std::endl;
                            return false;
                        }
                        else if(l==j && aux.coeff(k,j) != mat.coeff(k,i)) {
                            std::cerr << __func__ << "@" << __LINE__ << std::endl;
                            return false;
                        }
                        else if(l!=i && l!= j && aux.coeff(k,l) != mat.coeff(k,l)) {
                            std::cerr << __func__ << "@" << __LINE__ << std::endl;
                            return false;
                        }
                    }
                }
            }
        }
        // Update
        for(std::size_t i = 0; i < num_vecs; ++i) {
            mat.vec(i) = quasi_xorshift(mat.vec(i));
        }
    }

    return true;
}

template <class T, BitLA::MatrixMode mode>
bool test_axpy()
{
    auto mat = random_mat<mode>(
        T{314159265358979ull}, std::make_index_sequence<num_vecs>()
        );

    for(std::size_t cnt = 0; cnt < num_loop; ++cnt) {
        // Test axpy rows
        for(std::size_t i = 0; i < mat.rows(); ++i) {
            for(std::size_t j = 0; j < mat.rows(); ++j) {
                auto aux = mat;
                if(aux.axpy_rows(false, i,j) != mat) {
                    std::cerr << __func__ << "@" << __LINE__ << std::endl;
                    return false;
                }

                aux.axpy_rows(true,i,j);
                for(std::size_t k = 0; k < mat.rows(); ++k) {
                    for(std::size_t l = 0; l < mat.cols(); ++l) {
                        if(k==j && aux.coeff(j,l) != (mat.coeff(i,l)^mat.coeff(j,l))) {
                            std::cerr << __func__ << "@" << __LINE__ << std::endl;
                            return false;
                        }
                        else if(k!=j && aux.coeff(k,l) != mat.coeff(k,l)) {
                            std::cerr << __func__ << "@" << __LINE__ << std::endl;
                            return false;
                        }
                    }
                }
            }
        }
        // Test axpy rows
        for(std::size_t i = 0; i < mat.cols(); ++i) {
            for(std::size_t j = 0; j < mat.cols(); ++j) {
                auto aux = mat;
                if(aux.axpy_cols(false, i,j) != mat) {
                    std::cerr << __func__ << "@" << __LINE__ << std::endl;
                    return false;
                }

                aux.axpy_cols(true,i,j);
                for(std::size_t k = 0; k < mat.rows(); ++k) {
                    for(std::size_t l = 0; l < mat.cols(); ++l) {
                        if(l==j && aux.coeff(k,j) != (mat.coeff(k,i)^mat.coeff(k,j))) {
                            std::cerr << __func__ << "@" << __LINE__ << std::endl;
                            return false;
                        }
                        else if(l!=j && aux.coeff(k,l) != mat.coeff(k,l)) {
                            std::cerr << __func__ << "@" << __LINE__ << std::endl;
                            return false;
                        }
                    }
                }
            }
        }
        // Update
        for(std::size_t i = 0; i < num_vecs; ++i) {
            mat.vec(i) = quasi_xorshift(mat.vec(i));
        }
    }

    return true;
}

int main(int, char**)
{
    std::cout << "\e[34;1m---\nTest scalar op\n---\e[m" << std::endl;
    std::cout << "std::bitset@RowMajor" << std::endl;
    if (!test_scalar<std::bitset<num_bits>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "std::bitset@ColumnMajor" << std::endl;
    if (!test_scalar<std::bitset<num_bits>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 8bit chunks@RowMajor" << std::endl;
    if (!test_scalar<BitArray::BitArray<num_bits,uint8_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 8bit chunks@ColumnMajor" << std::endl;
    if (!test_scalar<BitArray::BitArray<num_bits,uint8_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks@RowMajor" << std::endl;
    if (!test_scalar<BitArray::BitArray<num_bits,uint16_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks@ColumnMajor" << std::endl;
    if (!test_scalar<BitArray::BitArray<num_bits,uint16_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks@RowMajor" << std::endl;
    if (!test_scalar<BitArray::BitArray<num_bits,uint32_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks@ColumnMajor" << std::endl;
    if (!test_scalar<BitArray::BitArray<num_bits,uint32_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks@RowMajor" << std::endl;
    if (!test_scalar<BitArray::BitArray<num_bits,uint64_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks@ColumnMajor" << std::endl;
    if (!test_scalar<BitArray::BitArray<num_bits,uint64_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;

    std::cout << "\e[34;1m---\nTest swap op\n---\e[m" << std::endl;
    std::cout << "std::bitset@RowMajor" << std::endl;
    if (!test_swap<std::bitset<num_bits>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "std::bitset@ColumnMajor" << std::endl;
    if (!test_swap<std::bitset<num_bits>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 8bit chunks@RowMajor" << std::endl;
    if (!test_swap<BitArray::BitArray<num_bits,uint8_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 8bit chunks@ColumnMajor" << std::endl;
    if (!test_swap<BitArray::BitArray<num_bits,uint8_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks@RowMajor" << std::endl;
    if (!test_swap<BitArray::BitArray<num_bits,uint16_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks@ColumnMajor" << std::endl;
    if (!test_swap<BitArray::BitArray<num_bits,uint16_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks@RowMajor" << std::endl;
    if (!test_swap<BitArray::BitArray<num_bits,uint32_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks@ColumnMajor" << std::endl;
    if (!test_swap<BitArray::BitArray<num_bits,uint32_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks@RowMajor" << std::endl;
    if (!test_swap<BitArray::BitArray<num_bits,uint64_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks@ColumnMajor" << std::endl;
    if (!test_swap<BitArray::BitArray<num_bits,uint64_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;

    std::cout << "\e[34;1m---\nTest axpy op\n---\e[m" << std::endl;
    std::cout << "std::bitset@RowMajor" << std::endl;
    if (!test_axpy<std::bitset<num_bits>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "std::bitset@ColumnMajor" << std::endl;
    if (!test_axpy<std::bitset<num_bits>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 8bit chunks@RowMajor" << std::endl;
    if (!test_axpy<BitArray::BitArray<num_bits,uint8_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 8bit chunks@ColumnMajor" << std::endl;
    if (!test_axpy<BitArray::BitArray<num_bits,uint8_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks@RowMajor" << std::endl;
    if (!test_axpy<BitArray::BitArray<num_bits,uint16_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 16bit chunks@ColumnMajor" << std::endl;
    if (!test_axpy<BitArray::BitArray<num_bits,uint16_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks@RowMajor" << std::endl;
    if (!test_axpy<BitArray::BitArray<num_bits,uint32_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 32bit chunks@ColumnMajor" << std::endl;
    if (!test_axpy<BitArray::BitArray<num_bits,uint32_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks@RowMajor" << std::endl;
    if (!test_axpy<BitArray::BitArray<num_bits,uint64_t>,BitLA::MatrixMode::RowMajor>())
        return EXIT_FAILURE;
    std::cout << "BitArray with 64bit chunks@ColumnMajor" << std::endl;
    if (!test_axpy<BitArray::BitArray<num_bits,uint64_t>,BitLA::MatrixMode::ColumnMajor>())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
