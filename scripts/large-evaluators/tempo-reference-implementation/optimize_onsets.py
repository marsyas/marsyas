#!/usr/bin/env python

import glob
import os.path

import numpy
import scipy.io.wavfile
import pylab

import defs_class
import onset_strength

ONSETS_DIR = "/home/gperciva/Downloads/onsets-combo/"
onsets_filenames = glob.glob(
    os.path.join(ONSETS_DIR, "*.lOst"))
onsets_filenames.sort()

def load_wavfile(filename):
    sample_rate, data_unnormalized = scipy.io.wavfile.read(filename)
    maxval = numpy.iinfo(data_unnormalized.dtype).max+1
    data_normalized = (numpy.array(data_unnormalized, dtype=numpy.float64)
        / float(maxval))
    return sample_rate, data_normalized

defs = defs_class.Defs()

for onsets_filename in onsets_filenames:
    onsets_basename = os.path.basename(onsets_filename)
    print onsets_basename
    wav_filename = onsets_filename[:-5] + ".wav"
    onsets = numpy.loadtxt(onsets_filename)
    #print onsets
    #print wav_filename

    wav_sr, wav_data = load_wavfile(wav_filename)
    oss_sr, oss_data = onset_strength.onset_strength_signal(
        defs, wav_sr, wav_data)
    # remove delay due to filter and flux
    oss_data = oss_data[7+1:]
    oss_ts = numpy.arange(len(oss_data)) / oss_sr
    oss_ts_last = oss_ts[-1]

    onsets_clipped = [ons for ons in onsets if ons < oss_ts_last]

    pylab.plot(oss_ts, oss_data, '.-')
    #wav_ts = numpy.arange(len(wav_data)) / float(wav_sr)
    #pylab.plot(wav_ts, wav_data)
    for ons in onsets_clipped:
        onset_time_radius = 0.05 # seconds
        pylab.axvspan(ons-onset_time_radius, ons+onset_time_radius,
            color="green", alpha=0.3)
        pylab.axvline(ons, color="green")

    pylab.savefig("out/%s" % (onsets_basename[:-5]+".png"))
    pylab.clf()
        #pylab.show()
    #numpy.savetxt("foo.txt", onsets)

    #exit(1)



