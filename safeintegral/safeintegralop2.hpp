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
#ifndef SAFEOPERATIONS2_HPP
#define SAFEOPERATIONS2_HPP

#include "errors.hpp"
#include "safeintegralop_cmp.hpp"


#include <limits>
#include <type_traits>
#include <cstdint>

#include <optional>

namespace safeintegralop {


	// All functions in the namespace "details" are for private use
	namespace details{
		template <typename T0>
		constexpr auto safe_abs(const T0 v) -> typename std::make_unsigned<T0>::type {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T0);
			using T0_u = typename std::make_unsigned<T0>::type;
			return v>=T0{0} ? static_cast<T0_u>(v) : static_cast<T0_u>(-(v+1))+1;
		}

		// safe_add -----------------------------
		template <typename T0,  typename T1, typename T2>
		constexpr std::optional<T0> safe_add_uu(const T1 a, const T2 b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T0,T1,T2);
			using T0_s = typename std::make_unsigned<T0>::type;
			using Tu = typename std::common_type<typename std::make_unsigned<T0>::type, T1, T2>::type;
			return (Tu(a) <= std::numeric_limits<Tu>::max() - Tu(b)) && (Tu(a) + Tu(b) <= T0_s{std::numeric_limits<T0>::max()}) ? T0(Tu(a)+Tu(b)) : std::optional<T0>{};
		}

		template <typename T0,  typename T1, typename T2>
		constexpr std::optional<T0> safe_add_ss(const T1 a, const T2 b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T0,T1,T2);
			using Ts = typename std::common_type<typename std::make_signed<T0>::type, T1, T2>::type;
			using Tu = typename std::common_type<typename std::make_unsigned<T0>::type, typename std::make_unsigned<T1>::type, typename std::make_unsigned<T2>::type>::type;
			// notice that max<T0> could be > max<Ts>, therefore we need
			// safe_add_uu in case T(a)+T(b) is too big for T, but not for T0
			return
			  b >= T2{0} ?
			    ( a > T1{0} ? safe_add_uu<T0>(Tu(a),Tu(b)) :
			                  ((Ts(a) <= std::numeric_limits<Ts>::max() - Ts(b)) && safeintegralop::in_range<T0>(Ts(a) + Ts(b))) ? T0(Ts(a) + Ts(b)) : std::optional<T0>{}
			    ) :
			    (Ts(a) >= std::numeric_limits<Ts>::min() - Ts(b) && safeintegralop::in_range<T0>(Ts(a) + Ts(b)) ? T0(Ts(a) + Ts(b)) : std::optional<T0>{} );
	}

		template <typename T0,  typename T1, typename T2>
		constexpr std::optional<T0> safe_add_su(const T1 a, const T2 b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T0,T1,T2);
			using T1_u = typename std::make_unsigned<T1>::type;
			using Ts = typename std::common_type<typename std::make_signed<T0>::type, T1, typename std::make_signed<T2>::type>::type;

			return
			  a>=T1{0} ? safe_add_uu<T0>(T1_u(a),b) :
			  safeintegralop::in_range<Ts>(b) ? safe_add_ss<T0>(a, Ts(b)) :
			  // b > max(T1) (-> b>=|min(T1)|) => b-|a| is alway ok in T2, doing a+1 to avoid possible overflow for -a, safe since a <=-1
			  (safeintegralop::in_range<T0>(b - (T2(-(a+1))+1)) ? T0(b - (T2(-(a+1))+1)) : std::optional<T0>{});
		}

	} // end details

	/// Usage:
	///  size_t i == ...
	///  short j = ...
	///  auto res = safe_add<int>(i,j); // performs i+j without causing overflows and saves the result in an int. If the result cannot be represented, it returns an empty std::optional<int>
	template <typename T0,  typename T1, typename T2>
	constexpr std::optional<T0> safe_add(const T1 a, const T2 b) noexcept {
		SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T0,T1,T2);
		return
		  (std::is_signed<T1>::value && std::is_signed<T2>::value) ? details::safe_add_ss<T0>(a,b) :
		  (std::is_unsigned<T1>::value && std::is_unsigned<T2>::value) ? details::safe_add_uu<T0>(a,b) :
		  std::is_signed<T1>::value ? details::safe_add_su<T0>(a, b) : details::safe_add_su<T0>(b,a);
	}

	// All functions in the namespace "details" are for private use
	namespace details{
		// safe_diff -----------------------------
		template <typename T0,  typename T1, typename T2>
		constexpr std::optional<T0> safe_diff_ss(const T1 a, const T2 b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T0,T1,T2);
			return b > T2{0} ? safe_add<T0>(a,-b) : safe_add<T0>(a, safe_abs(b));
		}

		template <typename T0,  typename T1, typename T2>
		constexpr std::optional<T0> safe_diff_uu(const T1 a, const T2 b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T0,T1,T2);
			using Tu = typename std::common_type<typename std::make_unsigned<T0>::type, T1, T2>::type;
			return
			  Tu(a)>=Tu(b) ? (safeintegralop::in_range<T0>(Tu(a) - Tu(b)) ? T0(Tu(a)-Tu(b)) : std::optional<T0>{} ) :
			  // with a=0, b = |min| or similar combinations will overflow, since b>a, b-1 is safe
			  (safeintegralop::cmp_less_eq(Tu(b) - Tu(a), safe_abs(std::numeric_limits<T0>::min())) ? -T0(Tu(b-1) - Tu(a))-1 : std::optional<T0>{});
		}

		template <typename T0,  typename T1, typename T2>
		constexpr std::optional<T0> safe_diff_su(const T1 a, const T2 b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T0,T1,T2);
			using T1_u = typename std::make_unsigned<T1>::type;
			using Tu = typename std::common_type<typename std::make_unsigned<T0>::type, typename std::make_unsigned<T1>::type, T2>::type;
			return
			  a >= T1{0} ? safe_diff_uu<T0>(T1_u(a),b) :
			  // a-b == -(b+(-a))
			  (safe_add<Tu>(Tu(b), safe_abs(a)).has_value() && safeintegralop::cmp_less_eq(Tu(b) + safe_abs(a), safe_abs(std::numeric_limits<T0>::min()))) ? -T0(Tu(b) + safe_abs(a)) : std::optional<T0>{};
		}

		template <typename T0,  typename T1, typename T2>
		constexpr std::optional<T0> safe_diff_us(const T1 a, const T2 b) noexcept {
			SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T0,T1,T2);
			using T2_u = typename std::make_unsigned<T2>::type;
			return b >= T2{0} ? safe_diff_uu<T0>(a,T2_u(b)) : safe_add<T0>(a, safe_abs(b));
		}
	} // end details

	/// Usage:
	///  int i == ...
	///  size_t j = ...
	///  auto res = safe_diff<short>(i,j); // performs i-j without causing overflows and saves the result in an short. If the result cannot be represented, it returns an empty std::optional<short>
	template <typename T0,  typename T1, typename T2>
	constexpr std::optional<T0> safe_diff(const T1 a, const T2 b) noexcept {
		SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T0,T1,T2);
		return
		  (std::is_signed<T1>::value && std::is_signed<T2>::value) ? details::safe_diff_ss<T0>(a,b) :
		  (std::is_unsigned<T1>::value && std::is_unsigned<T2>::value) ? details::safe_diff_uu<T0>(a,b) :
		  std::is_signed<T1>::value ? details::safe_diff_su<T0>(a, b) : details::safe_diff_us<T0>(a,b);
	}

	/// Usage:
	///  int i == ...
	///  size_t j = ...
	///  auto res = safe_mult<short>(i,j); // performs i*j without causing overflows and saves the result in an short. If the result cannot be represented, it returns an empty std::optional<short>
	template <typename T0,  typename T1, typename T2>
	constexpr std::optional<T0> safe_mult(const T1 a, const T2 b) noexcept {
		SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T0,T1,T2);
		using T0_s = typename std::make_unsigned<T0>::type;
		return
		  (b == T2{0}) ? T0{0} : // leaving the test "a == 0" out seems to generate less assembly, do not know if it is more efficient
		  // max >= ab > 0 <-> |max| >= |a||b| > 0
		  ((a > T1{0}) == (b > T2{0})) ?  (T0_s(std::numeric_limits<T0>::max()) / details::safe_abs(b) >= details::safe_abs(a) ? T0(details::safe_abs(a) * details::safe_abs(b)) : std::optional<T0>{} ) :
		  // min <= ab < 0 <-> |min| >= |a||b| >0
		  (details::safe_abs(std::numeric_limits<T0>::min()) / details::safe_abs(b) >= details::safe_abs(a) ? -T0(details::safe_abs(a) * details::safe_abs(b)) : std::optional<T0>{} );
	}

	/// Usage:
	///  int i == ...
	///  size_t j = ...
	///  auto res = safe_div<short>(i,j); // performs i/j without causing overflows and saves the result in an short. If the result cannot be represented, it returns an empty std::optional<short>
	template <typename T0,  typename T1, typename T2>
	constexpr std::optional<T0> safe_div(const T1 a, const T2 b) noexcept {
		SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T0,T1,T2);
		using T0_s = typename std::make_unsigned<T0>::type;
		return
		  (b == T2{0}) ? std::optional<T0>{} :
		  ((a > T1{0}) == (b > T2{0})) ? (details::safe_abs(a)/details::safe_abs(b) <= T0_s{std::numeric_limits<T0>::max()} ? T0(details::safe_abs(a)/details::safe_abs(b)) : std::optional<T0>{}):
		  details::safe_abs(a)/details::safe_abs(b) == 0 ? T0{0} :
		  // if a/b == min it will overflow, T(|a/b|-1) is safe since |a/b| > 0
		  (details::safe_abs(a)/details::safe_abs(b) <= details::safe_abs(std::numeric_limits<T0>::min()) ? -T0(details::safe_abs(a)/details::safe_abs(b)-1)+1 : std::optional<T0>{});
	}

	namespace ct {
		// constants for testing
		constexpr std::uint64_t max64u = std::numeric_limits<std::uint64_t>::max();
		constexpr std::uint64_t min64u = std::numeric_limits<std::uint64_t>::min();
		constexpr std::uint32_t max32u = std::numeric_limits<std::uint32_t>::max();
		constexpr std::uint32_t max32u_1 = std::numeric_limits<std::uint32_t>::max()-1;
		constexpr std::uint32_t min32u = std::numeric_limits<std::uint32_t>::min();
		constexpr std::uint16_t max16u = std::numeric_limits<std::uint16_t>::max();
		constexpr std::uint16_t min16u = std::numeric_limits<std::uint16_t>::min();
		constexpr std::uint8_t max08u = std::numeric_limits<std::uint8_t>::max();
		constexpr std::uint8_t max08u_1 = std::numeric_limits<std::uint8_t>::max()-1;
		constexpr std::uint8_t min08u = std::numeric_limits<std::uint8_t>::min();

		constexpr std::int64_t max64s = std::numeric_limits<std::int64_t>::max();
		constexpr std::int64_t min64s = std::numeric_limits<std::int64_t>::min();
		constexpr std::int32_t max32s = std::numeric_limits<std::int32_t>::max();
		constexpr std::int32_t min32s = std::numeric_limits<std::int32_t>::min();
		constexpr std::int32_t max32s_1 = std::numeric_limits<std::int32_t>::max()-1;
		constexpr std::int16_t max16s = std::numeric_limits<std::int16_t>::max();
		constexpr std::int16_t max16s_1 = std::numeric_limits<std::int16_t>::max()-1;
		constexpr std::int16_t min16s = std::numeric_limits<std::int16_t>::min();
		constexpr std::int8_t max08s = std::numeric_limits<std::int8_t>::max();
		constexpr std::int8_t max08s_1 = std::numeric_limits<std::int8_t>::max()-1;
		constexpr std::int8_t min08s = std::numeric_limits<std::int8_t>::min();
		constexpr std::int8_t min08s_1 = std::numeric_limits<std::int8_t>::min()+1;

		// safe_add -------
		static_assert(safe_add<std::uint32_t>(std::uint32_t(1), std::uint32_t(1)) == 2, "dumb test");
		static_assert(!safe_add<std::uint32_t>(max32u, std::uint32_t(1)), "overflow");
		static_assert(safe_add<std::uint32_t>(max08u_1, std::uint32_t(1)) == max08u, "exact max");
		static_assert(safe_add<std::uint64_t>(max32u, max32u)==2*std::uint64_t(max32u), "in range after overflow: max32u+max32u < max64u");
		static_assert(!safe_add<std::uint32_t>(max32u, max32u), "!in range after overflow: max32u+max32u > max32u");

		static_assert(safe_add<std::int32_t>(1, 1) == 2, "dumb test");
		static_assert(!safe_add<std::int32_t>(max32s, 1), "overflow");
		static_assert(safe_add<std::int32_t>(max32s_1, 1) == max32s, "exact max");
		static_assert(safe_add<std::int32_t>(std::int64_t(max32s)+1, -2) == std::int64_t(max32s)-1, "in range after overflow: (max32s+1)+ (-2) < max32u");
		static_assert(!safe_add<std::int32_t>(std::int64_t(max32s)+2, -1), "!in range after overflow: (max32s+2) + (-1) > max32s");

		static_assert(safe_add<std::uint32_t>(1, 1) == 2, "dumb test");
		static_assert(!safe_add<std::uint32_t>(max64s, 1), "overflow");
		static_assert(safe_add<std::uint32_t>(2*std::int64_t(max32u), -std::int64_t(max32u)) == max32u, "exact max");
		static_assert(safe_add<std::uint32_t>(std::int64_t(max32u)+1, -2) == std::int64_t(max32u)-1, "in range after overflow");
		static_assert(safe_add<std::uint32_t>(-1, +2) == 1, "in range after overflow");
		static_assert(safe_add<std::uint32_t>(max32s, std::int32_t(1)) == std::uint32_t(max32s)+1, "in range after overflow");

		static_assert(safe_add<std::uint32_t>(1, 1u) == 2, "dumb test");
		static_assert(!safe_add<std::uint32_t>(max64s, 1u), "overflow");
		static_assert(safe_add<std::uint32_t>(2*std::uint64_t(max32s), -std::int64_t(max32s)) == max32s, "exact max");
		static_assert(safe_add<std::uint32_t>(std::uint64_t(max32s)+1, -2) == max32s_1, "in range after overflow");
		static_assert(safe_add<std::uint32_t>(-1, 2u) == 1, "in range after overflow");
		static_assert(safe_add<std::uint32_t>(min32s, std::uint64_t(std::uint64_t(max32s)+1)) == 0, "");

		// safe_diff
		static_assert(safe_diff<std::uint32_t>(std::int32_t(1), std::int32_t(1)) == 0, "dumb test");
		static_assert(safe_diff<std::uint32_t>(std::int32_t(2), std::int32_t(1)) == 1, "dumb test");
		static_assert(!safe_diff<std::uint32_t>(std::int32_t(0), std::int32_t(1)), "overflow");
		static_assert(safe_diff<std::int32_t>(std::int32_t(0), std::int32_t(1)) == -1, "dumb test");

		static_assert(!safe_diff<std::int32_t>(std::int32_t(0), min32s), "overflow: 0 - min08s == 128 > max08s");
		static_assert(safe_diff<std::uint32_t>(std::int32_t(0), min32s) == details::safe_abs(min32s), "in range after overflow: 0 - min08s == 128 < max08u");


		static_assert(safe_diff<std::uint32_t>(std::uint32_t(1), std::uint32_t(1)) == 0, "dumb");
		static_assert(safe_diff<std::uint32_t>(std::uint32_t(2), std::uint32_t(1)) == 1, "dumb");
		static_assert(!safe_diff<std::uint32_t>(std::uint32_t(0), std::uint32_t(1)), "overflow: 0 - 1 = -1 < 0");
		static_assert(safe_diff<std::int32_t>(std::uint32_t(0), std::uint32_t(1)) == -1, "no overflow");
		static_assert(safe_diff<std::int32_t>(std::uint32_t(1), std::uint32_t(2)) == -1, "no overflow");


		static_assert(safe_diff<std::uint32_t>(std::int32_t(2), std::uint32_t(1)) == 1, "dumb test");
		static_assert(safe_diff<std::int32_t>(std::int32_t(2), std::uint32_t(1)) == 1, "dumb test");
		static_assert(!safe_diff<std::uint32_t>(std::int32_t(0), std::uint32_t(1)), "overflow");
		static_assert(safe_diff<std::int32_t>(std::int32_t(0), std::uint32_t(1)) == -1, "dumb test");

		static_assert(!safe_diff<std::uint32_t>(std::int32_t(-2), std::uint32_t(1)), "dumb test");
		static_assert(safe_diff<std::int32_t>(std::int32_t(-1), std::uint32_t(1)) == -2, "dumb test");
		static_assert(!safe_diff<std::int32_t>(max64s, 0), "dumb test");

		static_assert(safe_diff<std::uint32_t>(std::uint32_t(1), std::int32_t(1)) == 0, "dumb test");
		static_assert(!safe_diff<std::uint32_t>(std::uint32_t(1), std::int32_t(2)), "dumb test");
		static_assert(safe_diff<std::uint32_t>(std::uint32_t(1), std::int32_t(-1)) == 2, "dumb test");
		static_assert(safe_diff<std::uint32_t>(std::uint32_t(2), std::int32_t(-1)) == 3, "dumb test");
		static_assert(safe_diff<std::int32_t>(std::uint32_t(1), std::int32_t(-1)) == 2, "dumb test");
		static_assert(safe_diff<std::int32_t>(std::uint32_t(2), std::int32_t(-1)) == 3, "dumb test");
		static_assert(!safe_diff<std::int32_t>(max32u, std::int32_t(-1)), "dumb test");
		static_assert(safe_diff<std::uint32_t>(max32u_1, std::int32_t(-1)) == max32u, "dumb test");
		static_assert(!safe_diff<std::int32_t>(max64u, 0), "dumb test");

		// safe_mult
		static_assert(safe_mult<std::uint32_t>(std::uint32_t(1), std::uint32_t(1)) == 1, "dumb");
		static_assert(safe_mult<std::uint32_t>(max32u, std::uint32_t(0)) == 0, "dumb");
		static_assert(safe_mult<std::uint32_t>(max32u, std::uint32_t(1)) == max32u, "dumb");
		static_assert(!safe_mult<std::uint32_t>(max32u, std::uint32_t(2)), "dumb");

		static_assert(safe_mult<std::int32_t>(std::int32_t(1), std::int32_t(1)) == 1, "dumb");
		static_assert(safe_mult<std::int32_t>(max64s, std::int32_t(0)) == 0, "dumb");
		static_assert(safe_mult<std::int32_t>(max32s, std::int32_t(1)) == max32s, "dumb");
		static_assert(!safe_mult<std::int32_t>(max32s, std::int32_t(2)), "dumb");

		static_assert(safe_mult<std::int32_t>(std::int32_t(-1), std::int32_t(1)) == -1, "dumb");
		static_assert(safe_mult<std::int32_t>(max64s, std::int32_t(0)) == 0, "dumb");
		static_assert(!safe_mult<std::int32_t>(min32s, std::int32_t(-1)), "dumb");
		static_assert(safe_mult<std::int32_t>(max32s, std::int32_t(-1)) == -max32s, "dumb");
		static_assert(!safe_mult<std::int32_t>(max32s, std::int32_t(-2)), "dumb");

		static_assert(!safe_mult<std::uint32_t>(std::int32_t(-1), std::int32_t(1)), "dumb");
		static_assert(safe_mult<std::uint32_t>(max64s, std::int32_t(0)) == 0, "dumb");
		static_assert(safe_mult<std::uint32_t>(min32s, std::int32_t(-1)) == details::safe_abs(min32s), "dumb");
		static_assert(safe_mult<std::uint32_t>(min32s, std::int32_t(0)) == 0, "dumb");
		static_assert(!safe_mult<std::uint32_t>(max32s, std::int32_t(-1)), "dumb");
		static_assert(safe_mult<std::uint32_t>(max32s, std::int32_t(2)) == 2*std::uint32_t(max32s), "dumb");

		static_assert(safe_mult<std::int32_t>(std::int32_t(-1), std::uint32_t(1)) == -1, "dumb");
		static_assert(safe_mult<std::int32_t>(max64s, std::int32_t(0)) == 0, "dumb");
		static_assert(!safe_mult<std::int32_t>(std::int32_t(-1), max64s), "dumb");
		static_assert(safe_mult<std::int32_t>(std::int32_t(-1), max32s) == -max32s, "dumb");
		static_assert(!safe_mult<std::int32_t>(std::uint32_t(-2), max32s), "dumb");

		static_assert(!safe_mult<std::uint32_t>(std::int32_t(-1), std::uint32_t(1)), "dumb");
		static_assert(safe_mult<std::uint32_t>(max32s, std::uint32_t(0)) == 0, "dumb");
		static_assert(safe_mult<std::uint32_t>(min32s, std::uint32_t(0)) == 0, "dumb");
		static_assert(!safe_mult<std::uint32_t>(std::int32_t(-1), max32s), "dumb");
		static_assert(safe_mult<std::uint32_t>(max32s, std::uint32_t(2)) == 2*std::uint32_t(max32s), "dumb");


		// safe_div
		static_assert(safe_div<std::uint32_t>(std::uint32_t(1), std::uint32_t(1)) == 1, "dumb");
		static_assert(!safe_div<std::uint32_t>(std::uint32_t(1), std::uint32_t(0)), "dumb");
		static_assert(safe_div<std::uint32_t>(max32u, std::uint32_t(1))==max32u, "dumb");
		static_assert(safe_div<std::uint32_t>(max32u, std::uint32_t(2))==max32u/2, "dumb");
		static_assert(!safe_div<std::uint32_t>(max64u, std::uint32_t(2)), "dumb");
		static_assert(safe_div<std::uint32_t>(max64u, max64u)==1, "dumb");

		static_assert(safe_div<std::int32_t>(std::uint32_t(1), std::uint32_t(1)) == 1, "dumb");
		static_assert(!safe_div<std::int32_t>(std::uint32_t(1), std::uint32_t(0)), "dumb");
		static_assert(safe_div<std::int32_t>(std::uint32_t(max32s), std::uint32_t(1)) == max32s, "dumb");
		static_assert(safe_div<std::int32_t>(std::uint32_t(max32s), std::uint32_t(2)) == max32s/2, "dumb");
		static_assert(!safe_div<std::int32_t>(max64u, std::uint32_t(2)), "dumb");
		static_assert(safe_div<std::int32_t>(max64u, max64u)==1, "dumb");

		static_assert(safe_div<std::int32_t>(std::int32_t(1), std::uint32_t(1)) == 1, "dumb");
		static_assert(!safe_div<std::int32_t>(std::int32_t(1), std::uint32_t(0)), "dumb");
		static_assert(safe_div<std::int32_t>(max32s, std::uint32_t(1)) == max32s, "dumb");
		static_assert(safe_div<std::int32_t>(max32s, std::uint32_t(2)) == max32s/2, "dumb");
		static_assert(!safe_div<std::int32_t>(max64u, std::uint32_t(2)), "dumb");
		static_assert(safe_div<std::int32_t>(max64s, std::uint64_t(max64s)) == 1, "dumb");

		static_assert(safe_div<std::uint32_t>(std::int32_t(1), std::uint32_t(1)) == 1, "dumb");
		static_assert(!safe_div<std::uint32_t>(std::int32_t(1), std::uint32_t(0)), "dumb");
		static_assert(safe_div<std::uint32_t>(max32s, std::uint32_t(1)) == max32s, "dumb");
		static_assert(safe_div<std::uint32_t>(max32s, std::uint32_t(2)) == max32s/2, "dumb");
		static_assert(!safe_div<std::uint32_t>(max64u, std::uint32_t(2)), "dumb");
		static_assert(safe_div<std::uint32_t>(max64s, std::uint64_t(max64s)) == 1, "dumb");

		static_assert(safe_div<std::int32_t>(std::uint32_t(1), std::uint32_t(1)) == 1, "dumb");
		static_assert(!safe_div<std::int32_t>(std::uint32_t(1), std::uint32_t(0)), "dumb");
		static_assert(!safe_div<std::int32_t>(max32u, std::uint32_t(1)), "dumb");
		static_assert(safe_div<std::int32_t>(max32u, std::uint32_t(2)) == max32u/2, "dumb");
		static_assert(!safe_div<std::int32_t>(max64u, std::uint32_t(2)), "dumb");
		static_assert(safe_div<std::int32_t>(max64u, std::uint64_t(max64s)) == max64u/std::uint64_t(max64s), "dumb");

		static_assert(safe_div<std::uint32_t>(std::uint32_t(1), std::uint32_t(1)) == 1, "dumb");
		static_assert(!safe_div<std::uint32_t>(std::uint32_t(1), std::uint32_t(0)), "dumb");
		static_assert(safe_div<std::uint32_t>(max32u, std::uint32_t(1)) == max32u, "dumb");
		static_assert(safe_div<std::uint32_t>(max32u, std::uint32_t(2)) == max32u/2, "dumb");
		static_assert(!safe_div<std::uint32_t>(max64u, std::uint32_t(2)), "dumb");
		static_assert(safe_div<std::uint32_t>(max64u, std::uint64_t(max64s)) == max64u/std::uint64_t(max64s), "dumb");
	}
}


#endif // SAFEOPERATIONS2_HPP
