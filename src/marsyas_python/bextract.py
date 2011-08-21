#!/usr/bin/env python

# bextract implemented using the swig python Marsyas bindings
# George Tzanetakis, January, 16, 2007 
# revised Graham Percival, 2011 August 20

import sys
import marsyas
import marsyas_util

try:
    mf_filename = sys.argv[1]
    arff_filename = mf_filename.replace(".mf", ".arff")
    mpl_filename = mf_filename.replace(".mpl", ".arff")
except:
    print "Usage: py-bextract.py filename.mf"
    sys.exit(1)

# Create top-level patch
net = marsyas_util.create(
    ["Series/extract_network",
        ["SoundFileSource/src",
         "TimbreFeatures/featExtractor",
         "TextureStats/tStats",
         "Annotator/annotator",
         "WekaSink/wsink",
    ]])

# link the controls to coordinate things
net.linkControl("mrs_string/filename",
    "SoundFileSource/src/mrs_string/filename")
net.linkControl("mrs_bool/hasData",
    "SoundFileSource/src/mrs_bool/hasData")
net.linkControl("WekaSink/wsink/mrs_string/currentlyPlaying",
    "SoundFileSource/src/mrs_string/currentlyPlaying")
net.linkControl("Annotator/annotator/mrs_real/label",
    "SoundFileSource/src/mrs_real/currentLabel")
net.linkControl("SoundFileSource/src/mrs_natural/nLabels",
    "WekaSink/wsink/mrs_natural/nLabels")

# set up features to extract
net.updControl("TimbreFeatures/featExtractor/mrs_string/enableTDChild",
    marsyas.MarControlPtr.from_string("ZeroCrossings/zcrs"))
net.updControl("TimbreFeatures/featExtractor/mrs_string/enableLPCChild",
    marsyas.MarControlPtr.from_string("Series/lspbranch"))
net.updControl("TimbreFeatures/featExtractor/mrs_string/enableLPCChild",
    marsyas.MarControlPtr.from_string("Series/lpccbranch"))
net.updControl("TimbreFeatures/featExtractor/mrs_string/enableSPChild",
    marsyas.MarControlPtr.from_string("MFCC/mfcc"))
net.updControl("TimbreFeatures/featExtractor/mrs_string/enableSPChild",
    marsyas.MarControlPtr.from_string("SCF/scf"))
net.updControl("TimbreFeatures/featExtractor/mrs_string/enableSPChild",
    marsyas.MarControlPtr.from_string("Rolloff/rf"))
net.updControl("TimbreFeatures/featExtractor/mrs_string/enableSPChild",
    marsyas.MarControlPtr.from_string("Flux/flux"))
net.updControl("TimbreFeatures/featExtractor/mrs_string/enableSPChild",
    marsyas.MarControlPtr.from_string("Centroid/cntrd"))
net.updControl("TimbreFeatures/featExtractor/mrs_string/enableSPChild",
    marsyas.MarControlPtr.from_string("Series/chromaPrSeries"))

# setup filenames 
net.updControl("mrs_string/filename",
    marsyas.MarControlPtr.from_string(mf_filename))
net.updControl("WekaSink/wsink/mrs_string/labelNames", 
    net.getControl("SoundFileSource/src/mrs_string/labelNames"))
net.updControl("WekaSink/wsink/mrs_string/filename",
    marsyas.MarControlPtr.from_string(arff_filename))

# do the processing, extracting features, and writing to weka file 
previouslyPlaying = ""
while net.getControl("SoundFileSource/src/mrs_bool/hasData").to_bool():
    currentlyPlaying = net.getControl("SoundFileSource/src/mrs_string/currentlyPlaying").to_string()
    if (currentlyPlaying != previouslyPlaying):
        print "Processing: ",
        print net.getControl("SoundFileSource/src/mrs_string/currentlyPlaying").to_string()
    net.tick() # update time
    previouslyPlaying = currentlyPlaying

