#!/usr/bin/env python

import compare_tempi

a = "/home/gperciva/src/audio-research/foo.mf"
b = "/home/gperciva/src/audio-research/bar.mf"

compare_tempi.check_files_in_mf(a, a)
compare_tempi.check_files_in_mf(a, b)


