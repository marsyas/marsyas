## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2002 George Tzanetakis 
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


MAINTAINERCLEANFILES    = Makefile.in


EXTRA_DIST 		= Doxyfile
EXTRA_DIST += Qt/
EXTRA_DIST += MATLAB/
EXTRA_DIST += distributed/

dist-hook:
	rm -rf `find $(distdir)/Qt/ -name .svn`
	rm -rf `find $(distdir)/MATLAB/ -name .svn`
	rm -rf `find $(distdir)/distributed/ -name .svn`


EXTRA_DIST += PCA.cpp PCA.h
EXTRA_DIST += marsyas.pro
EXTRA_DIST += NetworkUDPSink.cpp NetworkSocket.cpp NetworkTCPSink.cpp NetworkTCPSource.cpp NetworkUDPSource.cpp

INCLUDES                = -I$(top_builddir) -I$(top_srcdir) -I$(Includes) 

pkginclude_HEADERS      = MrsLog.h \
VScheduler.h Scheduler.h ScheduledEvent.h SchedulerManager.h \
TmTimer.h TmTime.h TmSampleCount.h TmGetTime.h TmControlValue.h TimerFactory.h \
MarEvent.h EvGetUpd.h EvValUpd.h Repeat.h Heap.h \
EvExpr.h ExScanner.h ExParser.h ExSymTbl.h ExNode.h ExVal.h Expr.h ExCommon.h \
realvec.h common.h \
SoundFileSource.h AuFileSource.h \
WavFileSource.h BaseAudioSink.h Mono2Stereo.h \
AudioSource.h AudioSink.h SoundFileSink.h \
AuFileSink.h WavFileSink.h FileName.h MarSystem.h \
MarSystemTemplateAdvanced.h MarSystemTemplateBasic.h \
MarControlManager.h MarControl.h MarControlValue.h \
Gain.h Spectrum.h InvSpectrum.h \
Collection.h fft.h Rms.h Peak2Rms.h PlotSink.h \
AutoCorrelation.h Peaker.h Peaker1.h HalfWaveRectifier.h \
TimeStretch.h Conversions.h MaxArgMax.h MinArgMin.h \
Series.h \
 ShiftInput.h ShiftOutput.h Shifter.h PvFold.h PvConvert.h PeConvert.h PeClust.h PeSynOsc.h PeSynFFT.h OverlapAdd.h PeResidual.h PeUtilities.h PeClusters.h PeSimilarity.h Polar2Cartesian.h Cartesian2Polar.h FlowCutSource.h \
PvConvolve.h PvOscBank.h PvUnconvert.h PvOverlapadd.h Windowing.h Hamming.h \
Fanout.h Fanin.h Negative.h SineSource.h MarSystemManager.h \
PowerSpectrum.h Centroid.h Rolloff.h \
Flux.h Memory.h Mean.h StandardDeviation.h ZeroCrossings.h \
Socket.h Thread.h GaussianClassifier.h AbsSoundFileSink.h AbsSoundFileSource.h \
Confidence.h MarFileSink.h WekaSink.h MFCC.h SCF.h SFM.h \
WaveletPyramid.h WaveletStep.h Daub4.h OnePole.h RtAudio.h RtError.h Accumulator.h Shredder.h \
NetworkSocket.h NetworkTCPSource.h NetworkTCPSink.h NetworkUDPSource.h NetworkUDPSink.h Messager.h \
SocketException.h Talk.h \
TimeLine.h Color.h Communicator.h Server.h AbsMax.h FullWaveRectifier.h \
Norm.h DownSampler.h WaveletBands.h Sum.h Product.h\
PeakPeriods2BPM.h Histogram.h BeatHistoFeatures.h MslModel.h \
CommandLineOptions.h RawFileSource.h FM.h \
ANN_node.h Annotator.h ZeroRClassifier.h Summary.h KNNClassifier.h Filter.h \
Kurtosis.h Skewness.h ERB.h CollectionFileSource.h MeddisHairCell.h ViconFileSource.h \
Cascade.h Parallel.h SeneffEar.h MP3FileSource.h OggFileSource.h ClassOutputSink.h \
ClipAudioRange.h SilenceRemove.h RtMidi.h Stk.h Limiter.h CompExp.h NoiseGate.h \
NoiseSource.h Plucked.h ADSR.h Envelope.h Delay.h NormMaxMin.h HarmonicEnhancer.h \
Reassign.h Normalize.h AMDF.h SMO.h SOM.h LPC.h LSP.h LPCC.h BICchangeDetector.h QGMMModel.h WekaSource.h RealvecSource.h \
RealvecSink.h Power.h MemorySource.h MidiInput.h Esitar.h MidiOutput.h mididevices.h \
NumericLib.h NormCut.h vmblock.h lu.h basis.h dsound.h \
AbsSoundFileSource2.h AudioSink2.h AudioSource2.h SoundFileSource2.h WavFileSource2.h

