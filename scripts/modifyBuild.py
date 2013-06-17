#!/usr/bin/python
#     modifyBuild.py:
# library for adding or removing
# ource files to the Marsyas build process.
#
# Do not call manually; instead use
#     addMyMar, addNotmar, removeMyMar, or removeNotMar.
#
# action: 1 == add filename.   2 == remove filename.
import os
import sys

marsyasBaseDir = os.path.dirname(sys.argv[0])
marsyasBaseDir = os.path.abspath( marsyasBaseDir )
marsyasBaseDir = os.path.join(marsyasBaseDir+os.sep+'..'+os.sep)
marsyasBaseDir = os.path.abspath( marsyasBaseDir )

def modify_manager(filename,action):
    fileToEdit = os.path.join(marsyasBaseDir, 'src', 'marsyas',
        'core', 'MarSystemManager.cpp')
    filelines = open( fileToEdit ).readlines()
    if (action==1):
        for line in filelines:
            if (line.find('\"'+filename+'.h"')>=0):
                print "New MarSystem already added?!"
                sys.exit()
    newfile = open( fileToEdit, 'w')
    for line in filelines:
        if (action==1):
            if (line[0:14] == '//modifyHeader'):
                insertLine = '#include \"' + filename + '.h\"'
                print "Adding line 1 of 2 to " + fileToEdit
                newfile.write(insertLine + '\n')
            if (line[0:17] == '\t//modifyRegister'):
                insertLine = '\tregisterPrototype(\"' + filename + '", new ' + filename +'(\"'+filename.lower()+'pr\"));'
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

def modify_CMakelists(source_file,action):
    fileToEdit = os.path.join(marsyasBaseDir, 'src',
        'marsyas', 'marsystems', 'CMakeLists.txt')
    filelines = open( fileToEdit ).readlines()
    newfile = open( fileToEdit, 'w')
    for line in filelines:
        if (action==1):
            if (line[0:15] == ')#modifySources'):
                insertLine = '\t'+source_file+'.cpp'
                print "Adding line 1 of 1 to " + fileToEdit
                newfile.write(insertLine+'\n')
            newfile.write(line)
        if (action==2):
            if (line[1:].find(source_file)>=0):
                print "Removed line 1 of 1 from " + fileToEdit
                continue
            newfile.write(line)
    newfile.close()

def modify_CMake_compile_lists(source_file, action, filename):
    fileToEdit = os.path.join(marsyasBaseDir, 'src',
        'marsyas', 'marsystems', filename)
    filelines = open( fileToEdit ).readlines()
    newfile = open( fileToEdit, 'w')
    for line in filelines:
        if (action==1):
        #    if (line[0:15] == ')#modifySources'):
        #        insertLine = source_file+'.cpp'
        #        print "Adding line 1 of 1 to " + fileToEdit
        #        newfile.write(insertLine+'\n')
            newfile.write(line)
        if (action==2):
            if (line.find(source_file)>=0):
                print "Removed line 1 of 1 from " + fileToEdit
                continue
            newfile.write(line)
    if action == 1:
        print "Added line 1 of 1 to " + fileToEdit
        newfile.write(source_file+'\n')
    newfile.close()


def process(source_filename, marsystem, action):
    if (marsystem):
        modify_manager(source_filename,action)

     #modify_CMakelists(source_filename,action)
    modify_CMake_compile_lists(source_filename + ".cpp",
        action, "CMake-compile-cpp.txt")
    modify_CMake_compile_lists(source_filename + ".h",
        action, "CMake-compile-h.txt")

    if (action==1):
        print "    Don't forget to add your source files to the",
        print "Marsyas SVN tree!"

    if (action==2):
        print "    Don't forget to remove your source files from the",
        print "Marsyas SVN tree!"


