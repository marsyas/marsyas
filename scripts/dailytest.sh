#!/bin/sh
### update as necessary
baseDir=~/usr/src/marsyas
newDir=~/usr/src/dailytest-marsyas
logfile=~/usr/src/dailytest-`date +%y%m%d`


### actual script
rm -rf $newDir
cd $baseDir
svn export . $newDir

cd $newDir
./configure

make &> $logfile-testbuild.log
PASS=$?

if [ "$PASS" = "0" ]
then
	echo Build succeeded...
else
	echo Build FAILED!
	exit
fi

scripts/regression_tests.py &> $logfile-regtests.log
PASS=$?
if [ "$PASS" = "0" ]
then
	echo Regression tests succeeded...
else
	echo Regression tests FAILED!
	exit
fi


