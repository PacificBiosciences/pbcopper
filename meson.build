project(
  'pbcopper',
  'cpp',
  version : '0.4.0',
  default_options : [
    'buildtype=release',
    'warning_level=3',
    'cpp_std=c++11',
    'b_ndebug=if-release'],
  license : 'BSD-3',
  meson_version : '>= 0.46.0')

############
# CXXFLAGS #
############

pbcopper_warning_flags = []
cpp = meson.get_compiler('cpp')
foreach cflag: [
  '-Wduplicated-cond',
  '-Wduplicated-branches',
  '-Wlogical-op',
  '-Wrestrict',
  '-Wnull-dereference',
  '-Wuseless-cast',
  '-Wdouble-promotion',
  '-Wshadow',
  '-Wformat=1']
    if cpp.has_argument(cflag)
      pbcopper_warning_flags += cflag
    endif
endforeach

################
# dependencies #
################

# threads
pbcopper_thread_dep = dependency('threads', required : true)

# boost
pbcopper_boost_dep = dependency('boost', required : true)

###########
# headers #
###########

subdir('include')

#####################
# sources + library #
#####################

subdir('src')

##########
# extras #
##########

if not meson.is_subproject()
  # examples
  if get_option('build-examples')
    subdir('examples')
  endif

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
