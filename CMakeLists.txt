########################################################################
# CMake build script for PacBio pbcopper library.
########################################################################
cmake_policy(SET CMP0048 NEW)  # lets us set version in project()
cmake_minimum_required(VERSION 3.0)

project(pbcopper VERSION 0.0.1 LANGUAGES CXX C)

# project options
option(pbcopper_build_shared    "Build pbcopper as a shared library."         OFF)
option(pbcopper_build_tests     "Build pbcopper's unit tests."                ON)
option(pbcopper_use_ccache      "Build pbcopper using ccache, if available."  ON)

# project paths
set(pbcopper_RootDir     ${pbcopper_SOURCE_DIR})
set(pbcopper_DocsDir     ${pbcopper_RootDir}/docs)
set(pbcopper_IncludeDir  ${pbcopper_RootDir}/include)
set(pbcopper_SourceDir   ${pbcopper_RootDir}/src)
set(pbcopper_TestsDir    ${pbcopper_RootDir}/tests)
set(pbcopper_ThirdParty  ${pbcopper_RootDir}/third-party)

if(NOT pbcopper_OutputDir)
    set(pbcopper_OutputDir ${pbcopper_RootDir})
endif()

set(pbcopper_LibDir  ${pbcopper_OutputDir}/lib)
file(MAKE_DIRECTORY  ${pbcopper_LibDir})

# project configuration
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake ${CMAKE_MODULE_PATH})
include(pbcopper-compilerflags)
include(pbcopper-ccache)
include(pbcopper-dependencies)
include(pbcopper-libtype)

# project components
add_subdirectory(src)
add_subdirectory(docs)      # build with 'make doc' or 'make docs'
add_subdirectory(examples)  # build with 'make examples'

if(pbcopper_build_tests)
    enable_testing()
    add_subdirectory(tests)
endif()
