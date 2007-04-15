#!/usr/bin/python
import os
import sys

PREAMBLE="\\documentclass{article}\n\
\\usepackage[margin=2.5cm]{geometry}\n\
\\usepackage{graphicx}\n\
\\begin{document}\n"
POSTAMBLE="\\end{document}"

noteFile = sys.argv[1]
#outFile = sys.argv[2]
outFile = 'score.latex'

file = open( noteFile, 'r')
noteLines = file.readlines()
file.close()

def symbolMapping( note ):
	note = note - 61 ## FIXME: possibly incorrect value
	if (note<0):
		return ''
	if (note==0):
		return 'sa'
	if (note==1):
		return 'komal-re'
	if (note==2):
		return 're'
	if (note==3):
		return 'komal-ga'
	if (note==4):
		return 'ga'
	if (note==5):
		return 'ma'
	if (note==6):
		return 'tivra-ma'
	if (note==7):
		return 'pa'
	if (note==8):
		return 'komal-dha'
	if (note==9):
		return 'dha'
	if (note==10):
		return 'komal-ni'
	if (note==11):
		return 'ni'
	if (note==12):
		return 'sa^'
	print 'DEBUG: left-over note: ' + str(note)
	return ''
	

def addSymbol( i ):
	curNote = note[i]
	if (curNote==55):
		return ''
	toReturn = symbolMapping( curNote )
	if (toReturn != ''):
		toReturn = '\includegraphics[scale=0.5]{font/' + toReturn + '}\n'
	return toReturn

note = []
beam = []
curNote = 0
curDur = 0.0
curBeam = 0
for i in range(len(noteLines)-1):
	curNote = int(round(float( noteLines[i].split()[1] ))) 
	if (curNote>=61):
		curDur = int(noteLines[i+1].split()[0]) - int(noteLines[i].split()[0])
		# quantize
		curDur = float( curDur / 44.0 )
		curBeam = 1 ## default
		if ( (curDur > 0.9) and (curDur < 1.1)):
			curBeam = 1
		if ( (curDur > 0.4) and (curDur < 0.6)):
			curBeam = 2
		if ( (curDur > 0.3) and (curDur < 0.4)):
			curBeam = 3
		if ( (curDur > 0.2) and (curDur < 0.3)):
			curBeam = 4

#		print str(curNote) + ' ' + str(curBeam)
		note.append( curNote )
		beam.append( curBeam )

file = open( outFile, 'w')
file.write(PREAMBLE)
i=0
while (i < len(note) ):
	curBeam = beam[i]
	print curBeam
	file.write('\mbox{')
	for j in range( curBeam ):
		file.write( addSymbol(i) )
		i = i+1
	file.write('}\hspace{9mm}\n')

file.write(POSTAMBLE + '\n')
file.close()

os.system('pdflatex score.latex')

