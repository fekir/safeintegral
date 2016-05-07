/*
	Copyright (C) 2015-2016 Federico Kircheis

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

#include <limits>
#include <type_traits>


namespace safeintegralop {
	enum class sign { negative = -1, zero = 0, positive = 1 };

	template <typename T>
	constexpr std::size_t precision();

	// All functions in this namespace are for private use, you should use all the function outside of this namespace
	namespace details{

		// Implementation for signum
		template <typename T>
		constexpr sign signum_unsigned(const T x) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return x > T{0} ? sign::positive : sign::zero;
		}

		template <typename T>
		constexpr sign signum_signed(const T x) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return
				x > T{0} ? sign::positive :
				x < T{0} ? sign::negative :
				sign::zero; // x==T{0}
		}

		template <class T>
		constexpr std::size_t pop(const std::size_t precision, const T num) {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return (num == T{0}) ? precision : pop(((num % 2 != 0) ? precision+1 : precision), num >> 1);
		}

		template <typename T>
		constexpr bool isSafeAbs_unsigned(const T) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return true;
		}

		template <typename T>
		constexpr bool isSafeAbs_signed(const T a) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return (a != std::numeric_limits<T>::min());
		}

		template <typename T>
		constexpr bool isSafeAdd_unsigned(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return (b == T{0}) || (a <= std::numeric_limits<T>::max() - b);
		}

		template <typename T>
		constexpr bool isSafeAdd_signed(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return
				(b == T{0}) ? true :
				(b > T{0}) ? (a <= std::numeric_limits<T>::max() - b) :
				(a >= std::numeric_limits<T>::min() - b);
		}

		template <typename T>
		constexpr bool isSafeDiff_unsigned(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return a >= b;
		}

		template <typename T>
		constexpr bool isSafeDiff_signed(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return
				(b == T{0}) ? true :
				(b > T{0}) ? (a >= std::numeric_limits<T>::min() + b) :
				(a <= std::numeric_limits<T>::max() + b);
		}

		template <typename T>
		constexpr bool isSafeMod_unsigned(const T, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return b!=T{0};
		}

		template <typename T>
		constexpr bool isSafeMod_signed(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return (b == T{-1}) ? (a != std::numeric_limits<T>::min()) :  (b != T{0});
		}

		template <typename T>
		constexpr bool isSafeMult_unsigned(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return
				(a==T{0} || b== T{0} || b==T{1} || a == T{1}) ? true :
				(b < T{1}) ? true :
				(a <= std::numeric_limits<T>::max() / b);
		}

		template <typename T>
		constexpr bool isSafeMult_signed(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return
				(a==T{0} || b== T{0} || b==T{1} || a == T{1}) ? true :
				(b==T{-1}) ? a != std::numeric_limits<T>::min() : // a/-1 == a*-1 --> overflow if a == minvalue
				(b>T{-1} && b<T{1}) ? true :
				( (a <= std::numeric_limits<T>::max() / b) && (a >= std::numeric_limits<T>::min() / b)); // |b| >1, espansione
		}

		template <typename T>
		constexpr bool isSafeDiv_unsigned(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return
				(a == T{0} || b == T{1}) ? true :
				(b == T{0}) ? false :
				(b>T{1}) ? true :
				(a < std::numeric_limits<T>::max() * b);
		}

		template <typename T>
		constexpr bool isSafeDiv_signed(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return
				(a == T{0} || b == T{1}) ? true :
				(b == T{0}) ? false :
				(b == T{-1}) ? (a != std::numeric_limits<T>::min()) :
				(b>T{1} || b<T{-1}) ? true :
				( (a < std::numeric_limits<T>::max() * b) && (a > std::numeric_limits<T>::min() * b));
		}


		template <typename T>
		constexpr bool isSafeLeftShift_unsigned(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return !( b >= precision<T>() || (a > (std::numeric_limits<T>::max() >> b)) );
		}

		template <typename T>
		constexpr bool isSafeLeftShift_signed(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return !( (a < T{0}) || (b < T{0}) || (b >= precision<T>()) || (a > (std::numeric_limits<T>::max() >> b)));
		}

		template <typename T>
		constexpr bool isSafeRightShift_unsigned(const T, const T) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return true;
		}

		template <typename T>
		constexpr bool isSafeRightShift_signed(const T a, const T b) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			return (b >= T{0} && a >= T{0});
		}
	} // end details

	template <typename T>
	constexpr sign signum(const T x) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::signum_unsigned(x) : details::signum_signed(x);
	}

	/// From SecureCoding:
	/// Integer types in C have both a size and a precision. The size indicates
	/// the number of bytes used by an object and can be retrieved for any object
	/// or type using the sizeof operator.
	/// The precision of an integer type is the number of bits it uses to
	/// represent values, excluding any sign and padding bits.
	///
	/// Padding bits contribute to the integer's size, but not to its precision.
	/// Consequently, inferring the precision of an integer type from its size may
	/// result in too large a value, which can then lead to incorrect assumptions about
	/// the numeric range of these types.
	/// Programmers should use correct integer precisions in their code, and in particular,
	/// should not use the sizeof operator to compute the precision of an integer type on
	/// architectures that use padding bits or in strictly conforming/portable programs.
	template <typename T>
	constexpr std::size_t precision(){
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return details::pop(0, std::numeric_limits<T>::max());
	}


	template <typename T>
	constexpr bool isSafeAbs(const T a) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeAbs_unsigned(a) : details::isSafeAbs_signed(a);
	}

	/**
	 * Checks if it is safe do add two numbers, ie if the operation a+b does not overflow/underflow
	 */
	template <typename T>
	constexpr bool isSafeAdd(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeAdd_unsigned(a, b) : details::isSafeAdd_signed(a, b);
	}

	template <typename T>
	constexpr bool isSafeDiff(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeDiff_unsigned(a, b) : details::isSafeDiff_signed(a, b);
	}

	template <typename T>
	constexpr bool isSafeMod(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeMod_unsigned(a, b) : details::isSafeMod_signed(a, b);
	}

	template <typename T>
	constexpr bool isSafeMult(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeMult_unsigned(a,b) : details::isSafeMult_signed(a,b);
	}

	template <typename T>
	constexpr bool isSafeDiv(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeDiv_unsigned(a,b) : details::isSafeDiv_signed(a,b);
	}


	template <typename T>
	constexpr bool isSafeLeftShift(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeLeftShift_unsigned(a,b) : details::isSafeLeftShift_signed(a,b);
	}

	// Right-shift operations do not overflow, but the result of a right-shift
	// of a signed negative number is implementation-dependent
	template <typename T>
	constexpr bool isSafeRightShift(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeRightShift_unsigned(a,b) : details::isSafeRightShift_signed(a,b);
	}
}

#endif // SAFEOPERATIONS_H
