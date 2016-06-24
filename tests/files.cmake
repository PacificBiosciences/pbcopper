# test case headers
set( pbcopperTest_H

    ${pbcopper_TestsDir}/include/OStreamRedirector.h
)

# test case source files
set( pbcopperTest_CPP

    # cli
    ${pbcopper_TestsDir}/src/cli/test_CLI.cpp
    ${pbcopper_TestsDir}/src/cli/test_HelpPrinter.cpp
    ${pbcopper_TestsDir}/src/cli/test_Interface.cpp
    ${pbcopper_TestsDir}/src/cli/test_Option.cpp
    ${pbcopper_TestsDir}/src/cli/test_ResolvedToolContract.cpp
    ${pbcopper_TestsDir}/src/cli/test_Results.cpp
    ${pbcopper_TestsDir}/src/cli/test_ToolContractJsonPrinter.cpp
    ${pbcopper_TestsDir}/src/cli/test_VersionPrinter.cpp

    # data
    ${pbcopper_TestsDir}/src/data/test_Interval.cpp
    ${pbcopper_TestsDir}/src/data/test_MovieName.cpp
    ${pbcopper_TestsDir}/src/data/test_ReadName.cpp
    ${pbcopper_TestsDir}/src/data/test_RSMovieName.cpp

    # JSON
    ${pbcopper_TestsDir}/src/json/test_JSON.cpp

    # logging
    ${pbcopper_TestsDir}/src/logging/test_Logging.cpp

    # stream
    ${pbcopper_TestsDir}/src/stream/test_Stream.cpp

    # utility
    ${pbcopper_TestsDir}/src/utility/test_CallbackTimer.cpp
    ${pbcopper_TestsDir}/src/utility/test_Stopwatch.cpp
    ${pbcopper_TestsDir}/src/utility/test_SystemInfo.cpp
)
