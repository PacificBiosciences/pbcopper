########################################################################
# CMake build script for PacBio pbcopper library.
########################################################################
cmake_policy(SET CMP0048 NEW)  # lets us set version in project()
cmake_minimum_required(VERSION 3.2)

project(pbcopper VERSION 0.4.0 LANGUAGES CXX C)
enable_testing()

# project options
option(pbcopper_build_shared    "Build pbcopper as a shared library."         OFF)
option(pbcopper_use_ccache      "Build pbcopper using ccache, if available."  ON)
option(pbcopper_build_tests     "Build pbcopper tests."                       ON)
option(pbcopper_build_docs      "Build pbcopper docs."                        ON)
option(pbcopper_build_examples  "Build pbcopper examples."                    ON)

# project paths
set(pbcopper_RootDir       ${CMAKE_CURRENT_LIST_DIR})
set(pbcopper_DocsDir       ${pbcopper_RootDir}/docs)
set(pbcopper_IncludeDir    ${pbcopper_RootDir}/include)
set(pbcopper_SourceDir     ${pbcopper_RootDir}/src)
set(pbcopper_TestsDir      ${pbcopper_RootDir}/tests)
set(pbcopper_ThirdPartyDir ${pbcopper_RootDir}/third-party)

if(NOT pbcopper_OutputDir)
    set(pbcopper_OutputDir ${CMAKE_CURRENT_BINARY_DIR})
endif()

set(pbcopper_LibDir  ${pbcopper_OutputDir}/lib)
file(MAKE_DIRECTORY  ${pbcopper_LibDir})

# project configuration
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake ${CMAKE_MODULE_PATH})
include(pbcopper-compilerflags)
include(pbcopper-ccache)
include(pbcopper-dependencies)
include(pbcopper-libtype)

# config generation
include(pbcopper-gitsha1)
find_git_sha1(COPPER_GIT_SHA1)
configure_file(
    ${pbcopper_SourceDir}/utility/PbcopperGitHash.cpp.in
    ${CMAKE_BINARY_DIR}/generated/utility/PbcopperGitHash.cpp
)
configure_file(
    ${pbcopper_SourceDir}/utility/PbcopperVersion.cpp.in
    ${CMAKE_BINARY_DIR}/generated/utility/PbcopperVersion.cpp
)

# main library
add_subdirectory(src)

# project extras - only build on demand
#
if (pbcopper_build_docs)
    add_subdirectory(docs)      # build with 'make doc' or 'make docs'
endif()

if (pbcopper_build_examples)
    add_subdirectory(examples)  # build with 'make examples'
endif()

if (pbcopper_build_tests)
    add_subdirectory(tests)
endif()
