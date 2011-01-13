#!/usr/bin/env python

from pylab import *
import os 
import sys
from marsyas import MarSystemManager,MarControlPtr



def multires(infile):
	print "MultiResolution phasevocoding for " + infile
	
	# Build the processing network
	mng = MarSystemManager()
	pvseries = mng.create("Series", "pvseries")
	pvseries.addMarSystem(mng.create("SoundFileSource", "src"))
	pvseries.addMarSystem(mng.create("ShiftInput", "si"))
	pvseries.addMarSystem(mng.create("Windowing", "fo"));
	pvseries.addMarSystem(mng.create("Spectrum", "spk"));
	pvseries.addMarSystem(mng.create("PvConvert", "conv"));
	pvseries.addMarSystem(mng.create("PvOscBank", "pob"));
	pvseries.addMarSystem(mng.create("ShiftOutput", "so"));
	pvseries.addMarSystem(mng.create("SoundFileSink", "dest"))


	pvseriesLong = mng.create("Series", "pvseriesLong")
	pvseriesLong.addMarSystem(mng.create("SoundFileSource", "src"))
	pvseriesLong.addMarSystem(mng.create("ShiftInput", "si"))
	pvseriesLong.addMarSystem(mng.create("Windowing", "fo"));
	pvseriesLong.addMarSystem(mng.create("Spectrum", "spk"));
	pvseriesLong.addMarSystem(mng.create("PvConvert", "conv"));
	pvseriesLong.addMarSystem(mng.create("PvOscBank", "pob"));
	pvseriesLong.addMarSystem(mng.create("ShiftOutput", "so"));
	pvseriesLong.addMarSystem(mng.create("SoundFileSink", "dest"))


	
	filename = pvseries.getControl("SoundFileSource/src/mrs_string/filename")
	inSamples = pvseries.getControl("mrs_natural/inSamples")
	awinSize = pvseries.getControl("ShiftInput/si/mrs_natural/winSize")
	swinSize = pvseries.getControl("PvOscBank/pob/mrs_natural/winSize")
	analysisHop = pvseries.getControl("PvConvert/conv/mrs_natural/Decimation")
	synthesisHop = pvseries.getControl("PvOscBank/pob/mrs_natural/Interpolation")
	synthesisHop1 = pvseries.getControl("ShiftOutput/so/mrs_natural/Interpolation")
	convertMode = pvseries.getControl("PvConvert/conv/mrs_string/mode")
	outfname = pvseries.getControl("SoundFileSink/dest/mrs_string/filename")
	notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/hasData")
	sinusoids = pvseries.getControl("PvConvert/conv/mrs_natural/Sinusoids");
	pitchShift = pvseries.getControl("PvOscBank/pob/mrs_real/PitchShift");
	zeroPhasing = pvseries.getControl("Windowing/fo/mrs_bool/zeroPhasing");
	wintype = pvseries.getControl("Windowing/fo/mrs_string/type");
	zeroPadding = pvseries.getControl("Windowing/fo/mrs_natural/zeroPadding");
	outData1  = pvseries.getControl("Spectrum/spk/mrs_realvec/processedData")



	filenameLong = pvseriesLong.getControl("SoundFileSource/src/mrs_string/filename")
	inSamplesLong = pvseriesLong.getControl("mrs_natural/inSamples")
	outData = pvseriesLong.getControl("ShiftInput/si/mrs_realvec/processedData");
	awinSizeLong = pvseriesLong.getControl("ShiftInput/si/mrs_natural/winSize")
	swinSizeLong = pvseriesLong.getControl("PvOscBank/pob/mrs_natural/winSize")
	analysisHopLong = pvseriesLong.getControl("PvConvert/conv/mrs_natural/Decimation")
	synthesisHopLong = pvseriesLong.getControl("PvOscBank/pob/mrs_natural/Interpolation")
	synthesisHop1Long = pvseriesLong.getControl("ShiftOutput/so/mrs_natural/Interpolation")
	convertModeLong = pvseriesLong.getControl("PvConvert/conv/mrs_string/mode")
	outfnameLong = pvseriesLong.getControl("SoundFileSink/dest/mrs_string/filename")
	notemptyLong = pvseriesLong.getControl("SoundFileSource/src/mrs_bool/hasData")
	sinusoidsLong = pvseriesLong.getControl("PvConvert/conv/mrs_natural/Sinusoids");
	pitchShiftLong = pvseriesLong.getControl("PvOscBank/pob/mrs_real/PitchShift");
	zeroPhasingLong = pvseriesLong.getControl("Windowing/fo/mrs_bool/zeroPhasing");
	wintypeLong = pvseriesLong.getControl("Windowing/fo/mrs_string/type");
	zeroPaddingLong = pvseriesLong.getControl("Windowing/fo/mrs_natural/zeroPadding");
	outData1Long  = pvseriesLong.getControl("Spectrum/spk/mrs_realvec/processedData")

	
	[name, ext] = os.path.splitext(os.path.basename(infile))
	outsfname = name + "_multires" + ext
	outsfnameLong = name + "_multiresLong" + ext


	zeroPhasing.setValue_bool(True);
	wintype.setValue_string("Hanning");
	zeroPadding.setValue_natural(3 * 512);
	awinSize.setValue_natural(512);
	swinSize.setValue_natural(2048);
	synthesisHop.setValue_natural(128);
	synthesisHop1.setValue_natural(128);
	analysisHop.setValue_natural(128);
	sinusoids.setValue_natural(120);
	pitchShift.setValue_real(0.66);
	convertMode.setValue_string("sorted");
	inSamples.setValue_natural(128);
	filename.setValue_string(infile);
	outfname.setValue_string(outsfname);


	zeroPhasingLong.setValue_bool(True);
	wintypeLong.setValue_string("Hanning");
	zeroPaddingLong.setValue_natural(0);
	awinSizeLong.setValue_natural(2048);
	swinSizeLong.setValue_natural(2048);
	synthesisHopLong.setValue_natural(128);
	synthesisHop1Long.setValue_natural(128);
	analysisHopLong.setValue_natural(128);
	sinusoidsLong.setValue_natural(120);
	pitchShiftLong.setValue_real(0.66);
	convertModeLong.setValue_string("sorted");
	inSamplesLong.setValue_natural(128);
	filenameLong.setValue_string(infile);
	outfnameLong.setValue_string(outsfnameLong);
	
	l
	xLong = zeros(2048, float)
	xcorr = zeros(2048, float)
	mpx = zeros(2048, float)
	correlations = zeros(13525, float)
	times=zeros(13525, float)
	iterations = 0
	time_in_seconds = 0.0;
	while notempty.to_bool():
		iterations = iterations + 1
		if (iterations>0):
#			raw_input("Press enter to continue")
			timedata = outData.to_realvec()
			plotdata = outData1.to_realvec()
			plotdataLong = outData1Long.to_realvec()

			
			for i in range(0, len(plotdata)):
				x[i] = plotdata[i]
				xLong[i] = plotdataLong[i]
				xcorr[i] = x[i] * xLong[i] 

			mx = abs(x)
			mxLong = abs(xLong)
			correlations[iterations] = mean(xcorr)
			times[iterations] = time_in_seconds
			df = mx - mpx;
			df2 = mx - mxLong;
			# subplot(321)
# 			plot(mx[0:500], 'r-')
# 			subplot(322)
# 			plot(mxLong[0:500], 'b-')
# 			subplot(323)
# 			plot(timedata, 'b-');
# 			subplot(324)
# 			plot(df[0:500] * 1000, 'y-')
# 			subplot(325)
# 			plot(df2[0:500] * 1000, 'b-')
			#plot(mx[0:500], 'r-')
			#hold(True)
			#plot(mpx[0:500], 'g-')
			#hold(True)			
			#plot(mxLong[0:500], 'b-')
#			hold(False)
#			show()
			mpx = mx
			
		pvseries.tick()
		pvseriesLong.tick()
		time_in_seconds = time_in_seconds + (128.0 / 44100.0);
		print time_in_seconds
		print iterations
	plot(times, correlations * 100000000.0, 'b-')
	show()
	raw_input("Hello")


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
	notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/hasData")
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
	notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/hasData")
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
	notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/hasData")
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
	notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/hasData")
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
	notempty = pnet.getControl("SoundFileSource/src/mrs_bool/hasData")
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
	notempty = pvseries.getControl("SoundFileSource/src/mrs_bool/hasData")
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

filelist = [ "/Volumes/My Passport/ivl/Sept Clean Clips/A chord.wav"]
filelist = ["/Volumes/My Passport/ivl/Sept Clean Clips/RiffChordCombo.wav"] 


for i in filelist:
	# identity(i)
	# scaled(i)
	#classic(i)
	# transient(i)
	# oscbank(i)
	multires(i)

