#!/usr/bin/env python

import sys

import os.path
import numpy
import scipy.io.wavfile
import scipy.signal

import pylab

import mar_collection

import onset_strength
import beat_histogram
import beat_phase
import late_heuristic


def load_wavfile(filename):
    sample_rate, data_unnormalized = scipy.io.wavfile.read(filename)
    maxval = numpy.iinfo(data_unnormalized.dtype).max+1
    data_normalized = (numpy.array(data_unnormalized, dtype=numpy.float64)
        / float(maxval))
    return sample_rate, data_normalized



def main(filename, plot=False):
    wav_sr, wav_data = load_wavfile(filename)
    oss_sr, oss_data = onset_strength.onset_strength_signal(wav_sr, wav_data,
        #plot=True)
        plot=False)
    #print "OSS sr, len(data), seconds:\t", oss_sr, len(oss_data), len(oss_data)/oss_sr
    candidate_bpms = beat_histogram.beat_histogram(oss_sr, oss_data,
        #plot=True)
        plot=False)
    #candidate_bpms = [30, 60, 120, 180]
    #bpms = candidate_bpms
    bpm1, bpm2 = beat_phase.beat_phase(oss_sr, oss_data, candidate_bpms,
        #plot=True)
        plot=plot)

    bpm = late_heuristic.late_heuristic(bpm1, bpm2, candidate_bpms[-1][0])

    #bpm_i = summed_beat_histograms.argmax()
    #bpm = histogram_bpms[bpm_i]
    if plot:
        pylab.show()
    #print bpm
    #return bpm, candidate_bpms
    return bpm


if __name__ == "__main__":
    user_filename = sys.argv[1]
    if user_filename[-3:] == ".mf":
        coll = mar_collection.MarCollection(user_filename)
        new_filename = os.path.basename(
            user_filename[:-3] + "-my.mf")
        newer = mar_collection.MarCollection(new_filename)
        #out = open('detailed-cands.txt', 'w')
        num_files = len(coll.data)
        i = 0
        for dat in coll.data:
            filename = dat[0]
            bpm_ground = dat[1]
            #print filename
            bpm_detect = main(filename)
            #out.write("%s\t%s\n" % (filename, cands))
            newer.set_item( filename, bpm_detect)
            i += 1
            print "%i / %i" % (i, num_files)
        newer.write()
        #out.close()
    else:
        bpm = main(user_filename, plot=True)
        print "BPM: %.2f" % bpm


