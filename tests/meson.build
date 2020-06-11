pbcopper_test_cpp_sources = files([
  # align
  'src/align/test_Alignment.cpp',
  'src/align/test_BandedChainAlign.cpp',
  'src/align/test_Seeds.cpp',

  #cli2
  'src/cli2/test_CLI.cpp',
  'src/cli2/test_CommandLineParser.cpp',
  'src/cli2/test_InterfaceHelpPrinter.cpp',
  'src/cli2/test_MultiToolInterfaceHelpPrinter.cpp',
  'src/cli2/test_OptionTranslator.cpp',
  'src/cli2/test_OptionValue.cpp',
  'src/cli2/test_PositionalArgumentTranslator.cpp',
  'src/cli2/test_Results.cpp',
  'src/cli2/test_VersionPrinter.cpp',

  # data
  'src/dagcon/test_Alignment.cpp',
  'src/dagcon/test_AlignmentGraph.cpp',

  # data
  'src/data/test_Accuracy.cpp',
  'src/data/test_Cigar.cpp',
  'src/data/test_Frames.cpp',
  'src/data/test_GenomicInterval.cpp',
  'src/data/test_Interval.cpp',
  'src/data/test_MappedRead.cpp',
  'src/data/test_MovieName.cpp',
  'src/data/test_QualityValues.cpp',
  'src/data/test_Read.cpp',
  'src/data/test_ReadName.cpp',
  'src/data/test_RSMovieName.cpp',
  'src/data/test_RSReadName.cpp',
  'src/data/test_SNR.cpp',

  # JSON
  'src/json/test_JSON.cpp',

  # library
  'src/library/test_Library.cpp',

  # logging
  'src/logging/test_Logging.cpp',

  # parallel
  'src/parallel/test_WorkQueue.cpp',
  'src/parallel/test_FireAndForget.cpp',
  'src/parallel/test_FireAndForgetIndexed.cpp',

  # pbmer
  'src/pbmer/test_Dbg.cpp',
  'src/pbmer/test_DbgNode.cpp',
  'src/pbmer/test_DnaBit.cpp',
  'src/pbmer/test_KFGraph.cpp',
  'src/pbmer/test_Kmer.cpp',
  'src/pbmer/test_Mers.cpp',
  'src/pbmer/test_Parser.cpp',

  # qgram
  'src/qgram/test_Index.cpp',

  # reports
  'src/reports/test_Report.cpp',

  # utility
  'src/utility/test_Alarm.cpp',
  'src/utility/test_Base64.cpp',
  'src/utility/test_FileUtils.cpp',
  'src/utility/test_MinMax.cpp',
  'src/utility/test_MoveAppend.cpp',
  'src/utility/test_MoveInsert.cpp',
  'src/utility/test_OStreamRedirect.cpp',
  'src/utility/test_PartitionElements.cpp',
  'src/utility/test_Ssize.cpp',
  'src/utility/test_Stopwatch.cpp',
  'src/utility/test_StringUtils.cpp',
])

# find GoogleTest
pbcopper_gtest_dep = dependency(
  'gtest',
  main : true,
  fallback : ['gtest', 'gtest_main_dep'])

pbcopper_PbcopperTestData_h_config = configuration_data()
pbcopper_PbcopperTestData_h_config.set('Pbcopper_TestsDir', meson.current_source_dir())
pbcopper_PbcopperTestData_h_config.set('CMAKE_CURRENT_BINARY_DIR', meson.current_build_dir())
pbcopper_PbcopperTestData_h_config.set('GeneratedDir', meson.current_build_dir())
pbcopper_PbcopperTestData_h_config.set('GeneratedTestDataDir', meson.current_build_dir())

pbcopper_PbcopperTestData_h = configure_file(
  input : files('include/PbcopperTestData.h.in'),
  output : 'PbcopperTestData.h',
  configuration : pbcopper_PbcopperTestData_h_config)
pbcopper_test_cpp_sources += pbcopper_PbcopperTestData_h

pbcopper_test = executable(
  'pbcopper_test', [
    pbcopper_test_cpp_sources],
  dependencies : [pbcopper_gtest_dep, pbcopper_thread_dep, pbcopper_boost_dep],
  include_directories : [pbcopper_include_directories, include_directories('include')],
  link_with : [pbcopper_lib],
  cpp_args : pbcopper_warning_flags,
  install : false)

#########
# tests #
#########

test(
  'pbcopper formatting check',
  pbcopper_clang_formatter,
  args : [
    '--all'],
  workdir : meson.source_root())

test(
  'pbcopper EOF check',
  pbcopper_eof_check,
  workdir : meson.source_root())

test(
  'pbcopper gtest unittests',
  pbcopper_test,
  args : [
    '--gtest_output=xml:' + join_paths(meson.build_root(), 'pbcopper-gtest-unittests.xml')],
  env : [
    'ARGS=-V',
    'VERBOSE=1'])
