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
  # utility
  # ---------
  'utility/CallbackTimer.cpp'])

pbcopper_lib = library(
  'pbcopper', [
    pbcopper_PbcopperVersion_h,
    pbcopper_PbcopperGitHash_h,
    pbcopper_cpp_sources],
  soversion : 0,
  version : meson.project_version(),
  install : true,
  dependencies : [pbcopper_thread_dep, pbcopper_boost_dep],
  include_directories : pbcopper_include_directories,
  cpp_args : ['-DPBCOPPER_LIBRARY', pbcopper_warning_flags])
