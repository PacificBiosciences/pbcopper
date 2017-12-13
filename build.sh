#!/bin/bash

echo "# DEPENDENCIES"
echo "## Load modules"
source /mnt/software/Modules/current/init/bash
module load gcc/6.4.0 cmake ccache ninja boost doxygen
export CC=gcc CXX=g++

echo "# BUILD"
echo "## Create build directory "
if [ ! -d build ] ; then mkdir build ; fi

echo "## Build source"
( cd build &&\
  rm -rf * &&\
  CMAKE_BUILD_TYPE=ReleaseWithAssert cmake -GNinja .. )

( cd build && ninja )

echo "# TEST"
echo "# Run unit tests"
( cd build &&  ninja check-xunit )
