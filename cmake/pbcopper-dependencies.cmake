
if(NOT Boost_INCLUDE_DIRS)
    find_package(Boost REQUIRED)
endif()

if (NOT ZLIB_INCLUDE_DIRS OR NOT ZLIB_LIBRARIES)
    find_package(ZLIB REQUIRED)
endif()
