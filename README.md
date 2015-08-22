# safeintegral

From Wikipedia:

> In computer programming, an integer overflow occurs when an arithmetic operation attempts to
create a numeric value that is too large to be represented within the available storage space.
For instance, taking the arithmetic mean of two numbers by adding them and dividing by two, as done
in many search algorithms, causes error if the sum (although not the resulting mean) is too large to
be represented, and hence overflows. The most common result of an overflow is that the least
significant representable bits of the result are stored; the result is said to wrap.

Most programmers do not handle overflows, since it is a tedious and error-prone activity.
Even simple functions, like the following average function, has an undefined behaviour if the input parameters are big
enough:

	int average(int a, int b){
		return (a+b)/2;
	}

The operation sum may overflow, (even if the final result would be in range) introducing an undefined behaviour.
There are two possibilities:
	a) write a function that does not overflow
	b) write a function that can return errors

In this case it's always possible to write a function that does not overflow:

	int average(int a, int b){
		return ( a/2 + b/2 + (a%2) * (b%2));
	}


But in other cases it's not always possible to avoid overflow errors, for example in the following case, with some
inputs, the result would not be in range

	int doublethevalue(int a){
		return a*2;
	}

So you should change your function to:

	errotype doublethevalue(int& a){
		if(a>max/2){
			return errortype::tobig;
		}
		a*=2;
	return errortype::noerror;
	}

but then, every time you want to do some operations, you would have to check if the operation is supported or not.
An alternative function could be

	int doublethevalue(int a){
		if(a>max/2){
			throw MathOperationError("Value is too big");
		}
		return a*2;
	}

in that case you do not need to check every time for errors, but you still have to remember to use these custom
functions instead of using all the normal arithmetic operators.

That's why this project provides functions to detect if in an operation will occur undefined behaviour and a
safeintegral class. The ladder has implemented all arithmetic and binary operators, which will throw exceptions
in the case an overflow would occur.

A simple example of usage:

	#include "../safeintegral/safeintegral.hpp"
	#include<iostream>

	template < class T >
	constexpr T factorial(T n) {
		return (n <= 1 ? 1 : n * factorial(n - 1));
	}

	int main(){
		auto s = make_safe(2l); // or use: safe_long s = 2;

		if(factorial(s)==4){
			std::cout << "Everything is fine.\n";
		} else {
			std::cout << "There is something wrong!\n"
		}
		return 0;
	}
