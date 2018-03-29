#!/usr/bin/env bash

echo "################"
echo "# DEPENDENCIES #"
echo "################"

echo "## Load modules"
source /mnt/software/Modules/current/init/bash

module load gcc meson ccache ninja gtest
if [[ $USER == "bamboo" ]]; then
  export CCACHE_DIR=/mnt/secondary/Share/tmp/bamboo.mobs.ccachedir
  export CCACHE_TEMPDIR=/scratch/bamboo.ccache_tempdir
fi
export CCACHE_COMPILERCHECK='%compiler% -dumpversion'
export CCACHE_BASEDIR=$PWD


echo "#########"
echo "# TESTS #"
echo "#########"

echo "## Unit tests"
ninja -C build test
