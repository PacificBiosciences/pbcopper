#!/usr/bin/env bash
set -e

echo "################"
echo "# DEPENDENCIES #"
echo "################"

echo "## Load modules"
source /mnt/software/Modules/current/init/bash

module load gcc meson ccache ninja boost doxygen gtest
if [[ $USER == "bamboo" ]]; then
  export CCACHE_DIR=/mnt/secondary/Share/tmp/bamboo.mobs.ccachedir
  export CCACHE_TEMPDIR=/scratch/bamboo.ccache_tempdir
fi
export CCACHE_COMPILERCHECK='%compiler% -dumpversion'
export CCACHE_BASEDIR=$PWD
export CXX="ccache g++"


echo "#########"
echo "# BUILD #"
echo "#########"

case "${bamboo_planRepository_branchName}" in
  develop|master)
    PREFIX_ARG="/mnt/software/p/pbcopper/${bamboo_planRepository_branchName}"
    ;;
  *)
    ;;
esac

echo "## Configure source"
# in order to make shared libraries consumable
# by conda and other package managers
export LDFLAGS="-static-libstdc++ -static-libgcc"

meson \
  --backend ninja \
  --buildtype release \
  -Db_ndebug=true \
  --strip \
  --default-library shared \
  --warnlevel 3 \
  --libdir lib \
  --wrap-mode nofallback \
  --prefix "${PREFIX_ARG:-/usr/local}" \
  build .

echo "## Build source"
ninja -C build -v

if [[ -z ${PREFIX_ARG+x} ]]; then
  echo "Not installing anything (branch: ${bamboo_planRepository_branchName}), exiting."
  exit 0
fi

echo "###########"
echo "# INSTALL #"
echo "###########"

echo "## Cleaning out old installation from /mnt/software"
rm -rf "${PREFIX_ARG}"/*

echo "## Installing to /mnt/software"
ninja -C build -v install

echo "## Creating artifact"
# install into staging dir with --prefix /usr/local
# in order to sidestep all the artifact policy
rm -rf staging
meson configure -Dprefix=/usr/local build
DESTDIR="${PWD}/staging" ninja -C build -v install

( cd staging && tar zcf ../pbcopper-SNAPSHOT.tgz . )
md5sum  pbcopper-SNAPSHOT.tgz | awk -e '{print $1}' >| pbcopper-SNAPSHOT.tgz.md5
sha1sum pbcopper-SNAPSHOT.tgz | awk -e '{print $1}' >| pbcopper-SNAPSHOT.tgz.sha1

UNSUPPORTED_URL=http://ossnexus.pacificbiosciences.com/repository/unsupported
curl -vn --upload-file pbcopper-SNAPSHOT.tgz      $UNSUPPORTED_URL/gcc-6.4.0/pbcopper-SNAPSHOT.tgz
curl -vn --upload-file pbcopper-SNAPSHOT.tgz.md5  $UNSUPPORTED_URL/gcc-6.4.0/pbcopper-SNAPSHOT.tgz.md5
curl -vn --upload-file pbcopper-SNAPSHOT.tgz.sha1 $UNSUPPORTED_URL/gcc-6.4.0/pbcopper-SNAPSHOT.tgz.sha1
