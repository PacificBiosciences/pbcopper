#!/bin/bash

echo "# DEPENDENCIES"
echo "## Load modules"
source /mnt/software/Modules/current/init/bash
module load gcc/4.9.2 cmake ccache ninja

echo "# TEST"
echo "## Unit tests"
( cd build &&  ninja check-xunit )
