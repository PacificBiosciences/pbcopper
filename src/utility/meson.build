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

pbcopper_cpp_sources += configure_file(
  input : 'PbcopperVersion.cpp.in',
  output : 'PbcopperVersion.cpp',
  configuration : pbcopper_config)

pbcopper_cpp_sources += vcs_tag(
  input : 'PbcopperGitHash.cpp.in',
  output : 'PbcopperGitHash.cpp',
  command : ['git', 'describe', '--always', '--dirty=*'],
  replace_string : '@COPPER_GIT_SHA1@')
