#!/bin/bash

echo "# DEPENDENCIES"
echo "## Load modules"
source /mnt/software/Modules/current/init/bash
module load gcc/6.4.0 cmake ccache_base ninja
if [[ $USER == "bamboo" ]]; then
  export CCACHE_DIR=/mnt/secondary/Share/tmp/bamboo.mobs.ccachedir
  export CCACHE_TEMPDIR=/scratch/bamboo.ccache_tempdir
fi
export CCACHE_COMPILERCHECK='%compiler% -dumpversion'
export CCACHE_BASEDIR=$PWD

echo "# TEST"
echo "## Unit tests"
( cd build &&  ninja check-xunit )
