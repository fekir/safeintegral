#include "catch.hpp"

#include "../safeintegral/safeintegral.hpp"

#include <cstdint>

constexpr auto s11 = make_safe(14);
constexpr auto s21 = make_safe(2);
static_assert(s11 + s21 == 16, "");
static_assert(s11 - s21 == 12, "");
static_assert(s11 * s21 == 28, "");
static_assert(s11 / s21 == 7, "");
static_assert(s11 % s21 == 0, "");


template < class T >
constexpr T factorial(T n) {
	return (n <= 1 ? 1 : n * factorial(n - 1));
}

// First two test for profiling
TEST_CASE( "Factorial of long int", "[factorial1][hide]" ) {
	auto c = 20l;
	for(int i =0; i<900000; ++i) {
		REQUIRE(factorial(c) == 2432902008176640000l);
	}
}

TEST_CASE( "Factorial of safenum<long int>", "[factorial2][hide]" ) {
	auto c = make_safe(20l);
	for(int i =0; i<900000; ++i) {
		REQUIRE(factorial(c).getvalue() == 2432902008176640000l);
	}
}

TEST_CASE("All types of increment (positive)", "[increment][positive]"){
	auto i = 2l;
	auto s = make_safe(i);

	SECTION("preincrement"){
		REQUIRE(++s == ++i);
		REQUIRE(++s == ++i);
		REQUIRE(s == i);
	}

	SECTION( "postincrement") {
		REQUIRE(s++ == i++);
		REQUIRE(s++ == i++);
		REQUIRE(s == i);
	}
}

TEST_CASE("All types of increment (negative)", "[increment][negative]"){
	auto s = make_safe(std::numeric_limits<long int>::max()-1l);

	SECTION("preincrement"){
		REQUIRE_NOTHROW(++s);
		REQUIRE_THROWS_AS(++s, std::out_of_range);
		REQUIRE(s==std::numeric_limits<long int>::max());
	}

	SECTION( "postincrement") {
		REQUIRE_NOTHROW(s++);
		REQUIRE_THROWS_AS(s++, std::out_of_range);
		REQUIRE(s==std::numeric_limits<long int>::max());
	}
}

TEST_CASE("All types of decrement (positive)", "[decrement][positive]"){
	auto i = 2l;
	auto s = make_safe(i);

	SECTION("predecrement"){
		REQUIRE(--s == --i);
		REQUIRE(--s == --i);
		REQUIRE(s == i);
	}

	SECTION( "postdecrement") {
		REQUIRE(s-- == i--);
		REQUIRE(s-- == i--);
		REQUIRE(s == i);
	}
}

TEST_CASE("All types of decrement (negative)", "[decrement][negative]"){
	auto s = make_safe(std::numeric_limits<long int>::min()+1l);

	SECTION("preincrement"){
		REQUIRE_NOTHROW(--s);
		REQUIRE_THROWS_AS(--s, std::out_of_range);
		REQUIRE(s==std::numeric_limits<long int>::min());
	}

	SECTION( "postincrement") {
		REQUIRE_NOTHROW(s--);
		REQUIRE_THROWS_AS(s--, std::out_of_range);
		REQUIRE(s==std::numeric_limits<long int>::min());
	}
}

TEST_CASE( "arithmetic op+", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE(getvalue(s+s) == i+i);
}

TEST_CASE( "arithmetic op+ (negative)", "[negative]" ) {
	auto s = make_safe(std::numeric_limits<long int>::max()-15);
	REQUIRE_NOTHROW(s+15l);
	REQUIRE_THROWS_AS(s+16l, std::out_of_range);

	REQUIRE_NOTHROW(make_safe(std::numeric_limits<long int>::max())+0l);
}

TEST_CASE( "arithmetic op- (positive)", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE(getvalue(s-1l) == i-1);
}

TEST_CASE( "arithmetic op- (negative)", "[negative]" ) {
	auto s = make_safe(std::numeric_limits<long int>::min()+15);
	REQUIRE_NOTHROW(s-15l);
	REQUIRE_THROWS_AS(s-16l, std::out_of_range);

	REQUIRE_NOTHROW(make_safe(std::numeric_limits<long int>::min())+0l);
}

TEST_CASE( "arithmetic op*", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE(getvalue(s*2l) == i*2l);
}

TEST_CASE( "arithmetic op* (negative)", "[negative]" ) {
	auto s = make_safe(std::numeric_limits<long int>::max()/2-1);
	REQUIRE_NOTHROW(s*2l);
	REQUIRE_THROWS_AS(s*3l, std::out_of_range);

	REQUIRE_NOTHROW(make_safe(std::numeric_limits<long int>::max())*1l);
}

TEST_CASE( "arithmetic op/", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE(getvalue(s/2l) == i/2);
}

TEST_CASE( "arithmetic op/ (negative)", "[negative]" ) {
	auto s = make_safe(std::numeric_limits<long int>::min());
	REQUIRE_NOTHROW(s/1l);
	REQUIRE_THROWS_AS(s/0l, std::out_of_range);
	REQUIRE_THROWS_AS(s/-1l, std::out_of_range);
}

TEST_CASE( "arithmetic op%", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE(getvalue(s%2l) == i%2l);
}

TEST_CASE( "arithmetic op% (negative)", "[negative]" ) {
	auto s = make_safe(std::numeric_limits<long int>::min());
	REQUIRE_NOTHROW(s%1l);
	REQUIRE_THROWS_AS(s%0l, std::out_of_range);
	REQUIRE_THROWS_AS(s%-1l, std::out_of_range);
	REQUIRE_NOTHROW(s%-2l);

}

TEST_CASE( "arithmetic op+=", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	s+=1l;
	i+=1l;
	REQUIRE(getvalue(s) == i);
}

TEST_CASE( "arithmetic op+= (negative)", "[negative]" ) {
	auto s = make_safe(std::numeric_limits<long int>::max());
	REQUIRE_NOTHROW(s+=0l);
	REQUIRE_THROWS_AS(s+=1l, std::out_of_range);
}

TEST_CASE( "arithmetic op-=", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	s-=1l;
	i-=1l;
	REQUIRE(getvalue(s) == i);
}

TEST_CASE( "arithmetic op-= (negative)", "[negative]" ) {
	auto s = make_safe(std::numeric_limits<long int>::min());
	REQUIRE_NOTHROW(s-=0l);
	REQUIRE_THROWS_AS(s-=1l, std::out_of_range);
}

TEST_CASE( "arithmetic op*=", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	s*=5l;
	i*=5l;
	REQUIRE(getvalue(s) == i);
}

TEST_CASE( "arithmetic op*= (negative)", "[negative]" ) {
	auto s = make_safe(std::numeric_limits<long int>::min()/2l+1l);
	REQUIRE_NOTHROW(s*=2l);
	REQUIRE_THROWS_AS(s*=3l, std::out_of_range);
}

TEST_CASE( "arithmetic op/=", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	s/=5l;
	i/=5l;
	REQUIRE(getvalue(s) == i);
}

TEST_CASE( "arithmetic op/= (negative)", "[negative]" ) {
	auto s = make_safe(std::numeric_limits<long int>::min());
	REQUIRE_NOTHROW(s/=1l);
	REQUIRE_THROWS_AS(s/=-1l, std::out_of_range);
}

TEST_CASE( "arithmetic op%=", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	s%=5l;
	i%=5l;
	REQUIRE(getvalue(s) == i);
}

TEST_CASE( "arithmetic op%= (negative)", "[negative]" ) {
	auto s = make_safe(std::numeric_limits<long int>::min());
	REQUIRE_THROWS_AS(s%=0l, std::out_of_range);
	REQUIRE_THROWS_AS(s%=-1l, std::out_of_range);
}

TEST_CASE( "bitwise op~", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE(getvalue(~s) == (~i));
}

TEST_CASE( "bitwise op&", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE(getvalue(s&1l) == (i&1l));
}

TEST_CASE( "bitwise op|", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE(getvalue(s|1l) == (i|1));
}

TEST_CASE( "bitwise op^", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE(getvalue(s^2l) == (i^2l));
}

TEST_CASE( "bitwise op<<", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE(getvalue(s<<2l) == (i<<2l));
}

TEST_CASE( "bitwise op<< (negative)", "[negative]" ) {
	auto i = -2l;
	auto s = make_safe(i);
	REQUIRE_THROWS_AS(s<<2l, std::out_of_range);
}

TEST_CASE( "bitwise op<< (negative) 2", "[negative]" ) {
	auto s = make_safe<int64_t>(1);
	REQUIRE_NOTHROW(s<<62l);
	REQUIRE_THROWS_AS(s<<63l, std::out_of_range);
}

TEST_CASE( "bitwise op>>", "[positive]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE(getvalue(s>>2l) == (i>>2l));
}

TEST_CASE( "bitwise op>> (negative)", "[negative]" ) {
	auto i = -2l;
	auto s = make_safe(i);
	REQUIRE_THROWS_AS(s>>2l, std::out_of_range);
}

TEST_CASE( "bitwise op>> (negative) 2", "[negative]" ) {
	auto i = 2l;
	auto s = make_safe(i);
	REQUIRE_THROWS_AS(s>>-2l, std::out_of_range);
}
