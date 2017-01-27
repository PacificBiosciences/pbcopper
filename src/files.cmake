
set(pbcopper_H

    # -------
    # global
    # -------

    ${pbcopper_IncludeDir}/pbcopper/Config.h
    ${CMAKE_BINARY_DIR}/generated/pbcopper/utility/Version.h

    # -------
    # align
    # -------

    ${pbcopper_IncludeDir}/pbcopper/align/Seed.h
    ${pbcopper_IncludeDir}/pbcopper/align/Seeds.h

    ${pbcopper_IncludeDir}/pbcopper/align/internal/Seed-inl.h
    ${pbcopper_IncludeDir}/pbcopper/align/internal/Seeds-inl.h

    # -------
    # cli
    # -------

    ${pbcopper_IncludeDir}/pbcopper/cli/CLI.h
    ${pbcopper_IncludeDir}/pbcopper/cli/HelpPrinter.h
    ${pbcopper_IncludeDir}/pbcopper/cli/Interface.h
    ${pbcopper_IncludeDir}/pbcopper/cli/Option.h
    ${pbcopper_IncludeDir}/pbcopper/cli/Parser.h
    ${pbcopper_IncludeDir}/pbcopper/cli/PositionalArg.h
    ${pbcopper_IncludeDir}/pbcopper/cli/Results.h
    ${pbcopper_IncludeDir}/pbcopper/cli/SingleDashMode.h
    ${pbcopper_IncludeDir}/pbcopper/cli/VersionPrinter.h

    ${pbcopper_IncludeDir}/pbcopper/cli/internal/Option-inl.h
    ${pbcopper_IncludeDir}/pbcopper/cli/internal/Results-inl.h

    # ------------------
    # cli/toolcontract
    # ------------------

    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/Config.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/Driver.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/InputFileType.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/OutputFileType.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/JsonPrinter.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/ResolvedToolContract.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/ResourceType.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/Task.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/TaskType.h

    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/internal/Config-inl.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/internal/Driver-inl.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/internal/InputFileType-inl.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/internal/OutputFileType-inl.h
    ${pbcopper_IncludeDir}/pbcopper/cli/toolcontract/internal/Task-inl.h

    # -------
    # data
    # -------

    ${pbcopper_IncludeDir}/pbcopper/data/CCSTag.h
    ${pbcopper_IncludeDir}/pbcopper/data/Interval.h
    ${pbcopper_IncludeDir}/pbcopper/data/MovieName.h
    ${pbcopper_IncludeDir}/pbcopper/data/Position.h
    ${pbcopper_IncludeDir}/pbcopper/data/ReadName.h
    ${pbcopper_IncludeDir}/pbcopper/data/RSMovieName.h
    ${pbcopper_IncludeDir}/pbcopper/data/RSReadName.h
    ${pbcopper_IncludeDir}/pbcopper/data/Zmw.h

    ${pbcopper_IncludeDir}/pbcopper/data/internal/Interval-inl.h
    ${pbcopper_IncludeDir}/pbcopper/data/internal/MovieName-inl.h
    ${pbcopper_IncludeDir}/pbcopper/data/internal/ReadNameBase.h
    ${pbcopper_IncludeDir}/pbcopper/data/internal/ReadNameBase-inl.h
    ${pbcopper_IncludeDir}/pbcopper/data/internal/RSMovieName-inl.h

    # -------
    # JSON
    # -------

    ${pbcopper_IncludeDir}/pbcopper/json/JSON.h

    ${pbcopper_IncludeDir}/pbcopper/json/internal/json.hpp

    # -------
    # logging
    # -------

    ${pbcopper_IncludeDir}/pbcopper/logging/Logging.h
    ${pbcopper_IncludeDir}/pbcopper/logging/internal/Logging-inl.h

    # -------
    # qgram
    # -------

    ${pbcopper_IncludeDir}/pbcopper/qgram/Index.h
    ${pbcopper_IncludeDir}/pbcopper/qgram/IndexHit.h
    ${pbcopper_IncludeDir}/pbcopper/qgram/IndexHits.h

    ${pbcopper_IncludeDir}/pbcopper/qgram/internal/Hashing-inl.h
    ${pbcopper_IncludeDir}/pbcopper/qgram/internal/Index-inl.h


    # -------
    # stream
    # -------

    ${pbcopper_IncludeDir}/pbcopper/stream/Stream.h

    # -------
    # utility
    # -------

    ${pbcopper_IncludeDir}/pbcopper/utility/CallbackTimer.h
    ${pbcopper_IncludeDir}/pbcopper/utility/EnumClassHash.h
    ${pbcopper_IncludeDir}/pbcopper/utility/MoveAppend.h
    ${pbcopper_IncludeDir}/pbcopper/utility/SafeSubtract.h
    ${pbcopper_IncludeDir}/pbcopper/utility/Stopwatch.h
    ${pbcopper_IncludeDir}/pbcopper/utility/StringUtils.h
    ${pbcopper_IncludeDir}/pbcopper/utility/SystemInfo.h

    ${pbcopper_IncludeDir}/pbcopper/utility/internal/CallbackTimer-inl.h
    ${pbcopper_IncludeDir}/pbcopper/utility/internal/Stopwatch-inl.h
    ${pbcopper_IncludeDir}/pbcopper/utility/internal/StringUtils-inl.h
    ${pbcopper_IncludeDir}/pbcopper/utility/internal/SystemInfo-inl.h
)

set(pbcopper_CPP

    # -------
    # cli
    # -------

    ${pbcopper_SourceDir}/cli/CLI.cpp
    ${pbcopper_SourceDir}/cli/HelpPrinter.cpp
    ${pbcopper_SourceDir}/cli/Interface.cpp
    ${pbcopper_SourceDir}/cli/Option.cpp
    ${pbcopper_SourceDir}/cli/Parser.cpp
    ${pbcopper_SourceDir}/cli/Results.cpp
    ${pbcopper_SourceDir}/cli/VersionPrinter.cpp

    # ------------------
    # cli/toolcontract
    # ------------------

    ${pbcopper_SourceDir}/cli/toolcontract/JsonPrinter.cpp
    ${pbcopper_SourceDir}/cli/toolcontract/ResolvedToolContract.cpp

    # -------
    # data
    # -------

    ${pbcopper_SourceDir}/data/MovieName.cpp
    ${pbcopper_SourceDir}/data/RSMovieName.cpp

    # ---------
    # logging
    # ---------

    ${pbcopper_SourceDir}/logging/Logging.cpp

    # ---------
    # utility
    # ---------

    ${pbcopper_SourceDir}/utility/CallbackTimer.cpp
)
