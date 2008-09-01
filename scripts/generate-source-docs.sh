#!/bin/sh

# runs the source-highlight program on the files in
#   doc/examples/
# to produce files ready for inclusion in the docs in
#  doc/source-doc/

#### FIXME: this assumes that the user is in the topdir.
####        use argv[0] or something to figure this out instead,
####        so that this script can be run from anywhere.
cd doc/examples/

for f in *.cpp
do
	source-highlight --output-dir=../source-doc/ -f html $f
	source-highlight --output-dir=../source-doc/ -f texinfo $f
done
for f in Makefile.osx Makefile.linux
do
	source-highlight --output-dir=../source-doc/ -s perl -f html $f
	source-highlight --output-dir=../source-doc/ -s perl -f texinfo $f
done


cd Qt4-tutorial/

for f in *.cpp *.h
do
	source-highlight --output-dir=../../source-doc/ -f html $f
	source-highlight --output-dir=../../source-doc/ -f texinfo $f
done
source-highlight --output-dir=../../source-doc/ -s perl -f html tutorial.pro
source-highlight --output-dir=../../source-doc/ -s perl -f texinfo tutorial.pro


