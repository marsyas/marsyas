#!/usr/bin/env python

import sys
import numpy


def accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    diff = abs(bpm_detected - bpm_ground)
    if diff <= tolerance * bpm_ground:
        return True
    return False

def double_harmonic_accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    for m in [1, 2]:
    #for m in [1, 2, 3]:
        diff = abs(m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return m
        #diff = abs(1.0/m*bpm_detected - bpm_ground)
        #if diff <= tolerance * bpm_ground:
        #    return 1.0/m
    return 0

def extended_harmonic_accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    for m in [1, 2, 3]:
    #for m in [1, 2, 3]:
        diff = abs(m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return m
        diff = abs(1.0/m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return 1.0/m
    return 0


filename = sys.argv[1]

lines = open(filename).readlines()
out = open("weka.arff", 'w')

INTRO = """@relation mults
@attribute tempo1 numeric
@attribute tempo2 numeric
@attribute tempo3 numeric
@attribute rel12 numeric
@attribute rel13 numeric
@attribute str12 numeric
@attribute str13 numeric
@attribute class {1.0,2.0}
@data
"""
#@attribute class {0.5,1.0,2.0}
out.write(INTRO)


for line in lines:
    if not line.startswith("Cands:"):
        continue
    #print line
    sl = line.split("\t")
    detected = float(sl[1])
    ground_truth = float(sl[6])

    mult = double_harmonic_accuracy(detected, ground_truth)
    #if accuracy(detected, ground_truth):
    #    mult = 1.0
    #else:
    #    if accuracy(2*detected, ground_truth):
    #        mult = 2.0
    
    vec = numpy.zeros(8)
    for i in range(1, 6):
        vec[i-1] = sl[i]
    for i in range(7, 9):
        vec[i-2] = sl[i]
    vec[7] = mult

    if mult > 0:
        text = ",".join( [str(v) for v in vec] )
        out.write(text+"\n")

out.close()
