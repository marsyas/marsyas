#!/usr/bin/env python

import sys
import pickle
import pylab


pickle_filenames = sys.argv[1:]

for pickle_filename in pickle_filenames:
    pickle_file = open(pickle_filename, 'rb')
    oss_sr, oss_data = pickle.load(pickle_file)
    pickle_file.close()

    pylab.plot(oss_data)

pylab.show()

