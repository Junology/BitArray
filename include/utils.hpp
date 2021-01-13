/*!
 * \file utils.hpp
 * \author Jun Yoshida
 * \copyright (c) 2019 Jun Yoshida.
 * The project is released under the MIT License.
 * \date Descember 9, 2019: created
 */

#pragma once

#include <type_traits>
#include <limits>

namespace BitArray {

/*******************************************!
 * \section Compile-time utility functions
 *******************************************/

//! Conjunction of compile-time bool values.
//! The class is derived from std::true_type if all template parameters are true.
//! It is derived from std::false_type otherwise.
//! \remark std::conjunction is since C++17.
template <bool... conds>
struct conjunction : public std::true_type {};

template <bool... conds>
struct conjunction<true, conds...> : public conjunction<conds...> {};

template <bool... conds>
struct conjunction<false, conds...> : public std::false_type {};

//! Conjunction value
template <bool... conds>
constexpr bool conjunction_v = conjunction<conds...>::value;


//! The function that produce the following values:
//! > bitwave(1) == 0b...01010101;
//! > bitwave(2) == 0b...00110011;
//! > bitwave(4) == 0b...00001111;
//! > // and so on...
template <class T>
constexpr T bitwave(std::size_t width) noexcept
{
    if (width == 0)
        return T{};

    T result = static_cast<T>(~static_cast<T>(0u)) >> (std::numeric_limits<T>::digits - width);

    for(width <<= 1; width < std::numeric_limits<T>::digits; width <<= 1) {
        result |= result << width;
    }

    return result;
}

/*!
 * \function popcount
 * Population-count (aka. Hamming weight).
 * To explain the algorithm, for a bit sequence x, we denote by x[i,j] the subsequence of x from the i-th bit through the j-th.
 * In particular, we write x[i]:=x[i,i] the i-th bit of x, so we have
 *   > popcount(x) == x[0] + x[1] + x[2] + x[3] + ...;
 * For this, let us define the following bit sequence
 *   > x' = (x & 0b...01010101) + ( (x>>1) & 0b...01010101);
 * By definition, one can see that
 *   > x'[2i,2i+1] == x[2i] + x[2i+1];
 * We further set
 *   > x'' = (x' & 0b...00110011) + ( (x>>2) & 0b...00110011);
 * Then, one obtains
 *   > x''[4i,4i+3] == x[2i,2i+1] + x[2i+2,2i+3];
 *   > x''[4i,4i+3] == x[2i] + x[2i+1] + x[2i+2] + x[2i+3];
 * Therefore, continuing the process, we can compute popcount(x).
 */
namespace _impl {
template <class T, std::size_t w>
struct bitwave_const {
    enum {value = bitwave<T>(w)};
};

template <
    class T, std::size_t i,
    std::enable_if_t<(i >= std::numeric_limits<T>::digits),int> = 0>
constexpr T popcount_impl(T x) noexcept {
    return x;
}

template <
    class T, std::size_t i,
    std::enable_if_t<(i < std::numeric_limits<T>::digits),int> = 0>
constexpr T popcount_impl(T x) noexcept {
    return popcount_impl<T,(i<<1)>(
        (x & bitwave_const<T,i>::value)
        + ((x>>i) & bitwave_const<T,i>::value)
        );
}

} // end namespace _impl

template <class T>
constexpr T popcount(T x) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "std::is_unsigned<T>::value == false");

    return _impl::popcount_impl<T,1>(x);

    /*
    // Hopefully, the compiler should unfold this loop.
    for(std::size_t i = 1; i < std::numeric_limits<T>::digits; i <<= 1) {
        x = (x & bitwave<T>(i)) + ( (x>>i) & bitwave<T>(i) );
    }

    return x;
    */
}

//! Count trailing ones (using the population count above).
template <class T>
constexpr T counttrail1(T x) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "std::is_unsigned<T>::value == false");

    // All we have to do is just counting bits after clearing all bits but the trailing 1s.
    return popcount<T>((~x & (x+1))-1);
}

//! Count trailing zeros (using the population count above).
template <class T>
constexpr T counttrail0(T x) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "std::is_unsigned<T>::value == false");

    return popcount<T>(~x & (x-1));
}


} // end namespace BitArray
