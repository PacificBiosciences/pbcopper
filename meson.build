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

################
# dependencies #
################

# threads
pbcopper_thread_dep = dependency('threads', required : true)

# boost
pbcopper_boost_dep = dependency('boost', required : true)

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

if get_option('enable-pbcopper-build-examples')
  subdir('examples')
endif

#################
# documentation #
#################

if get_option('enable-pbcopper-build-docs')
  subdir('docs')
endif

#########
# tests #
#########

if get_option('enable-pbcopper-tests')
  subdir('tests')
endif
