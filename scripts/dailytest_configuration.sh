#!/bin/bash

configurationName=$1
gitDir=$2
testDir=$3
cmakeOptions=$4

print_usage() {
  echo "-- Usage: dailytest_configuration <git dir> <test dir>"
}

if [[ (-z $gitDir) ]]; then
  print_usage
  echo "!! Git directory not given!"
  exit 1
fi

if [[ (-z $testDir) ]]; then
  print_usage
  echo "!! Test directory not given!"
  exit 1
fi

if [[ !($gitDir =~ ^/) || !($testDir =~ ^/) ]]; then
  print_usage
  echo "!! Directories must be given as absolute paths!"
  exit 1
fi

######### non-changing definitions
buildDir=$testDir/build
logDir=$testDir/logs
report=$logDir/`date +%y%m%d`-report.txt

logBase=$logDir/`date +%y%m%d`-$configurationName
configLog=$logBase-config.log
buildLog=$logBase-build.log
testingLog=$logBase-testing.log
sanityLog=$logBase-sanity.log
continuousLog=$logBase-continuous.log
distLog=$logBase-dist.log
distcheckLog=$logBase-distcheck.log
manualsLog=$logBase-manuals.log
doxyLog=$logBase-doxy.log

report() {
  echo $1 | tee -a $report
}

#  $1 is the command
#  $2 is the log file for the command
#  $3 is the step name (ie Build, Sanity, Coffee, Dist)

testthing() {
  # print command
  echo ">> $1"

  # log command
  echo >> $2
  echo ">> $1" >> $2
  echo >> $2

  # execute command
  $1 >> $2 2>&1
  PASS=$?
  if [ $PASS = 0 ]
  then
    report "-- $3: OK"
  else
    report "!! $3: FAILED!"
    report "!! Last up-to-50 lines of output from failing command:"
    report ""
    tail -n 50 $2
    tail -n 50 $2 >> $report
    exit 1
  fi
}

report "## Testing configuration: $configurationName"

# cleanup build dir
rm -rf $buildDir
mkdir -p $buildDir

### Do tests
cd $buildDir

testthing "cmake $gitDir -DMARSYAS_TESTS=ON $cmakeOptions" $configLog "CMake"

testthing make $buildLog "Build"

testthing "make test" $testingLog "Testing"



#testthing "scripts/regtest_sanity.py" $sanityLog Sanity
#testthing "scripts/regtest_coffee.py $coffeeDir" $coffeeLog Coffee
#testthing "make Continuous" $continuousLog Continuous

## comment out doc building for now, as doxygen is not installed:
#testthing "make doxy" $doxyLog "Doxygen docs"
#testthing "make docs" $manualsLog "Manuals"

#testthing "make dist" $distLog "Make dist"

#testthing "make distcheck" $distcheckLog "Make distcheck"
