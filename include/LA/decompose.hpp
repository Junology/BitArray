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

//! Transform a matrix into a matrix which is lower-triangular up to a permutation of rows.
//! \code
//! auto mat = mat0;
//! auto [v,w] = plu(mat);
//! assert(is_upper_triangular(v));
//! assert(mat*v == mat0 && v*w == ident && w*v == ident);
//! \endcode
//! \post mat is lower-triangular up to a permutation of rows.
template <class T,std::size_t m,std::size_t n>
constexpr
std::array<Matrix<BitArray::BitArray<n,T>,n,MatrixMode::ColumnMajor>,2>
plu(Matrix<BitArray::BitArray<m,T>,n,MatrixMode::ColumnMajor> & mat)
    noexcept
{
    using tmat_t = Matrix<BitArray::BitArray<n,T>,n,MatrixMode::ColumnMajor>;
    constexpr auto ident = tmat_t::diagonal(BitArray::BitArray<n,T>{}.flip());
    std::array<tmat_t,2> result{ident, ident};

    for(std::size_t i = 0; i+1 < n; ++i) {
        // Compute the top non-zero row (aka. pivot) in the i-th column.
        std::size_t piv = mat.vec(i).countTrail0();
        if (piv == m) // The column is itself zero.
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

} // close namespace decomp
} // close namespace BitLA
