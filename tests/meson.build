pbcopper_test_CLI_cpp_config = configuration_data()
pbcopper_test_CLI_cpp_config.set('PBCOPPER_VERSION', meson.project_version())

pbcopper_test_CLI_cpp = configure_file(
  input : files('src/cli/test_CLI.cpp.in'),
  output : 'test_CLI.cpp',
  configuration : pbcopper_test_CLI_cpp_config)

pbcopper_test_cpp_sources = files([
  # align
  'src/align/test_Seeds.cpp',

  # cli
  'src/cli/test_HelpPrinter.cpp',
  'src/cli/test_Interface.cpp',
  'src/cli/test_Option.cpp',
  'src/cli/test_ResolvedToolContract.cpp',
  'src/cli/test_Results.cpp',
  'src/cli/test_ToolContractJsonPrinter.cpp',
  'src/cli/test_VersionPrinter.cpp',

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
  'src/data/test_Cigar.cpp',
  'src/data/test_CigarOperation.cpp',
  'src/data/test_Interval.cpp',
  'src/data/test_MovieName.cpp',
  'src/data/test_ReadName.cpp',
  'src/data/test_RSMovieName.cpp',
  'src/data/test_RSReadName.cpp',

  # JSON
  'src/json/test_JSON.cpp',

  # logging
  'src/logging/test_Logging.cpp',

  # parallel
  'src/parallel/test_WorkQueue.cpp',
  'src/parallel/test_FireAndForget.cpp',
  'src/parallel/test_FireAndForgetIndexed.cpp',

  # qgram
  'src/qgram/test_Index.cpp',

  # stream
  'src/stream/test_Stream.cpp',

  # utility
  'src/utility/test_CallbackTimer.cpp',
  'src/utility/test_FileUtils.cpp',
  'src/utility/test_Stopwatch.cpp',
  'src/utility/test_StringUtils.cpp',
  'src/utility/test_SystemInfo.cpp'
])

# find GoogleTest
pbcopper_gtest_dep = dependency('gtest_main', fallback : ['gtest', 'gtest_main_dep'])

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
    pbcopper_test_CLI_cpp,
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
  'pbcopper gtest unittests',
  pbcopper_test,
  args : [
    '--gtest_output=xml:' + join_paths(meson.build_root(), 'pbcopper-gtest-unittests.xml')],
  env : [
    'ARGS=-V',
    'VERBOSE=1'])
