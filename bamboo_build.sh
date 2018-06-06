#!/usr/bin/env bash
set -vex

################
# DEPENDENCIES #
################

## Load modules
type module >& /dev/null || . /mnt/software/Modules/current/init/bash

module purge

module load meson
module load ninja

module load boost
module load doxygen


BOOST_ROOT="${BOOST_ROOT%/include}"
# unset these variables to have meson discover all
# boost-dependent variables from BOOST_ROOT alone
unset BOOST_INCLUDEDIR
unset BOOST_LIBRARYDIR

export CC="ccache gcc"
export CXX="ccache g++"
export CCACHE_BASEDIR="${PWD}"

if [[ $USER == bamboo ]]; then
  export CCACHE_DIR=/mnt/secondary/Share/tmp/bamboo.${bamboo_shortPlanKey}.ccachedir
  export CCACHE_TEMPDIR=/scratch/bamboo.ccache_tempdir
fi

case "${bamboo_planRepository_branchName}" in
  develop|master)
    export PREFIX_ARG="/mnt/software/p/pbcopper/${bamboo_planRepository_branchName}"
    export BUILD_NUMBER="${bamboo_globalBuildNumber:-0}"
    ;;
  *)
    export BUILD_NUMBER="0"
    ;;
esac

# in order to make shared libraries consumable
# by conda and other package managers
export LDFLAGS="-static-libstdc++ -static-libgcc"

for i in "system-gcc" "gcc/8.1.0" "gcc"; do
  # 1. load either current MOBS GCC or RHEL-default GCC
  if [[ ${i} == system-gcc ]]; then
    module load gtest/gcc48
  else
    module load ${i} gtest
  fi
  module load ccache

  export CURRENT_BUILD_DIR="build_gcc=${i/\//_}"
  export ENABLED_TESTS="true"

  bash scripts/ci/build.sh
  bash scripts/ci/test.sh

  module unload ccache gtest
  [[ ${i} != system-gcc ]] && module unload gcc
done

# create symlink so Bamboo can find the xunit output
ln -s "${CURRENT_BUILD_DIR}" build

if [[ -z ${PREFIX_ARG+x} ]]; then
  echo "Not installing anything (branch: ${bamboo_planRepository_branchName}), exiting."
  exit 0
fi

# reload GCC to prevent default `g++` from mis-linking
module load gcc

bash scripts/ci/install.sh

if [[ ${BUILD_NUMBER} == 0 ]]; then
  echo "Build number is 0, hence not creating artifact"
  exit 0
fi

## Creating artifact
# install into staging dir with --prefix /usr/local
# in order to sidestep all the artifact policy
rm -rf staging
meson configure -Dprefix=/usr/local -Dtests=false "${CURRENT_BUILD_DIR}"
DESTDIR="${PWD}/staging" ninja -C "${CURRENT_BUILD_DIR}" -v install

( cd staging && tar zcf ../pbcopper-SNAPSHOT.tgz . )
md5sum  pbcopper-SNAPSHOT.tgz | awk -e '{print $1}' >| pbcopper-SNAPSHOT.tgz.md5
sha1sum pbcopper-SNAPSHOT.tgz | awk -e '{print $1}' >| pbcopper-SNAPSHOT.tgz.sha1

UNSUPPORTED_URL=http://ossnexus.pacificbiosciences.com/repository/unsupported
curl -vn --upload-file pbcopper-SNAPSHOT.tgz      $UNSUPPORTED_URL/gcc-6.4.0/pbcopper-SNAPSHOT.tgz
curl -vn --upload-file pbcopper-SNAPSHOT.tgz.md5  $UNSUPPORTED_URL/gcc-6.4.0/pbcopper-SNAPSHOT.tgz.md5
curl -vn --upload-file pbcopper-SNAPSHOT.tgz.sha1 $UNSUPPORTED_URL/gcc-6.4.0/pbcopper-SNAPSHOT.tgz.sha1
