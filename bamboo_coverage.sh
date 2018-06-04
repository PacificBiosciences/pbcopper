#!/usr/bin/env bash
set -vex

## Load modules
type module >& /dev/null || . /mnt/software/Modules/current/init/bash

module purge

module load gcc
module load ccache

module load meson
module load ninja

module load boost
module load doxygen
module load gcov
module load gtest

#####################
# BUILD & RUN TESTS #
#####################

rm -rf build
mkdir build 
cd build

meson \
  --werror \
  --backend ninja \
  --buildtype debug \
  --default-library shared \
  --libdir lib \
  --wrap-mode nofallback \
  --prefix "${PREFIX_ARG:-/usr/local}" \
  -Db_coverage=true \
  ..

ninja test

############
# COVERAGE #
############

find . -type f -iname '*.o' | xargs gcov -acbrfu {} \; >/dev/null && \
mkdir coverage && pushd coverage && mv ../*.gcov . && \
sed -i -e 's@Source:@Source:../@' *.gcov && \
sed -i -e 's@Graph:@Graph:../@' *.gcov && \
sed -i -e 's@Data:@Data:../@' *.gcov 
