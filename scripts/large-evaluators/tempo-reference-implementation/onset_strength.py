import numpy
import scipy.signal
import pylab

import overlap

def marsyas_hamming(N):
    ns = numpy.arange(N)
    hamming = 0.54 - 0.46 * numpy.cos( 2*numpy.pi*ns / (N-1.0))
    return hamming

def onset_strength_signal(defs, wav_sr, wav_data, plot=False):
    ### overlapping time data
    # add extra window of zeros at beginning to match marsyas
    overlapped = overlap.sliding_window(
        numpy.append(
            numpy.zeros(defs.OSS_WINDOWSIZE - defs.OSS_HOPSIZE),
            wav_data),
        #wav_data,
        defs.OSS_WINDOWSIZE, defs.OSS_HOPSIZE)
    oss_sr = wav_sr / float(defs.OSS_HOPSIZE)
    #print oss_sr
    if defs.OPTIONS_ONSET == 0:
        rms = numpy.sqrt( numpy.mean(overlapped**2, axis=1))
        #dif = numpy.clip( rms[1:] - rms[:-1], 0, numpy.Inf)
        #return oss_sr, dif
        return oss_sr, rms


    windowed = overlapped * marsyas_hamming(
        #scipy.signal.get_window( "hamming",
        defs.OSS_WINDOWSIZE)

    ### log-magnitude of FFT
    ffts = scipy.fftpack.fft(windowed, defs.OSS_WINDOWSIZE,  axis=1)
    ffts_abs = abs(ffts)[:,:ffts.shape[1]/2 + 1]
    # extra scaling to match Marsyas FFT output
    ffts_abs /= defs.OSS_WINDOWSIZE
    logmag = numpy.log(1.0 + 1000.0 * ffts_abs)

    ### flux
    flux = numpy.zeros( ffts_abs.shape[0] ) # output time signal
    prev = numpy.zeros( ffts_abs.shape[1] )
    for i in xrange( 0, ffts_abs.shape[0] ):
        diff = logmag[i] - prev
        diffreduced = diff[1:] # to match Marsyas
        diffclipped = diffreduced.clip(min=0)
        prev = numpy.copy(logmag[i])
        flux[i] = sum(diffclipped)
        #if i < 2:
        #    print diffclipped

    #numpy.savetxt('flux.txt', flux)
    ### clear out first window
    #flux[0] = 0.0
    if defs.OPTIONS_ONSET == 1:
        return oss_sr, flux
        

    if plot:
        ts = numpy.arange( len(flux) ) / oss_sr
        pylab.figure()
        #pylab.plot( ts, flux)
    ### filter
    if defs.OSS_LOWPASS_CUTOFF > 0 and defs.OPTIONS_ONSET < 3:
        b = scipy.signal.firwin(defs.OSS_LOWPASS_N,
            defs.OSS_LOWPASS_CUTOFF / (oss_sr/2.0) )
        #print b
        filtered_flux = scipy.signal.lfilter(b, 1.0, flux)

        #b, a = scipy.signal.butter(2, 0.1 / (oss_sr/2.0),
        #    btype="high")
        #filtered_flux = scipy.signal.filtfilt(b, a, flux)
    else:
        filtered_flux = flux

    if plot:
        ts = numpy.arange( len(filtered_flux) ) / oss_sr
        pylab.plot( ts, filtered_flux, label="filtered")
        pylab.title("Onset strength signal")

    ts = numpy.arange( len(filtered_flux) ) / oss_sr
    if defs.WRITE_ONSETS:
        #cutoff = int(2048*128/44100.0 * oss_sr)
        cutoff = 2048
        #print "cutoff", cutoff
        #print logmag.shape
        logmag_short = logmag[:cutoff,]
        #print logmag_short.shape
        numpy.savetxt('out/logmag.txt',
            logmag_short.transpose())
            #logmag[:cutoff,].transpose())

        numpy.savetxt('out/flux.txt',
            numpy.vstack( (ts, flux)).transpose() )
        numpy.savetxt('out/onset_strength.txt',
            numpy.vstack( (ts, filtered_flux)).transpose() )


    num_bh_frames = int(len(filtered_flux) / defs.BH_HOPSIZE)
    filtered_flux = filtered_flux[:num_bh_frames * defs.BH_HOPSIZE]

    if defs.CHECK_REFERENCE:
        calc = filtered_flux
        ref = numpy.loadtxt(
            "reference/%s/onset_signal_strength.txt" % defs.basename)
        delta = calc - ref
        maxerr = max(abs(delta))
        if maxerr < 1e-12:
            print "OSS ok, maximum deviation %.2g" % maxerr
        else:
            pylab.figure()
            pylab.title("OSS: calculated - reference")
            pylab.plot(delta)
            pylab.show()
            exit(1)
    if plot:
        pylab.legend()
    return oss_sr, filtered_flux


