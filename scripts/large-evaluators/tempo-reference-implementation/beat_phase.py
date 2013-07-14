import numpy
import pylab

import overlap

import scipy.signal


def calc_pulse_trains(bpm, window, sr):
    period = int(round(60.0 * sr / bpm))
    #pulse_trains = numpy.zeros( (period, length) )
    num_offsets = period
    samples = len(window)

    #print bpm, period
    bp_mags = numpy.zeros( num_offsets )
    for phase in range(samples-1, samples-1-period, -1):
        #print "# ", phase
        mag = 0.0
        #num_beats = int( (samples - offset) / period )
        #for beat in range(num_beats):
        for b in range(4):
            ind = int(phase - b*period)
            if ind >= 0:
                mag += window[ind]
            #print ind, mag
            # slow down by 2
            ind = int(phase - b*2*period)
            if ind >= 0:
                mag += 0.5*window[ind]
            #print ind, mag
            # slow down by 3
            ind = int(phase - b*1.5*period)
            if ind >= 0:
                mag += 0.5*window[ind]
            #print ind, mag
        #print "   ", i, mag
        bp_mags[samples-1-phase] = mag
        #bp_mags[period] = numpy.sum(values) / numpy.sum(train)

    bp_max = max(bp_mags)
    bp_var = numpy.var(bp_mags)
    return bp_max, bp_var

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
    bp_var = numpy.var(bp_mags)
    return bp_max, bp_var


def beat_phase(defs, oss_sr, oss_data, candidate_bpms_orig, plot=False):
    ### overlap
    overlapped = overlap.sliding_window(
        #numpy.append(
        #    numpy.zeros(defs.BH_WINDOWSIZE - defs.BH_HOPSIZE),
        #    oss_data[:-2*defs.BH_HOPSIZE]),
        oss_data,
        defs.BP_WINDOWSIZE, defs.BP_HOPSIZE)
    #beat_histogram_sr = oss_sr / defs.BP_HOPSIZE
    
    #print candidate_bpms_orig
    candidate_bpms = candidate_bpms_orig
    bphase = numpy.zeros(defs.BPM_MAX)
    for i in xrange(overlapped.shape[0]):
        cands = candidate_bpms[i]
        #if i in defs.extra:
        #    cands = range(defs.BPM_MIN, defs.BPM_MAX)
        #    print "BP altering cands"

        onset_scores = numpy.zeros(len(cands))
        tempo_scores = numpy.zeros(len(cands))
        for j, bpm in enumerate(cands):
        #for j, bpm in enumerate(candidate_bpms):
            if bpm == 0:
                continue
            mag, var = calc_pulse_trains(bpm, overlapped[i], oss_sr)
            #bpms_max[i] += mag
            #bpms_std[i] += std
            ### correct up to here
            #print i, bpm, mag, std
            tempo_scores[j] = mag
            #print tempo_scores[j]
            onset_scores[j] = var
        tempo_scores /= tempo_scores.sum()
        onset_scores /= onset_scores.sum()

        tempo_scores = tempo_scores + onset_scores
        tempo_scores /= tempo_scores.sum()

        # find best 2 scores
        besti = tempo_scores.argmax()
        bestbpm = round(cands[besti])
        #bestbpm = candidate_bpms[besti]
        #print candidate_bpms[i]
        #print tempo_scores
        beststr = tempo_scores[besti]
        #tempo_scores[besti] = 0.0
        #second_besti = tempo_scores.argmax()
        #second_bestbpm = candidate_bpms[i][second_besti]
        #second_beststr = tempo_scores[second_besti]

        #if i >= (defs.BP_WINDOWSIZE / defs.BP_HOPSIZE):
        bphase[ int(bestbpm) ] += beststr
        #print bestbpm, "\t", beststr

        #if i in defs.extra:
        #    gnd = 120
        #    if gnd*0.96 <= bestbpm <= gnd*1.04:
        #        print "YES", i
            

        if defs.WRITE_BP:
            numpy.savetxt("out/bp-%i.txt" % (i+1),
                numpy.vstack((cands, tempo_scores)).transpose())
            numpy.savetxt("out/bp-peak-%i.txt" % (i+1),
                numpy.vstack((int(bestbpm), beststr)).transpose())

    if defs.WRITE_BP:
        bp = open('out/beat_phase.txt', 'w')
        for b in bphase:
            if b == 0:
                text = "0\n"
            else:
                text = "%.5f\n" % b
            bp.write(text)
        bp.close()

    if plot:
        pylab.figure()
        pylab.plot(numpy.arange(len(bphase)), bphase,
            label="marsyas")
        pylab.title("bphase")
        pylab.legend()

    bpm = bphase.argmax()
    bpm_strength = bphase[bpm]

    return bpm, bphase


