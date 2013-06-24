#!/usr/bin/env python

import sys
import numpy
import pylab

PLOT = True
PLOT = False

INCLUDE_05 = False
#INCLUDE_05 = True

def accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    diff = abs(bpm_detected - bpm_ground)
    if diff <= tolerance * bpm_ground:
        return True
    return False

def double_harmonic_accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    for m in [1, 2]:
        diff = abs(m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return m
        if not INCLUDE_05:
            continue
        diff = abs(1.0/m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return 1.0/m
    return 0

def extended_harmonic_accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    for m in [1, 2, 3]:
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

INTRO_BEGIN = """@relation mults"""
FEATURES = [
    #'above_p1',
    #'below_p1',
    #'above_p2',
    #'below_p2',
    #'above_p3',
    #'below_p3',
    'energy_under',
    'energy_over',
    'energy_residual',
    'str05',
    'str10',
    'str20',
    'str_residual',
    #'strp1',
    #'strp2',
    #'strp3',
    #'strp_residual',
    #'rel1',
    'rel2',
    'rel3',
    #'has2',
    #'has3',
    'num_non_zero',
    #'energyinpeak',
    ]

if INCLUDE_05:
    OUTRO = """@attribute heuristic_bpm numeric
@attribute class {0.5,1.0,2.0}
@data
"""
else:
    OUTRO = """@attribute heuristic_bpm numeric
@attribute class {1.0,2.0}
@data
"""
#OUTRO = """@attribute class {0.5,1.0,2.0}
#@attribute class {0.5,1.0,2.0}
#@attribute class {1.0,2.0}
#@attribute class {1.0,2.0,1.5}
#@attribute class {0.25,0.333333333333,0.5,0.666666666667,1.0,1.5,2.0,3.0,4.0}

INTRO = "%s\n%s\n%s" % (
    INTRO_BEGIN,
    '\n'.join(["@attribute bp-%s numeric" % x for x in FEATURES]),
    OUTRO
    )
#@attribute 19 numeric

out.write(INTRO)



harmonics = 0
cause_problems = 0
total = 0
multsdict = {}
grounds = []
failures = []
minmax = []

for line in lines:
    sl = line.rstrip().split("\t")
    if line.startswith("features_orig:"):
        detected = float(sl[-2])
        ground_truth = float(sl[-1])
        vec = numpy.zeros(len(sl)-2)
        for i in range(1,len(sl)-1):
            vec[i-1] = float(sl[i])
        minmax.append(vec)
    #if not line.startswith("features_orig:"):
    if not line.startswith("features_normalized:"):
        continue
    #detected_norm = float(sl[-2])
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

    if mult > 0:
        if not accuracy(mult*detected, ground_truth):
            print "failure"
            print detected, ground_truth, mult
            exit(1)
    else:
        if extended_harmonic_accuracy(2.0*detected, ground_truth):
            mult = 2.0
            #print "extra 2"
        elif INCLUDE_05:
            if extended_harmonic_accuracy(0.5*detected, ground_truth):
                mult = 0.5
            #print "extra 0.5"
    #if accuracy(detected, ground_truth):
    #    mult = 1.0
    #else:
    #    if accuracy(2*detected, ground_truth):
    #        mult = 2.0
    #if extended_harmonic_accuracy(detected, ground_truth):
    if mult > 0:
        harmonics += 1
        #if extended_harmonic_accuracy(mult*detected, ground_truth) == 0:
        if accuracy(mult*detected, ground_truth) == 0:
            cause_problems += 1
            # don't multipy value; penalize MIREX but keep HARMONIC
            mult = 1.0
    total += 1
    
    vec = numpy.zeros(len(sl)-1+1-1)
    for i in range(1,len(sl)-1):
        vec[i-1] = float(sl[i])
    vec[-1] = mult
    #vec[0] = detected


    div = detected / ground_truth
    if mult > 0:
        text = ",".join( [str(v) for v in vec] )
        out.write(text+"\n")
    else:
        failures.append(div)

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
lowbound = 40
highbound = 180
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
print "min/max ground truths:\t", min(grounds), max(grounds)

if PLOT:
    pylab.xlabel("ground")
    pylab.ylabel("detected")
    pylab.figure()

    pylab.hist(grounds, bins=100)
    pylab.show()

#pylab.hist(failures, bins=200)
#pylab.show()


vals = numpy.array(minmax)
mins = vals.min(axis=0)
maxs = vals.max(axis=0)

print "    const mrs_real mins[] = {",
for m in mins:
    print str(m) + ",",
print "0 };"

print "    const mrs_real maxs[] = {",
for m in maxs:
    print str(m) + ",",
print "0 };"

print "num features:\t", len(mins)


