#!/bin/sh
### update as necessary
baseDir=~/usr/src/marsyas
newDir=~/usr/src/dailytest-marsyas
logBase=~/usr/src/dailytest-`date +%y%m%d`
buildLog=$logBase-build.log
regtestLog=$logBase-regtest.log
lastGoodVersion=~/usr/src/dailytest-lastgood.txt


### actual script
rm -rf $newDir
cd $baseDir
version=`svn info | grep Revision | cut -c 11-`
svn export . $newDir

cd $newDir
./configure

make &> $buildLog
PASS=$?

if [ "$PASS" = "0" ]
then
	echo Build succeeded...
else
	echo Build FAILED!
	mail -s "Daily Marsyas Autotester" gperciva@uvic.ca < tail -n 50 $buildLog
	exit
fi

scripts/regression_tests.py &> $regtestLog
PASS=$?
if [ "$PASS" = "0" ]
then
	echo Regression tests succeeded...
else
	echo Regression tests FAILED!
	mail -s "Daily Marsyas Autotester" gperciva@uvic.ca < $regtestLog
	exit
fi


echo $version > $lastGoodVersion

# temporary end here.  Once the other testing system is in place, we'll
# have another test (of course).
mail -s "Daily Marsyas Autotester" gperciva@uvic.ca < $regtestLog


