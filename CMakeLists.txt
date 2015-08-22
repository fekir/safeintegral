cmake_minimum_required(VERSION 2.8.8)

# Project name and type
project(SafeIntegral CXX)

# The version number.
set (PROJ1_VERSION_MAJOR 0)
set (PROJ1_VERSION_MINOR 1)

##########################################################
# Compiler Flags                                         #
##########################################################
include(CMakeFlags.txt)
# set language and warnings flags for different build types
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PROF_FLAGS}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${PROF_FLAGS}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LANG_FLAG} ${WARN_FLAGS} ${SAN_FLAGS} ${DEBUG_FLAGS} ${OPT_FLAGS} ${PROF_FLAGS}")
Message("CMAKE_CXX_FLAGS" ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CMAKE_CXX_FLAGS}")

##########################################################
# Test Options/settings                                  #
##########################################################
# Includes Catch in the project:
enable_testing(true)
add_test("TestSafeIntegral" testsafeintegral)

add_subdirectory(catch)
include_directories(${CATCH_INCLUDE_DIR} ${COMMON_INCLUDES})

add_executable(testsafeintegral ${SOURCE_FILES}
	test/maintest.cpp
	test/testlongint.cpp
	safeintegral/safeintegral.hpp
	safeintegral/safeintegralop.hpp
)
add_dependencies(testsafeintegral catch)
