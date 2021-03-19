/*!
 * \file iterators.hpp
 * \author Jun Yoshida
 * \copyright (c) 2019-2021 Jun Yoshida.
 * The project is released under the MIT License.
 * \date March, 8 2021: created

In this file, there are classes and functions for iterating the data associated to BitArray objects.
Note that, here, the meaning of "iteration" may vary.
For example, \sa{PopIterator} class traverses all `true` bits, and de-referencing results the index of such bits.
For each such class, say 'BitIterator' class, one can traverse the corresponding data as follows:
\code{.cpp}
BitArray<10> bitarr{0b1001110000};
// ...
for (auto itr : range<BitIterator>(bitarr)) {
  std::cout << *itr << std::endl;
}
\endcode
In the case BitIterator = \sa{PopIterator}, we get
> 4
> 5
> 6
> 9
 */

#pragma once

#include <iterator>
#include "../BitArray.hpp"

namespace BitArray {


/************************//*!
    General Framework
****************************/

//! Trait class for `BitIterator`s
template <template<std::size_t,class> class Itr, std::size_t N, class T>
struct BitIteratorTraits
{
private:
    //! Check if the given iterator type has `is_const` member of a type convertible to bool.
    struct has_is_const
    {
        template <class U>
        static std::true_type test(
            std::enable_if_t<
            std::is_convertible<decltype(U::is_const), bool>::value,
            int
            >
            );

        template <class U>
        static std::false_type test(...);

        enum : bool {
            value = decltype(test<Itr<N,T>>(std::declval<int>()))::value
        };
    };

public:
    enum : bool {
        is_const =
          std::conditional_t<
            has_is_const::value,
            std::integral_constant<bool, Itr<N,T>::is_const>,
            std::false_type
          >::value
    };

    using target_ref_t = std::conditional_t<
        is_const,
        BitArray<N,T> const&, BitArray<N,T>&>;

    //! Types for access via iterators.
    using value_type = typename Itr<N,T>::value_type;
    using reference_type = typename Itr<N,T>::reference_type;
    using iterator = Itr<N,T>;
};


/*!
 * The class for container-like access to BitArray.
 */
template <template<std::size_t,class> class Itr, std::size_t N, class T>
class BitContainer;

//! The function generating range-accessor.
//! \tparam Itr The target BitIterator type.
//! \param src A BitArray object where the generated iterator traverses.
//! \return Range accessor.
template <template <std::size_t,class> class Itr, std::size_t N, class T>
constexpr auto range(BitArray<N,T> &src) noexcept
    -> std::enable_if_t<
        !(BitIteratorTraits<Itr, N, T>::is_const),
        BitContainer<Itr,N,T>
        >
{
    return BitContainer<Itr,N,T>(src);
}

//! The function generating range-accessor.
//! \tparam Itr The target BitIterator type.
//! \param src A BitArray object where the generated iterator traverses.
//! \return Range accessor.
template <template <std::size_t,class> class Itr, std::size_t N, class T>
constexpr auto range(BitArray<N,T> const& src) noexcept
    -> std::enable_if_t<
        BitIteratorTraits<Itr, N, T>::is_const,
        BitContainer<Itr,N,T>
        >
{
    return BitContainer<Itr,N,T>(src);
}

//! \name Empty classes for tag dispatch.
//! \{
struct ContainerBeginTag {};
struct ContainerEndTag {};
//! \}


//! Range accessor to BitArray objects
//! \tparam Itr Associated BitIterator type.
//! \tparam N The number of bits in the source BitArray object.
//! \tparam T The chunk_type of the source BitArray type.
template <template<std::size_t,class> class Itr, std::size_t N, class T>
class BitContainer
    : public BitIteratorTraits<Itr,N,T>
{
    using traits = BitIteratorTraits<Itr,N,T>;

    friend BitContainer range<Itr,N,T>(typename traits::target_ref_t) noexcept;

public:
    //! Associated types
    using value_type = typename Itr<N,T>::value_type;
    using reference_type = typename Itr<N,T>::reference_type;
    using iterator = Itr<N,T>;

private:
    typename traits::target_ref_t m_target;

    //! Only friend functions can create an instance.
    constexpr BitContainer(typename traits::target_ref_t target) noexcept
        : m_target(target)
    {}

public:
    constexpr BitContainer(BitContainer const&) noexcept = default;
    constexpr BitContainer(BitContainer &&) noexcept = default;

    ~BitContainer() = default;

    constexpr BitContainer& operator=(BitContainer const&) noexcept = default;
    constexpr BitContainer& operator=(BitContainer &&) noexcept = default;

    constexpr iterator begin() noexcept {
        return Itr<N,T>(ContainerBeginTag{}, m_target.m_arr);
    }

    constexpr iterator end() noexcept {
        return Itr<N,T>(ContainerEndTag{}, m_target.m_arr);
    }
};


//! Iterator visiting all the true bits.
//! Dereferencing it gives the (0-begining) index of the bit.
template <std::size_t N, class T>
class PopIterator : public std::forward_iterator_tag
{
    friend class BitArray<N,T>;
    friend class BitContainer<PopIterator,N,T>;

public:
    using value_type = std::size_t;
    using reference_type = std::size_t;
    using BitArrayT = BitArray<N,T>;
    using chunk_type = typename BitArrayT::chunk_type;

    enum : bool {
        is_const = true
    };

    enum : std::size_t {
        nchunks = BitArrayT::nchunks,
        chunkbits = BitArrayT::chunkbits,
    };

    static_assert(nchunks > 0, "Zero length BitArray is not allowed.");

private:
    chunk_type const *m_chunks;
    std::size_t m_pos{};
    chunk_type m_value{};

    //! Generated by begin() function.
    constexpr PopIterator(ContainerBeginTag, chunk_type const (&chunks)[nchunks])
        noexcept
        : m_chunks(chunks), m_pos(0), m_value(static_cast<chunk_type>(chunks[0]))
    {
        while(!m_value && m_pos+1 < nchunks) {
            m_value = m_chunks[++m_pos];
        }
    }

    //! Generated by end() function.
    constexpr PopIterator(ContainerEndTag, chunk_type const (&chunks)[nchunks])
        noexcept
        : m_chunks(chunks), m_pos(nchunks-1), m_value{}
    {}

public:
    // Required special member functions.
    constexpr PopIterator() noexcept = default;
    constexpr PopIterator(PopIterator const&) noexcept = default;
    constexpr PopIterator& operator=(PopIterator const&) noexcept = default;

    //! Comparable in the canonical way.
    constexpr bool operator==(PopIterator const& other) const noexcept {
        return (m_chunks == nullptr && other.m_chunks == nullptr)
            || (m_chunks == other.m_chunks
                && m_pos == other.m_pos
                && m_value == other.m_value);
    }

    constexpr bool operator!=(PopIterator const& other) const noexcept {
        return !(*this == other);
    }

    constexpr size_t operator*() const noexcept {
        return counttrail0(m_value) + m_pos*chunkbits;
    }

    // Prefix increment operator.
    constexpr PopIterator& operator++() noexcept {
        // De-flag the lowest true bit.
        m_value &= ~(m_value ^ (m_value-1));
        // If there is no true bit anymore, go to the next chunk if any.
        while(!m_value && m_pos+1 < nchunks) {
            m_value = m_chunks[++m_pos];
        }
        return *this;
    }

    // Postfix increment operator.
    constexpr PopIterator& operator++(int) noexcept {
        PopIterator aux = *this;
        ++(*this);
        return aux;
    }
};

} // end namespace BitArray
