#!/usr/bin/env python

from pylab import *
import os 
import sys
from marsyas import MarSystemManager,MarControlPtr


def oscbank(infile):
	print "Osc Bank phasevocoding for " + infile 
	# Build the processing network
	mng = MarSystemManager()
	a = ("Series", "pvseries")
	pvseries = mng.create("Series", "pvseries")
	pvseries.addMarSystem(mng.create("SoundFileSource", "src"))
	pvseries.addMarSystem(mng.create("PhaseVocoderOscBank", "pvoc"))
	pvseries.addMarSystem(mng.create("SoundFileSink", "dest"))
	
	filename = pvseries.getControl("SoundFileSource/src/mrs_string/filename")
	inSamples = pvseries.getControl("mrs_natural/inSamples")
	winSize = pvseries.getControl("PhaseVocoderOscBank/pvoc/mrs_natural/winSize")
	fftSize = pvseries.getControl("PhaseVocoderOscBank/pvoc/mrs_natural/FFTSize")
	analysisHop = pvseries.getControl("PhaseVocoderOscBank/pvoc/mrs_natural/Decimation")
	synthesisHop = pvseries.getControl("PhaseVocoderOscBank/pvoc/mrs_natural/Interpolation")
	convertMode = pvseries.getControl("PhaseVocoderOscBank/pvoc/mrs_string/convertMode")
	outfname = pvseries.getControl("SoundFileSink/dest/mrs_string/filename")
	notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/notEmpty")
	phaselock = pvseries.getControl("PhaseVocoderOscBank/pvoc/mrs_bool/phaselock")
	pos = pvseries.getControl("SoundFileSource/src/mrs_natural/pos");
	sinusoids = pvseries.getControl("PhaseVocoderOscBank/pvoc/mrs_natural/Sinusoids");
	pitchShift = pvseries.getControl("PhaseVocoderOscBank/pvoc/mrs_real/PitchShift");
	
	[name, ext] = os.path.splitext(os.path.basename(infile))
	outsfname = name + "_oscbank" + ext

	winSize.setValue_natural(4096);
	fftSize.setValue_natural(4096);
	synthesisHop.setValue_natural(1024);
	analysisHop.setValue_natural(768);
	sinusoids.setValue_natural(40);
	pitchShift.setValue_real(1.5);
	
	convertMode.setValue_string("sorted");
	inSamples.setValue_natural(768);
	filename.setValue_string(infile);
	


	outfname.setValue_string(outsfname);
	


	i = 0 
	while notempty.to_bool():
		i = i + 1
		pvseries.tick()

		



def transient(infile):
	print "Transient  phasevocoding for " + infile 
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
	outsfname = name + "_transient" + ext
	
    #classic

	filename.setValue_string(infile);
	inSamples.setValue_natural(768);
	analysisHop.setValue_natural(768);
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
	sinusoids = pvseries.getControl("PhaseVocoder/pvoc/PvConvert/sinusoids");
	
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
	sinusoids.setValue_natural(40);
	convertMode.setValue_string("sorted");
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


filelist = ["/Users/gtzan/data/sound/ivl_orig/dj_1_mono.wav",
			"/Users/gtzan/data/sound/ivl_orig/dj_2_mono.wav",
			"/Users/gtzan/data/sound/ivl_orig/Karaoke_mono.wav"]

filelist = ["/Users/gtzan/data/sound/ivl_orig/transientorig.wav"]

filelist = ["/Users/gtzan/data/sound/ivl_orig/laura_mono.wav"]

for i in filelist:
	# identity(i)
	# scaled(i)
	#classic(i)
	# transient(i)
	oscbank(i)

