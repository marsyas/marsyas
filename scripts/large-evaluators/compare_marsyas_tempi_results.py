#!/usr/bin/env python

import sys
import glob
import os.path

import compare_tempi



def get_mf_filenames():
    if len(sys.argv) < 3:
        print "Need:"
        print "  1) dir of ground truth"
        print "  2) base dir of check-marsyas.py output"
        exit(1)
    ground_dirname = sys.argv[1]
    detected_dirname = sys.argv[2]
    mf_ground_filenames = glob.glob(
        os.path.join(ground_dirname, "*.mf"))
    mf_detected_filenames = [
        os.path.join(f, os.path.basename(f) + "-detected.mf")
        for f in
        glob.glob(os.path.join(detected_dirname, "*")) ]
    mf_ground_filenames.sort()
    mf_detected_filenames.sort()
    return mf_ground_filenames, mf_detected_filenames


def main():
    mf_ground_filenames, mf_detected_filenames = get_mf_filenames()

    for mf_g, mf_d in zip(mf_ground_filenames, mf_detected_filenames):
        data = compare_tempi.get_results(mf_d, mf_g)
        harmonic_accuracy = data[5]
        basename = os.path.basename(mf_g)
        print "%.2f\t%s" % (harmonic_accuracy, basename)


if __name__ == '__main__':
    main()


