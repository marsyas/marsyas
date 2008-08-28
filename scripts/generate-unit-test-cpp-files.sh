#!/bin/sh

#
# Generate all the cxxtest xxx_runner.cpp files from the input
# Test_xxx.h files using the python script cxxtestgen.
#
# cxxtest is a GPLed test framework and can be found at:
#
# http://cxxtest.sourceforge.net/
#
#
# sness@sness.net (c) 2008 GPLv3
#

#
# sness - Do this the brute force way for now.  Later when we get a
# whole bunch of tests do it smarter.
#

# Change directory into the unit_tests directory so that cxxtestgen.py inserts
# the proper paths into the generated .ccp files
cd src/tests/unit_tests/

echo "Building tests for NormMaxMin"
../../otherlibs/cxxtest/cxxtestgen.py --error-printer -o NormMaxMin_runner.cpp TestNormMaxMin.h

echo "Building tests for WekaSource"
../../otherlibs/cxxtest/cxxtestgen.py --error-printer -o WekaSource_runner.cpp TestWekaSource.h

cd ../../..
