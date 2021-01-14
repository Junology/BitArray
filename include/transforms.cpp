/*!
 * \file transforms.hpp
 * \author Jun Yoshida
 * \copyright (c) 2021 Jun Yoshida.
 * The project is released under the MIT License.
 * \date January 14, 2021: created
 */

#pragma once

#include <bitset>
#include <string>

#include "BitArray.hpp"

namespace BitArray {

//! A set of conversion functions * -> BitArray.
namespace from {

//! \function bitset Conversion from std::bitset.
namespace _impl {
template <std::size_t N, std::size_t... is>
BitArray<N>
bitset_impl(std::index_sequence<is...>, std::bitset<N> const& src) noexcept
{
    constexpr std::bitset<N> ullmask{~0x0ull};
    return BitArray<N>(
        static_cast<typename BitArray<N>::chunk_type>(
            ((src >> (BitArray<N>::chunk_bits*is)) & ullmask).to_ullong()
            )
        ...
        );
}

} // end namespace _impl
template <std::size_t N>
BitArray<N>
bitset(std::bitset<N> const& src) noexcept
{
    return bitset_impl(std::make_index_sequence<BitArray<N>::length>(), src);
}

} // end namespace BitArray::from

//! A set of conversion functions BitArray -> *.
namespace to {

//! \function bitset Conversion to std::bitset.
template <std::size_t N, class T>
std::bitset<N>
bitset_impl(BitArray<N,T> const& src) noexcept
{
    std::bitset<N> result{};
    for(std::size_t i = 0; i < BitArray<N,T>::length; ++i) {
        result |= std::bitset<N>(src.getChunk(i)) << (i*BitArray<N,T>::chunkbits);
    }

    return result;
}
} // end namespace BitArray::to

} // end namespace BitArray
