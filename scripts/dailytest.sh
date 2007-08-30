#!/bin/sh
### update as necessary
baseDir=~/usr/src/marsyas
newDir=~/usr/src/testing

cd $baseDir
svn export . $newDir

cd $newDir
./configure
nice make
# add bash test to see if the above command failed

