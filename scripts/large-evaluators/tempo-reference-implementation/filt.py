#!/usr/bin/env python

import numpy
import scipy.signal
import pylab


print "31"
b, a = scipy.signal.butter(4, 31.0 / (344.53125/2.0))
print b
print a
w, h = scipy.signal.freqz(b, a)
h_db = 20*numpy.log10(abs(h))
pylab.plot(w, h_db, label="exact Butterworth 31")

print "FIR, 32"
N = 16
b = scipy.signal.firwin(N, 30.0 / (344.53125/2.0))
#print b
print "    mrs_realvec bcoeffs(1, %i);" % N
for i, x in enumerate(b):
    print "    bcoeffs(%i) = %.16f;" % (i, x)

w, h = scipy.signal.freqz(b, 1.0)
h_db = 20*numpy.log10(abs(h))
pylab.plot(w, h_db, label="FIR")

pylab.show()


