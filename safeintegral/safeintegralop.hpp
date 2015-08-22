#ifndef SAFEOPERATIONS_H
#define SAFEOPERATIONS_H

/**
 * From www.securecoding.cert.org
 *
 * Signed integer overflow is undefined behavior.
 * Unsigned integer overflow is defined as wrapping.
 *
 * In both cases, most of the time, software is not written in a way to handle
 * overflows or wrapping correctly (for example in pointer arithmetic, expression
 * in square brackets [], ...), and most of the times checks are not implemented correctly.
 *
 * The functions defined in safeintegralop permits to detects if it is save fo perform
 * a certain operation, like an addition, shift or subtraction, applying precondition tests.
 *
 * These function are templated to adapt themselves with all signed and unsigned integral types
 * (char, short, int, long, long long, and any other type implemented by the compiler).
 *
 * All functions are also marked as noexcept and constexpr.
 *
 */


#include <limits>
#include <type_traits>


namespace safeintegralop {
	enum class sign { negative = -1, zero = 0, positive = 1 };

	template <class T, class = typename std::enable_if<std::is_integral<T>::value>::type> constexpr
	unsigned int precision();

	namespace details{

		// Implementation for signum
		template <typename T> constexpr
		sign signum_unsigned(const T x) noexcept {
			return x > T{0} ? sign::positive : sign::zero;
		}

		template <typename T> constexpr
		sign signum_signed(const T x) noexcept {
			return
				x > T{0} ? sign::positive :
				x < T{0} ? sign::negative :
				sign::zero; // x==T{0}
		}

		template <class T>
		constexpr unsigned int pop(const unsigned int precision, const T num) {
			return (num == T{0}) ? precision : pop(((num % 2 != 0) ? precision+1 : precision), num >> 1);
		}

		template <typename T> constexpr
		bool isSafeAbs_unsigned(const T) noexcept {
			return true;
		}

		template <typename T> constexpr
		bool isSafeAbs_signed(const T a) noexcept {
			return (a != std::numeric_limits<T>::min()); // ovvero a>min
		}

		template <typename T> constexpr
		bool isSafeAdd_unsigned(const T a, const T b) noexcept {
			return (b == T{0}) || (a <= std::numeric_limits<T>::max() - b);
		}

		template <typename T> constexpr
		bool isSafeAdd_signed(const T a, const T b) noexcept {
			return
				(b == T{0}) ? true :
				(b > T{0}) ? (a <= std::numeric_limits<T>::max() - b) :
				(a >= std::numeric_limits<T>::min() - b); // b<0
		}

		template <typename T> constexpr
		bool isSafeDiff_unsigned(const T a, const T b) noexcept {
			return a >= b;
		}

		template <typename T>constexpr
		bool isSafeDiff_signed(const T a, const T b) noexcept {
			return
				(b == T{0}) ? true :
				(b > T{0}) ? (a >= std::numeric_limits<T>::min() + b) :
				(a <= std::numeric_limits<T>::max() + b); // b<0
		}

		template <typename T> constexpr
		bool isSafeMod_unsigned(const T, const T b) noexcept {
			return b!=T{0};
		}

		template <typename T> constexpr
		bool isSafeMod_signed(const T a, const T b) noexcept {
			return (b == T{-1}) ? (a != std::numeric_limits<T>::min()) :  (b != T{0});
		}

		template <typename T> constexpr
		bool isSafeMult_unsigned(const T a, const T b) noexcept {
			return
				(a==T{0} || b== T{0} || b==T{1} || a == T{1}) ? true : // pto fisso
				(b < T{1}) ? true : // contrazione
				(a <= std::numeric_limits<T>::max() / b); // b>1, espansione
		}

		template <typename T> constexpr
		bool isSafeMult_signed(const T a, const T b) noexcept {
			return
				(a==T{0} || b== T{0} || b==T{1} || a == T{1}) ? true : // pto fisso
				(b==T{-1}) ? a != std::numeric_limits<T>::min() : // a/-1 == a*-1 --> overflow if a == minvalue
				(b>T{-1} && b<T{1}) ? true : // contrazione
				( (a <= std::numeric_limits<T>::max() / b) && (a >= std::numeric_limits<T>::min() / b)); // |b| >1, espansione
		}

		template <typename T> constexpr
		bool isSafeDiv_unsigned(const T a, const T b) noexcept {
			return
				(a == T{0} || b == T{1}) ? true : // pto fisso
				(b == T{0}) ? false :
				(b>T{1}) ? true :   // contrazione
				(a < std::numeric_limits<T>::max() * b); // b<1, espansione
		}

		template <typename T> constexpr
		bool isSafeDiv_signed(const T a, const T b) noexcept {
			return
				(a == T{0} || b == T{1}) ? true : // pto fisso
				(b == T{0}) ? false :
				(b == T{-1}) ? (a != std::numeric_limits<T>::min()) :
				(b>T{1} || b<T{-1}) ? true :   // contrazione
				( (a < std::numeric_limits<T>::max() * b) && (a > std::numeric_limits<T>::min() * b));// |b|<1 && b!=0, espansione
		}


		template <typename T> constexpr
		bool isSafeLeftShift_unsigned(const T a, const T b) noexcept {
			return !( b >= precision<T>() || (a > (std::numeric_limits<T>::max() >> b)) );
		}

		template <typename T> constexpr
		bool isSafeLeftShift_signed(const T a, const T b) noexcept {
			return !( (a < T{0}) || (b < T{0}) || (b >= precision<T>()) || (a > (std::numeric_limits<T>::max() >> b)));
		}

		template <typename T> constexpr
		bool isSafeRightShift_unsigned(const T, const T) noexcept {
			return true;
		}

		template <typename T> constexpr
		bool isSafeRightShift_signed(const T, const T b) noexcept {
			return (b >= T{0});
		}
	} // end details

	template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type> constexpr
	sign signum(const T x) noexcept {
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
	template <class T, class = typename std::enable_if<std::is_integral<T>::value>::type> constexpr
	unsigned int precision(){
		return details::pop(0, std::numeric_limits<T>::max());
	}


	template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type> constexpr
	bool isSafeAbs(const T a) noexcept {
		return std::is_unsigned<T>::value ? details::isSafeAbs_unsigned(a) : details::isSafeAbs_signed(a);
	}

	/**
	 * Checks if it is safe do add two numbers, ie if the operation a+b does not overflow/underflow
	 */
	template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type> constexpr
	bool isSafeAdd(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::isSafeAdd_unsigned(a, b) : details::isSafeAdd_signed(a, b);
	}

	template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type> constexpr
	bool isSafeDiff(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::isSafeDiff_unsigned(a, b) : details::isSafeDiff_signed(a, b);
	}

	template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type> constexpr
	bool isSafeMod(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::isSafeMod_unsigned(a, b) : details::isSafeMod_signed(a, b);
	}

	template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type> constexpr
	bool isSafeMult(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::isSafeMult_unsigned(a,b) : details::isSafeMult_signed(a,b);
	}

	template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type> constexpr
	bool isSafeDiv(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::isSafeDiv_unsigned(a,b) : details::isSafeDiv_signed(a,b);
	}


	template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type> constexpr
	bool isSafeLeftShift(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::isSafeLeftShift_unsigned(a,b) : details::isSafeLeftShift_signed(a,b);
	}

	// Right-shift operations do not overflow, but the result of a right-shift
	// of a signed negative number is implementation-dependent
	template <typename T, class = typename std::enable_if<std::is_integral<T>::value>::type> constexpr
	bool isSafeRightShift(const T a, const T b) noexcept {
		return std::is_unsigned<T>::value ? details::isSafeRightShift_unsigned(a,b) : details::isSafeRightShift_signed(a,b);
	}
}

#endif // SAFEOPERATIONS_H
