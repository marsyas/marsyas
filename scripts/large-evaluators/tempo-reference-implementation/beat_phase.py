import numpy
import pylab

import overlap

#import scipy.signal


def calc_pulse_trains(bpm, window, sr):
    period = int(round(60.0 * sr / bpm))
    #pulse_trains = numpy.zeros( (period, length) )
    num_offsets = period
    samples = len(window)

    #print "#\t%.2f\t%i" % (bpm, period)

    bp_mags = numpy.zeros( num_offsets )
    for phase in range(samples-1, samples-1-period, -1):
        #print "# %i" %(phase)
        mag = 0.0
        #num_beats = int( (samples - offset) / period )
        #for beat in range(num_beats):
        #print
        for b in range(4):
            ind = int(phase - b*period)
            if ind >= 0:
                mag += window[ind]
            #print "\t%i\t%f" % (ind, mag)

            # slow down by 2
            ind = int(phase - b*period*2)
            if ind >= 0:
                mag += 0.5*window[ind]
            #print "\t%i\t%f" % (ind, mag)

            # slow down by 3
            ind = int(phase - b*period*3/2)
            if ind >= 0:
                mag += 0.5*window[ind]
            #print "\t%i\t%f" % (ind, mag)
        #if phase == 1869:
            #print phase, mag
        #    exit(1)
        #print "   ", i, mag
        bp_mags[samples-1-phase] = mag
        #bp_mags[period] = numpy.sum(values) / numpy.sum(train)

    bp_max = max(bp_mags)
    bp_var = numpy.var(bp_mags)
    #print bpm, period, bp_max, bp_var
    #numpy.savetxt("mags.txt", bp_mags)
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
    
    if defs.WRITE_BP:
        bp_accum = open("out/bp-accum.txt", "w")
    #print candidate_bpms_orig
    candidate_bpms = candidate_bpms_orig
    bphase = numpy.zeros(defs.BPM_MAX)
    for i in xrange(overlapped.shape[0]):
        cands = candidate_bpms[i]

        onset_scores = numpy.zeros(len(cands))
        tempo_scores = numpy.zeros(len(cands))
        for j, bpm in enumerate(cands):
            if bpm == 0:
                continue
            mag, var = calc_pulse_trains(bpm, overlapped[i], oss_sr)
            tempo_scores[j] = mag
            onset_scores[j] = var
        if defs.WRITE_BP:
            numpy.savetxt("out/bp-%i.txt" % (i+1),
                numpy.vstack((cands, tempo_scores, onset_scores)).transpose())
        tempo_scores /= tempo_scores.sum()
        onset_scores /= onset_scores.sum()

        combo_scores = tempo_scores + onset_scores
        combo_scores /= combo_scores.sum()

        # find best score
        besti = combo_scores.argmax()
        bestbpm = round(cands[besti])
        beststr = combo_scores[besti]


        bphase[ int(bestbpm) ] += beststr
            
        if defs.WRITE_BP:
            #numpy.savetxt("out/bp-%i.txt" % (i+1),
            #    numpy.vstack((cands, tempo_scores, onset_scores, combo_scores)).transpose())
            #numpy.savetxt("out/bp-peak-%i.txt" % (i+1),
            #    numpy.vstack((int(bestbpm), beststr)).transpose())
            text = "%i\t%.15f\n" % (int(bestbpm), beststr)
            bp_accum.write(text)

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
    #bpm_strength = bphase[bpm]

    if defs.CHECK_REFERENCE:
        calc = bphase
        ref = numpy.loadtxt(
            "reference/%s/beat_phase.txt" % defs.basename)
        delta = calc - ref
        maxerr = numpy.abs(delta).max()
        if maxerr < 1e-6:
            print "BP ok, maximum deviation %.2g" % maxerr
        else:
            pylab.figure()
            pylab.title("BP: calculated - reference")
            pylab.plot(delta)
            pylab.figure()
            pylab.plot(calc)
            pylab.plot(ref)
            pylab.show()
            exit(1)

    if defs.WRITE_BP:
        bp_accum.close()
    return bpm, bphase


