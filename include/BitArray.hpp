/*!
 * \file BitArray.hpp
 * \author Jun Yoshida
 * \copyright (c) 2019-2021 Jun Yoshida.
 * The project is released under the MIT License.
 * \date January, 5 2020: created
 */

#pragma once

#include <algorithm>
#include <limits>
#include <ios>

#include "detail/utils.hpp"

namespace BitArray {

//! The default type of chunks in BitArray.
using default_chunk_t = unsigned long long;

/*!
 * Personal re-implementation of std::bitset with constexpr supports even in C++14.
 * It is guaranteed that every bit out of range is 0.
 * \tparam N The number of bits
 * \tparam T The type carrying bits.
 */
template <std::size_t N, class T = default_chunk_t>
class BitArray {
    static_assert(std::is_unsigned<T>::value, "The type T is not unsigned or integral.");
    static_assert( N > 0, "Zero-sized array is prohibited");

    //! Have access to BitArray of different lengths.
    template<std::size_t M, class U>
    friend class BitArray;

    template<template<std::size_t,class> class, std::size_t, class>
    friend class BitContainer;

public:
    using chunk_type = T;

    enum : std::size_t {
        numbits = N,
        chunkbits = std::numeric_limits<chunk_type>::digits,
        nchunks = (N + chunkbits - 1) / chunkbits,
        max_index = nchunks-1,
        endbits = N % chunkbits
    };

    enum chunkval : chunk_type {
        zero = static_cast<chunk_type>(0u),
        one = static_cast<chunk_type>(1u),
        nzero = static_cast<chunk_type>(~zero)
    };

    template <std::size_t n>
    struct lowmask {
        enum : chunk_type {
            mask = (0u < n)
                ? (n < static_cast<std::size_t>(chunkbits))
                  ? static_cast<chunk_type>(chunkval::nzero >> (chunkbits - n))
                : static_cast<chunk_type>(chunkval::nzero)
                : 0u
        };
    };

    template <std::size_t i>
    struct chunk_traits {
        enum : chunk_type {
            mask = (i<nchunks)
            ? (endbits > 0 && i+1 == nchunks // check if tail bit.
               ? static_cast<chunk_type>(lowmask<endbits>::mask)
               : static_cast<chunk_type>(chunkval::nzero))
            : static_cast<chunk_type>(chunkval::zero)
        };
    };

private:
    chunk_type m_arr[nchunks];

    //! Initialize from an array of chunk_type
    template <std::size_t... is>
    constexpr BitArray(std::index_sequence<is...>, chunk_type const (&arr)[sizeof...(is)])
      : m_arr{arr[is]...}
    {}

    template <std::size_t... is, class larger_t>
    constexpr BitArray(std::index_sequence<is...>, larger_t x0)
        : m_arr{static_cast<chunk_type>(x0 >> (std::numeric_limits<chunk_type>::digits*is))...}
    {}

public:
    /** Constructors **/
    //! Default constructor.
    //! Bits are cleared to 0 since array elements are initialized in the same way as objects with static durations.
    constexpr BitArray() noexcept
      : m_arr{}
    {}

    //! Can be constructed from chunk_type.
    constexpr BitArray(chunk_type x0) noexcept
        : m_arr{static_cast<chunk_type>(x0 & chunk_traits<0>::mask)}
    {}

    //! Constructor from unsigned types larger than chunk_type.
    template <
        class larger_t,
        std::enable_if_t<
            std::is_unsigned<larger_t>::value
            && (std::numeric_limits<larger_t>::digits > chunkbits),
        int
        > = 0>
    constexpr BitArray(larger_t x0) noexcept
        : BitArray(std::make_index_sequence<1+(std::numeric_limits<larger_t>::digits-1)/chunkbits>(), x0)
    {}

    //! Can be constructed from smaller BitArray with same chunk_type.
    template<size_t M, std::enable_if_t<(M<N), int> = 0>
    constexpr BitArray(BitArray<M,chunk_type> const& src) noexcept
      : BitArray(std::make_index_sequence<BitArray<M,chunk_type>::nchunks>(), src.m_arr)
    {}

    //! Construct from a sequence of chunk_type's.
    //! A version for the case where the last byte doesn't need rounding.
    template <
        class... Ts,
        std::enable_if_t<
            conjunction_v<
                (sizeof...(Ts)+2 <= nchunks),
                (sizeof...(Ts)+2 < nchunks || endbits == 0),
                std::is_convertible<Ts,chunk_type>::value...
                >,
            int
            > = 0
        >
    constexpr BitArray(chunk_type x0, chunk_type x1, Ts... xs) noexcept
        : m_arr{static_cast<chunk_type>(x0),
                static_cast<chunk_type>(x1),
                static_cast<chunk_type>(xs)...}
    {}

    //! Construct from a sequence of chunk_type's.
    //! A version for the case where the last byte needs rounding.
    template <
        class... Ts,
        std::enable_if_t<
            conjunction_v<
                (sizeof...(Ts)+2 <= nchunks),
                (sizeof...(Ts)+2 == nchunks && endbits > 0),
                std::is_convertible<Ts,chunk_type>::value...
                >,
            int
            > = 0
        >
    constexpr BitArray(chunk_type x0, chunk_type x1, Ts... xs) noexcept
        : m_arr{static_cast<chunk_type>(x0),
                static_cast<chunk_type>(x1),
                static_cast<chunk_type>(xs)...}
    {
        m_arr[nchunks-1] &= lowmask<endbits>::mask;
    }

    //! Copy constructor is the default one.
    constexpr BitArray(BitArray<N,chunk_type> const&) noexcept = default;

    //! Move constructor is the default one.
    constexpr BitArray(BitArray<N,chunk_type>&&) noexcept = default;

    /**************************!
     * \name Basic operations
     **************************/
    //@{
    //! Get the number of bits.
    constexpr std::size_t size() const noexcept
    {
        return numbits;
    }

    //! Set all bits.
    constexpr BitArray& set() noexcept {
        constexpr BitArray result = ~(BitArray{});
        return (*this) = result;
    }

    //! Set a bit in the given position.
    constexpr BitArray& set(std::size_t pos, bool value = true) noexcept
    {
        // If the position is out-of-range, nothing happen.
        if (pos >= numbits)
            return *this;

        std::size_t gpos = pos / chunkbits;
        std::size_t lpos = pos % chunkbits;
        if(value)
            m_arr[gpos] |= (chunkval::one) << lpos;
        else
            m_arr[gpos] &= ~(chunkval::one << lpos);

        return *this;
    }

    //! Restet all bits
    constexpr BitArray& reset() noexcept
    {
        constexpr BitArray result{};
        return (*this) = result;
    }

    //! Reset a bit in the given position.
    constexpr BitArray& reset(std::size_t pos) noexcept
    {
        return set(pos, false);
    }

    //! Flip all the bits
    constexpr BitArray<N,chunk_type>& flip() noexcept
    {
        constexpr BitArray filter = ~(BitArray{});
        return (*this) ^= filter;
    }

    //! Flip a specific bit
    constexpr BitArray<N,chunk_type>& flip(std::size_t pos) noexcept
    {
        std::size_t gpos = pos / chunkbits;
        std::size_t lpos = pos % chunkbits;
        m_arr[gpos] ^= (chunkval::one << lpos);
        return *this;
    }

    //! Test if a bit in the given position is true.
    constexpr bool test(std::size_t pos) const noexcept
    {
        // If the position is out-of-range, the function always returns false.
        if (pos > numbits)
            return false;

        std::size_t gpos = pos / chunkbits;
        std::size_t lpos = pos % chunkbits;
        return m_arr[gpos] & (chunkval::one << lpos);
    }

    //! Test if all the bits are true
    constexpr bool all() const noexcept
    {
        for(std::size_t i = 0; i < nchunks; ++i) {
            if (m_arr[i] != get_mask(i))
                return false;
        }
        return true;
    }

    //! Test if there is a bit that is set.
    constexpr bool any() const noexcept
    {
        for(auto x : m_arr)
            if(x) return true;

        return false;
    }

    //! Test if none of bits is set.
    constexpr bool none() const noexcept
    {
        return !any();
    }

    //! Population-count (aka. Hamming weight).
    //! The result may be incorrect when N > std::numeric_limits<std::size_t>::max().
    constexpr size_t count() const noexcept
    {
        size_t result = 0;

        for(auto x : m_arr) {
            result += static_cast<size_t>(popcount(x));
        }

        return result;
    }

    //! Count trailing ones.
    constexpr size_t countTrail1() const noexcept {
        size_t result = 0;
        for(auto x : m_arr) {
            size_t r = counttrail1(x);
            result += r;
            if (r < chunkbits)
                break;
        }
        return result;
    }

    //! Count trailing zeros.
    constexpr std::size_t countTrail0() const noexcept {
        std::size_t result = 0;
        for(std::size_t i = 0; i < nchunks; ++i) {
            std::size_t r = m_arr[i]
                ? counttrail0<chunk_type>(m_arr[i])
                : chunkbits;

            // Not at the end byte.
            if (endbits == 0 || i+1 < nchunks)
                result += r;
            // At the end byte.
            else {
                result += std::min(r, static_cast<std::size_t>(endbits));
                break;
            }

            if (r < chunkbits)
                break;
        }
        return result;
    }

    //! Get the states of the bits as an array of characters.
    constexpr std::array<char,numbits>
    digits(char c0 = '0', char c1 = '1')
        const noexcept
    {
        return digits_impl(c0, c1, std::make_index_sequence<numbits>());
    }

    std::string to_string() const noexcept
    {
        auto digs = digits();
        return std::string(digs.data(), digs.size());
    }
    //@}

    /**********************************!
     * \name Operations on sub-arrays
     **********************************/
    //@{
    //! Get the value of chunks.
    //! \param i The index of chunks.
    //! \return The value of the i-th chunk or 0 if i >= BitArray::nchunks.
    constexpr chunk_type getChunk(std::size_t i) const noexcept
    {
        return i < nchunks ? m_arr[i] : static_cast<chunk_type>(chunkval::zero);
    }

    //! Slicing array; a version for slicing into a larger bit-array.
    //! Hence, it is actually just a cast with right shifts.
    //! \param i The position of the lowest bit in the slice.
    template <size_t n>
    constexpr auto slice(std::size_t i) const noexcept
        -> std::enable_if_t<(n>=N),BitArray<n,chunk_type>>
    {
        return BitArray<n>(*this) >> i;
    }

    //! Slicing array; a version for slicing into a smaller bit-array.
    //! \param i The position of the lowest bit in the slice.
    template <size_t n>
    constexpr auto slice(std::size_t i) const noexcept
        -> std::enable_if_t<(n<N),BitArray<n,chunk_type>>
    {
        return slice_impl<n>(
            std::make_index_sequence<BitArray<n,chunk_type>::nchunks>(), i);
    }

    //! Inactivate lower bits.
    //! \param n The number of bits ignored.
    constexpr BitArray<N,chunk_type> lowcut(std::size_t n) const noexcept
    {
        return n >= N ? BitArray<N,chunk_type>{} : lowcut_impl(std::make_index_sequence<nchunks>(), n);
    }

    //! Inactivate higher bits.
    //! \param n The number of bits kept considered.
    constexpr BitArray<N,chunk_type> lowpass(std::size_t n) const noexcept
    {
        return n >= N ? *this : lowpass_impl(std::make_index_sequence<nchunks>(), n);
    }

    //! Replace subarray with smaller array
    //! \param i The index of the lowest bit to be replaced.
    template <size_t M, std::enable_if_t<(M<=N),int> = 0 >
    constexpr void replace(
        size_t i, BitArray<M,chunk_type> const& src, size_t wid = M)
        noexcept
    {
        // Check if the operation is trivial.
        // Thanks to this, we can assume i < N in what follows.
        if (i >= N)
            return;

        std::size_t gpos = i / chunkbits;
        std::size_t lpos = i % chunkbits;

        BitArray<M+chunkbits,chunk_type> src_adj = BitArray<M+chunkbits,chunk_type>{src} << lpos;
        BitArray<M+chunkbits,chunk_type> mask
            = BitArray<M+chunkbits,chunk_type>{BitArray<M,chunk_type>{}.flip().lowpass(wid)} << lpos;

        // The number of loops
        // Note that we here assume i < N so that gpos < nchunks.
        std::size_t num
            = std::min(nchunks-gpos, (M + lpos + chunkbits - 1) / chunkbits);
        for(size_t j = 0; j < num; ++j) {
            m_arr[gpos+j] &= ~(mask.m_arr[j]);
            m_arr[gpos+j] |= src_adj.m_arr[j];
        }
    }
    //@}


    /********************!
     * \name Iterators
     ********************
    //@{
    //! Return the begining of iterators visiting true bits.
    constexpr PopIterator<BitArray> popBegin() const noexcept {
        return PopIterator<BitArray>(m_arr, 0, m_arr[0]);
    }

    //! Return the end of iterators visiting true bits.
    constexpr PopIterator<BitArray> popEnd() const noexcept {
        return PopIterator<BitArray>(m_arr, nchunks-1, 0);
    }
    //@}
    // **/


    /***************************!
     * \name Operator overloads
     ***************************/
    //@{
    constexpr BitArray<N,chunk_type>& operator=(BitArray<N,chunk_type> const&) noexcept = default;
    constexpr BitArray<N,chunk_type>& operator=(BitArray<N,chunk_type>&&) noexcept = default;

    //! Copy from smaller BitArray (with the same chunk_type).
    template<size_t M, std::enable_if_t<(M<N),int> = 0>
    constexpr BitArray<N,chunk_type>& operator=(BitArray<M,chunk_type> const &src) noexcept
    {
        std::fill(
            std::copy(std::begin(src.m_arr),
                      std::end(src.m_arr),
                      std::begin(m_arr) ),
            std::end(m_arr), 0);

        return *this;
    }

    //! Copy from chunk-type
    template <
        class U,
        std::enable_if_t<
            std::is_same<U, chunk_type>::value,
            int
            > = 0
        >
    constexpr BitArray<N,chunk_type>& operator=(U x) noexcept
    {
        m_arr[0] = x;
        for(size_t i = 1; i < nchunks; ++i) {
            m_arr[i] = chunkval::zero;
        }

        return *this;
    }

    explicit constexpr operator bool() const noexcept
    {
        return any();
    }

    constexpr bool operator==(BitArray<N,chunk_type> const &other) const noexcept
    {
        for(std::size_t i = 0; i < nchunks; ++i)
            if(m_arr[i] != other.m_arr[i]) return false;

        return true;
    }

    constexpr bool operator!=(BitArray<N,chunk_type> const &other) const noexcept
    {
        return !((*this)==other);
    }

    constexpr BitArray<N,chunk_type>& operator&=(BitArray<N,chunk_type> const &other) noexcept
    {
        for(size_t i = 0; i < nchunks; ++i)
            m_arr[i] &= other.m_arr[i];
        return *this;
    }

    constexpr BitArray<N,chunk_type>& operator|=(BitArray<N,chunk_type> const &other) noexcept
    {
        for(size_t i = 0; i < nchunks; ++i)
            m_arr[i] |= other.m_arr[i];
        return *this;
    }

    constexpr BitArray<N,chunk_type>& operator^=(BitArray<N,chunk_type> const &other) noexcept
    {
        for(size_t i = 0; i < nchunks; ++i)
            m_arr[i] ^= other.m_arr[i];
        return *this;
    }

    constexpr BitArray<N,chunk_type> operator~() const noexcept
    {
        return not_impl(std::make_index_sequence<nchunks>());
    }

    constexpr BitArray<N,chunk_type> operator<<(std::size_t n) const noexcept
    {
        return lshift_impl(std::make_index_sequence<nchunks>(), n);
    }

    constexpr BitArray<N,chunk_type> operator>>(std::size_t n) const noexcept
    {
        return rshift_impl(std::make_index_sequence<nchunks>(), n);
    }

    //! Prefix increment operator
    constexpr BitArray<N,chunk_type>& operator++() noexcept {
        return increment_impl(std::integral_constant<std::size_t,0>());
    }

    //! Postfix increment operator
    constexpr BitArray<N,chunk_type> operator++(int) noexcept {
        auto aux = *this;
        ++(*this);
        return aux;
    }

    //@}

protected:
    /** Some query on chunks **/
    static constexpr chunk_type get_mask(std::size_t i) noexcept {
        return i<nchunks
            ? (endbits > 0 && i+1 == nchunks // check if tail bit.
               ? static_cast<chunk_type>(lowmask<endbits>::mask)
               : static_cast<chunk_type>(chunkval::nzero))
            : static_cast<chunk_type>(chunkval::zero);
    }

    /** Implementations **/

    template <std::size_t... is>
    constexpr std::array<char,numbits>
    digits_impl(char c0, char c1, std::index_sequence<is...>)
        const noexcept
    {
        char str[numbits]{};

        // Digits in the last chunk
        {
            auto const last_digits = bindigits(m_arr[nchunks-1],c0,c1);
            for(std::size_t j = 0; j < endbits; ++j) {
                str[j] = last_digits[(chunkbits-endbits) + j];
            }
        }

        // Digits in the other chunks
        for(std::size_t i = 0; i+1 < nchunks; ++i) {
            auto const chunk_digits = bindigits(m_arr[nchunks - i - 2],c0,c1);
            for(std::size_t j = 0; j < chunkbits; ++j) {
                str[endbits + chunkbits*i + j] = chunk_digits[j];
            }
        }

        return {str[is]...};
    }

    template <size_t n, size_t... is>
    constexpr auto slice_impl(std::index_sequence<is...>, std::size_t i) const noexcept
        -> std::enable_if_t<(n<N),BitArray<n,chunk_type>>
    {
        std::size_t gpos = i / chunkbits;
        std::size_t lpos = i % chunkbits;

        return BitArray<n,chunk_type>{static_cast<chunk_type>(
                gpos+is >= nchunks
                ? 0u
                : ((m_arr[gpos+is] >> lpos) | (gpos+is+1>=nchunks ? 0u : (m_arr[gpos+is+1] << (chunkbits-lpos))))
                )...};
    }

    template <std::size_t... is>
    constexpr BitArray<N,chunk_type> lowcut_impl(std::index_sequence<is...>, std::size_t n) const noexcept
    {
        std::size_t gpos = n / chunkbits;
        std::size_t lpos = n % chunkbits;
        chunk_type mask{static_cast<chunk_type>(chunkval::nzero << lpos)};

        return BitArray<N,chunk_type>(
            static_cast<chunk_type>(
                is > gpos ? m_arr[is]
                : (is == gpos ? m_arr[is] & mask : 0u)
            )...
        );
    }

    template <std::size_t... is>
    constexpr BitArray<N,chunk_type> lowpass_impl(std::index_sequence<is...>, std::size_t n) const noexcept
    {
        std::size_t gpos = n / chunkbits;
        std::size_t lpos = n % chunkbits;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnarrowing"
        chunk_type mask{lpos > 0u ? static_cast<chunk_type>(chunkval::nzero >> (chunkbits - lpos)) : chunkval::zero};
#pragma GCC diagnostic pop

        return BitArray<N,chunk_type>(
            static_cast<chunk_type>(
                is < gpos ? m_arr[is]
                : (is == gpos ? m_arr[is] & mask : 0)
            )...
        );
    }

    template <std::size_t... is>
    constexpr BitArray<N,chunk_type> not_impl(std::index_sequence<is...>) const noexcept
    {
        return BitArray<N,chunk_type>(static_cast<chunk_type>(~m_arr[is])...);
    }

    template<std::size_t... is>
    constexpr BitArray<N,chunk_type> lshift_impl(std::index_sequence<is...>, std::size_t n) const noexcept
    {
        std::size_t gpos = n / chunkbits;
        std::size_t lpos = n % chunkbits;
        return BitArray<N,chunk_type>(
            static_cast<chunk_type>(
                (is > gpos)
                ? ((m_arr[is-gpos] << lpos) | (m_arr[is-gpos-1] >> (chunkbits-lpos))) & chunk_traits<is>::mask
                : ((is==gpos) ? ((m_arr[0] << lpos) & chunk_traits<is>::mask) : 0u)
            )...
        );
    }

    template<std::size_t... is>
    constexpr BitArray<N,chunk_type> rshift_impl(std::index_sequence<is...>, std::size_t n) const noexcept
    {
        std::size_t gpos = n / chunkbits;
        std::size_t lpos = n % chunkbits;
        return BitArray<N,chunk_type>(
            static_cast<chunk_type>(
                is+gpos < nchunks-1
                ? (m_arr[is+gpos] >> lpos | m_arr[is+gpos+1] << (chunkbits-lpos))
                : ((is+gpos == nchunks-1) ? (m_arr[nchunks-1] >> lpos) : chunkval::zero )
            )...
        );
    }

    //! Implementation of the (prefix) increment operator.
    template <
        std::size_t i,
        std::enable_if_t<(i<max_index), int> = 0>
    inline constexpr BitArray<N,chunk_type>& increment_impl(std::integral_constant<std::size_t, i>) noexcept
    {
        if (m_arr[i] == std::numeric_limits<chunk_type>::max()) {
            m_arr[i] = static_cast<chunk_type>(0u);

            return increment_impl(std::integral_constant<std::size_t,i+1>());
        }
        else {
            ++(m_arr[i]);
            return *this;
        }
    }

    inline constexpr BitArray<N,chunk_type>& increment_impl(std::integral_constant<std::size_t, max_index>) noexcept
    {
        if (m_arr[max_index] == std::numeric_limits<chunk_type>::max()) {
            m_arr[max_index] = static_cast<chunk_type>(0u);
        }
        else {
            ++(m_arr[max_index]);
            m_arr[max_index] &= chunk_traits<nchunks>::mask;
        }
        return *this;
    }
}; // End of BitArray class definition.

//!\group Non-member operator overloads
//!\{
template<size_t N, class T>
constexpr BitArray<N,T> operator&(BitArray<N,T> lhs, BitArray<N,T> const& rhs) noexcept
{
    lhs &= rhs;
    return lhs;
}

template<size_t N, class T>
constexpr BitArray<N,T> operator|(BitArray<N,T> lhs, BitArray<N,T> const& rhs) noexcept
{
    lhs |= rhs;
    return lhs;
}

template<size_t N, class T>
constexpr BitArray<N,T> operator^(BitArray<N,T> lhs, BitArray<N,T> const& rhs) noexcept
{
    lhs ^= rhs;
    return lhs;
}

template<class C, size_t N, class T>
std::ostream& operator<<(std::basic_ostream<C>& out, BitArray<N,T> const& bit)
{
    for(size_t i = 0; i < N; ++i)
        out.put(bit.test(N-i-1) ? '1' : '0');

    return out;
}
//!\}


} // end namespace BitArray

