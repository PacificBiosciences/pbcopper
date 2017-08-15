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

pbcopper_PbcopperVersion_h = configure_file(
  input : 'PbcopperVersion.h.in',
  output : 'PbcopperVersion.h',
  configuration : pbcopper_config)

# replace git commit id
pbcopper_PbcopperGitHash_h = vcs_tag(
  input : 'PbcopperGitHash.h.in',
  output : 'PbcopperGitHash.h',
  command : ['git', 'describe', '--always', '--dirty=*'],
  replace_string : '@COPPER_GIT_SHA1@')
