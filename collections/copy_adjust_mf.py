#!/usr/bin/env python

OMIT_BLANK_LABELS = True
OMIT_MISSING_FILES = True
COPY_ONLY_TEMPOS = True

OMIT_FILES = ["loopBPMs_tempos.mf", "ismir2004_tempos.mf"]
OMIT_FILES = ["loopBPMs_tempos.mf"]

import sys
import glob
import os.path

NEW_DIR=sys.argv[1]
GEORGE_DIR1="/Users/gtzan/data/sound/"
GEORGE_DIR2="/Users/George/data/sound/"

print "name\t\t\t\t\t\tblank\tmissing"
print "----\t\t\t\t\t\t-----\t-------"
if COPY_ONLY_TEMPOS:
    files=glob.glob("*_tempos.mf")
else:
    files=glob.glob("*.mf")
files = [ f for f in files if f not in OMIT_FILES ]

missing = []
for f in files:
    blank_labels = 0
    missing_files = 0
    lines = open(f).readlines()
    new_mf_filename = os.path.join( NEW_DIR, os.path.basename(f))
    out = open(new_mf_filename, 'w')
    for line in lines:
        if len(line) < 2:
            continue
        oldfilename = line.split('\t')[0].rstrip()
        try:
            label = line.split('\t')[1].rstrip()
        except:
            oldfilename = oldfilename.rstrip()
            label = ""
            if OMIT_BLANK_LABELS:
                blank_labels += 1
                continue

        newfilename = oldfilename.replace(GEORGE_DIR1, NEW_DIR)
        newfilename = newfilename.replace(GEORGE_DIR2, NEW_DIR)
        if OMIT_MISSING_FILES:
            if not os.path.exists(newfilename):
                missing_files += 1
                missing.append(newfilename)
                print newfilename
                continue
        out.write("%s\t%s\n" % (newfilename, label))
    out.close()
    print "%40s\t%i\t%i" % (f, blank_labels, missing_files)


