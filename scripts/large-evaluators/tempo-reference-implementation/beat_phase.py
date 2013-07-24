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
    #if bpm == 66:
    #    numpy.savetxt("input.txt", window)
    #    numpy.savetxt("foo.txt", bp_mags)
    #numpy.savetxt("mags.txt", bp_mags)
    #exit(1)
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
        #print "aaaa", len(cands), cands[-1]
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
            #print i, bpm, mag, var
            tempo_scores[j] = mag
            #print tempo_scores[j]
            onset_scores[j] = var
        #exit(1)
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
        #if i == 15:
        #    print cands
        #    print tempo_scores
        #    exit(1)

        #for k in range(len(tempo_scores)):
        #    #print cands[k], "\t", tempo_scores[k]
        #    print "%.4f" % tempo_scores[k],
        #print
        # zzz

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

    return bpm, bphase


