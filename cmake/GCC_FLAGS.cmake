##################################################################
# Settings for the GCC Compiler

set(SanitizerValues "NONE;SANADD;SANUNDEF;SANTHREAD" CACHE STRING
	"List of possible values for the sanitizers")
set(SanValue "SANUNDEF" CACHE STRING
	"SanValue chosen by the user at CMake configure time")

set_property(CACHE SanValue PROPERTY STRINGS ${SanitizerValues})

# default warnings
add_compile_options(-Wall -Wextra -pedantic -Wmain -Wunreachable-code -Wunknown-pragmas)
# multiple declaration, shadowing, undef
#add_compile_options(-Wshadow)
add_compile_options(-Wundef -Wredundant-decls)
# class/structs + init
add_compile_options(-Wnon-virtual-dtor -Wctor-dtor-privacy -Wnon-virtual-dtor -Wreorder -Winit-self -Wuninitialized)
# switch
add_compile_options(-Wswitch-default -Wswitch-enum)
# nullptr and casts warnings
#add_compile_options(-Wzero-as-null-pointer-constant -Wold-style-cast -Wuseless-cast) # to many warning with Qt
add_compile_options(-Wnonnull -Wcast-qual -Wcast-align -Wformat=2)
# arithmetic/numeric warnings
add_compile_options(-Wfloat-equal -Wsign-compare -Wconversion -Wsign-promo)
# project structure
add_compile_options(-Wmissing-include-dirs)
# logical operations
add_compile_options(-Wlogical-op)
# possible code structure problem
add_compile_options(-Wdisabled-optimization)
# errors
add_compile_options(-Werror=format-security)

add_compile_options(-Wno-missing-field-initializers)


option(DEBUG_ITERATORS "Additional debug information" OFF)
if(DEBUG_ITERATORS)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")
endif()

# trap/sanitizer settings
#set(SAN_FLAGS "${SAN_FLAGS}  -ftrapv -fstack-check -fstack-protector")

option(EFFCXX "Enable Effective C++ Warnings"  OFF)
if(EFFCXX)
	message(" ===== Adding Effective C++ Warnings")
	add_compile_options(-Weffc++)
endif()

# queste opzioni sono separate perchè non tutte compatibili e disponibili in gcc...
if("${SanValue}" STREQUAL "SANADD")
	message(" ===== Enable sanitizer for addresses ===== ")
	set(SAN_FLAGS "${SAN_FLAGS} -fsanitize=address") # not using add_compile_options since needs linking
elseif("${SanValue}" STREQUAL "SANUNDEF")
	message(" ===== Enable sanitizer for UB ===== ")
	#add_compile_options(-fsanitize-undefined-trap-on-error) # does not need linking, comparison??
	set(SAN_FLAGS "${SAN_FLAGS} -fsanitize=undefined") # not using add_compile_options since needs linking
elseif("${SanValue}" STREQUAL "SANTHREAD")
	message(" ===== Enable sanitizer for threads ===== ")
	set(SAN_FLAGS "${SAN_FLAGS} -fsanitize=thread") # not using add_compile_options since needs linking
endif()

option(PROFILE "Enable profiling" OFF)
if(PROFILE)
	message(" ===== Enable profiling ===== ")
	add_compile_options(-pg)
endif()


option(FORTIFY "Enable fortify sources" OFF)
if(FORTIFY)
	message(" ===== Enable fortify sources (always enabled in release mode) ===== ")
	add_definitions(-D_FORTIFY_SOURCE=2 -O2)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D_FORTIFY_SOURCE=2") # =1 when using -O1..

