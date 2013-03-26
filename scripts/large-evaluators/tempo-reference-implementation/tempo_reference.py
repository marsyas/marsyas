#!/usr/bin/env python

SHORT_DEBUG = 0

import sys
import os.path
import pickle

import numpy
import scipy.io.wavfile
import scipy.signal
import pylab

import mar_collection

import onset_strength
import beat_histogram
import beat_phase
import late_heuristic

import evaluate_bpms


def load_wavfile(filename):
    sample_rate, data_unnormalized = scipy.io.wavfile.read(filename)
    maxval = numpy.iinfo(data_unnormalized.dtype).max+1
    data_normalized = (numpy.array(data_unnormalized, dtype=numpy.float64)
        / float(maxval))
    return sample_rate, data_normalized



def bpm_of_file(defs, filename, plot=False, regen=False):
    ### handle OSS
    pickle_filename = filename + "-onsets-%i.pickle" % (
        defs.OPTIONS_ONSET)
    if os.path.exists(pickle_filename) and not regen:
        pickle_file = open(pickle_filename, 'rb')
        oss_sr, oss_data = pickle.load(pickle_file)
        pickle_file.close()
    else:
        wav_sr, wav_data = load_wavfile(filename)
        oss_sr, oss_data = onset_strength.onset_strength_signal(
            defs, wav_sr, wav_data,
            plot=False)
            #plot=True)
        pickle_file = open(pickle_filename, 'wb')
        pickle.dump( (oss_sr, oss_data), pickle_file, -1 )
        pickle_file.close()
    #print "OSS sr, len(data), seconds:\t", oss_sr, len(oss_data), len(oss_data)/oss_sr


    ### handle Beat Histogram
    pickle_filename = filename + "-bh-%i-%i.pickle" % (
        defs.OPTIONS_ONSET, defs.OPTIONS_BH)
    if os.path.exists(pickle_filename) and not regen:
        pickle_file = open(pickle_filename, 'rb')
        candidate_bpms = pickle.load(pickle_file)
        pickle_file.close()
    else:
        candidate_bpms = beat_histogram.beat_histogram(
            defs, oss_sr, oss_data,
            #plot=False)
            plot=plot)
        pickle_file = open(pickle_filename, 'wb')
        pickle.dump( (candidate_bpms), pickle_file, -1 )
        pickle_file.close()

    bpm1 = candidate_bpms[0]
    #return bpm, candidate_bpms
    #bpm1, bpm2 = beat_phase.beat_phase(defs, oss_sr, oss_data, candidate_bpms,
    #    plot=True)
        #plot=plot)

    #bpm = late_heuristic.late_heuristic(bpm1, bpm2, candidate_bpms[-1][0])

    if plot:
        pylab.show()
    #print bpm
    #return bpm, candidate_bpms
    return bpm1, candidate_bpms

def bpm_of_mf(defs, mf_filename, print_info=False):
    coll = mar_collection.MarCollection(mf_filename)
    new_filename = os.path.basename(
        mf_filename[:-3] + "-my-%i-%i.mf") % (
            defs.OPTIONS_ONSET,
            defs.OPTIONS_BH)
    newer = mar_collection.MarCollection(new_filename)
    #out = open('detailed-cands.txt', 'w')
    num_files = len(coll.data)
    i = 0
    good = 0
    if SHORT_DEBUG == 1:
        data = coll.data[:10]
    else:
        data = coll.data
    for dat in data:
        filename = dat[0]
        bpm_ground = float(dat[1])
        #print filename
        bpm_detect, cands = bpm_of_file(defs, filename)
        #out.write("%s\t%s\n" % (filename, cands))
        bpm_label = ','.join([str("%.3f") % f for f in cands])
        #print filename
        #print cands
        #print bpm_label
        newer.set_item( filename, bpm_label)
        i += 1
        #print "%i / %i" % (i, num_files)
        acc = evaluate_bpms.extended_harmonic_accuracy(bpm_detect, bpm_ground)
        if acc:
            good += 1
        accuracy = 100*float(good) / i
        if print_info:
            print "Accuracy: %.2f (%i/%i)" % (accuracy, good, i),
            if not acc:
                print "\tground: %.2f\tdetected: %.2f" % (
                    bpm_ground, bpm_detect),
                print filename
            print

    newer.write()
    #out.close()

    return accuracy




if __name__ == "__main__":
    import defs_class
    defs = defs_class.Defs()
    user_filename = sys.argv[1]
    if user_filename[-3:] == ".mf":
        bpm_of_mf(defs, user_filename, print_info=True)
    else:
        bpm, cands = bpm_of_file(defs, user_filename,
            plot=True, regen=True)
        print "BPM: %.2f" % bpm


