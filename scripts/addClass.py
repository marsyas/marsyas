#!/usr/bin/python
import os
import sys
import modifyBuild

# action: 1 == add filename.   2 == remove filename.
action = 1

try:
	source_filename = sys.argv[1]
except:
	print "Please enter the name of your new MarSystem"
	sys.exit()

marsyasBaseDir = os.path.dirname(sys.argv[0])
#print marsyasBaseDir
marsyasBaseDir = os.path.abspath( marsyasBaseDir )
#print marsyasBaseDir
marsyasBaseDir = os.path.join(marsyasBaseDir+os.sep+'..'+os.sep)
#print marsyasBaseDir
marsyasBaseDir = os.path.abspath( marsyasBaseDir )
#print marsyasBaseDir

modifyBuild.set_base_dir(marsyasBaseDir)

#modifyBuild.modify_manager(source_filename,action)
modifyBuild.modify_lib_release_makefile(source_filename,action)

buildfile = os.path.join(marsyasBaseDir, 'marsyas', 'Makefile.am')
modifyBuild.modify_h_file(source_filename, buildfile, action)

buildfile = os.path.join(marsyasBaseDir, 'marsyas', 'marsyas.pro')
modifyBuild.modify_h_file(source_filename, buildfile, action)

buildfile = os.path.join(marsyasBaseDir, 'marsyas', 'marsyas.pro')
modifyBuild.modify_cpp_file(source_filename, buildfile, action)


print "All done; please renerate autools with:"
print "aclocal && automake"

