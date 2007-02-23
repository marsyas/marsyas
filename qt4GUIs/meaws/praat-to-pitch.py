#!/usr/bin/python
import math
import os
import sys

soundFileName = sys.argv[1]
tempo = sys.argv[2]
os.system('Praat getpitch.praat ' + soundFileName)

def freq2midi(midi, f = 2**(1.0/12)):
	if (midi==0):
		return 0
	midilog = math.log(midi/440,2)
	return 12*midilog+57+12  # maybe?

file = open('praatPitchList.txt','r')
filelines=file.readlines()
file.close()

file = open('pitchList.txt','w')
for line in filelines:
	if ( line[0:27] == '                frequency ='):
		file.write( str(freq2midi( float(line[28:]) ))+'\n' )
file.close()


sampCount=0.0
skipped=0
pitchSum=0.0
numSamples = int( 44100.0/512 * 60.0/float(tempo) )
notes=0

file = open('pitchList.txt','r')
filelines=file.readlines()
file.close()
for line in filelines:
	pitch = float(line)
	if (pitch==0):
		skipped = skipped + 1
	else:
		pitchSum = pitchSum + pitch
	sampCount = sampCount + 1
	if (sampCount >= numSamples):
		#print str(pitchSum) + '  ' + str(sampCount)
		actualSamps = int(sampCount) - skipped
		if (actualSamps==0):
			avgPitch = 0
		else:
			avgPitch = float( pitchSum / actualSamps )
	#	if (notes>5):
		print avgPitch
		sampCount = sampCount - numSamples  ## yes, we want a float   # maybe
		pitchSum=0
		notes = notes + 1
		skipped=0

