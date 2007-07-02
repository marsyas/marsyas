#!/usr/bin/python
import os
import sys

runningDir = os.path.dirname(sys.argv[0])

print runningDir

absRunning = os.path.abspath( runningdir )

print absRunning

marsyasBaseDir = os.path.join(runningDir+os.sep+'..'+os.sep)

print marsyasBaseDir

print os.path.join( absRunning+os.sep+'..'+os.sep)


