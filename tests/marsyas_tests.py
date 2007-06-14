#!/usr/bin/python
import sys
import os
import filecmp

LOG_FILE = 'results.log'
test_file = 'testlist.txt'

if not(os.path.isdir('audio')):
	print "Please run this from the `tests' directory."
	sys.exit();

test_commands = []
test_answers = []
tests = open(test_file).readlines()

for line in tests:
	if (not(line[0]=='#') and not(line[0]=='\n')):
		line_split = line.split('\t')
		test_commands.append( line_split[0] )
		test_answers.append( line_split[1].strip() )
	
os.chdir('audio')
logfile = open('../'+LOG_FILE, 'w')
problem = 0
for i in range( len(test_commands) ):
	print test_commands[i]
	os.system(test_commands[i])
	# use .au files because identical-sounding .wav files can have
	# different headers
	if filecmp.cmp('out.au', test_answers[i]):
		logfile.write("Test " + str(i) + " successful");
	else:
		logfile.write("Test " + str(i) + " FAILED: " + test_commands[i])
		problem = 1
logfile.close

if not(problem):
	print "All tests passed.  Congratulations, you didn't",
	print "break Marsyas!  (this time)"
else:
	print "Some test(s) failed.  Please see " + LOG_FILE,
	print "for an account of the damage."
	print "  (tests are 0-indexed)"


