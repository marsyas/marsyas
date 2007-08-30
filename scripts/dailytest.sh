#!/bin/sh
### update as necessary
baseDir=~/usr/src/marsyas
newDir=~/usr/src/testing

cd $baseDir
#svn export . $newDir

cd $newDir
#./configure

if make
then
	echo PASSED
else
	echo FAILED
fi

