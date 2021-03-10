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

#include "../BitArray.hpp"

namespace BitArray {

//! A set of conversion functions * -> BitArray.
namespace from {

//! \function bitset Conversion from std::bitset.
namespace _impl {
template <class T, std::size_t N, std::size_t... is>
BitArray<N,T>
bitset_impl(std::index_sequence<is...>, std::bitset<N> const& src) noexcept
{
    using target_t = BitArray<N,T>;
    constexpr std::bitset<N> ullmask{~0ull};
    return target_t(
        static_cast<T>(
            ((src >> (target_t::chunkbits*is)) & ullmask).to_ullong()
            )
        ...
        );
}

} // end namespace _impl
template <class T = default_chunk_t, std::size_t N>
BitArray<N,T>
bitset(std::bitset<N> const& src) noexcept
{
    using target_t = BitArray<N,T>;
    return _impl::bitset_impl<T>(
        std::make_index_sequence<target_t::length>(), src);
}

} // end namespace BitArray::from

//! A set of conversion functions BitArray -> *.
namespace to {

//! \function bitset Conversion to std::bitset.
template <std::size_t N, class T>
std::bitset<N>
bitset(BitArray<N,T> const& src) noexcept
{
    std::bitset<N> result{};
    for(std::size_t i = 0; i < BitArray<N,T>::length; ++i) {
        result |= std::bitset<N>(src.getChunk(i)) << (i*BitArray<N,T>::chunkbits);
    }

    return result;
}
} // end namespace BitArray::to

} // end namespace BitArray
