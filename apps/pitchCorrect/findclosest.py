#!/usr/bin/python
import math
import os
import sys

fretFile = sys.argv[1]
praatPitchFile = sys.argv[2]

file = open( fretFile, 'r')
fretLines = file.readlines()
file.close()

file = open( praatPitchFile, 'r')
praatLines = file.readlines()
file.close()

fretIndex = 0
nextSamp = int( fretLines[fretIndex+1].split()[0] )
fretPitch = float( fretLines[fretIndex].split()[1] )
i=0
for line in praatLines:
#	print str(i) + ' ' + str(line),
	candidates = line.split()
	numCand = len(candidates)
	closest = 999999999 # infinity :)
	for j in range(numCand):
		cur = float( candidates[j] )
		closeDist = abs( fretPitch - closest )
		curDist = abs( fretPitch - cur )
		if ( curDist < closeDist ):
			closest = cur
	print closest

	i=i+1
#	if (i>12):
#		sys.exit(0)
	if (i >= nextSamp):
		nextSamp = int( fretLines[fretIndex+1].split()[0] )
		fretPitch = float( fretLines[fretIndex].split()[1] )
#		print "At " + str(i) + "start looking for "+str(fretPitch)
		fretIndex = fretIndex+1
		if ( fretIndex >= len(fretLines)-1 ):
			print "Abnormal exit"
			sys.exit(0)

