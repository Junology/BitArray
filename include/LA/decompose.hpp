/*!
 * \file decompose.hpp
 * \author Jun Yoshida
 * \copyright (c) 2019-2021 Jun Yoshida.
 * The project is released under the MIT License.
 * \date March, 20 2021: created
 */

#pragma once

#include <tuple>

#include "../BitArray.hpp"
#include "matrix.hpp"

namespace BitLA {
namespace decomp {

/*!
 * \function plu
 * Transform a matrix into a matrix which is lower-triangular up to a permutation of rows.
 * \code
 * auto mat = mat0;
 * auto [v,w] = plu(mat);
 * assert(is_upper_triangular(v));
 * assert(mat*v == mat0 && v*w == ident && w*v == ident);
 * \endcode
 * \post mat is lower-triangular up to a permutation of rows.
 */
namespace _impl {

//! \param f A function of signature std::size_t(herring::BitArray<m,T>) that returns the index of a non-zero bit. It must return m in the case the given vector has no non-zero bit.
template <class T,std::size_t m,std::size_t n, class F>
constexpr
std::array<Matrix<herring::BitArray<n,T>,n,MatrixMode::ColumnMajor>,2>
plu_impl(Matrix<herring::BitArray<m,T>,n,MatrixMode::ColumnMajor> & mat, F&& f)
    noexcept
{
    using tmat_t = Matrix<herring::BitArray<n,T>,n,MatrixMode::ColumnMajor>;
    constexpr auto ident = tmat_t::diagonal(herring::BitArray<n,T>{}.flip());
    std::array<tmat_t,2> result{ident, ident};

    for(std::size_t i = 0; i+1 < n; ++i) {
        // Specify a non-zero row (aka. pivot) in the i-th column in a certain mannar.
        std::size_t piv = f(mat.vec(i));

        // Skip zero-columns.
        if (piv == m)
            continue;

        // Transformation matrix
        auto tmat = ident;

        for(std::size_t j = i+1; j < n; ++j) {
            if(mat.vec(j).test(piv)) {
                mat.vec(j) ^= mat.vec(i);
                tmat.vec(j).set(i);
            }
        }
        // Here, notice that tmat is an invertible matrix of order 2;
        // i.e. tmat*tmat == ident.

        result[0] = tmat * result[0];
        result[1] = result[1] * tmat;
    }

    return result;
}

} // close namespace _impl

template <class T,std::size_t m,std::size_t n>
constexpr
std::array<Matrix<herring::BitArray<n,T>,n,MatrixMode::ColumnMajor>,2>
plu(Matrix<herring::BitArray<m,T>,n,MatrixMode::ColumnMajor> & mat)
    noexcept
{
    struct _lambda {
        constexpr std::size_t operator()(herring::BitArray<m,T> const& v) noexcept
        {
            return v.countTrail0();
        }
    };
    return _impl::plu_impl(mat, _lambda{});
}

//! Similar to \sa{plu} while this uses the last non-zero rows instead of the first.
template <class T,std::size_t m,std::size_t n>
constexpr
std::array<Matrix<herring::BitArray<n,T>,n,MatrixMode::ColumnMajor>,2>
plu_lnr(Matrix<herring::BitArray<m,T>,n,MatrixMode::ColumnMajor> & mat)
    noexcept
{
    struct _lambda {
        constexpr std::size_t operator()(herring::BitArray<m,T> const& v) noexcept
        {
            return v.msb();
        }
    };
    return _impl::plu_impl(mat, _lambda{});
}

} // close namespace decomp
} // close namespace BitLA
