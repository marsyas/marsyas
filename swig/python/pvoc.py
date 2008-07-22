#!/usr/bin/env python

from pylab import *
import os 
import sys
from marsyas import MarSystemManager,MarControlPtr





def classic(infile):
	print "Classic phasevocoding for " + infile 
	# Build the processing network
	mng = MarSystemManager()
	a = ("Series", "pvseries")
	pvseries = mng.create("Series", "pvseries")
	pvseries.addMarSystem(mng.create("SoundFileSource", "src"))
	pvseries.addMarSystem(mng.create("PhaseVocoder", "pvoc"))
	pvseries.addMarSystem(mng.create("SoundFileSink", "dest"))
	
	filename = pvseries.getControl("SoundFileSource/src/mrs_string/filename")
	inSamples = pvseries.getControl("mrs_natural/inSamples")
	winSize = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/winSize")
	fftSize = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/FFTSize")
	analysisHop = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/Decimation")
	synthesisHop = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/Interpolation")
	convertMode = pvseries.getControl("PhaseVocoder/pvoc/mrs_string/convertMode")
	unconvertMode = pvseries.getControl("PhaseVocoder/pvoc/mrs_string/unconvertMode")
	outfname = pvseries.getControl("SoundFileSink/dest/mrs_string/filename")
	notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/notEmpty")
	phaselock = pvseries.getControl("PhaseVocoder/pvoc/mrs_bool/phaselock")
	magHandle = pvseries.getControl("PhaseVocoder/pvoc/PvUnconvert/uconv/mrs_realvec/magnitudes"); 
	peakHandle = pvseries.getControl("PhaseVocoder/pvoc/PvUnconvert/uconv/mrs_realvec/peaks");
	pos = pvseries.getControl("SoundFileSource/src/mrs_natural/pos");
	
	
	[name, ext] = os.path.splitext(os.path.basename(infile))
	outsfname = name + "_classic" + ext
	
    #classic

	filename.setValue_string(infile);
	inSamples.setValue_natural(512);
	analysisHop.setValue_natural(512);
	synthesisHop.setValue_natural(1024);
	winSize.setValue_natural(4096);
	fftSize.setValue_natural(4096);
	convertMode.setValue_string("full");
	unconvertMode.setValue_string("classic");
	outfname.setValue_string(outsfname);
	
	i = 0 
	while notempty.to_bool():
		if (i == 0):
			phaselock.setValue_bool(True)
			
		i = i + 1
		pvseries.tick()

		
	print "Classic phasevocoding done. Output in "  + outsfname 



def scaled(infile):
	print "Scaled-phaselocking phasevocoding " + infile 

	# Build the processing network
	mng = MarSystemManager()
	a = ("Series", "pvseries")
	pvseries = mng.create("Series", "pvseries")
	pvseries.addMarSystem(mng.create("SoundFileSource", "src"))
	pvseries.addMarSystem(mng.create("PhaseVocoder", "pvoc"))
	pvseries.addMarSystem(mng.create("SoundFileSink", "dest"))
	
	filename = pvseries.getControl("SoundFileSource/src/mrs_string/filename")
	inSamples = pvseries.getControl("mrs_natural/inSamples")
	winSize = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/winSize")
	fftSize = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/FFTSize")
	analysisHop = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/Decimation")
	synthesisHop = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/Interpolation")
	convertMode = pvseries.getControl("PhaseVocoder/pvoc/mrs_string/convertMode")
	unconvertMode = pvseries.getControl("PhaseVocoder/pvoc/mrs_string/unconvertMode")
	outfname = pvseries.getControl("SoundFileSink/dest/mrs_string/filename")
	notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/notEmpty")
	phaselock = pvseries.getControl("PhaseVocoder/pvoc/mrs_bool/phaselock")
	magHandle = pvseries.getControl("PhaseVocoder/pvoc/PvUnconvert/uconv/mrs_realvec/magnitudes"); 
	peakHandle = pvseries.getControl("PhaseVocoder/pvoc/PvUnconvert/uconv/mrs_realvec/peaks");
	pos = pvseries.getControl("SoundFileSource/src/mrs_natural/pos");
	
	
	[name, ext] = os.path.splitext(os.path.basename(infile))
	outsfname = name + "_scaled" + ext
	
    #scaled phaselocking 

	filename.setValue_string(infile);
	inSamples.setValue_natural(512);
	analysisHop.setValue_natural(512);
	synthesisHop.setValue_natural(1024);
	winSize.setValue_natural(4096);
	fftSize.setValue_natural(4096);
	convertMode.setValue_string("analysis_scaled_phaselock");
	unconvertMode.setValue_string("scaled_phaselock");
	outfname.setValue_string(outsfname);
	
	i = 0 
	while notempty.to_bool():
		if (i == 0):
			phaselock.setValue_bool(True)
			
		i = i + 1
		pvseries.tick()
	print "Scaled-phaselocking phasevocoding done. Output in "  + outsfname 



def identity(infile):
	print "Identity-phaselocking phasevocoding " + infile 

	# Build the processing network
	mng = MarSystemManager()
	a = ("Series", "pvseries")
	pvseries = mng.create("Series", "pvseries")
	pvseries.addMarSystem(mng.create("SoundFileSource", "src"))
	pvseries.addMarSystem(mng.create("PhaseVocoder", "pvoc"))
	pvseries.addMarSystem(mng.create("SoundFileSink", "dest"))
	
	filename = pvseries.getControl("SoundFileSource/src/mrs_string/filename")
	inSamples = pvseries.getControl("mrs_natural/inSamples")
	winSize = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/winSize")
	fftSize = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/FFTSize")
	analysisHop = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/Decimation")
	synthesisHop = pvseries.getControl("PhaseVocoder/pvoc/mrs_natural/Interpolation")
	convertMode = pvseries.getControl("PhaseVocoder/pvoc/mrs_string/convertMode")
	unconvertMode = pvseries.getControl("PhaseVocoder/pvoc/mrs_string/unconvertMode")
	outfname = pvseries.getControl("SoundFileSink/dest/mrs_string/filename")
	notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/notEmpty")
	phaselock = pvseries.getControl("PhaseVocoder/pvoc/mrs_bool/phaselock")
	magHandle = pvseries.getControl("PhaseVocoder/pvoc/PvUnconvert/uconv/mrs_realvec/magnitudes"); 
	peakHandle = pvseries.getControl("PhaseVocoder/pvoc/PvUnconvert/uconv/mrs_realvec/peaks");
	pos = pvseries.getControl("SoundFileSource/src/mrs_natural/pos");
	
	
	[name, ext] = os.path.splitext(os.path.basename(infile))
	outsfname = name + "_identity" + ext
	
    #identity phaselocking 

	filename.setValue_string(infile);
	inSamples.setValue_natural(512);
	analysisHop.setValue_natural(512);
	synthesisHop.setValue_natural(1024);
	winSize.setValue_natural(4096);
	fftSize.setValue_natural(4096);
	convertMode.setValue_string("full");
	unconvertMode.setValue_string("identity_phaselock");
	outfname.setValue_string(outsfname);
	
	i = 0 
	while notempty.to_bool():
		if (i == 0):
			phaselock.setValue_bool(True)
			
		i = i + 1
		pvseries.tick()
# 		if (i==100):
# 			magdata = magHandle.to_realvec()
# 			peakdata = peakHandle.to_realvec()
# 			hold(False)
# 			plot(magdata)
# 			hold(True)
# 			plot(peakdata)
# 			hold(False)
# 			show()


		
	print "Identity-phaselocking phasevocoding done. Output in "  + outsfname 



	


filelist = ["/Users/gtzan/data/sound/ivl_orig/laura_mono.wav",
			"/Users/gtzan/data/sound/ivl_orig/x1.wav",
			"/Users/gtzan/data/sound/ivl_orig/acoustic_guitar_and_djembe_mono.wav",
			"/Users/gtzan/data/sound/ivl_orig/Pop1_mono.wav"]




for i in filelist:
	identity(i)
	scaled(i)
	classic(i)

