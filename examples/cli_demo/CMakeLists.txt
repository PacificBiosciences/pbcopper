
# includes/compile flags
include_directories(${pbcopper_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${pbcopper_CXX_FLAGS}")

# main exe
add_executable(cli_demo EXCLUDE_FROM_ALL
    main.cpp)
set_target_properties(cli_demo
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${examples_BinDir}
)

# libs/dependencies
add_dependencies(cli_demo pbcopper)
target_link_libraries(cli_demo ${pbcopper_LIBRARIES})

# add to 'make examples'
add_dependencies(examples cli_demo)
