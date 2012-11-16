#!/usr/bin/env python

import sys

import numpy
import scipy.io.wavfile
import scipy.signal

import pylab

BPM_MIN = 20
BPM_MAX = 250

### from:
# http://www.johnvinyard.com/blog/?p=268
def norm_shape(shape):
    '''
    Normalize numpy array shapes so they're always expressed as a
tuple,
    even for one-dimensional shapes.
     
    Parameters
        shape - an int, or a tuple of ints
     
    Returns
        a shape tuple
    '''
    try:
        i = int(shape)
        return (i,)
    except TypeError:
        # shape was not a number
        pass
 
    try:
        t = tuple(shape)
        return t
    except TypeError:
        # shape was not iterable
        pass
     
    raise TypeError('shape must be an int, or a tuple of ints')

def sliding_window(a,ws,ss = None,flatten = True):
    '''
    Return a sliding window over a in any number of dimensions
     
    Parameters:
        a  - an n-dimensional numpy array
        ws - an int (a is 1D) or tuple (a is 2D or greater)
representing the size
             of each dimension of the window
        ss - an int (a is 1D) or tuple (a is 2D or greater)
representing the
             amount to slide the window in each dimension. If not
specified, it
             defaults to ws.
        flatten - if True, all slices are flattened, otherwise,
there is an
                  extra dimension for each dimension of the inumpyut.
     
    Returns
        an array containing each n-dimensional window from a
    '''
     
    if None is ss:
        # ss was not provided. the windows will not overlap in any
        # direction.
        ss = ws
    ws = norm_shape(ws)
    ss = norm_shape(ss)
     
    # convert ws, ss, and a.shape to numpy arrays so that we can
    # do math in every
    # dimension at once.
    ws = numpy.array(ws)
    ss = numpy.array(ss)
    shape = numpy.array(a.shape)
     
     
    # ensure that ws, ss, and a.shape all have the same number of
    # dimensions
    ls = [len(shape),len(ws),len(ss)]
    if 1 != len(set(ls)):
        raise ValueError('a.shape, ws and ss must all have the same length. The were %s' % str(ls))
     
    # ensure that ws is smaller than a in every dimension
    if numpy.any(ws > shape):
        raise ValueError('ws cannot be larger than a in any dimension. a.shape was %s and ws was %s' % (str(a.shape),str(ws)))
     
    # how many slices will there be in each dimension?
    newshape = norm_shape(((shape - ws) // ss) + 1)
    # the shape of the strided array will be the number of slices
    # in each dimension
    # plus the shape of the window (tuple addition)
    newshape += norm_shape(ws)
    # the strides tuple will be the array's strides multiplied by
    # step size, plus
    # the array's strides (tuple addition)
    newstrides = norm_shape(numpy.array(a.strides) * ss) + a.strides
    strided = numpy.lib.stride_tricks.as_strided(a,shape = newshape,strides = newstrides)
    if not flatten:
        return strided
     
    # Collapse strided so that it has one more dimension than the
    # window.  I.e.,
    # the new array is a flat list of slices.
    meat = len(ws) if ws.shape else 0
    firstdim = (numpy.product(newshape[:-meat]),) if ws.shape else ()
    dim = firstdim + (newshape[-meat:])
    # remove any dimensions with size 1
    dim = filter(lambda i : i != 1,dim)
    return strided.reshape(dim)



def load_wavfile(filename):
    sample_rate, data_unnormalized = scipy.io.wavfile.read(filename)
    data_normalized = (numpy.array(data_unnormalized, dtype=numpy.float64)
        / float(numpy.iinfo(data_unnormalized.dtype).max))
    return sample_rate, data_normalized

def onset_strength_signal(wav_sr, wav_data, plot=False):
    ### overlapping time data
    overlapped = sliding_window(wav_data, 256, 128)
    oss_sr = wav_sr / 128.0
    windowed = overlapped * scipy.signal.get_window(
        "hamming", overlapped.shape[1])
    ### log-magnitude of FFT
    ffts = scipy.fftpack.fft(windowed, windowed.shape[1],  axis=1)
    ffts_abs = abs(ffts)[:,:ffts.shape[1]/2 + 1]
    logmag = numpy.log(1.0 + 1000.0 * ffts_abs)
    ### flux
    flux = numpy.zeros( ffts_abs.shape[0] ) # output time signal
    prev = numpy.zeros( ffts_abs.shape[1] )
    for i in xrange( ffts_abs.shape[0] ):
        diff = logmag[i] - prev
        diff = diff.clip(min=0)
        prev = logmag[i]
        flux[i] = sum(diff)
    if plot:
        ts = numpy.arange( len(flux) ) / oss_sr
        pylab.plot( ts, flux)
    ### filter
    b, a = scipy.signal.butter(2, 50.0 / 172.266 )
    filtered_flux = scipy.signal.filtfilt(b, a, flux)

    if plot:
        ts = numpy.arange( len(filtered_flux) ) / oss_sr
        pylab.plot( ts, filtered_flux)
    if plot:
        pylab.show()
    return oss_sr, filtered_flux

def periodicity(oss_sr, oss_data, plot=False):
    ### overlap
    overlapped = sliding_window(oss_data, 2048, 128)
    period_sr = oss_sr / 128.0
    ### autocorrelation
    # 2: linear
    # 1: cyclic
    #ffts = scipy.fftpack.fft(overlapped, 2*2048, axis=1)
    ffts = scipy.fftpack.fft(overlapped, 1*2048, axis=1)
    ffts_abs = abs(ffts)
    ffts_abs_scaled = ffts_abs**0.5
    autocorr = numpy.real(scipy.fftpack.ifft(ffts_abs_scaled, axis=1))[:,1:]
    ### convert to BPMs
    autocorr_bpms = numpy.array(
            4*60.0 * oss_sr / (numpy.arange( 1,autocorr.shape[1]+1 )),
            dtype=numpy.int64
        )/4.0
    #if plot:
    #    for i in xrange(autocorr.shape[0]):
    #        autocorr_bpms = autocorr_bpms.clip(min=BPM_MIN-1, max=BPM_MAX+1)
    #        pylab.plot(autocorr_bpms, autocorr[i])
    #        pylab.show()

    ### beat histogram
    # TODO: clean this up, it's icky!
    summed_beat_histograms = numpy.zeros( 4*BPM_MAX - 4*BPM_MIN)
    for j in xrange( autocorr.shape[0] ):
        Hn = numpy.zeros( 4*BPM_MAX - 4*BPM_MIN)
        Hn_bpms = numpy.arange( 4*BPM_MIN, 4*BPM_MAX) / 4.0
        Hn_counts = numpy.zeros( 4*BPM_MAX - 4*BPM_MIN)
        for i in xrange(autocorr.shape[1]):
            bpm = autocorr_bpms[i]
            if BPM_MIN <= bpm <= BPM_MAX:
                Hni = numpy.where(Hn_bpms == bpm)
                #print i, bpm
                Hn[Hni] += autocorr[j][i]
                Hn_counts[Hni] += 1
        # divide by the numbers added
        for i in xrange( len(Hn) ):
            if Hn[i] > 0:
                Hn[i] /= Hn_counts[i]
        ### TODO: linearly interpolate the rest

        summed_beat_histograms += Hn

    if plot:
        pylab.plot(Hn_bpms, summed_beat_histograms)
        pylab.show()


def main(filename):
    wav_sr, wav_data = load_wavfile(filename)
    oss_sr, oss_data = onset_strength_signal(wav_sr, wav_data,
        plot=False)
    print "OSS sr, len(data), seconds:\t", oss_sr, len(oss_data), len(oss_data)/oss_sr
    periodicity(oss_sr, oss_data, plot=True)


if __name__ == "__main__":
    filename = sys.argv[1]
    main(filename)


