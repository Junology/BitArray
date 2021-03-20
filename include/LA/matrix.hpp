/*!
 * \file matrix.hpp
 * \author Jun Yoshida
 * \copyright (c) 2019-2021 Jun Yoshida.
 * The project is released under the MIT License.
 * \date March, 16 2021: created
 */

#pragma once

#include <type_traits>
#include <utility>

#include "../detail/utils.hpp"
#include "traits.hpp"
#include "vector.hpp"

namespace BitLA {

enum class MatrixMode { RowMajor, ColumnMajor };

template <MatrixMode M>
using MatrixMode_t = std::integral_constant<MatrixMode, M>;


//! The type of matrices over the two-element field F2.
//! It is essentially a static size array of vectors of bits.
//! \tparam BA The type of vectors of bits. BitLA::BitLA_traits<BA> must be defined.
//! \tparam n The number of vectors in a matrix.
//! \tparam mode The mode of matrix; i.e. RowMajor or ColumnMajor. In RowMajor mode, each vector of bits is seen as a row vector, and similar in the other case.
template <class BA, std::size_t N, MatrixMode mode = MatrixMode::RowMajor>
class Matrix : public BitLA_traits<BA>
{
    template <class, std::size_t, MatrixMode>
    friend class Matrix;

public:
    using Vector = BA;

    enum : std::size_t {
        nvecs = N,
        veclength = BitLA_traits<Vector>::length(),
        ncols = (mode == MatrixMode::RowMajor) ? veclength : nvecs,
        nrows = (mode == MatrixMode::RowMajor) ? nvecs : veclength
    };

private:
    Vector m_vecs[nvecs];

public:
    //! \name Constructors
    //! \{
    constexpr Matrix() noexcept = default;
    constexpr Matrix(Matrix const&) noexcept = default;
    constexpr Matrix(Matrix &&) noexcept = default;

    template <
        class T, class... Ts,
        std::enable_if_t<
            1+sizeof...(Ts) == nvecs
            && !std::is_convertible<T, Matrix>::value
            && !std::is_same<std::decay_t<T>, std::piecewise_construct_t>::value,
            int
            > = 0
        >
    explicit constexpr Matrix(T &&arg, Ts&&... args) noexcept
        : m_vecs{arg, args...}
    {}

    template <
        class... Tuples,
        std::enable_if_t<sizeof...(Tuples)==nvecs,int> = 0
        >
    constexpr Matrix(std::piecewise_construct_t, Tuples&&... tups) noexcept
        : m_vecs{BitArray::make_from_tuple<Vector>(std::forward<Tuples>(tups))...}
    {}
    //! \}

    //! \name Destructor
    //! \{
    ~Matrix() = default;
    //! \}

    //! \name Assignment operators
    //! \{
    constexpr Matrix& operator=(Matrix const& src) noexcept = default;
    constexpr Matrix& operator=(Matrix && src) noexcept = default;
    //! \}

    //! \name Queries
    //! \{
    constexpr std::size_t rows() const noexcept
    {
        return nrows;
    }

    constexpr std::size_t cols() const noexcept
    {
        return ncols;
    }

    constexpr bool coeff(std::size_t r, std::size_t c) const noexcept
    {
        return coeff_impl<mode>(r,c);
    }

    constexpr Vector vec(std::size_t i) const
    {
        return const_cast<Matrix*>(this)->vec(i);
    }

    constexpr Vector& vec(std::size_t i)
    {
        if (i >= nvecs) throw std::range_error("BitLA::Matrix::vec: Index out of range.");
        return m_vecs[i];
    }

    //! This does not perfom range-check and so may cause SEGFAULT.
    //! us -> UnSafe
    constexpr Vector vec_us(std::size_t i) const noexcept
    {
        return const_cast<Matrix*>(this)->vec_us(i);
    }

    //! This does not perfom range-check and so may cause SEGFAULT.
    //! us -> UnSafe
    constexpr Vector& vec_us(std::size_t i) noexcept
    {
        return m_vecs[i];
    }
    //! \}

    //! \name Compound-operator overloads
    //! \{
    constexpr Matrix& operator+=(Matrix const& other) noexcept
    {
        for(std::size_t i = 0; i < nvecs; ++i)
            m_vecs[i] ^= other.m_vecs[i];
        return *this;
    }

    constexpr Matrix& operator-=(Matrix const& other) noexcept
    {
        return (*this) += other;
    }
    //! \}

    //! \name Construction of special types of matrices.
    //! \{

    //! The zero matrix.
    //! \warning This function assumes that the default-constructor of the vector type produces a zero-initialized instance.
    static constexpr Matrix zero() noexcept
    {
        return Matrix();
    }

    //! The diagonal matrix.
    static constexpr Matrix diagonal(BA const& diag_vec) noexcept
    {
        return diagonal_impl(diag_vec, std::make_index_sequence<nvecs>());
    }
    //! \}

private:

    /*!
     * \name Implementations that work in both modes.
     */
    //! \{
    template <std::size_t... Is>
    static constexpr
    Matrix diagonal_impl(
        BA const& diag_vec,
        std::index_sequence<Is...>
        ) noexcept
    {
        return Matrix(
            (Is < veclength && diag_vec.test(Is))
            ? BA{}.set(Is) : BA{}
            ...);
    }
    //! \}

    /*!
     * \name Implementations in RowMajor mode.
     */
    //! \{
    template <
        MatrixMode M,
        std::enable_if_t< M == MatrixMode::RowMajor, int> = 0
        >
    constexpr bool coeff_impl(std::size_t r, std::size_t c) const noexcept
    {
        return m_vecs[r].test(c);
    }
    //! \}

    /*!
     * \name Implementations in ColumnMajor mode.
     */
    //! \{
    template <
        MatrixMode M,
        std::enable_if_t< M == MatrixMode::ColumnMajor, int> = 0
        >
    constexpr bool coeff_impl(std::size_t r, std::size_t c) const noexcept
    {
        return m_vecs[c].test(r);
    }
    //! \}
};


/********************************
 ** Arithmatics of matrices
 ********************************/

//! Addition of the same size of matrices
template <class BA, std::size_t n, MatrixMode mode>
constexpr Matrix<BA,n,mode> operator+(
    Matrix<BA,n,mode> lhs,
    Matrix<BA,n,mode> const& rhs
    ) noexcept
{
    return lhs += rhs;
}

//! Subtraction of the same size of matrices
template <class BA, std::size_t n, MatrixMode mode>
constexpr Matrix<BA,n,mode> operator-(
    Matrix<BA,n,mode> lhs,
    Matrix<BA,n,mode> const& rhs
    ) noexcept
{
    return lhs -= rhs;
}

//! (Row vector) * (Row-major matrix)
template <
    class BA, std::size_t n,
    class V,
    std::enable_if_t<
        std::is_object<BitLA_traits<V>>::value,
        int
        > = 0
    >
constexpr BA
operator*(V const &vec, Matrix<BA,n,MatrixMode::RowMajor> const& rhs)
    noexcept
{
    BA result{};

    for(auto i : PopTraversable<V>::range(vec))
        result ^= rhs.vec_us(i);

    return result;
}

//! (Column-major matrix) * (column vector)
template <
    class BA, std::size_t n,
    class V,
    std::enable_if_t<
        std::is_object<BitLA_traits<V>>::value,
        int
        > = 0
    >
constexpr BA
operator*(Matrix<BA,n,MatrixMode::ColumnMajor> const& lhs, V const &vec)
    noexcept
{
    BA result{};

    for(auto i : PopTraversable<V>::range(vec))
        result ^= lhs.vec_us(i);

    return result;
}

//! \function operator*
//! Multiplication of two compatible matrices
namespace _impl{

template <class BAL, class BAR, std::size_t nL, std::size_t nR, std::size_t...Is>
constexpr
Matrix<BAR,nL,MatrixMode::RowMajor>
matrix_mul_row(
    Matrix<BAL,nL,MatrixMode::RowMajor> const& lhs,
    Matrix<BAR,nR,MatrixMode::RowMajor> const& rhs,
    std::index_sequence<Is...>
    ) noexcept
{
    return Matrix<BAR,nL,MatrixMode::RowMajor>(lhs.vec(Is)*rhs...);
}

template <class BAL, class BAR, std::size_t nL, std::size_t nR, std::size_t...Is>
constexpr
Matrix<BAL,nR,MatrixMode::ColumnMajor>
matrix_mul_col(
    Matrix<BAL,nL,MatrixMode::ColumnMajor> const& lhs,
    Matrix<BAR,nR,MatrixMode::ColumnMajor> const& rhs,
    std::index_sequence<Is...>
    ) noexcept
{
    return Matrix<BAL,nR,MatrixMode::ColumnMajor>(lhs*rhs.vec(Is)...);
}

} // close _impl

template <class BAL, class BAR, std::size_t nL, std::size_t nR>
constexpr
Matrix<BAR,nL,MatrixMode::RowMajor>
operator*(
    Matrix<BAL,nL,MatrixMode::RowMajor> const& lhs,
    Matrix<BAR,nR,MatrixMode::RowMajor> const& rhs
    ) noexcept
{
    return _impl::matrix_mul_row(lhs, rhs, std::make_index_sequence<nL>());
}

template <class BAL, class BAR, std::size_t nL, std::size_t nR>
constexpr
Matrix<BAL,nR,MatrixMode::ColumnMajor>
operator*(
    Matrix<BAL,nL,MatrixMode::ColumnMajor> const& lhs,
    Matrix<BAR,nR,MatrixMode::ColumnMajor> const& rhs
    ) noexcept
{
    return _impl::matrix_mul_col(lhs, rhs, std::make_index_sequence<nR>());
}


} // end namespace BitLA.
