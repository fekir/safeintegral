## Try to find catch (C++ Automated Test Cases in Headers)
## https://github.com/philsquared/Catch/
##
## Sets the following variables:
## CATCH_FOUND - System has catch library
## CATCH_INCLUDE_DIRS - catch include directory
##

set(PROG_FILES_X86_ENV "PROGRAMFILES(X86)")
set(PROG_FILES_ENV "ProgramW6432")

if(WIN32)
	find_path(CATCH_INCLUDE_DIR catch.hpp
		PATH "$ENV{${PROG_FILES_ENV}}/catch/"
		PATH "$ENV{${PROG_FILES_X86_ENV}}/catch/"
	)
else()
	find_path(CATCH_INCLUDE_DIR catch.hpp
		PATHS "/usr/include/"
	)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CATCH DEFAULT_MSG CATCH_INCLUDE_DIR)

mark_as_advanced( CATCH_INCLUDE_DIR )
set(CATCH_INCLUDE_DIRS ${CATCH_INCLUDE_DIR})