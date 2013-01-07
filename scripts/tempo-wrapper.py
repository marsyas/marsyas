#!/usr/bin/env python

import sys
import subprocess

OK_PERCENT_DIFFERENCE = 1.0

try:
    tempo_binary = sys.argv[1]
    args = sys.argv[2:-2]
    logfilename = sys.argv[-2]
    oldlogfilename = sys.argv[-1]
except:
    print "Syntax:  TEMPO_BINARY MF_FILENAME LOGFILENAME OLDLOGFILENAME"
    print "  OLDLOGFILE = '' for no comparison"
    sys.exit(0)

def make_log(cmd, logfilename):
    print cmd
    logfile = open(logfilename, 'w')
    p = subprocess.Popen(cmd, stdout=logfile, shell=True)
    p.communicate()
    logfile.close()

def read_accuracy(filename):
    tempo_output = open(filename).read()
    acc1 = 0.0
    acc2 = 0.0
    for line in tempo_output.split('\n'):
        if line.find("Correct MIREX Predictions") >= 0:
            values = line.split()[4].split('/')
            acc1 = float(values[0]) / float(values[1])
        if line.find("Correct Harmonic MIREX Predictions") >= 0:
            values = line.split()[5].split('/')
            acc2 = float(values[0]) / float(values[1])
    return acc1, acc2

def compare(logfilename, oldlogfilename):
    new_acc1, new_acc2 = read_accuracy(logfilename)
    old_acc1, old_acc2 = read_accuracy(oldlogfilename)
    # did out percentage accuracy drop?
    if new_acc1 < (old_acc1-OK_PERCENT_DIFFERENCE):
        return 1
    if new_acc2 < (old_acc2-OK_PERCENT_DIFFERENCE):
        return 1
    return 0

if __name__ == "__main__":
    make_log(tempo_binary+' '+' '.join(args), logfilename)
    #print logfilename, oldlogfilename
    if oldlogfilename != "''":
        sys.exit( compare(logfilename, oldlogfilename) )

