###########
# sources #
###########

pbcopper_cpp_sources = files([

  # -------
  # align
  # -------
  'align/AffineAlignment.cpp',
  'align/AlignConfig.cpp',
  'align/BandedChainAlignment.cpp',
  'align/ChainSeeds.cpp',
  'align/ChainSeedsConfig.cpp',
  'align/FindSeeds.cpp',
  'align/LinearAlignment.cpp',
  'align/LocalAlignment.cpp',
  'align/PairwiseAlignment.cpp',
  'align/Seed.cpp',
  'align/Seeds.cpp',
  'align/SparseAlignment.cpp',

  # ------------
  # align/cssw
  # ------------
  'align/cssw/ssw.c',
  'align/cssw/ssw_cpp.cpp',

  # -------
  # cli
  # -------
  'cli/CLI.cpp',
  'cli/HelpPrinter.cpp',
  'cli/Interface.cpp',
  'cli/Option.cpp',
  'cli/Parser.cpp',
  'cli/Results.cpp',
  'cli/VersionPrinter.cpp',

  # ------------------
  # cli/toolcontract
  # ------------------
  'cli/toolcontract/JsonPrinter.cpp',
  'cli/toolcontract/ResolvedToolContract.cpp',

  # -------
  # cli2
  # -------

  'cli2/CLI.cpp',
  'cli2/CommandLineParser.cpp',
  'cli2/HelpMetrics.cpp',
  'cli2/Interface.cpp',
  'cli2/InterfaceHelpPrinter.cpp',
  'cli2/MultiToolInterface.cpp',
  'cli2/MultiToolInterfaceHelpPrinter.cpp',
  'cli2/OptionTranslator.cpp',
  'cli2/Option.cpp',
  'cli2/OptionValue.cpp',
  'cli2/PositionalArgumentTranslator.cpp',
  'cli2/Result.cpp',
  'cli2/Results.cpp',
  'cli2/VersionPrinter.cpp',

  # -------
  # data
  # -------
  'data/Accuracy.cpp',
  'data/Cigar.cpp',
  'data/CigarOperation.cpp',
  'data/Clipping.cpp',
  'data/Frames.cpp',
  'data/GenomicInterval.cpp',
  'data/Interval.cpp',
  'data/MappedRead.cpp',
  'data/MovieName.cpp',
  'data/QualityValue.cpp',
  'data/QualityValues.cpp',
  'data/Read.cpp',
  'data/ReadId.cpp',
  'data/RSMovieName.cpp',
  'data/SNR.cpp',

  # ---------
  # logging
  # ---------
  'logging/Logging.cpp',
  'logging/LogConfig.cpp',
  'logging/LogLevel.cpp',
  'logging/LogMessage.cpp',

  # ---------
  # pbmer
  # ---------
  'pbmer/Dbg.cpp',
  'pbmer/DnaBit.cpp',
  'pbmer/Kmer.cpp',
  'pbmer/Mers.cpp',
  'pbmer/Parser.cpp',

  # ---------
  # reports
  # ---------
  'reports/Report.cpp',
  'reports/ReportPrinter.cpp',
  'reports/TaskReport.cpp',

  # -------------
  # third-party
  # -------------
  'third-party/edlib.cpp',

  # ---------
  # utility
  # ---------
  'utility/Alarm.cpp',
  'utility/Deleters.cpp',
  'utility/MemoryConsumption.cpp',
  'utility/Stopwatch.cpp',
  'utility/StringUtils.cpp'])

subdir('utility')

# install library if
# - either running as a proper project
# - or using shared libraries
pbcopper_lib_install = (not meson.is_subproject()) or (get_option('default_library') == 'shared')

pbcopper_lib = library(
  'pbcopper',
  pbcopper_cpp_sources,
  # use boost SONAME practice:
  #   cause ld startup issues before
  #   you even have the chance of running
  #   into ABI issues.
  soversion : meson.project_version(),
  version : meson.project_version(),
  install : pbcopper_lib_install,
  dependencies : [pbcopper_thread_dep, pbcopper_boost_dep],
  include_directories : pbcopper_include_directories,
  cpp_args : pbcopper_warning_flags)
