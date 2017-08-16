
# define pbcopper library dependencies
set(pbcopper_DependencyIncludes
    ${Boost_INCLUDE_DIRS}
)
set(pbcopper_DependencyLibraries
    ${CMAKE_THREAD_LIBS_INIT}
)

# set up library include dirs
include_directories(SYSTEM ${pbcopper_DependencyIncludes})
include_directories(${pbcopper_IncludeDir})

# grab library source files
include(files.cmake)
set(SOURCES
    ${pbcopper_H}
    ${pbcopper_CPP}
)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${pbcopper_CXX_FLAGS}")

# define pbcopper library
add_library(pbcopper ${SOURCES})
set_target_properties(pbcopper
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${pbcopper_LibDir}
    RUNTIME_OUTPUT_DIRECTORY ${pbcopper_LibDir}
    LIBRARY_OUTPUT_DIRECTORY ${pbcopper_LibDir}
)

# link dependency libs
target_link_libraries(pbcopper ${pbcopper_DependencyLibraries})
target_include_directories(pbcopper
    PUBLIC
    ${pbcopper_IncludeDir}
    ${pbcopper_DependencyIncludes}
    ${CMAKE_BINARY_DIR}/generated
)

# export variables for projects that use pbcopper
set(pbcopper_INCLUDE_DIRS
    ${pbcopper_IncludeDir} ${pbcopper_DependencyIncludes} ${CMAKE_BINARY_DIR}/generated
    CACHE INTERNAL
    "${PROJECT_NAME}: Include Directories"
    FORCE
)

set(pbcopper_LIBRARIES
    pbcopper ${pbcopper_DependencyLibraries}
    CACHE INTERNAL
    "${PROJECT_NAME}: Libraries"
    FORCE
)
