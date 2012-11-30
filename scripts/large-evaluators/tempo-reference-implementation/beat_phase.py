import numpy
import pylab

import defs
import overlap

import scipy.signal


def calc_pulse_trains(bpm, window, sr):
    period = int(round(60.0 * sr / bpm))
    #pulse_trains = numpy.zeros( (period, length) )
    num_offsets = period
    samples = len(window)

    #print "phase", period
    bp_mags = numpy.zeros( num_offsets )
    for i, offset in enumerate(range(
            samples-1, samples-1-period, -1)):
        #print "offset:", offset
        mag = 0.0
        #num_beats = int( (samples - offset) / period )
        #for beat in range(num_beats):
        for b in range(4):
            ind = offset - period*b
            if ind > 0:
                mag += window[ind]
            # slow down by 2
            ind = offset - 2*period*b
            if ind > 0:
                mag += 0.5*window[ind]
            # slow down by 3
            ind = offset - int(1.5*period*b)
            if ind > 0:
                mag += 0.5*window[ind]
        #print "   ", i, mag
        bp_mags[i] = mag
    bp_max = max(bp_mags)
    bp_std = numpy.var(bp_mags)
    return bp_max, bp_std


def beat_phase(oss_sr, oss_data, candidate_bpms_orig, plot=False):
    ### overlap
    overlapped = overlap.sliding_window(
        numpy.append(
            numpy.zeros(defs.BH_WINDOWSIZE - defs.BH_HOPSIZE),
            oss_data),
        #oss_data,
        defs.BP_WINDOWSIZE, defs.BP_HOPSIZE)
    #beat_histogram_sr = oss_sr / defs.BP_HOPSIZE

    #print candidate_bpms_orig
    candidate_bpms = candidate_bpms_orig
    #for bpm in candidate_bpms_orig:
    #    #print bpm, base
    #    candidate_bpms.add(bpm)
    #candidate_bpms = list(candidate_bpms)
    #candidate_bpms.sort()
    #print candidate_bpms

    bhisto = numpy.zeros(defs.BPM_MAX)
    #bpms_max = numpy.zeros( len(candidate_bpms) )
    #bpms_std = numpy.zeros( len(candidate_bpms) )
    for i in xrange(overlapped.shape[0]):
        onset_scores = numpy.zeros(len(candidate_bpms))
        tempo_scores = numpy.zeros(len(candidate_bpms))
        for j, bpm in enumerate(candidate_bpms[i]):
            mag, std = calc_pulse_trains(bpm, overlapped[i], oss_sr)
            #bpms_max[i] += mag
            #bpms_std[i] += std
            ### correct up to here
            #print i, bpm, mag, std
            tempo_scores[j] = mag
            onset_scores[j] = std
        tempo_scores /= tempo_scores.sum()
        onset_scores /= onset_scores.sum()

        tempo_scores = tempo_scores + onset_scores
        tempo_scores /= tempo_scores.sum()

        # find best 2 scores
        besti = tempo_scores.argmax()
        bestbpm = candidate_bpms[i][besti]
        beststr = tempo_scores[besti]
        tempo_scores[besti] = 0.0
        second_besti = tempo_scores.argmax()
        second_bestbpm = candidate_bpms[i][second_besti]
        second_beststr = tempo_scores[second_besti]

        if i >= (defs.BP_WINDOWSIZE / defs.BP_HOPSIZE):
            bhisto[ int(bestbpm) ] += beststr
            #bhisto[ int(second_bestbpm*4) ] += second_beststr

        #print bestbpm, '\t', beststr, '\t',
        #print second_bestbpm, '\t', second_beststr

    #b, a = scipy.signal.butter(4, 0.5)
    #filt = scipy.signal.filtfilt(b, a, bhisto)
    

    if plot:
        pylab.plot(numpy.arange(len(bhisto))/4.0, bhisto,
            label="marsyas")
        #pylab.plot(numpy.arange(len(bhisto))/4.0, filt,
        #    label="extra filtered")
        pylab.title("bhisto")
        pylab.legend()

    bpm1 = bhisto.argmax()
    bhisto[bpm1] = 0.0
    bpm2 = bhisto.argmax()
    return bpm1, bpm2


