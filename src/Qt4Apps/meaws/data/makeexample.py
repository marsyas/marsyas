#!/usr/bin/python
import sys
import os

# 384 = 128*3
#DIVFACTOR = 384/384

try:
  data_file = sys.argv[1]
except:
  print "Please specify an input file."
  sys.exit()
temp_file = "temp_makeexample.txt"

os.system("python midi.py -i "+data_file+" -p > "+temp_file)

inlines = open(temp_file).readlines()

time = 0
for i in range(len(inlines)):
	line = inlines[i]
	if ( line.find("NOTE_ON") > 0 ):
		splitline = line.split()
		for element in splitline:
			if ( element[:6] == 'pitch='):
				pitch = int( element[7:-1] )
				print str(pitch) + " " + str(time)
				time = 0
	if ( line.find("NOTE_ON") > 0 ):
		splitline = inlines[i-1].split()
#		print splitline
		for element in splitline:
			if ( element[:2] == 't='):
				time += int( element[3:-1] )
	if ( line.find("NOTE_OFF") > 0 ):
		splitline = inlines[i-1].split()
#		print splitline
		for element in splitline:
			if ( element[:2] == 't='):
				time += int( element[3:-1] )

print str(0) + " " + str(time)


