#!/usr/bin/env bash
set -vex

export BUILD_NUMBER="0"
export ENABLED_TESTS="true"
export SHOULD_INSTALL="false"

case "${GCC_VERSION}" in
  RHEL)
    module load gtest/gcc48

    # load SCL GCC
    source /opt/rh/devtoolset-6/enable
    ;;

  next)
    module load gcc/8.1.0
    module load gtest
    ;;

  ICC*)
    module load devtoolset/6
    module load composer_xe/${GCC_VERSION#ICC}
    module load gtest/gcc48

    CC="icc"
    CXX="icpc"
    ;;

  clang)
    module load gtest/gcc48

    source /opt/rh/llvm-toolset-6.0/enable
    CC="clang"
    CXX="clang++"
    ;;

  *)
    case "${bamboo_planRepository_branchName}-${BUILDTYPE:-release}-${ENABLED_UNITY_BUILD:-off}-${ENABLED_COVERAGE:-false}" in
      develop-release-off-false|master-release-off-false)
        export PREFIX_ARG="/mnt/software/p/pbcopper/${bamboo_planRepository_branchName}"
        export BUILD_NUMBER="${bamboo_globalBuildNumber:-0}"
        export SHOULD_INSTALL="${INSTALL_IMAGE:-false}"
        ;;
    esac

    module load gcc
    module load gtest
    ;;
esac

module load ccache

export CC="ccache ${CC:-gcc}"
export CXX="ccache ${CXX:-g++}"
export CCACHE_BASEDIR="${PWD}"

if [[ -z ${bamboo_planRepository_branchName+x} ]]; then
  : #pass
elif [[ ! -d /pbi/flash/bamboo/ccachedir ]]; then
  echo "[WARNING] /pbi/flash/bamboo/ccachedir is missing"
elif [[ $bamboo_planRepository_branchName == develop ]]; then
  export CCACHE_DIR=/pbi/flash/bamboo/ccachedir/${bamboo_shortPlanKey}.${bamboo_shortJobKey}.develop
  export CCACHE_TEMPDIR=/scratch/bamboo.ccache_tempdir
elif [[ $bamboo_planRepository_branchName == master ]]; then
  export CCACHE_DIR=/pbi/flash/bamboo/ccachedir/${bamboo_shortPlanKey}.${bamboo_shortJobKey}.master
  export CCACHE_TEMPDIR=/scratch/bamboo.ccache_tempdir
elif [[ $USER == bamboo ]]; then
  _shortPlanKey=$(echo ${bamboo_shortPlanKey}|sed -e 's/[0-9]*$//')
  export CCACHE_DIR=/pbi/flash/bamboo/ccachedir/${bamboo_shortPlanKey}.${bamboo_shortJobKey}
  if [[ -d /pbi/flash/bamboo/ccachedir/${_shortPlanKey}.${bamboo_shortJobKey}.develop ]]; then
    ( cd /pbi/flash/bamboo/ccachedir/
      cp -a ${_shortPlanKey}.${bamboo_shortJobKey}.develop $CCACHE_DIR
    )
  fi
  export CCACHE_TEMPDIR=/scratch/bamboo.ccache_tempdir
fi
