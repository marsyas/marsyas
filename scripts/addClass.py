#!/usr/bin/python
import os
import sys
import modifyBuild

marsystem = 0
# action: 1 == add filename.   2 == remove filename.
action = 1

try:
	source_filename = sys.argv[1]
except:
	print "Please enter the name of your new MarSystem"
	sys.exit()

modifyBuild.process(source_filename, marsystem, action)

