/*!
 * \file utils.hpp
 * \author Jun Yoshida
 * \copyright (c) 2019 Jun Yoshida.
 * The project is released under the MIT License.
 * \date Descember 9, 2019: created
 */

#pragma once

#include <type_traits>
#include <utility>
#include <limits>
#include <tuple>
#include <array>
#include <vector>

namespace BitArray {

/*******************************************!
 * \section Compile-time utility functions
 *******************************************/
constexpr bool allTrue(std::initializer_list<bool>&& flags) noexcept {
    for(auto& flag : flags)
        if (!flag) return false;
    return true;
}

template <class T>
constexpr T bitwave(size_t width) noexcept
{
    if (width == 0)
        return T{};

    T result = static_cast<T>(~static_cast<T>(0u)) >> (sizeof(T)*8 - width);

    for(width *= 2; width < sizeof(T)*8; width *= 2) {
        result |= result << width;
    }

    return result;
}

//! Population-count (aka. Hamming weight).
template <class T>
constexpr T popcount(T x) noexcept
{
    static_assert(std::is_unsigned<T>::value,
                  "std::is_unsigned<T>::value == false");

    // Hopefully, the compiler should unfold this loop.
    for(size_t i = 1; i < std::numeric_limits<T>::digits; i <<= 1) {
        x = (x & bitwave<T>(i)) + ( (x>>i) & bitwave<T>(i) );
    }

    return x;
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
