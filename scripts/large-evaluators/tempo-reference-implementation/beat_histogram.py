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


def find_peaks(defs, signal, number=10, peak_neighbors=1):
    candidates = []
    for i in xrange(4*defs.BPM_MIN+peak_neighbors,
        4*defs.BPM_MAX-peak_neighbors-1):
    #for i in xrange(200, 720):
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
    #pylab.figure()
    #pylab.plot(signal)
    for c in candidates[:number]:
        index = c[1]
        mag = c[0]
        peaks.append(index)
        #print c
        #pylab.plot(index, mag, 'o')
    #pylab.show()

    return numpy.array(peaks)

def autocorr_index_to_bpm(index, oss_sr):
    return 60.0*oss_sr / index
def bpm_to_autocorr_index(bpm, oss_sr):
    return 60.0*oss_sr / bpm

GCD_TOLERANCE = 0.1
def approximate_gcd(a, b):
    #print "gcd:", a, b
    if b < GCD_TOLERANCE:
        return a
    else:
        return approximate_gcd(b, math.fmod(a,b))

def approximate_lcm(a, b):
    #print "lcm:", a, b
    return a*b / approximate_gcd(a,b)

TOLERANCE = 1.04
def approximate_match(a, b):
    if a/TOLERANCE < b/TOLERANCE < a*TOLERANCE:
        return True
    if a/TOLERANCE < b*TOLERANCE < a*TOLERANCE:
        return True
    if b/TOLERANCE < a/TOLERANCE < b*TOLERANCE:
        return True
    if b/TOLERANCE < a*TOLERANCE < b*TOLERANCE:
        return True
    return False

MAX_BPM = 1000
def get_mults(bpm):
    cands = []
    k = 1
    cand = bpm*k
    while cand < MAX_BPM:
        cands.append(cand)
        k += 1
        cand = bpm*k
    return cands
        
def approximate_gcds(values):
    values = numpy.array(values)
    values = numpy.round(values)
    print "BPMS:\t", values
    combos = itertools.combinations(values, 3)
    lcms = {}
    for combo in combos:
        keep = set()
        lcm = 0
        mycands = get_mults(combo[0])
        cands = list(mycands)
        for v in combo[1:]:
            mycands = get_mults(v)
            keep = set()
            for a in mycands:
                for b in cands:
                    #print a, b,
                    if approximate_match(a, b):
                        #print "yes"
                        keep.add(a)
                        keep.add(b)
                    #else:
                    #    print "no"
            cands = keep
            #print "----"
            #print keep
        try:
            lcm = min(keep)
            if lcm in lcms:
                lcms[lcm] += 1
            else:
                lcms[lcm] = 1
        except:
            pass
        #print "lcm (%.1f, %.1f, %.1f):\t%.1f" %(
        #    combo[0], combo[1], combo[2], lcm)
    keeps = {}
    for l in lcms:
        done = False
        for k in keeps:
            if approximate_match(l, k):
                keeps[k] += lcms[l]
                done = True
                break
        if not done:
            keeps[l] = lcms[l]
    print keeps
    lcm = max(keeps.iteritems(), key=operator.itemgetter(1))[0]
    return lcm


def beat_histogram(defs, oss_sr, oss_data, plot=False):
    ### overlap
    overlapped = overlap.sliding_window(
        #numpy.append(
        #    numpy.zeros(defs.BH_WINDOWSIZE - defs.BH_HOPSIZE),
        #    oss_data[:-2*defs.BH_HOPSIZE]),
        oss_data,
        defs.BH_WINDOWSIZE, defs.BH_HOPSIZE)
    #beat_histogram_sr = oss_sr / defs.BH_HOPSIZE
    #for i in range(len(overlapped[0])):
    #    print overlapped[0][i]
    #exit(1)

    ### autocorrelation
    autocorr = autocorrelation(overlapped)

    ### beat histogram
    Hn = numpy.zeros( (autocorr.shape[0], 4*defs.BPM_MAX) )
    for i in xrange( autocorr.shape[0] ):
        #if i > 0 and i != (defs.BH_WINDOWSIZE / defs.BH_HOPSIZE):
        #    Hn[i] = Hn[i-1]
        prev_Hni = 4*defs.BPM_MAX-1
        pprev_Hni = prev_Hni
        sumamp = 0.0
        count = 1

        for j in xrange(1, autocorr.shape[1]):
            factor = 8/2
            Hni = int(oss_sr * 60.0 * factor / (j+1) + 0.5);
            #bpm = autocorr_bpms[i]
            if Hni < 4*defs.BPM_MAX:
                amp = autocorr[i][j]
                #print j, Hni, amp
                if amp < 0:
                    amp = 0
                if prev_Hni == Hni:
                    sumamp += amp
                    count += 1
                else:
                    sumamp += amp
                    Hn[i][prev_Hni] = sumamp / float(count)
                    sumamp = 0.0
                    count = 1
                ### linear interpolate not-set bins
                if pprev_Hni - prev_Hni > 1:
                    x0 = prev_Hni
                    x1 = pprev_Hni
                    y0 = Hn[i][prev_Hni]
                    y1 = Hn[i][pprev_Hni]
                    for k in xrange(prev_Hni+1, pprev_Hni):
                        Hn[i][k] = y0 + (y1-y0)*(k-x0)/(x1-x0)
                    #print x0, x1, y0, y1, Hn[i][pprev_Hni-1]
                pprev_Hni = prev_Hni
                prev_Hni = Hni
    #numpy.savetxt('bh.txt', Hn[0])

    #for a in range(0, 20):
    #    numpy.savetxt("bh-combo-%i.txt" % a, Hn[a])

    #if plot:
    #    pylab.figure()
    #    Hn_bpms = numpy.arange( 4*defs.BPM_MAX) / 4.0
    #    pylab.plot(Hn_bpms, summed_beat_histograms)
    #    pylab.title("Beat histogram")

    ### time stretch, add
    harmonic_strengthened_bh = numpy.zeros( Hn.shape )
    for i in xrange( Hn.shape[0] ):
        ### unchecked direct translation of marsyas
        factor2 = 0.5
        factor4 = 0.25
        stretched = numpy.zeros( Hn.shape[1] )
        numSamples = Hn.shape[1]
        for t in xrange( Hn.shape[1] ):
            ni = t*factor2
            li = int(ni) % numSamples
            ri = li + 1
            w = ni - li
            #print "%i\t%i\t%f\t%f" % (li, ri, w, ni)
            #zzz
            if ri < numSamples:
                stretched[t] += Hn[i][li] + w * (Hn[i][ri] - Hn[i][li])
            else:
                stretched[t] += Hn[t]

            ni = t*factor4
            li = int(ni) % numSamples
            ri = li + 1
            w = ni - li
            if ri < numSamples:
                stretched[t] += Hn[i][li] + w * (Hn[i][ri] - Hn[i][li])
            else:
                stretched[t] += Hn[t]
        harmonic_strengthened_bh[i] = (
            Hn[i]
            + stretched
            )

        if defs.WRITE_BH:
            samps = numpy.arange(defs.BH_WINDOWSIZE)
            numpy.savetxt("out/aq-%i.txt" % (i+1),
                numpy.vstack((samps, autocorr[i])).transpose())
            bpms = numpy.arange(4*defs.BPM_MAX)/4.0
            numpy.savetxt("out/bh-%i.txt" % (i+1),
                numpy.vstack((bpms, Hn[i])).transpose())
            numpy.savetxt("out/hbh-%i.txt" % (i+1),
                numpy.vstack((bpms, harmonic_strengthened_bh[i])).transpose())

    #for a in range(0, 20):
    #    numpy.savetxt("bh-combo-%i.txt" % a, harmonic_strengthened_bh[a])

    #if plot:
    #    Hn_bpms = numpy.arange( 4*defs.BPM_MAX) / 4.0
    #    pylab.plot(Hn_bpms, harmonic_strengthened_bh)

    ### pick top 8 candidates
    #peaks = []
    #for i in xrange( Hn.shape[0] ):
    #    these_peaks = find_peaks(harmonic_strengthened_bh[i],
    #        number=8, width=11)
    #    peaks.append(these_peaks)

    #summed = numpy.sum(harmonic_strengthened_bh, axis=0)
    #summed = numpy.sum(Hn, axis=0)

    if plot:
        pylab.figure()
        sHn = numpy.sum(Hn, axis=0)
        sHBH = numpy.sum(harmonic_strengthened_bh, axis=0)
        pylab.plot(numpy.arange(len(sHn))/4.0, sHn, label="sum")
        pylab.plot(numpy.arange(len(sHBH))/4.0, sHBH, label="enhanced")
        if defs.OPTIONS_BH == 3:
            b, a = scipy.signal.butter(1, 0.1)
            filtered = scipy.signal.filtfilt(b, a, sHBH)
            pylab.plot(numpy.arange(len(filtered))/4.0, filtered, label="filtered")
        pylab.title("Summed beat histogram")


#    folded_hist = numpy.zeros(60*4)
#    for i in xrange(1, len(summed)-1):
#        bpm = i/4.0
#        j = i
#        while bpm < 15:
#            bpm *= 2
#            j *= 2
#        while bpm > 30:
#            bpm /= 2.0
#            j /= 2.0
#        #j = int(round(j))
#        j = int(j)
#        #print "%i\tto\t%i" % (i, j)
#        if j >= len(folded_hist):
#            continue
#        folded_hist [j] += summed[i]
#


    if defs.WRITE_BH:
        combo_peaks = open('out/beat_histogram.txt', 'w')
    peaks = []
    bh_total = numpy.zeros( (Hn.shape[0], 10) )
    for i in xrange( Hn.shape[0] ):
        these_peaks = find_peaks(defs, harmonic_strengthened_bh[i],
            number=10, peak_neighbors=1)
        bh_total[i,:] = these_peaks
        if defs.WRITE_BH:
            tl = []
            for b in these_peaks:
                tl.append("%.2f" % (b/4.0))
            text = "  ".join(tl)
            combo_peaks.write( text + "\n")
            bpms = numpy.array(these_peaks)/4.0
            bpms_strengths = [harmonic_strengthened_bh[i][4*b] for b in bpms]
            numpy.savetxt("out/bh-peaks-%i.txt" % (i+1),
                numpy.vstack((bpms, bpms_strengths)).transpose())
        peaks.append( numpy.array(these_peaks) / 4.0)
    if defs.WRITE_BH:
        combo_peaks.close()

    if defs.CHECK_REFERENCE:
        calc = bh_total / 4.0
        ref = numpy.loadtxt(
            "reference/%s/beat_histogram.txt" % defs.basename)
        delta = calc - ref
        maxerr = numpy.abs(delta).max()
        if maxerr < 1e-12:
            print "BH ok, maximum deviation %.2g" % maxerr
        else:
            pylab.figure()
            pylab.title("BH: calculated - reference")
            pylab.plot(delta)
            pylab.show()
            exit(1)
    #cand_peaks = find_peaks(sHn,
    #        number=8, peak_neighbors=11) / 4.0
    #pylab.plot(numpy.arange(len(sHn))/4.0, sHn)
    #pylab.show()
    #pylab.plot(cand_peaks)
    return peaks

