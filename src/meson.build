###########
# sources #
###########

pbcopper_cpp_sources = files([

  'LibraryInfo.cpp',

  # -------
  # align
  # -------
  'align/AffineAlignment.cpp',
  'align/AlignConfig.cpp',
  'align/BandedChainAlignment.cpp',
  'align/ChainSeeds.cpp',
  'align/ChainSeedsConfig.cpp',
  'align/EdlibAlign.cpp',
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
  # dagcon
  # -------
  'dagcon/Alignment.cpp',
  'dagcon/AlignmentGraph.cpp',

  # -------
  # data
  # -------
  'data/Accuracy.cpp',
  'data/Cigar.cpp',
  'data/CigarOperation.cpp',
  'data/Clipping.cpp',
  'data/FrameEncoders.cpp',
  'data/Frames.cpp',
  'data/GenomicInterval.cpp',
  'data/Interval.cpp',
  'data/IntervalTree.cpp',
  'data/MappedRead.cpp',
  'data/MovieName.cpp',
  'data/QualityValue.cpp',
  'data/QualityValues.cpp',
  'data/Read.cpp',
  'data/ReadId.cpp',
  'data/RSMovieName.cpp',
  'data/SNR.cpp',
  'data/Strand.cpp',

  # ---------
  # logging
  # ---------
  'logging/Logging.cpp',
  'logging/LogConfig.cpp',
  'logging/LogLevel.cpp',
  'logging/LogMessage.cpp',

  # ---------
  # parallel
  # ---------
  'parallel/FireAndForget.cpp',
  'parallel/ThreadCount.cpp',

  # ---------
  # pbmer
  # ---------
  'pbmer/Dbg.cpp',
  'pbmer/DbgNode.cpp',
  'pbmer/DnaBit.cpp',
  'pbmer/Kmer.cpp',
  'pbmer/KFGraph.cpp',
  'pbmer/KFNode.cpp',
  'pbmer/Mers.cpp',
  'pbmer/SparseHaplotype.cpp',
  'pbmer/Parser.cpp',

  # ---------
  # poa
  # ---------
  'poa/PoaAlignmentMatrix.cpp',
  'poa/PoaConsensus.cpp',
  'poa/PoaGraph.cpp',
  'poa/PoaGraphImpl.cpp',
  'poa/PoaGraphTraversals.cpp',
  'poa/RangeFinder.cpp',
  'poa/SparsePoa.cpp',

  # ---------
  # reports
  # ---------
  'reports/Report.cpp',
  'reports/ReportPrinter.cpp',
  'reports/TaskReport.cpp',

  # -------------
  # third-party
  # -------------
  'third-party/base64/base64.cpp',
  'third-party/edlib/edlib.cpp',
  'third-party/ksw2/kalloc.c',
  'third-party/ksw2/ksw2_simde.c',
  'third-party/sparc/sparc.cpp',
  'third-party/sparc/sparcSimple.cpp',

  # ---------
  # utility
  # ---------
  'utility/Alarm.cpp',
  'utility/Base64.cpp',
  'utility/Deleters.cpp',
  'utility/MemoryConsumption.cpp',
  'utility/Stopwatch.cpp',
  'utility/StringUtils.cpp',
  'utility/TimeReporter.cpp',
])

# replace version strings
pbcopper_ver_arr = meson.project_version().split('.')
pbcopper_major_version = pbcopper_ver_arr[0]
pbcopper_minor_version = pbcopper_ver_arr[1]
pbcopper_patch_version = pbcopper_ver_arr[2]

pbcopper_config = configuration_data()
pbcopper_config.set('pbcopper_VERSION', meson.project_version())
pbcopper_config.set('pbcopper_VERSION_MAJOR', pbcopper_major_version)
pbcopper_config.set('pbcopper_VERSION_MINOR', pbcopper_minor_version)
pbcopper_config.set('pbcopper_VERSION_PATCH', pbcopper_patch_version)


subdir('utility')

pbcopper_cpp_sources += configure_file(
  input : 'LibraryVersion.h.in',
  output : 'LibraryVersion.h',
  configuration : pbcopper_config)

pbcopper_cpp_sources += vcs_tag(
  input : 'LibraryGitHash.h.in',
  output : 'LibraryGitHash.h',
  command : ['git', 'describe', '--always', '--dirty=*'],
  replace_string : '@COPPER_GIT_SHA1@')

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
  c_args : pbcopper_c_flags,
  cpp_args : pbcopper_cpp_flags)
