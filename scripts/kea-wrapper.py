#!/usr/bin/env python

import sys
import subprocess

OK_PERCENT_DIFFERENCE = 2.0

try:
    kea_binary = sys.argv[1]
    kea_args = sys.argv[2:-2]
    logfilename = sys.argv[-2]
    oldlogfilename = sys.argv[-1]
except:
    print "Syntax:  KEA_BINARY ARG1 ARG2 ... ARGn LOGFILE OLDLOGFILE"
    print "  OLDLOGFILE = '' for no comparison"
    sys.exit(0)

def make_log(cmd, logfilename):
    print cmd
    logfile = open(logfilename, 'w')
    p = subprocess.Popen(cmd, stdout=logfile, shell=True)
    p.communicate()
    logfile.close()

def read_accuracy(filename):
    kea_output = open(filename).read()
    for line in kea_output.split('\n'):
        if line.find("Correctly Classified Instances") >= 0:
            splitline = line.split()
            accuracy = float(splitline[4])
            return accuracy
    return None

def compare(logfilename, oldlogfilename):
    new_accuracy = read_accuracy(logfilename)
    old_accuracy = read_accuracy(oldlogfilename)
    # did out percentage accuracy drop?
    if new_accuracy < (old_accuracy-OK_PERCENT_DIFFERENCE):
        return 1
    return 0

if __name__ == "__main__":
    make_log(kea_binary+' '+' '.join(kea_args), logfilename)
    if oldlogfilename != "''":
        sys.exit( compare(logfilename, oldlogfilename) )

