#!/usr/bin/python
import os
import sys

try:
	newName = sys.argv[1]
except:
	print "Please enter the name of your new MarSystem"
	sys.exit()

oldName = 'MarSystemTemplateBasic'

marsyasDir = os.path.dirname( sys.argv[0] )
writeDir = os.getcwd()

def copyWithSub( oldFile, newFile ):
	oldFile = open( os.path.join( marsyasDir, oldFile)).readlines()
	file = open( os.path.join(writeDir, newFile), 'w')
	for line in oldFile:
		newLine = line
		newLine = newLine.replace(oldName, newName)
		# for the #define
		newLine = newLine.replace(oldName.upper(), newName.upper())
		# don't print // comments, but still print /* comments.
		# we use /* comments for copyright and doxy.  I'm not being
		# lazy by not removing those comments.  :)
		if not(newLine.strip()[0:2] == '//'):
			file.write( newLine )
	file.close()

copyWithSub(oldName + '.h', newName + '.h')
copyWithSub(oldName + '.cpp', newName + '.cpp')



