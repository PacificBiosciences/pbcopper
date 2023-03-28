project(
  'pbcopper',
  ['cpp', 'c'],
  version : '2.2.0',
  default_options : [
    'buildtype=release',
    'warning_level=3',
    'cpp_std=c++20',
    'c_std=c17',
    'b_ndebug=if-release'],
  license : 'BSD-3',
  meson_version : '>= 0.57.0')

############
# CXXFLAGS #
############

pbcopper_cpp_test_flags = [
  '-Wduplicated-cond',
  '-Wduplicated-branches',
  '-Wlogical-op',
  '-Wrestrict',
  '-Wdouble-promotion',
  '-Wshadow',
  '-Wformat=1',
]

pbcopper_cpp_flags = []
cpp = meson.get_compiler('cpp')
foreach cflag: pbcopper_cpp_test_flags
  if cpp.has_argument(cflag)
    pbcopper_cpp_flags += cflag
  endif
endforeach

pbcopper_arch_flags = {
  # ARM64 (ARMv8-A, Apple M1)
  'aarch64': [
    '-march=armv8.2-a',
  ],
  'x86_64': [
    '-msse4.1',
  ],
}

pbcopper_c_test_flags = [
  '-Wc++-compat',
  '-Wno-unused-parameter',
  '-Wno-unused-variable',
  '-Wno-sign-compare',
  '-Wno-unused-but-set-variable'
] + pbcopper_arch_flags.get(host_machine.cpu_family(), [])

pbcopper_c_flags = []
c = meson.get_compiler('c')
foreach cflag: pbcopper_c_test_flags
    if c.has_argument(cflag)
      pbcopper_c_flags += cflag
    endif
endforeach

################
# dependencies #
################

# threads
pbcopper_thread_dep = dependency('threads')

# boost
pbcopper_boost_dep = dependency('boost', include_type : 'system')

###########
# headers #
###########

subdir('include')

#####################
# sources + library #
#####################

subdir('src')

###############
# third-party #
###############

subdir('third-party')

##########
# extras #
##########

if not meson.is_subproject()
  # documentation
  if get_option('build-docs')
    subdir('docs')
  endif

  # tests
  if get_option('tests')
    pbcopper_clang_formatter = find_program('tools/check-formatting')
    subdir('tests')
  endif
endif

###################
# dependency info #
###################

if not meson.is_subproject()
  import('pkgconfig').generate(
    libraries : pbcopper_lib,
    version : meson.project_version(),
    name : 'pbcopper',
    filebase : 'pbcopper',
    description : 'C++ library providing a suite of data structures, algorithms, and utilities')
endif

pbcopper_dep = declare_dependency(
  include_directories : pbcopper_include_directories,
  link_with : pbcopper_lib,
  dependencies : [pbcopper_thread_dep, pbcopper_boost_dep],
  version : meson.project_version())
