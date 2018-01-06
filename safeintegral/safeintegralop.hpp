/*
	Copyright (C) 2015-2018 Federico Kircheis

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
#ifndef SAFEOPERATIONS_H
#define SAFEOPERATIONS_H

#if  __cplusplus > 201402L // compiling with c++17 or greater
#include "safeintegralop2.hpp"
#endif

#include "errors.hpp"
#include "safeintegralop_cmp.hpp"


#include <limits>
#include <type_traits>
#include <cstdint>

namespace safeintegralop {

	enum class sign { negative = -1, zero = 0, positive = 1 };

	// All functions in this namespace are for private use, you should use all the function outside of this namespace
	namespace details{

		// Implementation for signum
		template <typename T>
		constexpr sign signum_unsigned(const T x) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return x > T{0} ? sign::positive : sign::zero;
		}

		template <typename T>
		constexpr sign signum_signed(const T x) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return x > T{0} ? sign::positive :x < T{0} ? sign::negative : sign::zero;
	    }

		template <typename T>
		constexpr bool is_safe_abs_unsigned(const T) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return true;
		}

		template <typename T>
		constexpr bool is_safe_abs_signed(const T a) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return (a != std::numeric_limits<T>::min());
		}

		template <typename T>
		constexpr bool is_safe_add_unsigned(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return (b == T{0}) || (a <= std::numeric_limits<T>::max() - b);
		}

		template <typename T>
		constexpr bool is_safe_add_signed(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return
			    (b == T{0}) ? true :
			    (b > T{0}) ? (a <= std::numeric_limits<T>::max() - b) :
			    (a >= std::numeric_limits<T>::min() - b);
		}

		template <typename T>
		constexpr bool is_safe_diff_unsigned(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return a >= b;
		}

		template <typename T>
		constexpr bool is_safe_diff_signed(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return
			    (b == T{0}) ? true :
			    (b > T{0}) ? (a >= std::numeric_limits<T>::min() + b) :
			    (a <= std::numeric_limits<T>::max() + b);
		}

		template <typename T>
		constexpr bool is_safe_mod_unsigned(const T, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return b!=T{0};
		}

		template <typename T>
		constexpr bool is_safe_mod_signed(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return (b == T{-1}) ? (a != std::numeric_limits<T>::min()) :  (b != T{0});
		}

		template <typename T>
		constexpr bool is_safe_mult_unsigned(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return
			    (a==T{0} || b== T{0} || b==T{1} || a == T{1}) ? true :
			    (b < T{1}) ? true :
			    (a <= std::numeric_limits<T>::max() / b);
		}

		template <typename T>
		constexpr bool is_safe_mult_signed(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return
			    (a==T{0} || b== T{0} || b==T{1} || a == T{1}) ? true :
			    (b==T{-1}) ? a != std::numeric_limits<T>::min() : // a/-1 == a*-1 --> overflow if a == minvalue
			    (b>T{-1} && b<T{1}) ? true :
			    ( (a <= std::numeric_limits<T>::max() / b) && (a >= std::numeric_limits<T>::min() / b)); // |b| >1, espansione
		}

		template <typename T>
		constexpr bool is_safe_div_unsigned(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return
			    (a == T{0} || b == T{1}) ? true :
			    (b == T{0}) ? false :
			    (b>T{1}) ? true :
			    (a < std::numeric_limits<T>::max() * b);
		}

		template <typename T>
		constexpr bool is_safe_div_signed(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return
			    (a == T{0} || b == T{1}) ? true :
			    (b == T{0}) ? false :
			    (b == T{-1}) ? (a != std::numeric_limits<T>::min()) :
			    (b>T{1} || b<T{-1}) ? true :
			    ( (a < std::numeric_limits<T>::max() * b) && (a > std::numeric_limits<T>::min() * b));
		}


		template <typename T>
		constexpr bool is_safe_leftshift_unsigned(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return !(b >= static_cast<T>(std::numeric_limits<T>::digits) || (a > (std::numeric_limits<T>::max() >> b)) );
		}

		template <typename T>
		constexpr bool is_safe_leftshift_signed(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return !( (a < T{0}) || (b < T{0}) || (b >= std::numeric_limits<T>::digits) || (a > (std::numeric_limits<T>::max() >> b)));
		}

		template <typename T>
		constexpr bool is_safe_rightshift_unsigned(const T, const T) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return true;
		}

		template <typename T>
		constexpr bool is_safe_rightshift_signed(const T a, const T b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T);
			return (b >= T{0} && a >= T{0});
		}

    } // end details

    /// This function represents the signature function for integral values.
    /// This function returns a sign, i.e. if the value is greater, equal, or less zero.
    template <typename T>
    constexpr sign signum(const T x) noexcept {
		return std::is_unsigned<T>::value ? details::signum_unsigned(x) : details::signum_signed(x);
	}

	/// This function checks if calculating the absolute value (i.e. abs(a) ) will overflow
	template <typename T>
	constexpr bool is_safe_abs(const T a) noexcept {
		return std::is_unsigned<T>::value ? details::is_safe_abs_unsigned(a) : details::is_safe_abs_signed(a);
	}

	/// This function checks if the addition two integral values (i.e. a+b ) will overflow
	template <typename T>
	constexpr bool is_safe_add(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::is_safe_add_unsigned(a, b) : details::is_safe_add_signed(a, b);
	}

	/// This function checks if the subtraction of one integral values from another (i.e. a-b ) will overflow
	template <typename T>
	constexpr bool is_safe_diff(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::is_safe_diff_unsigned(a, b) : details::is_safe_diff_signed(a, b);
	}

	/// This function checks if calculating the remainder of two integral values (i.e. a%b ) will overflow, and if the arguments are valid (i.e. b != 0)
	template <typename T>
	constexpr bool is_safe_mod(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::is_safe_mod_unsigned(a, b) : details::is_safe_mod_signed(a, b);
	}

	/// This function checks if the multiplication of two integral values (i.e. a*b ) will overflow
	template <typename T>
	constexpr bool is_safe_mult(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::is_safe_mult_unsigned(a,b) : details::is_safe_mult_signed(a,b);
	}

	/// This function checks if the division between two integral values (i.e. a/b ) will overflow, and if the arguments are valid (i.e. b != 0)
	template <typename T>
	constexpr bool is_safe_div(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::is_safe_div_unsigned(a,b) : details::is_safe_div_signed(a,b);
	}

	/// This function checks if the left shift between two integral values (i.e. a<<b ) will overflow, and if the arguments are valid (i.e. a>0 && b >0)
	template <typename T>
	constexpr bool is_safe_leftshift(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::is_safe_leftshift_unsigned(a,b) : details::is_safe_leftshift_signed(a,b);
	}

	/// This function checks if the arguments for the right shift operation between two integral values (i.e. a>>b ) are valid ( i.e. a>0 && b>0)
	template <typename T>
	constexpr bool is_safe_rightshift(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::is_safe_rightshift_unsigned(a,b) : details::is_safe_rightshift_signed(a,b);
	}

}

#endif // SAFEOPERATIONS_H
