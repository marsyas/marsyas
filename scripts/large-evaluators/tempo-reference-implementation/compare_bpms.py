#!/usr/bin/env python

import os.path
import sys
import glob

import evaluate_bpms

#OPTIONS = [(0,0), (1,0), (2,0), (2,1), (2,2)]


ground_filename = sys.argv[1]
basename = os.path.basename(ground_filename)[:-3]
detected_mfs = glob.glob(basename+"*.mf")
detected_mfs.sort()

print "  name %s\t 1 \t 2 \t 4 \t 8" % (
    ' '*(len(detected_mfs[0])-len("  name ")))
for detected_mf in detected_mfs:
    one = evaluate_bpms.process_mfs(ground_filename, detected_mf, 1)
    two = evaluate_bpms.process_mfs(ground_filename, detected_mf, 2)
    four = evaluate_bpms.process_mfs(ground_filename, detected_mf, 4)
    eight = evaluate_bpms.process_mfs(ground_filename, detected_mf, 8)
    print "%s\t%.1f\t%.1f\t%.1f\t%.1f" % (
        detected_mf, one, two, four, eight)


