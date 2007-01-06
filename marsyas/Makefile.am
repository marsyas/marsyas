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

DISTCLEANFILES		= stamp-h.in

EXTRA_DIST 		= config.h.in win_config.h Doxyfile

INCLUDES                = -I$(top_builddir) -I$(top_srcdir) -I$(Includes) 

pkginclude_HEADERS      = MrsLog.h \
VScheduler.h Scheduler.h ScheduledEvent.h SchedulerManager.h \
TmTimer.h TmTime.h TmSampleCount.h TmGetTime.h TmControlValue.h TimerFactory.h \
MarEvent.h EvGetUpd.h EvValUpd.h Repeat.h Heap.h \
EvExpr.h ExScanner.h ExParser.h ExSymTbl.h ExNode.h ExVal.h Expr.h \
realvec.h common.h \
SoundFileSource.h AuFileSource.h \
WavFileSource.h BaseAudioSink.h \
AudioSource.h AudioSink.h SoundFileSink.h \
AuFileSink.h WavFileSink.h FileName.h MarSystem.h \
MarSystemTemplateAdvanced.h MarSystemTemplateBasic.h \
MarControlManager.h MarControl.h MarControlValue.h \
Gain.h Spectrum.h InvSpectrum.h \
Collection.h fft.h Rms.h Peak2Rms.h PlotSink.h \
AutoCorrelation.h Peaker.h Peaker1.h HalfWaveRectifier.h \
TimeStretch.h Conversions.h MaxArgMax.h MinArgMin.h \
Series.h \
 ShiftInput.h ShiftOutput.h Shifter.h PvFold.h PvConvert.h PeConvert.h PeClust.h PeOverlapadd.h PeResidual.h PeUtilities.h PeClusters.h PeSimilarity.h \
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
Kurtosis.h Skewness.h ERB.h CollectionFileSource.h MeddisHairCell.h ViconFileSource.h Cascade.h Parallel.h SeneffEar.h MP3FileSource.h OggFileSource.h ClassOutputSink.h ClipAudioRange.h SilenceRemove.h RtMidi.h Stk.h Limiter.h CompExp.h NoiseGate.h NoiseSource.h Plucked.h ADSR.h Envelope.h Delay.h NormMaxMin.h HarmonicEnhancer.h Reassign.h Normalize.h AMDF.h SMO.h SOM.h LPC.h LPCwarped.h LPCResyn.h LSP.h LPCC.h WekaSource.h RealvecSource.h MemorySource.h config.h MidiInput.h Esitar.h MATLABengine.h NumericLib.h NormCut.h vmblock.h lu.h basis.h dsound.h 

lib_LIBRARIES         = libmarsyas.a

libmarsyas_a_SOURCES    = MrsLog.cpp \
VScheduler.cpp Scheduler.cpp ScheduledEvent.cpp SchedulerManager.cpp \
TmTimer.cpp TmTime.cpp TmSampleCount.cpp TmGetTime.cpp TmControlValue.cpp TimerFactory.cpp \
MarEvent.cpp EvGetUpd.cpp EvValUpd.cpp Repeat.cpp \
EvExpr.cpp ExScanner.cpp ExParser.cpp ExSymTbl.cpp ExNode.cpp ExVal.cpp Expr.cpp \
realvec.cpp \
SoundFileSource.cpp AuFileSource.cpp \
WavFileSource.cpp BaseAudioSink.cpp \
AudioSource.cpp AudioSink.cpp SoundFileSink.cpp \
AuFileSink.cpp WavFileSink.cpp FileName.cpp MarSystem.cpp \
MarSystemTemplateAdvanced.cpp MarSystemTemplateBasic.cpp \
MarControlManager.cpp MarControl.cpp MarControlValue.cpp \
Gain.cpp Spectrum.cpp InvSpectrum.cpp \
Collection.cpp fft.cpp Rms.cpp Peak2Rms.cpp PlotSink.cpp \
AutoCorrelation.cpp Peaker.cpp Peaker1.cpp HalfWaveRectifier.cpp \
TimeStretch.cpp Conversions.cpp MaxArgMax.cpp MinArgMin.cpp \
Series.cpp \
ShiftInput.cpp ShiftOutput.cpp Shifter.cpp PvFold.cpp PvConvert.cpp PeConvert.cpp PeClust.cpp PeOverlapadd.cpp PeResidual.cpp PeUtilities.cpp PeClusters.cpp PeSimilarity.cpp \
PvConvolve.cpp PvOscBank.cpp PvUnconvert.cpp PvOverlapadd.cpp Windowing.cpp Hamming.cpp \
Fanout.cpp Fanin.cpp Negative.cpp SineSource.cpp MarSystemManager.cpp \
PowerSpectrum.cpp Centroid.cpp Rolloff.cpp \
Flux.cpp Memory.cpp Mean.cpp StandardDeviation.cpp ZeroCrossings.cpp \
Thread.cpp Socket.cpp GaussianClassifier.cpp AbsSoundFileSink.cpp AbsSoundFileSource.cpp \
Confidence.cpp MarFileSink.cpp WekaSink.cpp MFCC.cpp SCF.cpp SFM.cpp \
WaveletPyramid.cpp WaveletStep.cpp Daub4.cpp OnePole.cpp RtAudio.cpp Accumulator.cpp Shredder.cpp \
NetworkSocket.cpp NetworkTCPSource.cpp NetworkTCPSink.cpp NetworkUDPSource.cpp NetworkUDPSink.cpp Messager.cpp \
Talk.cpp \
TimeLine.cpp Color.cpp Communicator.cpp Server.cpp AbsMax.cpp FullWaveRectifier.cpp \
Norm.cpp DownSampler.cpp WaveletBands.cpp Sum.cpp Product.cpp\
PeakPeriods2BPM.cpp Histogram.cpp BeatHistoFeatures.cpp MslModel.cpp \
CommandLineOptions.cpp RawFileSource.cpp FM.cpp \
ANN_node.cpp Annotator.cpp Summary.cpp ZeroRClassifier.cpp KNNClassifier.cpp Filter.cpp \
Kurtosis.cpp Skewness.cpp AMDF.cpp SMO.cpp SOM.cpp LPC.cpp LPCwarped.cpp LPCResyn.cpp LSP.cpp LPCC.cpp ERB.cpp CollectionFileSource.cpp MeddisHairCell.cpp ViconFileSource.cpp Cascade.cpp Parallel.cpp SeneffEar.cpp MP3FileSource.cpp OggFileSource.cpp ClassOutputSink.cpp ClipAudioRange.cpp SilenceRemove.cpp \
RtMidi.cpp Stk.cpp Limiter.cpp NoiseSource.cpp CompExp.cpp NoiseGate.cpp Plucked.cpp ADSR.cpp Envelope.cpp Delay.cpp NormMaxMin.cpp HarmonicEnhancer.cpp Reassign.cpp Normalize.cpp WekaSource.cpp RealvecSource.cpp MemorySource.cpp MidiInput.cpp Esitar.cpp NumericLib.cpp NormCut.cpp vmblock.cpp lu.cpp basis.cpp MATLABengine.cpp


