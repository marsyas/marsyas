#!/usr/bin/env python

import sys

def extended_harmonic_accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    for m in [1, 2, 3]:
    #for m in [1, 2, 3]:
        diff = abs(m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return True
        diff = abs(1.0/m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return True
    return False



cand_lines = open(sys.argv[1]).readlines()
limit = int(sys.argv[2])


total = len(cand_lines)
good = 0
for line in cand_lines:
    sl = line.split("\t")
    ground_truth = float(sl[1])
    cands = [float(sl[3]), float(sl[4]), float(sl[5])]
    for cand in cands[:limit]:
        corr = extended_harmonic_accuracy(cand, ground_truth)
        if corr:
            good += 1
            break

accuracy = 100*float(good) / total
print "Accuracy: %.2f (%i/%i)" % (accuracy, good, total)


