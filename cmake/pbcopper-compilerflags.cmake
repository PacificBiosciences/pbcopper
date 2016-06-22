
include(CheckCXXCompilerFlag)

# shared CXX flags for all source code & tests
if (MSVC)
    set(pbcopper_CXX_FLAGS "/Wall")
else()
    set(pbcopper_CXX_FLAGS "-std=c++11 -Wall")
endif()

# NOTE: quash clang warnings w/ Boost
check_cxx_compiler_flag("-Wno-unused-local-typedefs" HAS_NO_UNUSED_LOCAL_TYPEDEFS)
if(HAS_NO_UNUSED_LOCAL_TYPEDEFS)
    set(pbcopper_CXX_FLAGS "${pbcopper_CXX_FLAGS} -Wno-unused-local-typedefs")
endif()

