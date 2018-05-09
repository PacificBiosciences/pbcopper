
include(CheckCXXCompilerFlag)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# NOTE: quash clang warnings w/ Boost
check_cxx_compiler_flag("-Wno-unused-local-typedefs" HAS_NO_UNUSED_LOCAL_TYPEDEFS)
if(HAS_NO_UNUSED_LOCAL_TYPEDEFS)
    set(pbcopper_CXX_FLAGS "${pbcopper_CXX_FLAGS} -Wno-unused-local-typedefs")
endif()

