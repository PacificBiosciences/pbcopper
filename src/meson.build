###########
# sources #
###########

pbcopper_cpp_sources = files([
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
  'data/Cigar.cpp',
  'data/CigarOperation.cpp',
  'data/MovieName.cpp',
  'data/RSMovieName.cpp',

  # ---------
  # logging
  # ---------
  'logging/Logging.cpp',

  # ---------
  # reports
  # ---------
  'reports/Report.cpp',
  'reports/ReportPrinter.cpp',

  # ---------
  # utility
  # ---------
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
