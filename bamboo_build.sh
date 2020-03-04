#!/usr/bin/env bash
set -vex

################
# DEPENDENCIES #
################

## Load modules
type module >& /dev/null || . /mnt/software/Modules/current/init/bash

module purge

[[ ${GCC_VERSION} == ICC ]] && module use /pbi/dept/primary/modulefiles
module use /mnt/software/modulefiles

module load meson
module load ninja

module load boost
module load doxygen


BOOST_ROOT="${BOOST_ROOT%/include}"
# unset these variables to have meson discover all
# boost-dependent variables from BOOST_ROOT alone
unset BOOST_INCLUDEDIR
unset BOOST_LIBRARYDIR

# in order to make shared libraries consumable
# by conda and other package managers
export LDFLAGS=${LDFLAGS:-"-fuse-ld=gold -static-libstdc++ -static-libgcc"}

source scripts/ci/setup.sh
source scripts/ci/build.sh
source scripts/ci/test.sh

if [[ ${BUILD_NUMBER} == 0 ]]; then
  echo "Not installing anything (branch: ${bamboo_planRepository_branchName}), exiting."
  exit 0
fi

source scripts/ci/install.sh

## Creating artifact
# install into staging dir with --prefix /usr/local
# in order to sidestep all the artifact policy
rm -rf staging
meson configure -Dprefix=/usr/local "${CURRENT_BUILD_DIR:-build}"
DESTDIR="${PWD}/staging" ninja -C "${CURRENT_BUILD_DIR:-build}" -v install

( cd staging && tar zcf ../pbcopper-SNAPSHOT.tgz . )
md5sum  pbcopper-SNAPSHOT.tgz | awk -e '{print $1}' >| pbcopper-SNAPSHOT.tgz.md5
sha1sum pbcopper-SNAPSHOT.tgz | awk -e '{print $1}' >| pbcopper-SNAPSHOT.tgz.sha1

UNSUPPORTED_URL=http://ossnexus.pacificbiosciences.com/repository/unsupported
curl -vn --upload-file pbcopper-SNAPSHOT.tgz      $UNSUPPORTED_URL/gcc-6.4.0/pbcopper-SNAPSHOT.tgz
curl -vn --upload-file pbcopper-SNAPSHOT.tgz.md5  $UNSUPPORTED_URL/gcc-6.4.0/pbcopper-SNAPSHOT.tgz.md5
curl -vn --upload-file pbcopper-SNAPSHOT.tgz.sha1 $UNSUPPORTED_URL/gcc-6.4.0/pbcopper-SNAPSHOT.tgz.sha1
