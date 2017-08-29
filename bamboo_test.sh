#!/bin/bash

echo "# DEPENDENCIES"
echo "## Load modules"
source /mnt/software/Modules/current/init/bash
module load gcc/6.4.0 cmake ccache_base ninja

echo "# TEST"
echo "## Unit tests"
( cd build &&  ninja check-xunit )
