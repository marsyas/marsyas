#!/usr/bin/env python

import numpy.random
import late_heuristic

bphase = numpy.random.random(210)
bphase[47] = 0
bphase[127] = 0
bphase[197] = 0
bphase[60] += 2.0
bphase[120] += 1.0

info = late_heuristic.info_histogram(60, bphase, 0.05)

print info

numpy.savetxt("late-test-input.txt", bphase)
numpy.savetxt("late-test-output.txt", info)

#import pylab
#pylab.plot(bphase)
#pylab.show()

