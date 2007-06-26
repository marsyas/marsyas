#!/usr/bin/python
import os
import sys

try:
	addMar = sys.argv[1]
except:
	print "Please enter the name of your new MarSystem"
	sys.exit()

marsyasBaseDir = os.path.join(os.path.dirname(sys.argv[0])+os.sep+'..'+os.sep)

#print marsyasBaseDir

### manager
fileToEdit = os.path.join(marsyasBaseDir, 'marsyas', 'MarSystemManager.cpp')
filelines = open( fileToEdit ).readlines()
for line in filelines:
	if (line.find(addMar)>=0):
		print "New MarSystem already added?!"
		sys.exit()
newfile = open( fileToEdit, 'w')
for line in filelines:
	newfile.write(line)
	if (line[0:16] == '//addMar-include'):
		insertLine = '#include \"' + addMar + '.h\"'
		print "Adding line 1 of 2 to " + fileToEdit
		newfile.write(insertLine)
		newfile.write('\n')
	if (line[0:17] == '//addMar-register'):
		insertLine = '    registerPrototype(\"' + addMar + '", new '+addMar+'(\"'+addMar.lower()+'pr\"));'
		print "Adding line 2 of 2 to " + fileToEdit
		newfile.write(insertLine)
		newfile.write('\n')
newfile.close()

### buildsystem: autotools
fileToEdit = os.path.join(marsyasBaseDir, 'marsyas', 'Makefile.am')
filelines = open( fileToEdit ).readlines()
newfile = open( fileToEdit, 'w')
for line in filelines:
	newfile.write(line)
	if (line[0:16] == '#addMar-Makefile'):
		insertLine = addMar + '.h \\'
		print "Adding line 1 of 1 to " + fileToEdit
		newfile.write(insertLine)
		newfile.write('\n')
newfile.close()

fileToEdit = os.path.join(marsyasBaseDir, 'lib', 'release', 'Makefile.am')
filelines = open( fileToEdit ).readlines()
newfile = open( fileToEdit, 'w')
for line in filelines:
	newfile.write(line)
	if (line[0:16] == '#addMar-Makefile'):
		insertLine = '\t\t\t  $(top_srcdir)/marsyas/'+addMar+'.cpp \\'
		print "Adding line 1 of 1 to " + fileToEdit
		newfile.write(insertLine)
		newfile.write('\n')
newfile.close()


### qmake
fileToEdit = os.path.join(marsyasBaseDir, 'marsyas', 'marsyas.pro')
filelines = open( fileToEdit ).readlines()
newfile = open( fileToEdit, 'w')
for line in filelines:
	newfile.write(line)
	if (line[0:9] == '#addMar-h'):
		insertLine = '\t'+addMar+'.h \\'
		print "Adding line 1 of 2 to " + fileToEdit
		newfile.write(insertLine)
		newfile.write('\n')
	if (line[0:11] == '#addMar-cpp'):
		insertLine = '\t'+addMar+'.cpp \\'
		print "Adding line 2 of 2 to " + fileToEdit
		newfile.write(insertLine)
		newfile.write('\n')
newfile.close()




