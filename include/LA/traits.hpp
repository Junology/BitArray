/*!
 * \file traits.hpp
 * \author Jun Yoshida
 * \copyright (c) 2019-2021 Jun Yoshida.
 * The project is released under the MIT License.
 * \date March, 16 2021: created
 */

#pragma once

#include <type_traits>

namespace BitLA {

namespace _impl {

//! Essentially same as std::is_convertible while it is supposed that every type can be converted into void.
template <class From, class To>
struct conv_checker : public std::is_convertible<From,To> {};

template <class From>
struct conv_checker<From,void> : public std::true_type {};

//! Define a trait class that checks if an expression is available or not.
#define DEFINE_CHECKER_FOR_EXPR(name, tvar, expr, result_t)  \
    template <class TPARAM>                                  \
    class name {                                             \
        template <class tvar>                                \
        static auto test(int)                                \
            -> conv_checker<decltype(expr), result_t>;       \
        template <class> static std::false_type test(...);   \
    public:                                                  \
        enum : bool {                                        \
            value = decltype(test<TPARAM>(0))::value         \
        };                                                   \
    }

//! Define a trait class that checks if an expression is available and constexpr.
#define DEFINE_CHECKER_FOR_STATIC_EXPR(name, tvar, expr, result_t)  \
    template <class tvar>                                           \
    class name {                                                    \
        template <class R, R val = expr>                            \
        static std::true_type test(int);                            \
        template <class> static std::false_type test(...);          \
    public:                                                         \
        enum : bool {                                               \
            value = decltype(test<result_t>(0))::value              \
        };                                                          \
    }

//! Check if a given class has a static information on the number of bits.
DEFINE_CHECKER_FOR_STATIC_EXPR(
    has_numbits, T, T::numbits, std::size_t
    );

//! Check if a given class has size() const member function.
DEFINE_CHECKER_FOR_EXPR(
    has_size, T, std::declval<T const>().size(), std::size_t
    );

//! Check if a given class has size() constexpr member function.
DEFINE_CHECKER_FOR_STATIC_EXPR(
    has_constexpr_size, T, T().size(), std::size_t
    );

//! Check if a given class has test(std::size_t) const member function.
DEFINE_CHECKER_FOR_EXPR(
    has_test, T, std::declval<T const>().test(std::declval<std::size_t>()), bool
    );

//! Check if a given class has parity() const member function for parity of 'true' bits.
DEFINE_CHECKER_FOR_EXPR(
    has_parity, T, std::declval<T const>().parity(), bool
    );

//! Check if a given class has count() const member for pop-counting.
DEFINE_CHECKER_FOR_EXPR(
    has_count, T, std::declval<T const>().count(), std::size_t
    );

//! Check if the OR binary operation is available
DEFINE_CHECKER_FOR_EXPR(
    has_or, T, std::declval<T const>() | std::declval<T const>(), T
    );

//! Check if the AND binary operation is available
DEFINE_CHECKER_FOR_EXPR(
    has_and, T, std::declval<T const>() & std::declval<T const>(), T
    );

//! Check if the XOR binary operation is available
DEFINE_CHECKER_FOR_EXPR(
    has_xor, T, std::declval<T const>() ^ std::declval<T const>(), T
    );

//! Check if the right-shift is available.
DEFINE_CHECKER_FOR_EXPR(
    has_rshift, T, std::declval<T const>() >> std::declval<int>(), T
    );

//! Check if the xor-assignment operator is available.
DEFINE_CHECKER_FOR_EXPR(
    has_xor_assign, T, std::declval<T>() ^= std::declval<T const>(), void
    );

//! Check if the type has cast to bool.
DEFINE_CHECKER_FOR_EXPR(
    has_cast_to_bool, T, static_cast<bool>(std::declval<T>()), bool
    );

#undef DEFINE_CHECKER_FOR_EXPR
#undef DEFINE_CHECKER_FOR_STATIC_EXPR

} // end namespace _impl

//! Ensure a given class is suitable for the linear algebra over F2.
template <class T>
struct BitLA_traits
{
    //! T must be default constructible.
    static_assert(
        std::is_default_constructible<T>::value,
        "The type must be default-constructible."
        );

    //! The length can be obtained from size() const member function.
    static_assert(
        _impl::has_size<T>::value,
        "The type doesn't have a member function size() of an appropriate signature."
        );

    //! The states of bits are obtained from test() const member function.
    static_assert(
        _impl::has_test<T>::value,
        "The type doesn't have a member function test() of an appropriate signature."
        );

    //! XOR operation is available.
    static_assert(
        _impl::has_xor<T>::value,
        "XOR operation is not available"
        );

    //! XOR-assignment operation is available.
    static_assert(
        _impl::has_xor_assign<T>::value,
        "XOR-assignment operation is not available"
        );

    //! AND operation is available.
    static_assert(
        _impl::has_and<T>::value,
        "AND operation is not available"
        );

    //! The result of member function checkers.
    enum : bool {
        //! Check if size() function is constexpr or not.
        //! More precistly, this verifies the following two:
        //!
        //!  - The type is default-constructible at compile-time.
        //!  - size() member function available at compile-time for the default-constructed instance.
        //!
        //! \sa{length}
        has_constexpr_size = _impl::has_constexpr_size<T>::value,
        has_parity = _impl::has_parity<T>::value,
        has_count = _impl::has_count<T>::value,
        has_rshift = _impl::has_rshift<T>::value,
    };

    //! Compile-time bit-length
    //! \warning If has_constexpr_size == false, then the use of this function will cause an error at compile-time.
    template <class U = std::enable_if_t<has_constexpr_size, std::true_type>>
    static constexpr std::size_t length(U = U{}) noexcept {
        constexpr std::size_t ret = T().size();
        return ret;
    }
};

} // end namespace BitLA.
