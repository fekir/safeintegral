##################################################################
## Settings for the MSVC compiler

##################################################################
# STD settings

# remove "wrong" (and annoying) warnings
add_definitions(-D_NONSTDC_NO_WARNINGS) #https://msdn.microsoft.com/en-us/library/ttcz0bys.aspx

##################################################################
# do not optimize when building with debug info
#string(REPLACE "/O1" "/Od" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
#string(REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

##################################################################
# compiler warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
option(ALL_WARNING "Enable all warnings" OFF)
if(NOT ALL_WARNING)
	# disable since these warnings do not show possible errors, but rather informations
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4464")          # relative include path contains ".."
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4514")          # unreferenced inline function has been removed
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4571")          # catch(...) does not catch seh
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4625")          # copy constructor was implictly defined as deleted
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4626")          # assignment operator was implictly defined as deleted
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4710")          # function not inlined
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4711")          # function will be inlined
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4820")          # X bytes padding added
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd5026")          # move constructor was implictly defined as deleted
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd5027")          # move assignment was implictly defined as deleted
endif()

##################################################################
# errors
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /we4837 /we4289")      # trigraph extension
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /we4129")              # unrecognized character
#add_compile_options("/we4129") # unrecognized character
#add_compile_options($<$<LANG:CXX>:/we4129>)

##################################################################
# language
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:forScope /Zc:rvalueCast /Zc:wchar_t /Zc:strictStrings /Zc:inline /Zc:implicitNoexcept /Zc:auto") # disable language extensions
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Za")                 # disable all langauge extensions, does not work with windows headers

##################################################################
#security checks
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GS")                  # Buffer Security Check
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Gs")                  # Control Stack Checking Calls
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /guard:cf")            # Enable Control Flow Guard (also passed to linker)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /sdl")                 # Compile and Runtime checks
# /RTCc may reject conformant code -> solution: remove flag or define _ALLOW_RTCc_IN_STL
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /RTCc -D_ALLOW_RTCc_IN_STL")    # Check conversion to smaller types, incompatibel with something else than Debug

##################################################################
# optimization
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zc:inline") # Remove unused code and data
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Qpar")      # automatic parallelization of loops
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Gw")        # Optimize Global Data
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")        # Whole Program Optimization
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /OPT:REF")   # eliminates functions or data that is never used

# edit and continue - incompatible with  /guard:cf
#set(CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}          /ZI")   # eliminates functions or data never used
#set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /ZI")   # eliminates functions or data never used


