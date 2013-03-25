#!/usr/bin/env python
import os
import sys
import scipy.io.wavfile
import numpy

import mar_collection

def rewrite_file(filename):
    print "give attention to:\t", filename
    tmp = "tmp_wavfile.wav"
    cmd = "sox \"%s\" -t wavpcm \"%s\"" % (filename, tmp)
    os.system(cmd)
    os.system("cp \"%s\" \"%s\"" % (tmp, filename))



if __name__ == "__main__":
    user_filename = sys.argv[1]
    if user_filename[-3:] == ".mf":
        coll = mar_collection.MarCollection(user_filename)
        for filename in coll.get_filenames():
            try:
                sr, data_unnormalized = scipy.io.wavfile.read(filename)
                maxval = numpy.iinfo(data_unnormalized.dtype).max+1
            except:
                rewrite_file(filename)
            pass


