#!/usr/bin/env python

import numpy
import scipy.signal
import pylab


#print "31"
#b, a = scipy.signal.butter(4, 31.0 / (344.53125/2.0))
#print b
#print a
#w, h = scipy.signal.freqz(b, a)
#h_db = 20*numpy.log10(abs(h))
#pylab.plot(w, h_db, label="exact Butterworth 31")

#print "FIR, 15"
N = 15
#b = scipy.signal.firwin(N, 3.0 / (344.53125/2.0))
#b = scipy.signal.firwin(N, 7.0 / ((44100.0/256.0)/2.0))
b = scipy.signal.firwin(N, 7.0 / ((44100.0/512.0)/2.0))
a = numpy.ones(len(b))
#b = scipy.signal.firwin2(N,
#    [0.0, 4.0 / (344.53125/2.0/2.0), 1.0],
#    [0, 1.0, 0])
#print b

#N = 6
#print "IIR low, %i" % N
#b, a = scipy.signal.butter(N, 6.0 / (344.53125/2.0))


print "    mrs_realvec bcoeffs(1, %i);" % (N+1)
for i, x in enumerate(b):
    print "    bcoeffs(%i) = %.16f;" % (i, x)

#print "    mrs_realvec acoeffs(1, %i);" % (N+1)
#for i, x in enumerate(a):
#    print "    acoeffs(%i) = %.16f;" % (i, x)


if True:
    print "b = ["
    for i, x in enumerate(b):
        print "    %.16f " % x
    print "]';"
    #print "a = ["
    #for i, x in enumerate(a):
    #    print "    %.16f " % x
    #print "]';"

w, h = scipy.signal.freqz(b, 1.0)
h_db = 20*numpy.log10(abs(h))
pylab.plot(w, h_db, label="FIR")

b, a = scipy.signal.butter(6, 6.0 / (344.53125/2.0))
w, h = scipy.signal.freqz(b, a)
h_db = 20*numpy.log10(abs(h))
pylab.plot(w, h_db, label="IIR")


pylab.show()


