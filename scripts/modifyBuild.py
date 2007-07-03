#!/usr/bin/python
#	 modifyBuild.py:
# library for adding or removing
# ource files to the Marsyas build process.
#
# Do not call manually; instead use
#	 addMyMar, addNotmar, removeMyMar, or removeNotMar.
#
# action: 1 == add filename.   2 == remove filename.
import os
import sys

marsyasBaseDir = ''
def set_base_dir( newBaseDir ):
	global marsyasBaseDir
	marsyasBaseDir = newBaseDir

def modify_manager(filename,action):
	fileToEdit = os.path.join(marsyasBaseDir, 'marsyas', 'MarSystemManager.cpp')
	filelines = open( fileToEdit ).readlines()
	if (action==1):
		for line in filelines:
			if (line.find(filename)>=0):
				print "New MarSystem already added?!"
				sys.exit()
	newfile = open( fileToEdit, 'w')
	for line in filelines:
		if (action==1):
			if (line[0:17] == '#include \"Gain.h\"'):
				insertLine = '#include \"' + filename + '.h\"'
				print "Adding line 1 of 2 to " + fileToEdit
				newfile.write(insertLine + '\n')
			if (line[0:25] == '\tregisterPrototype(\"Gain\"'):
				insertLine = '\tregisterPrototype(\"' + filename + '", new' + filename +'(\"'+filename.lower()+'pr\"));'
				print "Adding line 2 of 2 to " + fileToEdit
				newfile.write(insertLine + '\n')
			newfile.write(line)
		if (action==2):
			if (line.find(filename)>=0):
				if (line[0:8] == '#include'):
					print "Removed line 1 of 2 from " + fileToEdit
					continue
				if (line[0:18] == '\tregisterPrototype'):
					print "Removed line 2 of 2 from " + fileToEdit
					continue
			newfile.write(line)
	newfile.close()

def modify_h_file (source_file, build_file, action):
	fileToEdit = build_file
	filelines = open( fileToEdit ).readlines()
	newfile = open( fileToEdit, 'w')
	for line in filelines:
		if (action==1):
			if (line[0:7] == '\tGain.h'):
				insertLine = '\t'+source_file + '.h \\'
				print "Adding line 1 of 1 (for .h) to " + fileToEdit
				newfile.write(insertLine + '\n')
			newfile.write(line)
		if (action==2):
			if (line.find(source_file)>=0):
				if (line.find('.h')>=0):
					print "Removed line 1 of 1 (for .h) from " + fileToEdit
					continue
			newfile.write(line)
	newfile.close()

def modify_cpp_file (source_file, build_file, action):
	fileToEdit = build_file
	filelines = open( fileToEdit ).readlines()
	newfile = open( fileToEdit, 'w')
	for line in filelines:
		if (action==1):
			if (line[0:9] == '\tGain.cpp'):
				insertLine = '\t'+source_file + '.cpp \\'
				print "Adding line 1 of 1 (for .cpp) to " + fileToEdit
				newfile.write(insertLine + '\n')
			newfile.write(line)
		if (action==2):
			if (line.find(source_file)>=0):
				if (line.find('.cpp')>=0):
					print "Removed line 1 of 1 (for .cpp) from " + fileToEdit
					continue
			newfile.write(line)
	newfile.close()

def modify_lib_release_makefile(source_file,action):
	fileToEdit = os.path.join(marsyasBaseDir, 'lib', 'release', 'Makefile.am')
	filelines = open( fileToEdit ).readlines()
	newfile = open( fileToEdit, 'w')
	for line in filelines:
		if (action==1):
			if (line[23:31] == 'Gain.cpp'):
				insertLine = '\t$(top_srcdir)/marsyas/'+source_file+'.cpp \\'
				print "Adding line 1 of 1 to " + fileToEdit
				newfile.write(insertLine+'\n')
			newfile.write(line)
		if (action==2):
			if (line[23:].find(source_file)>=0):
				print "Removed line 1 of 1 from " + fileToEdit
				continue
			newfile.write(line)
	newfile.close()

