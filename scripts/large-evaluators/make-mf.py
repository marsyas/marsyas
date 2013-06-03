#!/usr/bin/env python

import sys
import numpy
import pylab

PLOT = True
PLOT = False

def accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    diff = abs(bpm_detected - bpm_ground)
    if diff <= tolerance * bpm_ground:
        return True
    return False

def double_harmonic_accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    #for m in [1, 2]:
    for m in [1, 2]:
    #for m in [1, 2, 1.5]:
    #for m in [1, 2, 3]:
        diff = abs(m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return m
        diff = abs(1.0/m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return 1.0/m
    return 0

def extended_harmonic_accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    for m in [1, 2, 3, 1.5, 4]:
    #for m in [1, 2, 3]:
        diff = abs(m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return m
        diff = abs((1.0/m)*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return 1.0/m
    return 0


filename = sys.argv[1]

lines = open(filename).readlines()
out = open("weka.arff", 'w')

INTRO = """@relation mults
@attribute tempo0 numeric
@attribute tempo1 numeric
@attribute tempo2 numeric
@attribute rel10 numeric
@attribute rel20 numeric
@attribute absstr0 numeric
@attribute absstr1 numeric
@attribute absstr2 numeric
@attribute relstr10 numeric
@attribute relstr20 numeric
@attribute onlyone numeric
@attribute onlytwo numeric
@attribute halfstr numeric
@attribute relhalfstr numeric
@attribute hastwicestr numeric
@attribute twicestr numeric
@attribute reltwicestr numeric
@attribute residual_str numeric
@attribute a1 numeric
@attribute a2 numeric
@attribute a3 numeric
@attribute a4 numeric
@attribute a5 numeric
@attribute a6 numeric
@attribute class {0.5,1.0,2.0}
@data
"""
#@attribute class {0.5,1.0,2.0}
#@attribute class {1.0,2.0}
#@attribute class {1.0,2.0,1.5}
#@attribute class {0.25,0.333333333333,0.5,0.666666666667,1.0,1.5,2.0,3.0,4.0}
out.write(INTRO)

harmonics = 0
cause_problems = 0
total = 0
multsdict = {}
grounds = []

for line in lines:
    if not line.startswith("Cands:"):
        continue
    #print line
    sl = line.rstrip().split("\t")
    detected = float(sl[1])
    #detected = float(sl[19])
    ground_truth = float(sl[-1])
    grounds.append(ground_truth)

    mirex = False
    harmonic = False
    if accuracy(detected, ground_truth):
        mirex = True
    if extended_harmonic_accuracy(detected, ground_truth):
        harmonic = True

    mult = double_harmonic_accuracy(detected, ground_truth)
    #mult = extended_harmonic_accuracy(detected, ground_truth)

    try:
        multsdict[mult] += 1
    except:
        multsdict[mult] = 1
    #if accuracy(detected, ground_truth):
    #    mult = 1.0
    #else:
    #    if accuracy(2*detected, ground_truth):
    #        mult = 2.0
    #if extended_harmonic_accuracy(detected, ground_truth):
    if double_harmonic_accuracy(detected, ground_truth):
        harmonics += 1
        if not extended_harmonic_accuracy(mult*detected, ground_truth):
            cause_problems += 1
        # don't multipy value; penalize MIREX but keep HARMONIC
        #mult = 1.0
    total += 1
    
    vec = numpy.zeros(len(sl)-1+1-1)
    for i in range(1,len(sl)-1):
        vec[i-1] = float(sl[i])
    vec[-1] = mult
    vec[0] = detected

    if mult > 0:
        text = ",".join( [str(v) for v in vec] )
        out.write(text+"\n")

    div = detected / ground_truth
    if PLOT:
        #if mirex:
        #    pylab.plot(div, 'go')
        #elif harmonic:
        #    pylab.plot(div, 'go')
        #else:
        #    pylab.plot(div, 'ro')
        if mirex:
            pylab.plot(ground_truth, detected, 'g.')
        elif harmonic:
            pylab.plot(ground_truth, detected, 'b.')
        else:
            pylab.plot(ground_truth, detected, 'ro')

out.close()

print "Harmonic accuracy:\t%i / %i (%.3f)" % (
    harmonics, total, float(harmonics)/total)
print "New mults cause problems for:\t%i" % cause_problems

print multsdict

lowbound = 35
highbound = 6*35
lows = 0
highs = 0
for g in grounds:
    if g < lowbound:
        lows += 1
    if g > highbound:
        highs += 1
print "With bounds of %i - %i, we miss %i - %i (sum %i) out of %i (%.3f%%)" % (
    lowbound, highbound, lows, highs, lows+highs, total,
    float(lows+highs)/total
  )

if PLOT:
    pylab.figure()

    pylab.hist(grounds, bins=100)
    pylab.show()




