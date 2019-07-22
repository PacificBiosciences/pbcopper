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
      'pbcopper/align/AffineAlignment.h',
      'pbcopper/align/AlignConfig.h',
      'pbcopper/align/BandedChainAlignment.h',
      'pbcopper/align/ChainSeeds.h',
      'pbcopper/align/ChainSeedsConfig.h',
      'pbcopper/align/FindSeeds.h',
      'pbcopper/align/LinearAlignment.h',
      'pbcopper/align/LocalAlignment.h',
      'pbcopper/align/PairwiseAlignment.h',
      'pbcopper/align/Seed.h',
      'pbcopper/align/Seeds.h',
      'pbcopper/align/SparseAlignment.h']),
    subdir : 'pbcopper/align')

  # pbcopper/align/internal
  install_headers(
    files([
      'pbcopper/align/internal/BCAlignBlocks.h',
      'pbcopper/align/internal/BCAlignImpl.h']),
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

  # pbcopper/cli2
  install_headers(
    files([
      'pbcopper/cli2/CLI.h',
      'pbcopper/cli2/Interface.h',
      'pbcopper/cli2/MultiToolInterface.h',
      'pbcopper/cli2/Option.h',
      'pbcopper/cli2/OptionValue.h',
      'pbcopper/cli2/OptionGroup.h',
      'pbcopper/cli2/PositionalArgument.h',
      'pbcopper/cli2/Result.h',
      'pbcopper/cli2/Results.h',
      'pbcopper/cli2/ResultsHandler.h',
      'pbcopper/cli2/Tool.h'
      ]),
    subdir : 'pbcopper/cli2')

  # pbcopper/cli2/internal
  install_headers(
    files([
      'pbcopper/cli2/internal/BuiltinOptions.h',
      'pbcopper/cli2/internal/CommandLineParser.h',
      'pbcopper/cli2/internal/InterfaceData.h',
      'pbcopper/cli2/internal/InterfaceHelpPrinter.h',
      'pbcopper/cli2/internal/MultiToolInterfaceData.h',
      'pbcopper/cli2/internal/MultiToolInterfaceHelpPrinter.h',
      'pbcopper/cli2/internal/OptionData.h',
      'pbcopper/cli2/internal/OptionTranslator.h',
      'pbcopper/cli2/internal/PositionalArgumentData.h',
      'pbcopper/cli2/internal/PositionalArgumentTranslator.h',
      'pbcopper/cli2/internal/VersionPrinter.h',]),
    subdir : 'pbcopper/cli2/internal')

  # pbcopper/data
  install_headers(
    files([
      'pbcopper/data/Accuracy.h',
      'pbcopper/data/CCSTag.h',
      'pbcopper/data/Cigar.h',
      'pbcopper/data/CigarOperation.h',
      'pbcopper/data/Clipping.h',
      'pbcopper/data/Frames.h',
      'pbcopper/data/GenomicInterval.h',
      'pbcopper/data/Interval.h',
      'pbcopper/data/LocalContextFlags.h',
      'pbcopper/data/MappedRead.h',
      'pbcopper/data/MovieName.h',
      'pbcopper/data/Position.h',
      'pbcopper/data/QualityValue.h',
      'pbcopper/data/QualityValues.h',
      'pbcopper/data/Read.h',
      'pbcopper/data/ReadId.h',
      'pbcopper/data/ReadName.h',
      'pbcopper/data/RSMovieName.h',
      'pbcopper/data/RSReadName.h',
      'pbcopper/data/SNR.h',
      'pbcopper/data/Strand.h',
      'pbcopper/data/Zmw.h']),
    subdir : 'pbcopper/data')

  # pbcopper/data/internal
  install_headers(
    files([
      'pbcopper/data/internal/ClippingImpl.h',
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
      'pbcopper/logging/Logging.h',
      'pbcopper/logging/LogConfig.h',
      'pbcopper/logging/LogField.h',
      'pbcopper/logging/LogLevel.h',
      'pbcopper/logging/LogMessage.h']),
    subdir : 'pbcopper/logging')

  # pbcopper/parallel
  install_headers(
    files([
      'pbcopper/parallel/FireAndForget.h',
      'pbcopper/parallel/FireAndForgetIndexed.h',
      'pbcopper/parallel/WorkQueue.h']),
    subdir : 'pbcopper/parallel')

  # pbcopper/reports
  install_headers(
    files([
      'pbcopper/reports/Report.h']),
    subdir : 'pbcopper/reports')

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

  # pbcopper/utility
  install_headers(
    files([
      'pbcopper/utility/Deleters.h',
      'pbcopper/utility/FileUtils.h',
      'pbcopper/utility/MinMax.h',
      'pbcopper/utility/MoveAppend.h',
      'pbcopper/utility/PbcopperVersion.h',
      'pbcopper/utility/SafeSubtract.h',
      'pbcopper/utility/SequenceUtils.h',
      'pbcopper/utility/Stopwatch.h',
      'pbcopper/utility/StringUtils.h',
      'pbcopper/utility/SystemInfo.h',
      'pbcopper/utility/Unused.h']),
    subdir : 'pbcopper/utility')

  # pbcopper/utility/internal
  install_headers(
    files([
      'pbcopper/utility/internal/FileUtils-inl.h',
      'pbcopper/utility/internal/Stopwatch-inl.h',
      'pbcopper/utility/internal/SystemInfo-inl.h']),
    subdir : 'pbcopper/utility/internal')
endif

pbcopper_include_directories = include_directories('.')
