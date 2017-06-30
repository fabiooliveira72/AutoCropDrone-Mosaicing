#!/bin/bash
############################################################################
#
# Usage: compile [options] ...
#
# Prepare the the system to be compiled (CMake, make, ...).
#
# Options:
#  -h or --help ... show this help message
#  -c or clean  ... will remove all files and folders that were created during the compilation process
#  -cm or cm    ... using CMake to prepare the compilation
#  -mk or mk    ... only call make to build the executable with modifications made
#
############################################################################
set -e -o pipefail -u

if [ $# -lt 1 ] || [ "$1" == "-h" ] || [ "$1" == "--help" ]
 then no_of_lines=`cat $0 | awk 'BEGIN { n = 0; } \
                                 /^$/ { print n; \
                                        exit; } \
                                      { n++; }'`
      echo "`head -$no_of_lines $0`"
      exit 1
fi

if [ "$1" == "clean" ] || [ "$1" == "-c" ]
 then
      echo "Cleaning..."
      cmake -P distclean.cmake
      echo "Done."
      exit
fi

if [ "$1" == "cm" ] || [ "$1" == "-cm" ]
 then
      echo "Cleaning..."
      cmake -P distclean.cmake
      echo "Creating the build folder"
      mkdir build
      cd build
      echo "Running CMake..."
      cmake ..
      echo "Running Make..."
      make
      echo "Move executable"
      mv AutoCropDroneMosaicing.so ..
      echo "Done"
      exit
fi

if [ "$1" == "mk" ] || [ "$1" == "-mk" ]
 then
      echo "Going to build folder"
      cd build
      echo "Running Make..."
      make
      echo "Move executable"
      mv AutoCropDroneMosaicing.so ..
      echo "Done"
      exit
fi
