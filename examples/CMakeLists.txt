
# use this destination directory for any example exe's generated
set(examples_BinDir ${CMAKE_CURRENT_BINARY_DIR}/bin)
add_custom_target(examples)

# example exes
add_subdirectory(cli_demo)

