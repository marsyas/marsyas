#!/usr/bin/python 
import sys, os

def main():
	ground_truth = open(sys.argv[1]);
	predicted = open(sys.argv[2]);


	while 1:
		gline = ground_truth.readline()
		gline = gline[:-1]
		pline = predicted.readline()
		pline = pline[:-1]
		if gline: 
			gwords = gline.split('\t')
			pwords = pline.split('\t')
			diff = float(gwords[1]) - float(pwords[1]);
		        print gwords[0] + "\t" + gwords[1]+ "-" + pwords[1] + "\t" + str(diff)
		else: 
			break
	print "Done" 

main()







