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

		// could use the same implementation of in_range_signed_signed, but compiler may generate warning that t is always bigger than 0
		template <typename R, typename T>
		constexpr bool in_range_unsigned_unsigned(const T t) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			static_assert(std::is_integral<R>::value, "R needs to be an integral value");
			return (sizeof(T)>sizeof(R)) ?
			    (t < static_cast<T>(std::numeric_limits<R>::max())) :
			    (static_cast<R>(t) <std::numeric_limits<R>::max());
		}

		template <typename R, typename T>
		constexpr bool in_range_signed_signed(const T t) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			static_assert(std::is_integral<R>::value, "R needs to be an integral value");
			return (sizeof(T)>sizeof(R)) ?
			    (t <= static_cast<T>(std::numeric_limits<R>::max()) && t >= static_cast<T>(std::numeric_limits<R>::min())) :
			    (static_cast<R>(t) <= std::numeric_limits<R>::max() && static_cast<R>(t) >= std::numeric_limits<R>::max());
		}

		template <typename R, typename T>
		constexpr bool in_range_signed_unsigned(const T t) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			static_assert(std::is_integral<R>::value, "R needs to be an integral value");
			return (t < T{ 0 }) ? false :
			    (sizeof(T) / 2 <= sizeof(R)) ? true :
			    (t <= static_cast<T>(std::numeric_limits<R>::max()));
		}

		template <typename R, typename T>
		constexpr bool in_range_unsigned_signed(const T t) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			static_assert(std::is_integral<R>::value, "R needs to be an integral value");
			return (sizeof(T) >= sizeof(R) / 2) ? (t <= static_cast<T>(std::numeric_limits<R>::max())) : true;
		}

		template <typename R, typename T>
		constexpr bool in_range_unsigned(const T t) noexcept {
			return std::is_unsigned<R>::value ? in_range_unsigned_unsigned<R>(t) : in_range_unsigned_signed<R>(t);
		}

		template <typename R, typename T>
		constexpr bool in_range_signed(const T t) noexcept {
			return std::is_signed<R>::value ? in_range_signed_signed<R>(t) : in_range_signed_unsigned<R>(t);
		}

		// equivalent of operator== for different types
		template <typename T, typename U>
		constexpr bool cmp_equal_same_sign(const T t, const U u) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			static_assert(std::is_integral<U>::value, "U needs to be an integral value");
			return (sizeof(T)>sizeof(U)) ? (t == static_cast<T>(u)) : (static_cast<U>(t) == u);
		}

		template <typename T, typename U>
		constexpr bool cmp_equal_signed_unsigned(const T t, const U u) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			static_assert(std::is_integral<U>::value, "U needs to be an integral value");
			return (t<T{ 0 }) ? false : (sizeof(T) / 2>sizeof(U)) ? (t == static_cast<T>(u)) : (static_cast<U>(t) == u);
		}

		// equivalent of operator== for different integral types
		template <typename T, typename U>
		constexpr bool cmp_less_same_sign(const T t, const U u) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			static_assert(std::is_integral<U>::value, "U needs to be an integral value");
			return (sizeof(T)>sizeof(U)) ? (t < static_cast<T>(u)) : (static_cast<U>(t) < u);
		}

		template <typename T, typename U>
		constexpr bool cmp_less_signed_unsigned(const T t, const U u) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			static_assert(std::is_integral<U>::value, "U needs to be an integral value");
			return (t<T{ 0 }) ? true : (sizeof(T) / 2>sizeof(U)) ? (t < static_cast<T>(u)) : (static_cast<U>(t) < u);
		}

		template <typename T, typename U>
		constexpr bool cmp_less_unsigned_signed(const T t, const U u) noexcept {
			static_assert(std::is_integral<T>::value, "T needs to be an integral value");
			static_assert(std::is_integral<U>::value, "U needs to be an integral value");
			return (u<U{ 0 }) ? false : (sizeof(U) / 2>sizeof(T)) ? (static_cast<U>(t) < u) : (t < static_cast<T>(u));
		}
    } // end details

    /// This function represents the signature function for integral values.
    /// This function returns a sign, i.e. if the value is greater, equal, or less zero.
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

	/// This function checks if calculating the absolute value (i.e. abs(a) ) will overflow
	template <typename T>
	constexpr bool isSafeAbs(const T a) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeAbs_unsigned(a) : details::isSafeAbs_signed(a);
	}

	/// This function checks if the addition two integral values (i.e. a+b ) will overflow
	template <typename T>
	constexpr bool isSafeAdd(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeAdd_unsigned(a, b) : details::isSafeAdd_signed(a, b);
	}

	/// This function checks if the subtraction of one integral values from another (i.e. a-b ) will overflow
	template <typename T>
	constexpr bool isSafeDiff(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeDiff_unsigned(a, b) : details::isSafeDiff_signed(a, b);
	}

	/// This function checks if calculating the remainder of two integral values (i.e. a%b ) will overflow, and if the arguments are valid (i.e. b != 0)
	template <typename T>
	constexpr bool isSafeMod(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeMod_unsigned(a, b) : details::isSafeMod_signed(a, b);
	}

	/// This function checks if the multiplication of two integral values (i.e. a*b ) will overflow
	template <typename T>
	constexpr bool isSafeMult(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeMult_unsigned(a,b) : details::isSafeMult_signed(a,b);
	}

	/// This function checks if the division between two integral values (i.e. a/b ) will overflow, and if the arguments are valid (i.e. b != 0)
	template <typename T>
	constexpr bool isSafeDiv(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeDiv_unsigned(a,b) : details::isSafeDiv_signed(a,b);
	}

	/// This function checks if the left shift between two integral values (i.e. a<<b ) will overflow, and if the arguments are valid (i.e. a>0 && b >0)
	template <typename T>
	constexpr bool isSafeLeftShift(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeLeftShift_unsigned(a,b) : details::isSafeLeftShift_signed(a,b);
	}

	/// This function checks if the arguments for the right shift operation between two integral values (i.e. a>>b ) are valid ( i.e. a>0 && b>0)
	template <typename T>
	constexpr bool isSafeRightShift(const T a, const T b) noexcept {
		static_assert(std::is_integral<T>::value, "T needs to be an integral value");
		return std::is_unsigned<T>::value ? details::isSafeRightShift_unsigned(a,b) : details::isSafeRightShift_signed(a,b);
	}

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
