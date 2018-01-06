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
#ifndef SAFEOPERATIONS_ERRORS_HPP
#define SAFEOPERATIONS_ERRORS_HPP

#include <limits>
#include <type_traits>
#include <cstdint>

namespace safeintegralop {

	// All functions in the namespace "details" are for private use, you should use all the function outside of this namespace
	namespace details{
		// Most operations do not make sense on bool, even if the are considered integrals
		template <typename T>
		constexpr bool is_integral_not_bool_char(){
			using value_type = typename std::remove_cv<T>::type;
			return
			        !std::is_same<value_type,bool>::value && !std::is_same<value_type,char>::value &&
			        !std::is_same<value_type,char16_t>::value && !std::is_same<value_type,char32_t>::value &&
			        !std::is_same<value_type,wchar_t>::value && std::is_integral<T>::value;
		}
#if defined(SAFE_INTEGRAL_OP_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR) || defined(SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE) || \
	    defined(SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE) || defined(SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3)
#error "One of the \"SAFE_INTEGRAL_OP_..._INTEGRAL_NOT_BOOL_CHAR_...\" macros has been already defined elsewhere!"
#endif
#define SAFE_INTEGRAL_OP_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR " needs to be an integral (not bool or char/wchar_t) value type"
#define SAFE_INTEGRAL_OP_ASSERT_INTEGRAL_NOT_BOOL_CHAR_TYPE(T) \
	static_assert(safeintegralop::details::is_integral_not_bool_char<T>(), #T SAFE_INTEGRAL_OP_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR)
#define SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE(T, R) \
	static_assert(safeintegralop::details::is_integral_not_bool_char<T>(), #T SAFE_INTEGRAL_OP_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR);\
	static_assert(safeintegralop::details::is_integral_not_bool_char<R>(), #R SAFE_INTEGRAL_OP_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR)
#define SAFE_INTEGRAL_OP_ASSERT_INTEGRALS_NOT_BOOL_CHAR_TYPE3(T, R, U) \
	static_assert(safeintegralop::details::is_integral_not_bool_char<T>(), #T SAFE_INTEGRAL_OP_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR);\
	static_assert(safeintegralop::details::is_integral_not_bool_char<R>(), #R SAFE_INTEGRAL_OP_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR);\
	static_assert(safeintegralop::details::is_integral_not_bool_char<U>(), #U SAFE_INTEGRAL_OP_ERR_MSG_xxx_NEEDS_INTEGRAL_NOT_BOOL_CHAR)

	}
}
#endif // SAFEOPERATIONS_ERRORS_HPP
