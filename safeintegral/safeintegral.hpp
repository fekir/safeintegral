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
#ifndef SAFEMATH_SAFEMATH_H
#define SAFEMATH_SAFEMATH_H

#include "safeintegralop.hpp"

#include <limits>
#include <type_traits>
#include <stdexcept>

	/// This class rappresents an integral ot type T, that has no undefined behaviour. If an unsupported operation should
	// occur (like division by 0), or an overflow, an exception is throw.
	template<typename T, class = typename std::enable_if<std::is_integral<T>::value>::type>
	class safe_integral {
	private:
		T m;
	public:
		/// Returns the value rappresented by the class as integral
		constexpr T getvalue() const noexcept {
			return m;
		}

		/// Returns the value rappresented by the class as integral
		constexpr friend T getvalue(const safe_integral s) noexcept {
			return s.m;
		}

		/// Default constructor
		/// Wraps an integral ot type T
		/// The value is initialized to 0.
		/// You can disable this initialization defining the macro DONT_INIT_DEFAULT_CONSTRUCTOR_SAFEINTEGRAL
		/// It may(!) increase performance if creating vectors with many values
		/// Example Usage:
		/// @code
		/// 	safe_integral<int> i;;
		/// @endcode
		safe_integral():
		#if defined( DONT_INIT_DEFAULT_CONSTRUCTOR_SAFEINTEGRAL )
		#else
			m(T{0})
		#endif
		{};
		//template<typename U>
		//safe_integral(U) = delete; // we do not want implicit conversion --> but we need them for generig algorithm...

		/// Constructor
		/// Wraps an integral ot type T
		/// This constructor is not marked as explicit, simplifying the usage in generic code/algorithm functions
		/// Example Usage:
		/// @code
		/// 	safe_integral<int> i(5);
		/// @endcode
		constexpr safe_integral(T i) noexcept : m(i) { }

		/// Copy constructor
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5);
		/// @endcode
		constexpr safe_integral(const safe_integral &other) noexcept = default;

		/// Move constructor
		constexpr safe_integral(safe_integral &&other) noexcept = default;

		/// Destructor
		~safe_integral() = default;

		/// Assignment operators
		safe_integral &operator=(const safe_integral &arg) = default;

		/// Operator +=
		/// If the operation is not safe (in case of overlow), an exception is thrown
		/// @param rhs element to add to the current element
		/// @out       reference to current element
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5);
		/// 	i += 2;
		/// 	assert(i  == safe_integral<int>(7));
		/// @endcode
		safe_integral &operator+=(const safe_integral rhs) {
			if (!safeintegralop::isSafeAdd(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator+=");
			}
			this->m += rhs.m;
			return *this;
		}

		/// Operator -=
		/// If the operation is not safe (in case of overlow), an exception is thrown
		/// @param rhs element to substract to the current element
		/// @out       reference to current element
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5);
		/// 	i -= 2;
		/// 	assert(i  == safe_integral<int>(3));
		/// @endcode
		safe_integral &operator-=(const safe_integral rhs) {
			if (!safeintegralop::isSafeDiff(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator-=");
			}
			this->m -= rhs.m;
			return *this;
		}

		/// Operator *=
		/// If the operation is not safe (in case of overlow), an exception is thrown
		/// @param rhs element to multiply with the current element
		/// @out       reference to current element
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5);
		/// 	i *= 2;
		/// 	assert(i  == safe_integral<int>(10));
		/// @endcode
		safe_integral &operator*=(const safe_integral rhs) {
			if (!safeintegralop::isSafeMult(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator*=");
			}
			this->m *= rhs.m;
			return *this;
		}

		/// Operator /=
		/// If the operation is not safe (in case of overlow), an exception is thrown
		/// @param rhs element to divide by current element
		/// @out       reference to current element
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5);
		/// 	i /=2;
		/// 	assert(i  == safe_integral<int>(2));
		/// @endcode
		safe_integral &operator/=(const safe_integral rhs) {
			if (!safeintegralop::isSafeDiv(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator/=");
			}
			this->m /= rhs.m;
			return *this;
		}

		/// Operator %=
		/// If the operation is not safe (in case of overlow), an exception is thrown
		/// @param rhs element to perform modulus with the current element
		/// @out       reference to current element
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5);
		/// 	i %=2;
		/// 	assert(i  == safe_integral<int>(1));
		/// @endcode
		safe_integral &operator%=(const safe_integral rhs) {
			if (!safeintegralop::isSafeMod(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator%=");
			}
			this->m %= rhs.m;
			return *this;
		}

		/// Operator <<=
		/// If the operation is not safe (in case of overlow), an exception is thrown
		/// @param rhs element to shift with the current element
		/// @out       reference to current element
		safe_integral &operator<<=(const safe_integral &rhs) {
			if (!safeintegralop::isSafeLeftShift(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator<<=");
			}
			this->m %= rhs.m;
			return *this;
		}
		
		/// Operator >>=
		/// If the operation is not safe (in case of overlow), an exception is thrown
		/// @param rhs element to shift with the current element
		/// @out       reference to current element
		safe_integral &operator>>=(const safe_integral &rhs) noexcept {
			this->m >>= rhs.m;
			return *this;
		}

		/// Operator ++ (preincrement)
		/// If the operation is not safe (in case of overlow), an exception is thrown
		/// @out       the current element, bigger by one unit
		safe_integral &operator++() {
			if (!safeintegralop::isSafeAdd(this->m, T{1})) {
				throw std::out_of_range("overflow with operator++()");
			}
			++this->m;
			return *this;
		}

		/// Operator ++ (postincrement)
		/// If the operation is not safe (in case of overlow), an exception is thrown
		/// @out       the current element, bigger by one unit
		safe_integral operator++(int) {
			if (!safeintegralop::isSafeAdd(this->m, T{1})) {
				throw std::out_of_range("overflow with operator++(int)");
			}
			safe_integral tmp(*this); // copy
			operator++(); // pre-increment
			return tmp;   // return old value
		}
		
		/// Operator -- (predecrement)
		/// If the operation is not safe (in case of overlow), an exception is thrown
		/// @out       the current element, lesser by one unit
		safe_integral operator--() {
			if (!safeintegralop::isSafeDiff(this->m, T{1})) {
				throw std::out_of_range("overflow with operator--()");
			}
			--this->m;
			return *this;
		}

		/// Operator -- (postdecrement)
		/// If the operation is not safe (in case of overlow), an exception is thrown
		/// @out       the current element, lesser by one unit
		safe_integral operator--(int) {
			if (!safeintegralop::isSafeDiff(this->m, T{1})) {
				throw std::out_of_range("overflow with operator--(int)");
			}
			safe_integral tmp(*this); // copy
			operator--(); // pre-decrement
			return tmp;   // return old value
		}

		/// Operator +
		/// This operation is always safe, since it is a noop
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5);
		///		i = +i;
		///		assert(i == safe_integral<int>(5));
		/// @endcode
		constexpr safe_integral operator+() const noexcept { return *this; }
		
		/// Operator +
		/// If the operation is not safe (in case of overlow), an exception is thrown
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5);
		///		i = -i;
		///		assert(i == safe_integral<int>(-5));
		/// @endcode
		constexpr safe_integral operator-() const {
			return
				safeintegralop::isSafeDiff(0, this->m) ? safe_integral(-this->m) :
				throw std::out_of_range("overflow with unary operator-");
		}

		/// Operator +
		/// If the operation is not safe (in case of overlow), an exception is thrown
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5) + safe_integral<int>(5);
		///		assert(i == safe_integral<int>(10));
		/// @endcode
		constexpr friend safe_integral operator+(safe_integral lhs, const safe_integral &rhs) {
			return
				safeintegralop::isSafeAdd(lhs.m, rhs.m) ? safe_integral(lhs.m + rhs.m) :
				throw std::out_of_range("overflow with operator+");
		}

		/// Operator -
		/// If the operation is not safe (in case of overlow), an exception is thrown
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5) - safe_integral<int>(5);
		///		assert(i == safe_integral<int>(0));
		/// @endcode
		constexpr friend safe_integral operator-(safe_integral lhs, const safe_integral &rhs) {
			return
				safeintegralop::isSafeDiff(lhs.m, rhs.m) ? safe_integral(lhs.m - rhs.m) :
				throw std::out_of_range("overflow with operator-");
		}

		/// Operator /
		/// If the operation is not safe (in case of overlow), an exception is thrown
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5) / safe_integral<int>(5);
		///		assert(i == safe_integral<int>(1));
		/// @endcode
		constexpr friend safe_integral operator/(safe_integral lhs, const safe_integral &rhs) {
			return
				safeintegralop::isSafeDiv(lhs.m, rhs.m) ? safe_integral(lhs.m / rhs.m) :
				throw std::out_of_range("overflow with operator/");
		}

		/// Operator *
		/// If the operation is not safe (in case of overlow), an exception is thrown
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5) * safe_integral<int>(5);
		///		assert(i == safe_integral<int>(25));
		/// @endcode
		constexpr friend safe_integral operator*(safe_integral lhs, const safe_integral &rhs) {
			return safeintegralop::isSafeMult(lhs.m, rhs.m) ? safe_integral(lhs.m * rhs.m) :
					 throw std::out_of_range("overflow with operator*");
		}

		/// Operator %
		/// If the operation is not safe (in case of overlow), an exception is thrown
		///
		/// Example Usage:
		/// @code
		/// 	auto i = safe_integral<int>(5) % safe_integral<int>(5);
		///		assert(i == safe_integral<int>(0));
		/// @endcode
		constexpr friend safe_integral operator%(safe_integral lhs, const safe_integral &rhs) {
			return safeintegralop::isSafeMod(lhs.m, rhs.m) ? safe_integral(lhs.m % rhs.m) :
					 throw std::out_of_range("overflow with operator%");
		}

		constexpr safe_integral operator~() const noexcept {
			return safe_integral(~(this->m));
		}

		constexpr friend safe_integral operator&(safe_integral lhs, const safe_integral &rhs) noexcept {
			return safe_integral(lhs.m & rhs.m);
		}

		constexpr friend safe_integral operator|(safe_integral lhs, const safe_integral &rhs) noexcept {
			return safe_integral(lhs.m | rhs.m);
		}

		constexpr friend safe_integral operator^(safe_integral lhs, const safe_integral &rhs) noexcept {
			return safe_integral(lhs.m ^ rhs.m);
		}

		constexpr friend safe_integral operator<<(safe_integral lhs, const safe_integral &rhs) {
			return
				safeintegralop::isSafeLeftShift(lhs.m, rhs.m) ? safe_integral(lhs.m << rhs.m) :
				throw std::out_of_range("overflow with operator<<");
		}

		constexpr friend safe_integral operator>>(safe_integral lhs, const safe_integral &rhs) {
			return
				safeintegralop::isSafeRightShift(lhs.m, rhs.m) ? safe_integral(lhs.m >> rhs.m) :
				throw std::out_of_range("overflow with operator>>");
		}

		constexpr friend bool operator<(const safe_integral &lhs, const safe_integral &rhs) noexcept {
			return (lhs.m < rhs.m);
		}

		constexpr friend bool operator>(const safe_integral &lhs, const safe_integral &rhs) noexcept {
			return rhs < lhs;
		}

		constexpr friend bool operator<=(const safe_integral &lhs, const safe_integral &rhs) noexcept {
			return !(lhs > rhs);
		}

		constexpr friend bool operator>=(const safe_integral &lhs, const safe_integral &rhs) noexcept {
			return !(lhs < rhs);
		}

		constexpr friend bool operator==(const safe_integral &lhs, const safe_integral &rhs) noexcept {
			return lhs.m == rhs.m;
		}

		constexpr friend bool operator!=(const safe_integral &lhs, const safe_integral &rhs) noexcept {
			return !(lhs == rhs);
		}

		// print operators
		friend std::ostream &operator<<(std::ostream &os, const safe_integral<T> &value) {
			os << value.m;
			return os;
		}
	};

	template<typename T, class = typename std::enable_if<std::is_integral<T>::value>::type>
	constexpr safe_integral<T> make_safe(T i) {
		return safe_integral<T>(i);
	}


	inline void compile_self_test(){
		auto one = safe_integral<int>(1);
		auto two = safe_integral<int>(2);
		static_assert( noexcept(std::swap(one, two)) , "throwing swap" );
	}

	using safe_short     = safe_integral<short>;
	using safe_int       = safe_integral<int>;
	using safe_long      = safe_integral<long>;
	using safe_longlong  = safe_integral<long long>;

	using safe_ushort    = safe_integral<unsigned short>;
	using safe_uint      = safe_integral<unsigned int>;
	using safe_ulong     = safe_integral<unsigned long>;
	using safe_ulonglong = safe_integral<unsigned long long>;

	static_assert(sizeof(safe_short)     == sizeof(short),              "size are not the same");
	static_assert(sizeof(safe_int)       == sizeof(int),                "size are not the same");
	static_assert(sizeof(safe_ulonglong) == sizeof(unsigned long long), "size are not the same");

#endif //SAFEMATH_SAFEMATH_H
