###########
# headers #
###########

if not meson.is_subproject()

  # pbcopper
  install_headers(
    files([
      'pbcopper/PbcopperConfig.h']),
    subdir : 'pbcopper')

  # pbcopper/align
  install_headers(
    files([
      'pbcopper/align/Seed.h',
      'pbcopper/align/Seeds.h']),
    subdir : 'pbcopper/align')

  # pbcopper/align/internal
  install_headers(
    files([
      'pbcopper/align/internal/Seed-inl.h',
      'pbcopper/align/internal/Seeds-inl.h']),
    subdir : 'pbcopper/align/internal')

  # pbcopper/cli
  install_headers(
    files([
      'pbcopper/cli/CLI.h',
      'pbcopper/cli/HelpPrinter.h',
      'pbcopper/cli/Interface.h',
      'pbcopper/cli/Option.h',
      'pbcopper/cli/OptionFlags.h',
      'pbcopper/cli/Parser.h',
      'pbcopper/cli/PositionalArg.h',
      'pbcopper/cli/Results.h',
      'pbcopper/cli/SingleDashMode.h',
      'pbcopper/cli/VersionPrinter.h']),
    subdir : 'pbcopper/cli')

  # pbcopper/cli/internal
  install_headers(
    files([
      'pbcopper/cli/internal/Option-inl.h']),
    subdir : 'pbcopper/cli/internal')

  # pbcopper/cli/toolcontract
  install_headers(
    files([
      'pbcopper/cli/toolcontract/Config.h',
      'pbcopper/cli/toolcontract/Driver.h',
      'pbcopper/cli/toolcontract/InputFileType.h',
      'pbcopper/cli/toolcontract/JsonPrinter.h',
      'pbcopper/cli/toolcontract/OutputFileType.h',
      'pbcopper/cli/toolcontract/ResolvedToolContract.h',
      'pbcopper/cli/toolcontract/ResourceType.h',
      'pbcopper/cli/toolcontract/Task.h',
      'pbcopper/cli/toolcontract/TaskType.h']),
    subdir : 'pbcopper/cli/toolcontract')

  # pbcopper/cli/toolcontract/internal
  install_headers(
    files([
      'pbcopper/cli/toolcontract/internal/Driver-inl.h',
      'pbcopper/cli/toolcontract/internal/InputFileType-inl.h',
      'pbcopper/cli/toolcontract/internal/OutputFileType-inl.h',
      'pbcopper/cli/toolcontract/internal/Task-inl.h']),
    subdir : 'pbcopper/cli/toolcontract/internal')

  # pbcopper/data
  install_headers(
    files([
      'pbcopper/data/CCSTag.h',
      'pbcopper/data/Cigar.h',
      'pbcopper/data/CigarOperation.h',
      'pbcopper/data/Interval.h',
      'pbcopper/data/MovieName.h',
      'pbcopper/data/Position.h',
      'pbcopper/data/ReadName.h',
      'pbcopper/data/RSMovieName.h',
      'pbcopper/data/RSReadName.h',
      'pbcopper/data/Zmw.h']),
    subdir : 'pbcopper/data')

  # pbcopper/data/internal
  install_headers(
    files([
      'pbcopper/data/internal/CigarOperation-inl.h',
      'pbcopper/data/internal/Interval-inl.h',
      'pbcopper/data/internal/MovieName-inl.h',
      'pbcopper/data/internal/ReadNameBase-inl.h',
      'pbcopper/data/internal/ReadNameBase.h',
      'pbcopper/data/internal/RSMovieName-inl.h']),
    subdir : 'pbcopper/data/internal')

  # pbcopper/json
  install_headers(
    files([
      'pbcopper/json/JSON.h']),
    subdir : 'pbcopper/json')

  # pbcopper/json/internal
  install_headers(
    files([
      'pbcopper/json/internal/json.hpp']),
    subdir : 'pbcopper/json/internal')

  # pbcopper/logging
  install_headers(
    files([
      'pbcopper/logging/Logging.h']),
    subdir : 'pbcopper/logging')

  # pbcopper/logging/internal
  install_headers(
    files([
      'pbcopper/logging/internal/Logging-inl.h']),
    subdir : 'pbcopper/logging/internal')

  # pbcopper/parallel
  install_headers(
    files([
      'pbcopper/parallel/FireAndForget.h',
      'pbcopper/parallel/WorkQueue.h']),
    subdir : 'pbcopper/parallel')

  # pbcopper/qgram
  install_headers(
    files([
      'pbcopper/qgram/Index.h',
      'pbcopper/qgram/IndexHit.h',
      'pbcopper/qgram/IndexHits.h']),
    subdir : 'pbcopper/qgram')

  # pbcopper/qgram/internal
  install_headers(
    files([
      'pbcopper/qgram/internal/Hashing-inl.h',
      'pbcopper/qgram/internal/Index-inl.h']),
    subdir : 'pbcopper/qgram/internal')

  # pbcopper/stream
  install_headers(
    files([
      'pbcopper/stream/Stream.h']),
    subdir : 'pbcopper/stream')

  # pbcopper/utility
  install_headers(
    files([
      'pbcopper/utility/CallbackTimer.h',
      'pbcopper/utility/EnumClassHash.h',
      'pbcopper/utility/FileUtils.h',
      'pbcopper/utility/MoveAppend.h',
      'pbcopper/utility/PbcopperVersion.h',
      'pbcopper/utility/SafeSubtract.h',
      'pbcopper/utility/Stopwatch.h',
      'pbcopper/utility/StringUtils.h',
      'pbcopper/utility/SystemInfo.h',
      'pbcopper/utility/Unused.h']),
    subdir : 'pbcopper/utility')

  # pbcopper/utility/internal
  install_headers(
    files([
      'pbcopper/utility/internal/CallbackTimer-inl.h',
      'pbcopper/utility/internal/FileUtils-inl.h',
      'pbcopper/utility/internal/Stopwatch-inl.h',
      'pbcopper/utility/internal/StringUtils-inl.h',
      'pbcopper/utility/internal/SystemInfo-inl.h']),
    subdir : 'pbcopper/utility/internal')
endif

pbcopper_include_directories = include_directories('.')
