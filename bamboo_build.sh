#!/bin/bash

echo "# DEPENDENCIES"
echo "## Load modules"
source /mnt/software/Modules/current/init/bash
module load gcc/4.9.2 cmake ccache ninja boost doxygen
export CC=gcc CXX=g++

echo "# BUILD"
echo "## Create build directory "
if [ ! -d build ] ; then mkdir build ; fi

echo "## Build source"
( cd build &&\
  rm -rf * &&\
  CMAKE_BUILD_TYPE=ReleaseWithAssert cmake -GNinja .. )

( cd build && ninja )

rm -rf staging
mkdir staging
find include/pbcopper \( -name '*.h' -o -name '*.hpp' \) \
  | xargs tar cf - \
  | tar xvf - -C staging
( cd build && \
tar cf - lib/libpbcopper.a \
  | tar xvf - -C ../staging )
( cd staging && tar zcf ../pbcopper-SNAPSHOT.tgz lib include )
md5sum  pbcopper-SNAPSHOT.tgz | awk -e '{print $1}' >| pbcopper-SNAPSHOT.tgz.md5
sha1sum pbcopper-SNAPSHOT.tgz | awk -e '{print $1}' >| pbcopper-SNAPSHOT.tgz.sha1

UNSUPPORTED_URL=http://ossnexus.pacificbiosciences.com/repository/unsupported
curl -vn --upload-file pbcopper-SNAPSHOT.tgz      $UNSUPPORTED_URL/gcc-4.9.2/pbcopper-SNAPSHOT.tgz
curl -vn --upload-file pbcopper-SNAPSHOT.tgz.md5  $UNSUPPORTED_URL/gcc-4.9.2/pbcopper-SNAPSHOT.tgz.md5
curl -vn --upload-file pbcopper-SNAPSHOT.tgz.sha1 $UNSUPPORTED_URL/gcc-4.9.2/pbcopper-SNAPSHOT.tgz.sha1
