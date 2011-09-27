#!/usr/bin/env python
#
# Copyright 2011 Google Inc. All Rights Reserved.

"""Simplest possible test of Marsyas SWIG gindings"""

__author__ = 'snessnet@google.com (Steven Ness)'
import marsyas


  # Create the network
mng = marsyas.MarSystemManager()
net = mng.create("Series","series")
net.addMarSystem(mng.create("SoundFileSource", "src"))
net.addMarSystem(mng.create("Gain", "gain"))

  # Set the filename
net.updControl("SoundFileSource/src/mrs_string/filename", "foo.wav")

  # Get the name of the input sound file to test getControl
ctrl_fname = net.getControl("SoundFileSource/src/mrs_string/filename");
print ctrl_fname.to_string()
print ctrl_fname
#  print net.toString()

