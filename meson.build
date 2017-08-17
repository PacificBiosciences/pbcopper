project(
  'pbcopper',
  'cpp',
  version : '0.1.0',
  default_options : [
    'buildtype=release',
    'warning_level=3',
    'cpp_std=c++14'],
  license : 'BSD-3',
  meson_version : '>= 0.41.1')

############
# CXXFLAGS #
############

pbcopper_warning_flags = []
cpp = meson.get_compiler('cpp')
foreach cflag: [
  '-Wno-terminate',
  '-Wno-sign-compare',
  '-Wno-missing-field-initializers']
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

############
# examples #
############

if get_option('enable-build-examples')
  subdir('examples')
endif

#################
# documentation #
#################

if get_option('enable-build-docs')
  subdir('docs')
endif

#########
# tests #
#########

if get_option('enable-tests')
  subdir('tests')
endif

###################
# dependency info #
###################

import('pkgconfig').generate(
  libraries : pbcopper_lib,
  version : meson.project_version(),
  name : 'pbcopper',
  filebase : 'pbcopper',
  description : 'C++ library providing a suite of data structures, algorithms, and utilities')

pbcopper_dep = declare_dependency(
  include_directories : pbcopper_include_directories,
  link_with : pbcopper_lib,
  dependencies : [pbcopper_thread_dep, pbcopper_boost_dep],
  version : meson.project_version(),
  sources : pbcopper_cpp_sources,
  compile_args : pbcopper_warning_flags)
