cmake_minimum_required(VERSION 3.2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/)

# Project name and type
project(SafeIntegral CXX)

if(MSVC)
	# flags for msvc compiler
	include(MSVC_FLAGS)
else()
	# flags for gcc (mingw)/clang compiler
	include(GCC_FLAGS)
endif()

find_package(catch REQUIRED)
include_directories(${CATCH_INCLUDE_DIRS})

# The version number.
set (PROJ1_VERSION_MAJOR 0)
set (PROJ1_VERSION_MINOR 1)

set(SOURCE_FILES
	safeintegral/safeintegral.hpp
	safeintegral/safeintegralop.hpp
	safeintegral/safeintegralop_cmp.hpp
	safeintegral/errors.hpp
)

##########################################################
# Test settings

set(TEST_FILES
	
	test/testlongint.cpp
)

add_executable(${PROJECT_NAME}Test test/maintest.cpp
	${SOURCE_FILES} ${TEST_FILES}
)
set_property(TARGET ${PROJECT_NAME}Test PROPERTY CXX_STANDARD 11)
