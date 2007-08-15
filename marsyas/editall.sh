#!/bin/sh
for f in *.cpp
do
  g=`basename $f .cpp`
  vi $g.cpp $g.h
done

