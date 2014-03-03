import math
import itertools
import operator

import numpy
import pylab
import scipy.fftpack

import overlap

def autocorrelation(signal):
    """ this matches Marsyas exactly. """
    N = signal.shape[1]
    ffts = scipy.fftpack.fft(signal, 2*N, axis=1) / (2*N)
    ffts_abs = abs(ffts)
    ffts_abs_scaled = ffts_abs**0.5
    scratch = (scipy.fftpack.ifft(ffts_abs_scaled, axis=1
        ).real)*(2*N)
    xcorr = scratch[:,:N]
    return xcorr


def find_peaks(defs, signal, number=10, peak_neighbors=1,
        minsample=0, maxsample=None):
    candidates = []
    if maxsample is None:
        maxsample = len(signal)

    for i in xrange(minsample+peak_neighbors,
            maxsample - peak_neighbors-1):
        if signal[i-1] < signal[i] > signal[i+1]:
            ok = True
            for j in xrange(i-peak_neighbors, i):
                if signal[j] >= signal[i]:
                    ok = False
            for j in xrange(i+1, i+peak_neighbors):
                if signal[j] >= signal[i]:
                    ok = False
            if ok:
                candidates.append( (signal[i], i) )
    candidates.sort(reverse=True)

    peaks = []
    for c in candidates[:number]:
        index = c[1]
        #mag = signal[index]
        peaks.append(index)
    return numpy.array(peaks)

def autocorr_index_to_bpm(index, oss_sr):
    return 60.0*oss_sr / index
def bpm_to_autocorr_index(bpm, oss_sr):
    return 60.0*oss_sr / bpm


def calc_pulse_trains(lag, window, sr):
    period = lag
    num_offsets = period
    samples = len(window)

    bp_mags = numpy.zeros( num_offsets )
    for phase in range(samples-1, samples-1-period, -1):
        mag = 0.0
        for b in range(4):
            ind = int(phase - b*period)
            # this is I_{ P, phi, 1)
            if ind >= 0:
                mag += window[ind]

            # this is I_{ P, phi, 2)
            # slow down by 2
            ind = int(phase - b*period*2)
            if ind >= 0:
                mag += 0.5*window[ind]

            # this is I_{ P, phi, 1.5)
            # slow down by 3
            ind = int(phase - b*period*3/2)
            if ind >= 0:
                mag += 0.5*window[ind]
        bp_mags[samples-1-phase] = mag
    bp_max = max(bp_mags)
    bp_var = numpy.var(bp_mags)
    return bp_max, bp_var


def beat_period_detection(defs, oss_sr, oss_data, plot=False):
    ### 1) Overlap
    overlapped = overlap.sliding_window(
        #numpy.append(
        #    numpy.zeros(defs.BH_WINDOWSIZE - defs.BH_HOPSIZE),
        #    oss_data[:-2*defs.BH_HOPSIZE]),
        oss_data,
        defs.BH_WINDOWSIZE, defs.BH_HOPSIZE)
    #beat_period_sr = oss_sr / defs.BH_HOPSIZE

    ### 2) Generalized Autocorrelation
    autocorr = autocorrelation(overlapped)

    minlag = int(oss_sr*60.0 / defs.BPM_MAX)
    maxlag = int(oss_sr*60.0 / defs.BPM_MIN) + 1

    num_frames = autocorr.shape[0]
    #win_size = autocorr.shape[1]

    ### 3) Enhance Harmonics
    harmonic_enhanced = numpy.zeros( autocorr.shape )
    for i in xrange( num_frames ):
        auto = autocorr[i]
        stretched = numpy.zeros( defs.BH_WINDOWSIZE )
        for j in xrange( 512 ):
            stretched[j] = auto[2*j] + auto[4*j]
        harmonic_enhanced[i] = (
            auto + stretched
            )

    ### 4) Pick peaks
    peaks = numpy.zeros( (num_frames, 10) )
    for i in xrange( num_frames ):
        these_peaks = find_peaks(defs, harmonic_enhanced[i],
            number=10, peak_neighbors=1, minsample=minlag,
            maxsample=maxlag)
        peaks[i,:] = these_peaks

    ### 5) Evaluate pulse trains
    tempo_lags = numpy.zeros(num_frames)
    for i in xrange(num_frames):
        cands = peaks[i]
        onset_scores = numpy.zeros(len(cands))
        tempo_scores = numpy.zeros(len(cands))
        for j, cand in enumerate(cands):
            if cand == 0:
                continue
            lag = int(round(cand))
            mag, var = calc_pulse_trains(lag, overlapped[i], oss_sr)
            tempo_scores[j] = mag
            onset_scores[j] = var
        tempo_scores /= tempo_scores.sum()
        onset_scores /= onset_scores.sum()

        combo_scores = tempo_scores + onset_scores
        combo_scores /= combo_scores.sum()

        # find best score
        besti = combo_scores.argmax()
        bestlag = round(cands[besti])

        tempo_lags[i] = bestlag

    if defs.CHECK_REFERENCE:
        ref = numpy.loadtxt(
            "reference/BEATS-5-pulse.txt")
        delta = tempo_lags - ref
        maxerr = numpy.abs(delta).max()
        if maxerr < 1e-12:
            print "Beat pulse detection ok, maximum deviation %.2g" % maxerr
        else:
            pylab.figure()
            pylab.title("Beat pulse detection: this - reference")
            pylab.plot(delta)
            pylab.show()
            exit(1)
    return tempo_lags

