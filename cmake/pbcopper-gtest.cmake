
# pthreads needed by GoogleTest
find_package(Threads REQUIRED)

if (NOT GTEST_SRC_DIR)
    set(PREBUILT_GTEST_SRC ${pbcopper_RootDir}/../../../../prebuilt.tmpout/gtest/gtest_1.7.0/)
    if(EXISTS ${PREBUILT_GTEST_SRC})
        set(GTEST_SRC_DIR ${PREBUILT_GTEST_SRC})
    else()
        set(GTEST_SRC_DIR ${pbcopper_ThirdParty}/gtest/googletest) # keep old fallback behavior for external builds, for now at least
    endif()
endif()

add_subdirectory(
    ${GTEST_SRC_DIR}
    ${CMAKE_BINARY_DIR}/external/gtest/build
    EXCLUDE_FROM_ALL
)
