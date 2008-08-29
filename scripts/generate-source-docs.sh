#!/bin/sh

# runs the source-highlight program on the files in
#   doc/examples/
# to produce files ready for inclusion in the docs in
#  doc/source-doc/

#### FIXME: this assumes that the user is in the topdir.
####        use argv[0] or something to figure this out instead,
####        so that this script can be run from anywhere.
cd doc/examples/

source-highlight --output-dir=../source-doc/ -f html *.cpp
source-highlight --output-dir=../source-doc/ -f texinfo *.cpp

cd Qt4-tutorial/
source-highlight --output-dir=../../source-doc/ -f html *.cpp
source-highlight --output-dir=../../source-doc/ -f texinfo *.cpp
source-highlight --output-dir=../../source-doc/ -f html *.h
source-highlight --output-dir=../../source-doc/ -f texinfo *.h

