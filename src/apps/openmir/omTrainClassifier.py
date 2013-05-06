#!/usr/bin/python

#
#
#

import sys
import os
import datetime
import commands
import re
import time
import simplejson as json
from optparse import OptionParser

def run(inJsonFilename):
    inJson = open(inJsonFilename).read()
    data = json.loads(inJson)

    # Add a name for the output file that will be generated
    for item in data:
        newAudioFile = item['audioFile'].replace("/","_")
        item['inputFile'] = "/data/django/orchive/audio/%s.wav" % (item['audioFile'])
        item['outputFile'] = "/tmp/%s-%s-%s.wav" % (newAudioFile, item['startSec'], item['endSec'])

    # Run sox on each input file
    for item in data:
        startSec = float(item['startSec'])
        endSec = float(item['endSec'])
        lengthSec = endSec - startSec
        command = "sox %s %s trim %f %f" % (item['inputFile'], item['outputFile'], startSec, lengthSec)
        a = commands.getoutput(command)

    # Make .mf file
    ts = time.time()
    mfFilename = "/tmp/bextract-%i.mf" % ts
    mfFile = open(mfFilename, "w")
    for item in data:
        mfFile.write("%s\t%s\n" % (item['outputFile'], item['label']))
    mfFile.close()

    # Run bextract on audio file
    mplFilename = "/tmp/bextract-%i.mpl" % ts
    command = "bextract %s -pm -p %s" % (mfFilename, mplFilename)
    a = commands.getoutput(command)

    # Return .mpl file as text
    mplFile = open(mplFilename, "r")
    mplData = mplFile.read()

    # Remove temporary audio files when done
    for item in data:
        os.remove(item['outputFile'])
    os.remove(mfFilename)
    os.remove(mplFilename)

    print mplData


if __name__ == "__main__":
    usage = "usage: %prog [options] in.json"
    parser = OptionParser(usage)
    
    (options, args) = parser.parse_args()

    if len(sys.argv) < 2:
        parser.print_help()
        sys.exit(1)
    
    run(args[0])
