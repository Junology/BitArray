#include <iostream>
#include <bitset>

#include "LA/traits.hpp"
#include "BitArray.hpp"

int main(int, char**)
{
    static_assert(
        BitLA::BitLA_traits<BitArray::BitArray<256>>::has_parity == false,
        "BitArray does not have parity() const member function."
        );
    static_assert(
        BitLA::BitLA_traits<BitArray::BitArray<256>>::has_count == true,
        "BitArray has count() const member function."
        );
    static_assert(
        BitLA::BitLA_traits<BitArray::BitArray<256>>::has_rshift == true,
        "BitArray has right-shift operator overload."
        );

    return EXIT_SUCCESS;
}
