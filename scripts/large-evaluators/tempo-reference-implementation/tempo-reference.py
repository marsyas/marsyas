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
        plot=True)
    #    #plot=False)

    bpm = candidate_bpms
    return bpm
    #bpm1, bpm2 = beat_phase.beat_phase(oss_sr, oss_data, candidate_bpms,
    #    plot=True)
        #plot=plot)

    #bpm = late_heuristic.late_heuristic(bpm1, bpm2, candidate_bpms[-1][0])

    if plot:
        pylab.show()
    #print bpm
    #return bpm, candidate_bpms
    return bpm

def extended_harmonic_accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    for m in [1, 2, 3, 4, 6, 8]:
    #for m in [1, 2, 3]:
        diff = abs(m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return True
        diff = abs(1.0/m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return True
    return False


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
        good = 0
        #for dat in coll.data[:10]:
        for dat in coll.data:
            filename = dat[0]
            bpm_ground = float(dat[1])
            #print filename
            bpm_detect = main(filename)
            #out.write("%s\t%s\n" % (filename, cands))
            newer.set_item( filename, bpm_detect)
            i += 1
            #print "%i / %i" % (i, num_files)
            acc = extended_harmonic_accuracy(bpm_detect, bpm_ground)
            if acc:
                good += 1
            print "Accuracy: %.2f (%i/%i)" % (100*float(good) / i, good, i),
            if not acc:
                print "\tground: %.2f\tdetected: %.2f" % (
                    bpm_ground, bpm_detect),
                #print filename
            print

        newer.write()
        #out.close()
    else:
        bpm = main(user_filename, plot=True)
        print "BPM: %.2f" % bpm


