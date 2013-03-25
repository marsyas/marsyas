import numpy
import pylab

import overlap

import scipy.signal


def calc_pulse_trains(bpm, window, sr):
    period = int(round(60.0 * sr / bpm))
    #pulse_trains = numpy.zeros( (period, length) )
    num_offsets = period
    samples = len(window)

    bp_mags = numpy.zeros( num_offsets )
    origtrain, period_extra = make_impulse_train( bpm, samples, sr)
    for i in range(period):
        train = origtrain[i:i+samples]
        values = window * train
        #pylab.plot(train)
        #pylab.plot(values)
        #pylab.show()
        #exit(1)
        #print "offset:", offset
        #mag = 0.0
        #num_beats = int( (samples - offset) / period )
        #for beat in range(num_beats):
        #for b in range(4):
       # #    ind = offset - period*b
       # #    if ind > 0:
       #         mag += window[ind]
       #     # slow down by 2
       #     ind = offset - 2*period*b
       #     if ind > 0:
       #         mag += 0.5*window[ind]
       #     # slow down by 3
       #     ind = offset - int(1.5*period*b)
       #     if ind > 0:
       #         mag += 0.5*window[ind]
        #print "   ", i, mag
        bp_mags[i] = numpy.sum(values) / numpy.sum(train)
    bp_max = max(bp_mags)
    bp_std = numpy.var(bp_mags)
    return bp_max, bp_std

def make_impulse_train(bpm, num_samples, sr):
    #period = int(round(60.0 * sr / bpm))
    period = 60.0 * sr / bpm
    train = numpy.zeros( num_samples + int(period) )
    for i in range( int( (num_samples + period) / period) ):
        train[ period*i ] = 1.0
    #pylab.plot(train)
    #pylab.show()
    #exit(1)
    return train, period

def match_offset(train, window, period):
    values = numpy.zeros(period)
    samples = len(window)
    for i in range(int(period)):
        values[i] = numpy.sum( window * train[i:i+samples] )
    best_i = values.argmax()
    var = numpy.std(values)
    #pylab.plot(values)
    #pylab.show()
    #exit(1)
    return values[best_i], best_i, var


def make_sine_train(bpm, num_samples, sr):
    period = int(round(60.0 * sr / bpm))



def calc_sine_trains(bpm, window, sr):
    period = int(round(60.0 * sr / bpm))
    #pulse_trains = numpy.zeros( (period, length) )
    num_offsets = period
    samples = len(window)

    #print period
    bp_mags = numpy.zeros( num_offsets )
    for i, offset in enumerate(range(
            samples-1, samples-1-period, -1)):
        #train = (numpy.arange(samples) % period / float(period))
        train = numpy.sin( (offset+numpy.arange(samples)) *2*numpy.pi / period)
        train = train.clip(min=0)
        values = window * train

        #if i % 50 == 0:
        #    pylab.plot(window)
        #    pylab.plot(values)
        #if i > 300:
        #    pylab.show()
        #    exit(1)

        bp_mags[i] = numpy.sum(values) / numpy.sum(train)
    bp_max = max(bp_mags)
    bp_std = numpy.var(bp_mags)
    return bp_max, bp_std


def beat_phase(defs, oss_sr, oss_data, candidate_bpms_orig, plot=False):
    ### overlap
    overlapped = overlap.sliding_window(
        #numpy.append(
        #    numpy.zeros(defs.BH_WINDOWSIZE - defs.BH_HOPSIZE),
        #    oss_data),
        oss_data,
        defs.BP_WINDOWSIZE, defs.BP_HOPSIZE)
    #beat_histogram_sr = oss_sr / defs.BP_HOPSIZE

    #print candidate_bpms_orig
    candidate_bpms = candidate_bpms_orig
    candidate_bpms = candidate_bpms_orig[:4]
    #for bpm in candidate_bpms_orig:
    #    #print bpm, base
    #    candidate_bpms.add(bpm)
    #candidate_bpms = list(candidate_bpms)
    #candidate_bpms.sort()
    #print candidate_bpms
    if defs.OPTIONS_BP == 0:
        onset_scores = numpy.zeros(len(candidate_bpms))
        tempo_scores = numpy.zeros(len(candidate_bpms))
        pylab.plot(overlapped[0])
        for j, bpm in enumerate(candidate_bpms):
            #print "Evaluating %.1f BPM" % bpm
            #train = make_sine_train(bpm)
            train, period = make_impulse_train(bpm, len(overlapped[0]), oss_sr)

            for i in xrange(overlapped.shape[0]):
            #for i in xrange(1):
                mag, offset, var = match_offset(train, overlapped[i], period)
                #print "%.2f\t%.2f" % (
                #    mag / numpy.sum(train), var)

                onset_scores[j] += mag + var

                #pylab.title("actual signal")
                #pylab.plot(overlapped[i])
                #pylab.plot(train)
                #pylab.show()
            pylab.plot(30*train[period-8:])
        #print onset_scores / max(onset_scores)
        best_i = onset_scores.argmax()
        print onset_scores[0] + onset_scores[2]
        print onset_scores[1] + onset_scores[3]

        pylab.show()
        return candidate_bpms[best_i], 0

        exit(1)
        if False:
            if False:
                #mag, std = calc_pulse_trains(bpm, overlapped[i], oss_sr)
                mag, std = calc_sine_trains(bpm, overlapped[i], oss_sr)
                #print "%.1f\t%.3f\t%.3f" % (bpm, mag, std)
                onset_scores[j] += mag
                tempo_scores[j] += std
            #exit(1)
        #print candidate_bpms
        #print onset_scores
        #print tempo_scores
        best_i = tempo_scores.argmax()
        best_bpm = candidate_bpms[best_i]
        return best_bpm, 0


    bhisto = numpy.zeros(defs.BPM_MAX)
    #bpms_max = numpy.zeros( len(candidate_bpms) )
    #bpms_std = numpy.zeros( len(candidate_bpms) )
    for i in xrange(overlapped.shape[0]):
        onset_scores = numpy.zeros(len(candidate_bpms))
        tempo_scores = numpy.zeros(len(candidate_bpms))
        #for j, bpm in enumerate(candidate_bpms[i]):
        for j, bpm in enumerate(candidate_bpms):
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
        #bestbpm = candidate_bpms[i][besti]
        bestbpm = candidate_bpms[besti]
        beststr = tempo_scores[besti]
        #tempo_scores[besti] = 0.0
        #second_besti = tempo_scores.argmax()
        #second_bestbpm = candidate_bpms[i][second_besti]
        #second_beststr = tempo_scores[second_besti]

        if i >= (defs.BP_WINDOWSIZE / defs.BP_HOPSIZE):
            bhisto[ int(bestbpm) ] += beststr
            #bhisto[ int(second_bestbpm*4) ] += second_beststr

        #print bestbpm, '\t', beststr, '\t',
        #print second_bestbpm, '\t', second_beststr

    #b, a = scipy.signal.butter(4, 0.5)
    #filt = scipy.signal.filtfilt(b, a, bhisto)
    

    if plot:
        pylab.figure()
        pylab.plot(numpy.arange(len(bhisto)), bhisto,
            label="marsyas")
        #pylab.plot(numpy.arange(len(bhisto))/4.0, filt,
        #    label="extra filtered")
        pylab.title("bhisto")
        pylab.legend()

    bpm1 = bhisto.argmax()
    bhisto[bpm1] = 0.0
    bpm2 = bhisto.argmax()
    return bpm1, bpm2


