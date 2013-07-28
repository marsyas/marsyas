#!/usr/bin/env python

import itertools
import sys

import tempo_stem_file
import defs_class

#ONSETS = range(3)
#BH = range(2)
#OPTIONS = [zip(x, BH) for x
#    in itertools.permutations(ONSETS, len(BH))]
OPTIONS = range(5)

if __name__ == "__main__":
    user_filename = sys.argv[1]
    if user_filename[-3:] == ".mf":

        for opt in OPTIONS:
            ### set up options
            defs = defs_class.Defs()
            defs.OPTIONS_ONSET = min(opt, 2)
            defs.OPTIONS_BH = max(0, opt - 2)
            ### run algorithm
            accuracy = tempo_stem_file.bpm_of_mf(defs, user_filename)
            print accuracy

        import pylab
        pylab.show()



