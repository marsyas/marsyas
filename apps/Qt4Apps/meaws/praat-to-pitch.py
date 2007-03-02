#!/usr/bin/python
import math
import os
import sys

soundFileName = sys.argv[1]
tempo = sys.argv[2]

# get info from Praat
os.system('Praat getpitch.praat ' + soundFileName)
file = open('praatPitchList.txt','r')
filelines=file.readlines()
file.close()


def freq2midi(midi, f = 2**(1.0/12)):
	if (midi==0):
		return 0
	midilog = math.log(midi/440,2)
	return 12*midilog+57+12  # maybe?

sampCount=0.0
skipped=0
pitchSum=0.0
numSamples = int( 44100.0/165 * 60.0/float(tempo) )

file = open('notepitches.txt','w')
#extrafile = open ('bigpitchlist.txt','w')  # for debugging
for line in filelines:
	if ( line[0:27] == '                frequency ='):
		pitch = freq2midi( float(line[28:]) )
#		extrafile.write( str(pitch) +'\n')  # debug
		pitchSum = pitchSum + pitch
		if (pitch==0):
			skipped = skipped + 1
		sampCount = sampCount + 1
		if (sampCount >= numSamples):
			actualSamps = sampCount - skipped
			if (actualSamps==0):
				avgPitch = 0
			else:
				avgPitch = float( pitchSum / actualSamps )
#			print str(pitchSum) + '  ' + str(sampCount) + ' ' + str(actualSamps) + ' ' + str(avgPitch)
			file.write( str(avgPitch) + '\n')
			sampCount = sampCount - numSamples  ## yes, we want a float   # maybe
			pitchSum=0
			skipped=0

# extrafile.close()  # debug
file.close()

