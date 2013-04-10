#!/usr/bin/env python

import sys
import mar_collection


def exact_accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    diff = abs(bpm_detected - bpm_ground)
    if diff <= tolerance * bpm_ground:
        return True
    return False

def major_extended_harmonic_accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    m = 1
    cand = bpm_ground*m
    while cand < 1000:
        cand = bpm_ground*m
        diff = abs(bpm_detected - cand)
        if diff <= tolerance * bpm_ground:
            return True
        m += 1
    return False


def extended_harmonic_accuracy(bpm_detected, bpm_ground):
    tolerance = 0.04
    for m in [1, 2, 3]:
    #for m in [1, 2, 3]:
        diff = abs(m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return True
        diff = abs(1.0/m*bpm_detected - bpm_ground)
        if diff <= tolerance * bpm_ground:
            return True
    return False

def process_mfs(ground_mf, detected_mf, limit=None):
    # load ground truth
    ground_coll = mar_collection.MarCollection(ground_mf)
    ground_bpms = {}
    for dat in ground_coll.data:
        filename = dat[0]
        bpm_ground = float(dat[1])
        ground_bpms[filename] = bpm_ground

    user_coll = mar_collection.MarCollection(detected_mf)
    good = 0
    i = 0
    for dat in user_coll.data:
    #for dat in user_coll.data[:5]:
        filename = dat[0]
        cand_bpms = dat[1]
        bpm_ground = ground_bpms[filename]
        if "," in cand_bpms:
            cand_bpms = [float(a) for a in cand_bpms.split(',')]
        correct = False
        if limit is not None and limit is not 0:
            cand_bpms = cand_bpms[:limit]

        if limit == 0:
            for bpm_detected in cand_bpms[:1]:
                corr = exact_accuracy(bpm_detected, bpm_ground)
                if corr:
                    correct = True
            if correct:
                good += 1
            i += 1
        else:
            for bpm_detected in cand_bpms:
                corr = extended_harmonic_accuracy(bpm_detected, bpm_ground)
                if corr:
                    correct = True
            if correct:
                good += 1
            i += 1
        #print cand_bpms
    #print "Accuracy: %.2f (%i/%i)" % (100*float(good) / i, good, i)
    accuracy = 100*float(good) / len(user_coll.data)
    return accuracy
 


if __name__ == "__main__":
    ground_filename = sys.argv[1]
    user_filename   = sys.argv[2]
    zero = process_mfs(ground_filename, user_filename, 0)
    one = process_mfs(ground_filename, user_filename, 1)
    two = process_mfs(ground_filename, user_filename, 2)
    four = process_mfs(ground_filename, user_filename, 4)
    eight = process_mfs(ground_filename, user_filename, 8)
    print "%s\t%.1f\t%.1f\t%.1f\t%.1f\t%.1f" % (
        user_filename, zero, one, two, four, eight)


