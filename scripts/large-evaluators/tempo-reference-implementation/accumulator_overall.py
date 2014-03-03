#!/usr/bin/env python

import numpy
import scipy.stats
import pylab

def energy_in_histo_range(histo, low, high):
    index_low = round(low)
    index_high = round(high)
    if high == 1:
        index_high = len(histo)-1
    if high > len(histo)-1:
        high = len(histo)-1
    if low < 0:
        low = 0
    return sum(histo[index_low:index_high+1])


def info_histogram(bpm, histo, tolerance):
    energy_total = energy_in_histo_range(histo, 0, 1.0)
    energy_under = energy_in_histo_range(histo,
        0, bpm - tolerance) / energy_total

    str05 = energy_in_histo_range(histo,
        0.5*bpm-tolerance, 0.5*bpm+tolerance) / energy_total

    info = [ energy_under, str05 ]
    return info



def accumulator_overall(defs, tempo_lags, oss_sr):
    pdf = scipy.stats.norm.pdf(numpy.arange(2000)-1000, loc=0,
        scale=10)

    ### 1) convert to Gaussian, and
    ### 2) Accumulator (sum)
    accum = numpy.zeros(414)
    for lag in tempo_lags:
        begin = 1000-lag
        end = 1000-lag+414
        accum += pdf[begin:end]

    ### 3) Pick peak
    tempo_lag = numpy.argmax(accum)
    bpm = oss_sr*60 / tempo_lag

    ### Octave Decider
    mult = 1.0
    if defs.DOUBLE_TYPE == 1:
        if bpm <= 71.9:
            mult = 2.0
    elif defs.DOUBLE_TYPE == 2:
        features = info_histogram(tempo_lag, accum, 10)
        features.append(bpm)
        print features
        ## hard-coded values trained elsewhere; see tempo.cpp
        mins = [ 0.0321812, 1.68126e-83, 50.1745, ]
        maxs = [ 0.863237, 0.449184, 208.807, ]
        svm_weights51 = numpy.array([
            -1.9551, 0.4348, -4.6442, 3.2896
            ])
        svm_weights52 = numpy.array([
             -3.0408, 2.7591, -6.5367, 3.081
            ])
        svm_weights12 = numpy.array([
            -3.4624, 3.4397, -9.4897, 1.6297
            ])

        # normalize
        features_normalized = numpy.array(features)
        for i in range(len(features)):
            features_normalized[i] = (features[i] - mins[i]) / (maxs[i] - mins[i])

        # svm
        svm_sum51 = svm_weights51[-1] + numpy.dot(
            features_normalized, svm_weights51[:-1])
        svm_sum52 = svm_weights52[-1] + numpy.dot(
            features_normalized, svm_weights52[:-1])
        svm_sum12 = svm_weights12[-1] + numpy.dot(
            features_normalized, svm_weights12[:-1])
        if (svm_sum52 > 0) and (svm_sum12 > 0):
            mult = 2.0
        if (svm_sum51 <= 0) and (svm_sum52 <= 0):
            mult = 0.5

    return mult*bpm


