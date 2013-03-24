#!/usr/bin/env python

import itertools
import sys

import tempo_reference
import defs_class

tempo_reference.SHORT_DEBUG = 0

#ONSETS = range(3)
#INDUCTION = range(2)
#OPTIONS = [zip(x, INDUCTION) for x
#    in itertools.permutations(ONSETS, len(INDUCTION))]
OPTIONS = range(5)

if __name__ == "__main__":
    user_filename = sys.argv[1]
    if user_filename[-3:] == ".mf":

        for opt in OPTIONS:
            ### set up options
            defs = defs_class.Defs()
            defs.OPTIONS_ONSET = min(opt, 2)
            defs.OPTIONS_INDUCTION = max(0, opt - 2)
            ### run algorithm
            accuracy = tempo_reference.bpm_of_mf(defs, user_filename)
            print accuracy



