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

	winSize.setValue_natural(2048);
	fftSize.setValue_natural(2048);
	synthesisHop.setValue_natural(512);
	analysisHop.setValue_natural(512);
	sinusoids.setValue_natural(120);
	pitchShift.setValue_real(0.66);
	
	convertMode.setValue_string("sorted");
	inSamples.setValue_natural(512);
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
	pvseries = mng.create("Series", "pvseries")
	pvseries.addMarSystem(mng.create("SoundFileSource", "src"))
	pvseries.addMarSystem(mng.create("PhaseVocoder", "pvoc"))
	pvseries.addMarSystem(mng.create("StretchLinear", "sl"))
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

	sfactor = pvseries.getControl("StretchLinear/sl/mrs_real/stretch");
	
	
	[name, ext] = os.path.splitext(os.path.basename(infile))
	outsfname = "temp.wav"
	
    #scaled phaselocking 

	filename.setValue_string(infile);
	inSamples.setValue_natural(512);
	analysisHop.setValue_natural(512);
	synthesisHop.setValue_natural(341);
	winSize.setValue_natural(2048);
	fftSize.setValue_natural(2048);
	convertMode.setValue_string("analysis_scaled_phaselock");
	unconvertMode.setValue_string("scaled_phaselock");
	sfactor.setValue_real(1.0);
	outfname.setValue_string(outsfname);
	
	i = 0 
	while notempty.to_bool():
		if (i == 0):
			phaselock.setValue_bool(True)
			
		i = i + 1
		pvseries.tick()


	[name, ext] = os.path.splitext(os.path.basename(infile))
	outsfname = name + "_scaled" + ext

	pnet = mng.create("Series", "pnet")
	pnet.addMarSystem(mng.create("SoundFileSource", "src"))
	pnet.addMarSystem(mng.create("StretchLinear", "sl"))
	pnet.addMarSystem(mng.create("SoundFileSink", "dest"))

	filename = pnet.getControl("SoundFileSource/src/mrs_string/filename")
	notempty = pnet.getControl("SoundFileSource/src/mrs_bool/notEmpty")
	sfactor = pnet.getControl("StretchLinear/sl/mrs_real/stretch")
	outfname = pnet.getControl("SoundFileSink/dest/mrs_string/filename")
	inSamples = pnet.getControl("mrs_natural/inSamples")
	
	sfactor.setValue_real(1.5);
	filename.setValue_string("temp.wav");
	outfname.setValue_string(outsfname);
	size = pnet.getControl("SoundFileSource/src/mrs_natural/size");
	inSamples.setValue_natural(size.to_natural())
	
	while notempty.to_bool():
		pnet.tick()

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

laura = ["/Users/gtzan/data/sound/ivl_orig/laura_mono.wav"]

filelist = [ 
"/Volumes/My Passport/ivl/Sept Clean Clips/A chord.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/Ab chord.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/B chord.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/B-Bent Riff.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/B-bended.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/Bb chord.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/C chord.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/ChordPattern1.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/ChordPattern2.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/ChordPattern3.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/ChunckaChunka.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/D chord.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/E chord plus.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/E chord.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/F chord.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/G chord.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/HammerOnPulloff.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/HammerPull.wav", 
"/Volumes/My Passport/ivl/Sept Clean Clips/Power Chords2.wav", 
"/Volumes/My Passport/ivl/Sept Clean Clips/RiffChordCombo.wav", 
"/Volumes/My Passport/ivl/Sept Clean Clips/Staccato Riff.wav", 
"/Volumes/My Passport/ivl/Sept Clean Clips/TeleChords.wav", 
"/Volumes/My Passport/ivl/Sept Clean Clips/Whammied.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/metal country bender lick.wav",
"/Volumes/My Passport/ivl/Sept Clean Clips/whammy chords.wav", 
]

# filelist = ["/Users/gtzan/data/sound/rollers_examples/ComingThrough.wav"]
# filelist = [ "/Volumes/My Passport/ivl/Sept Clean Clips/A chord.wav"]



for i in laura:
	# identity(i)
	scaled(i)
	#classic(i)
	# transient(i)
	oscbank(i)

