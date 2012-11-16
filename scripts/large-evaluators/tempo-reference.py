#!/usr/bin/env python

import sys

import numpy
import scipy.io.wavfile
import scipy.signal

import pylab

PLOTS = False
PLOTS = True


# function from:
# http://mail.scipy.org/pipermail/numpy-discussion/2011-March/055202.html
def overlap_array( y, len_blocks, overlap=0 ):
     """
     Make use of strides to return a two dimensional whose
     rows come from a one dimensional array. Strides are
     used to return rows that partially overlap.

     Parameters
     ----------
     y        : a one dimensional array
     len_blocks : the row length. The length of chunks from y.
     overlap  : number of elements that overlap. From 0 (no
                overlap) to len_blocks-1 (almost full overlap).

     Returns
     -------
     x        : a strided array

     """
     overlap = int(overlap)
     len_blocks = int(len_blocks)

     if not type(y) == numpy.ndarray:
         raise ValueError( 'y must be a numpy.ndarray' )

     if overlap >= len_blocks:
         raise ValueError( 'overlap must be less than n_points' )

     # compute shape and strides of the strided vector
     strides = ( (len_blocks - overlap)*y.itemsize, y.itemsize )
     shape = ( 1 + (y.nbytes - len_blocks*y.itemsize)/strides[0],  len_blocks)

     # create a strided array
     return numpy.lib.stride_tricks.as_strided( y, shape=shape,
strides=strides )


def load_wavfile(filename):
    sample_rate, data_unnormalized = scipy.io.wavfile.read(filename)
    data_normalized = (numpy.array(data_unnormalized, dtype=numpy.float64)
        / float(numpy.iinfo(data_unnormalized.dtype).max))
    return sample_rate, data_normalized

def onset_strength_signal(time_data):
    ### overlapping time data
    overlapped = overlap_array(time_data, 256, overlap=128)
    windowed = overlapped * scipy.signal.get_window(
        "hamming", overlapped.shape[1])
    ### log-magnitude of FFT
    ffts = scipy.fftpack.fft(windowed, windowed.shape[1],  axis=1)
    ffts_abs = abs(ffts)[:,:ffts.shape[1]/2 + 1]
    logmag = numpy.log(1.0 + 1000.0 * ffts_abs)
    ### flux
    flux = numpy.zeros( ffts_abs.shape[0] )
    prev = numpy.zeros( ffts_abs.shape[1] )
    for i in xrange( ffts_abs.shape[0] ):
        diff = logmag[i] - prev
        diff = diff.clip(min=0)
        prev = logmag[i]
        flux[i] = sum(diff)
    if PLOTS:
        pylab.plot(flux)
    ### filter
    b, a = scipy.signal.butter(2, 50.0 / 172.266 )
    filtered_flux = scipy.signal.lfilter(b, a, flux)

    if PLOTS:
        pylab.plot(filtered_flux)

    if PLOTS:
        pylab.show()

def main(filename):
    sr, data = load_wavfile(filename)
    oss = onset_strength_signal(data)


if __name__ == "__main__":
    filename = sys.argv[1]
    main(filename)


