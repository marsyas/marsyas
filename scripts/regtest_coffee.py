#!/usr/bin/env python
import sys
import os
import filecmp

print "This is no longer used.  File only exists for archival interest"
sys.exit(1)


MIN_VERSION = 0.1 # since marsyas-coffee-data is sold seperately
LOG_FILE = 'results.log'

# chdir to test directory
marsyasBaseDir = os.path.dirname(sys.argv[0])
marsyasBaseDir = os.path.abspath( marsyasBaseDir )
marsyasBaseDir = os.path.join(marsyasBaseDir+os.sep+'..'+os.sep)
marsyasBaseDir = os.path.abspath( marsyasBaseDir )

#print os.getcwd()

# setup location of marsyas-coffee-data
if (len(sys.argv) < 2):
	print "Please specify location of extra data files!"
	sys.exit(1)
coffee_dir = sys.argv[1];
coffee_dir = os.path.abspath( coffee_dir )
version_file = os.path.join( coffee_dir, 'VERSION')
version_file = os.path.abspath( version_file )
if not( os.path.exists(version_file)):
	print "Please specify location of extra data files!"
	sys.exit(1)
version = float( open( version_file ).readline() )
if (version < MIN_VERSION):
	print "Please upgrade your version of the Marsyas Coffee Data"
	sys.exit(1)


# setup release/debug/installed mode
if len(sys.argv) > 2:
	mode = sys.argv[2]
else:
	mode = 'release'
if not( (mode=='release') or (mode=='debug') or (mode=='installed')):
	print "Please select `release', `debug', or `installed' mode."
	sys.exit(1)
if (mode == 'installed'):
	mode = ''

os.chdir (marsyasBaseDir + os.sep + 'regressionTests')
problem = 0

def doTests(test_filename, temp_filename):
	global logfile
	global problem
	test_commands = []
	test_answers = []

	# load tests
	tests = open('..'+os.sep+test_filename).readlines()
	for line in tests:
		if (not(line[0]=='#') and not(line[0]=='\n')):
			line_split = line.split('\t')
			test_commands.append( line_split[0] )
			if (len(line_split)==2):
				test_answers.append( line_split[1].strip() )
			else:
				test_answers.append('')

	# do tests
	for i in range( len(test_commands) ):
		command = ''
		if mode != '':
			command = '..%(sep)s..%(sep)sbin%(sep)s%(mode)s%(sep)s' % {'sep': os.sep, 'mode': mode}
		command += test_commands[i]
		command = command.replace('%CD', coffee_dir)
		if (os.system(command) != 0): # if something went wrong
			problem = 1
			logfile.write("Test " + str(i) + " FAILED:   " + test_commands[i]+'\n')
			os.remove( temp_filename )
			break
		if not(test_answers[i] == ''):
			# use .au files because identical-sounding .wav files can have
			# different headers
			test_answers[i] = os.path.abspath( test_answers[i] )
			if filecmp.cmp(temp_filename, test_answers[i]):
				logfile.write("Test " + str(i) + " successful\n")
			else:
				print temp_filename
				print test_answers[i]
				logfile.write("Test " + str(i) + " FAILED:   " + test_commands[i]+'\n')
				problem = 1
		else:
			# we would have found a problem above
			logfile.write("Test " + str(i) + " successful\n")
#	os.remove( temp_filename )

logfile = open(LOG_FILE, 'w')
os.chdir('input')
#logfile.write("--------- Audio tests\n")
#doTests('commit-audio.txt', 'out.au')
logfile.write("--------- Text output tests\n")
doTests('coffee-text.txt', 'out.txt')
logfile.close()

print "*********   TEST RESULTS *********"
if (problem):
	print "Some test(s) failed.  Please see regressionTests/" + LOG_FILE,
	print "for an account of the damage."
	print "  (tests are 0-indexed)"
	sys.exit(1)
else:
	print "All tests passed.  Congratulations, you didn't",
	print "break Marsyas!  (this time)"
	sys.exit(0)

