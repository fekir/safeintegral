
#ifndef SAFEMATH_SAFEMATH_H
#define SAFEMATH_SAFEMATH_H

#include "safeintegralop.hpp"

#include <limits>
#include <type_traits>
#include <stdexcept>

	template<typename T, class = typename std::enable_if<std::is_integral<T>::value>::type>
	class safe_integral {
	private:
		T m;
	public:
		constexpr T getvalue() noexcept {
			return m;
		}

		constexpr friend T getvalue(const safe_integral s) noexcept {
			return s.m;
		}

		///////////////////////////////////////
		// Constructor/Destructor
		safe_integral() = delete; // we do not want uninit variables!
		template<typename U>
		//safe_integral(U) = delete; // we do not want implicit conversion --> need them for generig algorithm...
		constexpr safe_integral(T i) noexcept : m(i) { }
		constexpr safe_integral(const T& i) noexcept : m(i) { }
		constexpr safe_integral(const safe_integral &other) noexcept = default;

		constexpr safe_integral(safe_integral &&other) noexcept = default;

		~safe_integral() = default;

		///////////////////////////////////////
		// Assignment operators
		safe_integral &operator=(const safe_integral &arg) = default;

		safe_integral &operator+=(const safe_integral rhs) {
			if (!safeintegralop::isSafeAdd(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator+=");
			}
			this->m += rhs.m;
			return *this;
		}

		safe_integral &operator-=(const safe_integral rhs) {
			if (!safeintegralop::isSafeDiff(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator-=");
			}
			this->m -= rhs.m;
			return *this;
		}

		safe_integral &operator*=(const safe_integral rhs) {
			if (!safeintegralop::isSafeMult(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator*=");
			}
			this->m *= rhs.m;
			return *this;
		}

		safe_integral &operator/=(const safe_integral rhs) {
			if (!safeintegralop::isSafeDiv(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator/=");
			}
			this->m /= rhs.m;
			return *this;
		}

		safe_integral &operator%=(const safe_integral rhs) {
			if (!safeintegralop::isSafeMod(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator%=");
			}
			this->m %= rhs.m;
			return *this;
		}

		safe_integral &operator<<=(const safe_integral &rhs) {
			if (!safeintegralop::isSafeLeftShift(this->m, rhs.m)) {
				throw std::out_of_range("overflow with operator<<=");
			}
			this->m %= rhs.m;
			return *this;
		}

		safe_integral &operator>>=(const safe_integral &rhs) noexcept {
			this->m >>= rhs.m;
			return *this;
		}

		///////////////////////////////////////
		// Increment and decrement operators
		safe_integral &operator++() { // preincrement
			if (!safeintegralop::isSafeAdd(this->m, T{1})) {
				throw std::out_of_range("overflow with operator++()");
			}
			++this->m;
			return *this;
		}

		safe_integral operator++(int) { // postincrement
			if (!safeintegralop::isSafeAdd(this->m, T{1})) {
				throw std::out_of_range("overflow with operator++(int)");
			}
			safe_integral tmp(*this); // copy
			operator++(); // pre-increment
			return tmp;   // return old value
		}

		safe_integral operator--() { // predecrement
			if (!safeintegralop::isSafeDiff(this->m, T{1})) {
				throw std::out_of_range("overflow with operator--()");
			}
			--this->m;
			return *this;
		}

		safe_integral operator--(int) { // postdecrement
			if (!safeintegralop::isSafeDiff(this->m, T{1})) {
				throw std::out_of_range("overflow with operator--(int)");
			}
			safe_integral tmp(*this); // copy
			operator--(); // pre-decrement
			return tmp;   // return old value
		}

		///////////////////////////////////////
		// Arithmetic operators
		constexpr safe_integral operator+() const noexcept { return *this; }

		constexpr safe_integral operator-() const {
			return
				safeintegralop::isSafeDiff(0, this->m) ? safe_integral(-this->m) :
				throw std::out_of_range("overflow with unary operator-");
		}

		constexpr friend safe_integral operator+(safe_integral lhs, const safe_integral &rhs) {
			return
				safeintegralop::isSafeAdd(lhs.m, rhs.m) ? safe_integral(lhs.m + rhs.m) :
				throw std::out_of_range("overflow with operator+");
		}

		constexpr friend safe_integral operator-(safe_integral lhs, const safe_integral &rhs) {
			return
				safeintegralop::isSafeDiff(lhs.m, rhs.m) ? safe_integral(lhs.m - rhs.m) :
				throw std::out_of_range("overflow with operator-");
		}

		constexpr friend safe_integral operator/(safe_integral lhs, const safe_integral &rhs) {
			return
				safeintegralop::isSafeDiv(lhs.m, rhs.m) ? safe_integral(lhs.m / rhs.m) :
				throw std::out_of_range("overflow with operator/");
		}

		constexpr friend safe_integral operator*(safe_integral lhs, const safe_integral &rhs) {
			return safeintegralop::isSafeMult(lhs.m, rhs.m) ? safe_integral(lhs.m * rhs.m) :
					 throw std::out_of_range("overflow with operator*");
		}

		constexpr friend safe_integral operator%(safe_integral lhs, const safe_integral &rhs) {
			return safeintegralop::isSafeMod(lhs.m, rhs.m) ? safe_integral(lhs.m % rhs.m) :
					 throw std::out_of_range("overflow with operator%");
		}

		///////////////////////////////////////
		// Bitwise operators
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

		///////////////////////////////////////
		// Relational operators
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

	static_assert(sizeof(safe_int) == sizeof(int), "size are not the same");


#endif //SAFEMATH_SAFEMATH_H
