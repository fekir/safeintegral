/*
	Copyright (C) 2015-2017 Federico Kircheis

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef SAFEOPERATIONS_CMP_H
#define SAFEOPERATIONS_CMP_H

#include <limits>
#include <type_traits>
#include <cstdint>

namespace safeintegralop {

	// All functions in this namespace are for private use, you should use all the function outside of this namespace
	namespace details{

#define FEK_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR " needs to be an integral (not bool or char/wchar_t) value type"
#define FEK_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T) static_assert(is_integral_not_bool_char<T>(), #T FEK_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR)
#define FEK_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE(T, R) \
	static_assert(is_integral_not_bool_char<T>(), #T FEK_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR);\
	static_assert(is_integral_not_bool_char<R>(), #R FEK_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR)

		// Most operatio do not make sense on bool, even if the are considered integrals
		template <typename T>
		constexpr bool is_integral_not_bool_char(){
			using value_type = typename std::remove_cv<T>::type;
			return
			    !std::is_same<value_type,bool>::value && !std::is_same<value_type,char>::value &&
			    !std::is_same<value_type,char16_t>::value && !std::is_same<value_type,char32_t>::value &&
			    !std::is_same<value_type,wchar_t>::value && std::is_integral<T>::value;
		}

		// could use the same implementation of in_range_signed_signed, but compiler may generate warning that t is always bigger than 0
		template <typename R, typename T>
		constexpr bool in_range_unsigned_unsigned(const T t) noexcept {
			FEK_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE(T,R);
			return (std::numeric_limits<T>::digits > std::numeric_limits<R>::digits) ?
			    (t < static_cast<T>(std::numeric_limits<R>::max())) :
			    (static_cast<R>(t) <std::numeric_limits<R>::max());
		}

		template <typename R, typename T>
		constexpr bool in_range_signed_signed(const T t) noexcept {
			FEK_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE(T,R);
			return (std::numeric_limits<T>::digits > std::numeric_limits<R>::digits) ?
			    (t <= static_cast<T>(std::numeric_limits<R>::max()) && t >= static_cast<T>(std::numeric_limits<R>::min())) :
			    (static_cast<R>(t) <= std::numeric_limits<R>::max() && static_cast<R>(t) >= std::numeric_limits<R>::max());
		}

		template <typename R, typename T>
		constexpr bool in_range_signed_unsigned(const T t) noexcept {
			FEK_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE(T,R);
			return (t < T{ 0 }) ? false :
			    (std::numeric_limits<T>::digits / 2 <= std::numeric_limits<R>::digits) ? true :
			    (t <= static_cast<T>(std::numeric_limits<R>::max()));
		}

		template <typename R, typename T>
		constexpr bool in_range_unsigned_signed(const T t) noexcept {
			FEK_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE(T,R);
			return (std::numeric_limits<T>::digits >= std::numeric_limits<R>::digits / 2) ? (t <= static_cast<T>(std::numeric_limits<R>::max())) : true;
		}

		template <typename R, typename T>
		constexpr bool in_range_unsigned(const T t) noexcept {
			return std::is_unsigned<R>::value ? in_range_unsigned_unsigned<R>(t) : in_range_unsigned_signed<R>(t);
		}

		template <typename R, typename T>
		constexpr bool in_range_signed(const T t) noexcept {
			return std::is_signed<R>::value ? in_range_signed_signed<R>(t) : in_range_signed_unsigned<R>(t);
		}

		// equivalent of operator== for different integral types
		template <typename T, typename U>
		constexpr bool cmp_equal_same_sign(const T t, const U u) noexcept {
			FEK_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE(T,U);
			return (std::numeric_limits<T>::digits>std::numeric_limits<U>::digits) ? (t == static_cast<T>(u)) : (static_cast<U>(t) == u);
		}

		template <typename T, typename U>
		constexpr bool cmp_equal_signed_unsigned(const T t, const U u) noexcept {
			FEK_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE(T,U);
			return (t<T{ 0 }) ? false : (std::numeric_limits<T>::digits / 2>std::numeric_limits<U>::digits) ? (t == static_cast<T>(u)) : (static_cast<U>(t) == u);
		}

		// equivalent of operator< for different integral types
		template <typename T, typename U>
		constexpr bool cmp_less_same_sign(const T t, const U u) noexcept {
			FEK_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE(T,U);
			return (std::numeric_limits<T>::digits>std::numeric_limits<U>::digits) ? (t < static_cast<T>(u)) : (static_cast<U>(t) < u);
		}

		template <typename T, typename U>
		constexpr bool cmp_less_signed_unsigned(const T t, const U u) noexcept {
			FEK_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE(T,U);
			return (t<T{ 0 }) ? true : (std::numeric_limits<T>::digits / 2>std::numeric_limits<U>::digits) ? (t < static_cast<T>(u)) : (static_cast<U>(t) < u);
		}

		template <typename T, typename U>
		constexpr bool cmp_less_unsigned_signed(const T t, const U u) noexcept {
			FEK_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE(T,U);
			return (u<U{ 0 }) ? false : (std::numeric_limits<U>::digits / 2>std::numeric_limits<T>::digits) ? (static_cast<U>(t) < u) : (t < static_cast<T>(u));
		}
    } // end details


	/// Usage:
	/// size_t i == ...
	/// if(in_range<DWORD>(i)){
	///  safe to use i as a DWORD value, parameter...
	/// } else {
	///  not possible to rappresent i as a DWORD
	/// }
	template <typename R, typename T>
	constexpr bool in_range(const T t) noexcept {
		return std::is_unsigned<T>::value ? details::in_range_unsigned<R>(t) : details::in_range_signed<R>(t);
	}

	// equivalent of operator== for different types
	/// Usage:
	/// size_t i == ...
	/// DWORD j == ...
	/// if(cmp_equal(i,j)){
	///  i and j rappresent the same quantity
	/// } else {
	///  i and j rappresents different quantities
	/// }
	template <typename T, typename U>
	constexpr bool cmp_equal(const T t, const U u) noexcept {
		return
		    (std::is_signed<T>::value == std::is_signed<U>::value) ? details::cmp_equal_same_sign(t, u) :
		    (std::is_signed<T>::value) ? details::cmp_equal_signed_unsigned(t, u) : details::cmp_equal_signed_unsigned(u,t);
	}

	// equivalent of operator< for different integral types
	/// Usage:
	/// size_t i == ...
	/// DWORD j == ...
	/// if(cmp_less(i,j)){
	///  i < j
	/// } else {
	///  i >= j
	/// }
	template <typename T, typename U>
	constexpr bool cmp_less(const T t, const U u) noexcept {
		return
		    (std::is_signed<T>::value == std::is_signed<U>::value) ? details::cmp_less_same_sign(t,u) :
		    (std::is_signed<T>::value) ? details::cmp_less_signed_unsigned(t, u) : details::cmp_less_unsigned_signed(t, u);
	}



	namespace ct {

		// Compile tests for in_range
		static_assert(in_range<short>(1),   "in range");
		static_assert(in_range<short>(1u),  "in range");
		static_assert(in_range<short>(1ul), "in range");
		static_assert(in_range<short>(-1l), "in range");


		static_assert(!in_range<int16_t>(std::numeric_limits<uint16_t>::max()), "in range");
		static_assert(in_range<int16_t>(std::numeric_limits<uint8_t>::max()), "in range");
		static_assert(!in_range<uint8_t>(-1), "in range, negative value, unsigned range");
		static_assert(in_range<int8_t>(-1), "in range, negative value, unsigned range");
		static_assert(in_range<uint8_t>(std::numeric_limits<uint16_t>::min()), "in range");
		static_assert(in_range<int8_t>(std::numeric_limits<uint16_t>::min()), "in range");
		static_assert(!in_range<uint8_t>(std::numeric_limits<int16_t>::min()), "in range");
		static_assert(!in_range<int8_t>(std::numeric_limits<int16_t>::min()), "in range");

		// Compile tests for cmp_equal
		static_assert(cmp_equal(1, 1),    "comparison same signed type, same value");
		static_assert(cmp_equal(1u, 1u),  "comparison same unsigned type, same value (2)");
		static_assert(cmp_equal(1ul, 1u), "comparison unsigned types, same value");
		static_assert(cmp_equal(1u, 1ul), "comparison unsigned types, same value (2)");
		static_assert(cmp_equal(1l, 1),   "comparison signed types, same value");
		static_assert(cmp_equal(1, 1l),   "comparison signed types, same value (2)");
		static_assert(cmp_equal(1ul, 1),  "comparison signed/unsigned types, same value");
		static_assert(cmp_equal(1, 1ul),  "comparison signed/unsigned types, same value (2)");


		static_assert(!cmp_equal(1, 2),    "comparison same signed type, different values");
		static_assert(!cmp_equal(1, -1),   "comparison same signed type, different values (2)");
		static_assert(!cmp_equal(1u, 2u),  "comparison same unsigned type, different values");
		static_assert(!cmp_equal(1ul, 2u), "comparison unsigned types, different values");
		static_assert(!cmp_equal(2u, 1ul), "comparison unsigned types, different values (2)");
		static_assert(!cmp_equal(1l, 2),   "comparison signed types, different values");
		static_assert(!cmp_equal(2, 1l),   "comparison signed types, different values (2)");
		static_assert(!cmp_equal(2ul, 1),  "comparison signed/unsigned types, different values");
		static_assert(!cmp_equal(1, 2ul),  "comparison signed/unsigned types, different values (2)");

		static_assert(!cmp_equal(std::numeric_limits<uint8_t>::max(), std::numeric_limits<int8_t>::max()),
		    "comparison unsigned/signed type");
		static_assert(!cmp_equal(std::numeric_limits<uint8_t>::max(), -1),
		    "comparison unsigned/signed type");


		// Compile tests for cmp_less
		static_assert(!cmp_less(1, 1),    "comparison same signed type, same value");
		static_assert(!cmp_less(1u, 1u),  "comparison same unsigned type, same value (2)");
		static_assert(!cmp_less(1ul, 1u), "comparison unsigned types, same value");
		static_assert(!cmp_less(1u, 1ul), "comparison unsigned types, same value (2)");
		static_assert(!cmp_less(1l, 1),   "comparison signed types, same value");
		static_assert(!cmp_less(1, 1l),   "comparison signed types, same value (2)");
		static_assert(!cmp_less(1ul, 1),  "comparison signed/unsigned types, same value");
		static_assert(!cmp_less(1, 1ul),  "comparison signed/unsigned types, same value (2)");

		static_assert(cmp_less(1, 2),    "comparison same signed type, different values");
		static_assert(!cmp_less(1, -1),  "comparison same signed type, different values (2)");
		static_assert(cmp_less(1u, 2u),  "comparison same unsigned type, different values");
		static_assert(cmp_less(1ul, 2u) ,"comparison unsigned types, different values");
		static_assert(!cmp_less(2u, 1ul),"comparison unsigned types, different values (2)");
		static_assert(cmp_less(1l, 2),   "comparison signed types, different values");
		static_assert(!cmp_less(2, 1l),  "comparison signed types, different values (2)");
		static_assert(cmp_less(1ul, 2),  "comparison signed/unsigned types, different values");
		static_assert(!cmp_less(2, 1ul), "comparison signed/unsigned types, different values (2)");

		static_assert(!cmp_less(std::numeric_limits<uint8_t>::max(), std::numeric_limits<int8_t>::max()),
		    "comparison unsigned/signed type");
		static_assert(!cmp_less(std::numeric_limits<uint8_t>::max(), -1),
		    "comparison unsigned/signed type");
	}

}

#endif // SAFEOPERATIONS_H
