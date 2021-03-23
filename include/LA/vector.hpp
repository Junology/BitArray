/*!
 * \file vector.hpp
 * \author Jun Yoshida
 * \copyright (c) 2019-2021 Jun Yoshida.
 * The project is released under the MIT License.
 * \date March, 17 2021: created
 */

#pragma once

#include "traits.hpp"

#include "../BitArray.hpp"
#include "../BitArray/iterators.hpp"

namespace BitLA {

/*******************************************************//*!
 ** \name Implementations of parity() functions
 ** \brief A function computing the parity of an array of bits
 ***********************************************************/
//!\{

//! A version using a parity() member function if any.
template <
    class T,
    std::enable_if_t<
        BitLA_traits<T>::has_parity,
        int
        > = 0
    >
constexpr bool parity(T const& bits) { return bits.parity(); }

//! A version using a count() member function if any.
template <
    class T,
    std::enable_if_t<
        !BitLA_traits<T>::has_parity && BitLA_traits<T>::has_count,
        int
        > = 0
    >
constexpr bool parity(T const& bits) { return bits.count() % 2; }

//! A version using right-shifts and OR-assignment operator if any.
template <
    class T,
    std::enable_if_t<
        !BitLA_traits<T>::has_parity && !BitLA_traits<T>::has_count
        && BitLA_traits<T>::has_rshift,
        int
        > = 0
    >
constexpr bool parity(std::decay_t<T> bits)
{
    if(bits.size() == 0)
        return false;

    auto bound = (bits.size() - 1) << 1;

    for(std::size_t i = 1; i <= bound; i <<= 1) {
        bits ^= bits >> i;
    }

    return bits.test(0);
}

//! The most naive implementation. This is chosen if the other ways are unavailable.
template <
    class T,
    std::enable_if_t<
        !BitLA_traits<T>::has_parity && !BitLA_traits<T>::has_count
        && !BitLA_traits<T>::has_rshift,
        int
        > = 0
    >
constexpr bool parity(T const &bits)
{
    bool result = false;
    std::size_t sz = bits.size();
    for(std::size_t i = 0; i < sz; ++i) {
        result ^= bits.test(i);
    }

    return result;
}

//!\}


/***********************************//**
 **  \name Traversing all true bits
 ***************************************/
//!\{

template <class BA>
struct PopTraversable
{
    template <class F>
    static constexpr void traverse(BA bits, F&& f) noexcept {
        for(std::size_t i = 0; i < bits.size(); ++i) {
            if (bits.test(i))
                f(i);
        }
    }

    // Forward declaration.
    // While GCC doesn't complain, clang does if I omit this.
    class Range;

    class Iterator {
        friend class PopTraversable::Range;

        std::size_t i_;
        BA const& bits_;

        constexpr Iterator(herring::ContainerBeginTag, BA const &bits) noexcept
            : i_(0), bits_(bits)
        {
            while(i_ < bits.size() && !bits.test(i_))
                ++i_;
        }

        constexpr Iterator(herring::ContainerEndTag, BA const &bits) noexcept
            : i_(bits.size()), bits_(bits)
        {}

    public:
        constexpr Iterator() = delete;
        constexpr Iterator(Iterator const&src) = default;

        constexpr Iterator& operator++() noexcept
        {
            do {
                ++i_;
            } while(i_ < bits_.size() && !bits_.test(i_));

            return *this;
        }

        constexpr Iterator& operator++(int) noexcept
        {
            Iterator aux{*this};
            ++(*this);
            return aux;
        }

        constexpr bool operator==(Iterator const& other) noexcept
        {
            return i_ == other.i_;
        }

        constexpr bool operator!=(Iterator const& other) noexcept
        {
            return !(*this == other);
        }

        constexpr int operator*() noexcept
        {
            return i_;
        }
    };

    class Range {
        friend constexpr Range PopTraversable::range(BA const& bits) noexcept;

        BA const& bits_;

        constexpr Range(BA const& bits) : bits_{bits} {}

    public:
        constexpr Iterator begin() noexcept
        {
            return Iterator(herring::ContainerBeginTag{}, bits_);
        }

        constexpr Iterator end() noexcept
        {
            return Iterator(herring::ContainerEndTag{}, bits_);
        }
    };

    static constexpr Range range(BA const& bits) noexcept
    {
        return Range(bits);
    }
};


template <class T, std::size_t n>
struct PopTraversable<herring::BitArray<n,T>>
{
    template <class F>
    static constexpr void traverse(herring::BitArray<n,T> const& bits, F&& f) noexcept {
        for(auto i : herring::range<herring::PopIterator>(bits)) {
            f(i);
        }
    }

    static constexpr auto range(herring::BitArray<n,T> const& bits) noexcept
    {
        return herring::range<herring::PopIterator>(bits);
    }
};

//!\}

} // end namespace BitLA
