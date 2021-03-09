#pragma once

#include <iterator>

namespace BitArray {

template <std::size_t, class>
class BitArray;

//! Iterator visiting all the true bits.
//! Dereferencing it gives the (0-begining) index of the bit.
template <class T>
class PopIterator;


template <std::size_t N, class T>
class PopIterator<BitArray<N,T>> : public std::forward_iterator_tag
{
    friend class BitArray<N,T>;

public:
    using value_type = std::size_t;
    using BitArrayT = BitArray<N,T>;
    using chunk_type = typename BitArrayT::chunk_type;

    enum : std::size_t {
        length = BitArrayT::length,
        chunkbits = BitArrayT::chunkbits,
    };

private:
    chunk_type const *m_chunks;
    std::size_t m_pos{};
    chunk_type m_value{};

    constexpr PopIterator(chunk_type const (&chunks)[length], size_t pos, chunk_type value) noexcept
        : m_chunks(chunks), m_pos(pos), m_value(pos < length ? static_cast<chunk_type>(chunks[pos] & value) : chunk_type{})
    {
        while(!m_value && m_pos+1 < length) {
            m_value = m_chunks[++m_pos];
        }
    }

public:
    // Required special member functions.
    constexpr PopIterator() noexcept = default;
    constexpr PopIterator(PopIterator const&) noexcept = default;
    constexpr PopIterator& operator=(PopIterator const&) noexcept = default;

    // Comparable in the canonical way.
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
        while(!m_value && m_pos+1 < length) {
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
