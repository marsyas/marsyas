#!/bin/sh
### update as necessary
baseDir=~/usr/src/marsyas
newDir=~/usr/src/dailytest-marsyas
logBase=~/usr/src/dailytest-`date +%y%m%d`
buildLog=$logBase-build.log
regtest_commitLog=$logBase-regcommit.log
regtest_coffeeLog=$logBase-regcoffee.log
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
	mail -s "Daily Marsyas Autotester: build FAILED" gperciva@uvic.ca < tail -n 50 $buildLog
	exit
fi

scripts/regtest_commit.py &> $regtest_commitLog
PASS=$?
if [ "$PASS" = "0" ]
then
	echo Commit regression tests succeeded...
else
	echo Commit regression tests FAILED!
	mail -s "Daily Marsyas Autotester: commit regtest FAILED" gperciva@uvic.ca < $regtest_commitLog
	exit
fi

scripts/regtest_coffee.py &> $regtest_coffeeLog
PASS=$?
if [ "$PASS" = "0" ]
then
	echo Coffee regression tests succeeded...
else
	echo Coffee regression tests FAILED!
	mail -s "Daily Marsyas Autotester: coffee regtest FAILED" gperciva@uvic.ca < $regtest_coffeeLog
	exit
fi

echo $version > $lastGoodVersion
mail -s "Daily Marsyas Autotester" gperciva@uvic.ca < "Everything is good"


