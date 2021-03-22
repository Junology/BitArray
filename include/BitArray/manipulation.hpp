/*!
 * \file manipulation.hpp
 * \author Jun Yoshida
 * \copyright (c) 2021 Jun Yoshida.
 * The project is released under the MIT License.
 * \date March 21, 2021: created
 */

#pragma once

#include <bitset>
#include <string>
#include <ios>

#include "../BitArray.hpp"
#include "iterators.hpp"

namespace BitArray {

//! Stream output
template<class C, size_t N, class T>
std::ostream& operator<<(std::basic_ostream<C>& out, BitArray<N,T> const& bit)
{
    for(size_t i = 0; i < N; ++i)
        out.put(bit.test(N-i-1) ? '1' : '0');

    return out;
}

//! counttrail0 for BitArray
template <std::size_t N, class T>
constexpr std::size_t counttrail0(BitArray<N,T> const& barr) noexcept
{
    return barr.countTrail0();
}

//! \function nextperm
//! Get the lexicographically next bit permutation
//! The code is based on the one found in the following:
//!   http://graphics.stanford.edu/~seander/bithacks.html#NextBitPermutation
//! \return true if overflow.
namespace _impl{
//! Define a trait class that checks if an expression is available or not.
#define DEFINE_CHECKER_FOR_EXPR(name, tvar, expr, result_t)     \
    template <class TPARAM>                                     \
    class name {                                                \
        template <class tvar>                                   \
        static auto test(int)                                   \
            -> std::is_convertible<decltype(expr), result_t>;   \
        template <class> static std::false_type test(...);      \
    public:                                                     \
    enum : bool {                                               \
        value = decltype(test<TPARAM>(0))::value                \
    };                                                          \
    }

DEFINE_CHECKER_FOR_EXPR(has_plus_one, T, std::declval<T>()+1, T);
DEFINE_CHECKER_FOR_EXPR(has_minus_one, T, std::declval<T>()-1, T);
DEFINE_CHECKER_FOR_EXPR(has_incr, T, ++std::declval<T>(), T);
DEFINE_CHECKER_FOR_EXPR(has_decr, T, --std::declval<T>(), T);

#undef DEFINE_CHECKER_FOR_EXPR

template <
    class T,
    std::enable_if_t<has_plus_one<T>::value,int> = 0
    >
constexpr T plus_one(T const& x) noexcept { return x+1; }

template <
    class T,
    std::enable_if_t<!has_plus_one<T>::value && has_incr<T>::value,int> = 0
    >
constexpr T plus_one(T x) noexcept { return ++x; }

template <
    class T,
    std::enable_if_t<has_minus_one<T>::value,int> = 0
    >
constexpr T minus_one(T const& x) noexcept { return x-1; }

template <
    class T,
    std::enable_if_t<!has_minus_one<T>::value && has_decr<T>::value,int> = 0
    >
constexpr T minus_one(T x) noexcept { return --x; }

} // close namespace _impl

template <class T>
constexpr bool nextperm(T &x) noexcept
{
    std::size_t i = counttrail0(x);
    x = x | _impl::minus_one(x);
    bool is_maximal = (x == static_cast<T>(~T{0ull}));
    x = _impl::plus_one(x) | (_impl::minus_one((~x & _impl::plus_one(x))) >> (i+1));
    return is_maximal;
}

//! The lexicographical index of a given bit-permutation.
template <std::size_t N, class T>
constexpr std::size_t permindex(BitArray<N,T> const& barr) noexcept
{
    std::size_t result = 0;
    std::size_t cnt = 0;
    for(auto i : range<PopIterator>(barr))
        result += binom(i,++cnt);

    return result;
}

} // end namespace BitArray
