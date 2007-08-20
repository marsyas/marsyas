## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2002-2007 George Tzanetakis 
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



lib_LIBRARIES         = libmarsyas.a

INCLUDES += -I@top_srcdir@/marsyas -I@top_srcdir@/marsyas/MATLAB
 

libmarsyas_a_SOURCES    = \
	$(top_srcdir)/marsyas/MrsLog.cpp \
	$(top_srcdir)/marsyas/VScheduler.cpp \
	$(top_srcdir)/marsyas/Scheduler.cpp \
	$(top_srcdir)/marsyas/ScheduledEvent.cpp \
	$(top_srcdir)/marsyas/SchedulerManager.cpp \
	$(top_srcdir)/marsyas/TmTimer.cpp \
	$(top_srcdir)/marsyas/TmTime.cpp \
	$(top_srcdir)/marsyas/TmSampleCount.cpp \
	$(top_srcdir)/marsyas/TmGetTime.cpp \
	$(top_srcdir)/marsyas/TmControlValue.cpp \
	$(top_srcdir)/marsyas/TimerFactory.cpp \
	$(top_srcdir)/marsyas/MarEvent.cpp \
	$(top_srcdir)/marsyas/EvGetUpd.cpp \
	$(top_srcdir)/marsyas/EvValUpd.cpp \
	$(top_srcdir)/marsyas/Repeat.cpp \
	$(top_srcdir)/marsyas/EvExpr.cpp \
	$(top_srcdir)/marsyas/ExScanner.cpp \
	$(top_srcdir)/marsyas/ExParser.cpp \
	$(top_srcdir)/marsyas/ExSymTbl.cpp \
	$(top_srcdir)/marsyas/ExNode.cpp \
	$(top_srcdir)/marsyas/ExVal.cpp \
	$(top_srcdir)/marsyas/Expr.cpp \
	$(top_srcdir)/marsyas/ExCommon.cpp \
	$(top_srcdir)/marsyas/realvec.cpp \
	$(top_srcdir)/marsyas/SoundFileSource.cpp \
	$(top_srcdir)/marsyas/SoundFileSource2.cpp \
	$(top_srcdir)/marsyas/AuFileSource.cpp \
	$(top_srcdir)/marsyas/WavFileSource.cpp \
	$(top_srcdir)/marsyas/WavFileSource2.cpp \
	$(top_srcdir)/marsyas/BaseAudioSink.cpp \
	$(top_srcdir)/marsyas/Mono2Stereo.cpp \
	$(top_srcdir)/marsyas/AudioSource.cpp \
	$(top_srcdir)/marsyas/AudioSource2.cpp \
	$(top_srcdir)/marsyas/AudioSink.cpp \
	$(top_srcdir)/marsyas/AudioSink2.cpp \
	$(top_srcdir)/marsyas/SoundFileSink.cpp \
	$(top_srcdir)/marsyas/AuFileSink.cpp \
	$(top_srcdir)/marsyas/WavFileSink.cpp \
	$(top_srcdir)/marsyas/FileName.cpp \
	$(top_srcdir)/marsyas/MarSystem.cpp \
	$(top_srcdir)/marsyas/MarSystemTemplateAdvanced.cpp \
	$(top_srcdir)/marsyas/MarSystemTemplateBasic.cpp \
	$(top_srcdir)/marsyas/MarControlManager.cpp \
	$(top_srcdir)/marsyas/MarControl.cpp \
	$(top_srcdir)/marsyas/MarControlValue.cpp \
	$(top_srcdir)/marsyas/Spectrum.cpp \
	$(top_srcdir)/marsyas/InvSpectrum.cpp \
	$(top_srcdir)/marsyas/Collection.cpp \
	$(top_srcdir)/marsyas/fft.cpp \
	$(top_srcdir)/marsyas/Rms.cpp \
	$(top_srcdir)/marsyas/Peak2Rms.cpp \
	$(top_srcdir)/marsyas/PlotSink.cpp \
	$(top_srcdir)/marsyas/AutoCorrelation.cpp \
	$(top_srcdir)/marsyas/Peaker.cpp \
	$(top_srcdir)/marsyas/PeakerAdaptive.cpp \
	$(top_srcdir)/marsyas/HalfWaveRectifier.cpp \
	$(top_srcdir)/marsyas/TimeStretch.cpp \
	$(top_srcdir)/marsyas/Conversions.cpp \
	$(top_srcdir)/marsyas/MaxArgMax.cpp \
	$(top_srcdir)/marsyas/MinArgMin.cpp \
	$(top_srcdir)/marsyas/Series.cpp \
	$(top_srcdir)/marsyas/ShiftInput.cpp \
	$(top_srcdir)/marsyas/ShiftOutput.cpp \
	$(top_srcdir)/marsyas/Shifter.cpp \
	$(top_srcdir)/marsyas/PvFold.cpp \
	$(top_srcdir)/marsyas/PvConvert.cpp \
	$(top_srcdir)/marsyas/PeakConvert.cpp \
	$(top_srcdir)/marsyas/PeakSynthOsc.cpp  \
	$(top_srcdir)/marsyas/PeakSynthOscBank.cpp \
	$(top_srcdir)/marsyas/Polar2Cartesian.cpp  \
	$(top_srcdir)/marsyas/Cartesian2Polar.cpp  \
	$(top_srcdir)/marsyas/FlowCutSource.cpp \
	$(top_srcdir)/marsyas/PeakSynthFFT.cpp \
	$(top_srcdir)/marsyas/OverlapAdd.cpp \
	$(top_srcdir)/marsyas/PeakResidual.cpp \
	$(top_srcdir)/marsyas/PeUtilities.cpp \
	$(top_srcdir)/marsyas/PvConvolve.cpp \
	$(top_srcdir)/marsyas/PvOscBank.cpp \
	$(top_srcdir)/marsyas/PvUnconvert.cpp \
	$(top_srcdir)/marsyas/PvOverlapadd.cpp \
	$(top_srcdir)/marsyas/Windowing.cpp \
	$(top_srcdir)/marsyas/Fanout.cpp \
	$(top_srcdir)/marsyas/Fanin.cpp \
	$(top_srcdir)/marsyas/Negative.cpp \
	$(top_srcdir)/marsyas/SineSource.cpp \
	$(top_srcdir)/marsyas/MarSystemManager.cpp \
	$(top_srcdir)/marsyas/PowerSpectrum.cpp \
	$(top_srcdir)/marsyas/Centroid.cpp \
	$(top_srcdir)/marsyas/Rolloff.cpp \
	$(top_srcdir)/marsyas/Flux.cpp \
	$(top_srcdir)/marsyas/Memory.cpp \
	$(top_srcdir)/marsyas/Mean.cpp \
	$(top_srcdir)/marsyas/StandardDeviation.cpp \
	$(top_srcdir)/marsyas/ZeroCrossings.cpp \
	$(top_srcdir)/marsyas/Thread.cpp \
	$(top_srcdir)/marsyas/Socket.cpp \
	$(top_srcdir)/marsyas/GaussianClassifier.cpp \
	$(top_srcdir)/marsyas/AbsSoundFileSink.cpp \
	$(top_srcdir)/marsyas/AbsSoundFileSource.cpp \
	$(top_srcdir)/marsyas/AbsSoundFileSource2.cpp \
	$(top_srcdir)/marsyas/Confidence.cpp \
	$(top_srcdir)/marsyas/MarFileSink.cpp \
	$(top_srcdir)/marsyas/WekaSink.cpp \
	$(top_srcdir)/marsyas/MFCC.cpp \
	$(top_srcdir)/marsyas/SCF.cpp \
	$(top_srcdir)/marsyas/SFM.cpp \
	$(top_srcdir)/marsyas/WaveletPyramid.cpp \
	$(top_srcdir)/marsyas/WaveletStep.cpp \
	$(top_srcdir)/marsyas/Daub4.cpp \
	$(top_srcdir)/marsyas/OnePole.cpp \
	$(top_srcdir)/marsyas/RtAudio.cpp \
	$(top_srcdir)/marsyas/Accumulator.cpp \
	$(top_srcdir)/marsyas/Shredder.cpp \
	$(top_srcdir)/marsyas/Messager.cpp \
	$(top_srcdir)/marsyas/Talk.cpp \
	$(top_srcdir)/marsyas/TimeLine.cpp \
	$(top_srcdir)/marsyas/Color.cpp \
	$(top_srcdir)/marsyas/Communicator.cpp \
	$(top_srcdir)/marsyas/Server.cpp \
	$(top_srcdir)/marsyas/AbsMax.cpp \
	$(top_srcdir)/marsyas/FullWaveRectifier.cpp \
	$(top_srcdir)/marsyas/Norm.cpp \
	$(top_srcdir)/marsyas/DownSampler.cpp \
	$(top_srcdir)/marsyas/WaveletBands.cpp \
	$(top_srcdir)/marsyas/Sum.cpp \
	$(top_srcdir)/marsyas/Product.cpp \
	$(top_srcdir)/marsyas/PeakPeriods2BPM.cpp \
	$(top_srcdir)/marsyas/Histogram.cpp \
	$(top_srcdir)/marsyas/BeatHistoFeatures.cpp \
	$(top_srcdir)/marsyas/MslModel.cpp \
	$(top_srcdir)/marsyas/CommandLineOptions.cpp \
	$(top_srcdir)/marsyas/RawFileSource.cpp \
	$(top_srcdir)/marsyas/FM.cpp \
	$(top_srcdir)/marsyas/ANN_node.cpp \
	$(top_srcdir)/marsyas/Annotator.cpp \
	$(top_srcdir)/marsyas/Summary.cpp \
	$(top_srcdir)/marsyas/ZeroRClassifier.cpp \
	$(top_srcdir)/marsyas/KNNClassifier.cpp \
	$(top_srcdir)/marsyas/Filter.cpp \
	$(top_srcdir)/marsyas/Biquad.cpp \
	$(top_srcdir)/marsyas/Kurtosis.cpp \
	$(top_srcdir)/marsyas/Skewness.cpp \
	$(top_srcdir)/marsyas/AMDF.cpp \
	$(top_srcdir)/marsyas/SMO.cpp \
	$(top_srcdir)/marsyas/SOM.cpp \
	$(top_srcdir)/marsyas/LPC.cpp \
	$(top_srcdir)/marsyas/LSP.cpp \
	$(top_srcdir)/marsyas/BICchangeDetector.cpp \
	$(top_srcdir)/marsyas/QGMMModel.cpp \
	$(top_srcdir)/marsyas/LPCC.cpp \
	$(top_srcdir)/marsyas/ERB.cpp \
	$(top_srcdir)/marsyas/CollectionFileSource.cpp \
	$(top_srcdir)/marsyas/MeddisHairCell.cpp \
	$(top_srcdir)/marsyas/ViconFileSource.cpp \
	$(top_srcdir)/marsyas/Cascade.cpp \
	$(top_srcdir)/marsyas/Parallel.cpp \
	$(top_srcdir)/marsyas/SeneffEar.cpp \
	$(top_srcdir)/marsyas/MP3FileSource.cpp \
	$(top_srcdir)/marsyas/OggFileSource.cpp \
	$(top_srcdir)/marsyas/ClassOutputSink.cpp \
	$(top_srcdir)/marsyas/ClipAudioRange.cpp \
	$(top_srcdir)/marsyas/SilenceRemove.cpp \
	$(top_srcdir)/marsyas/RtMidi.cpp \
	$(top_srcdir)/marsyas/SpectralSNR.cpp \
	$(top_srcdir)/marsyas/Stk.cpp \
	$(top_srcdir)/marsyas/StereoSpectrum.cpp \
	$(top_srcdir)/marsyas/StereoSpectrumFeatures.cpp \
	$(top_srcdir)/marsyas/Limiter.cpp \
	$(top_srcdir)/marsyas/NoiseSource.cpp \
	$(top_srcdir)/marsyas/CompExp.cpp \
	$(top_srcdir)/marsyas/NoiseGate.cpp \
	$(top_srcdir)/marsyas/Plucked.cpp \
	$(top_srcdir)/marsyas/ADSR.cpp \
	$(top_srcdir)/marsyas/Envelope.cpp \
	$(top_srcdir)/marsyas/Delay.cpp \
	$(top_srcdir)/marsyas/Vibrato.cpp \
	$(top_srcdir)/marsyas/NormMaxMin.cpp \
	$(top_srcdir)/marsyas/HarmonicEnhancer.cpp \
	$(top_srcdir)/marsyas/Reassign.cpp \
	$(top_srcdir)/marsyas/Normalize.cpp \
	$(top_srcdir)/marsyas/WekaSource.cpp \
	$(top_srcdir)/marsyas/RealvecSource.cpp \
	$(top_srcdir)/marsyas/RealvecSink.cpp \
	$(top_srcdir)/marsyas/Panorama.cpp \
	$(top_srcdir)/marsyas/Power.cpp \
	$(top_srcdir)/marsyas/MemorySource.cpp \
	$(top_srcdir)/marsyas/MidiInput.cpp \
	$(top_srcdir)/marsyas/Esitar.cpp \
	$(top_srcdir)/marsyas/MidiOutput.cpp \
	$(top_srcdir)/marsyas/NumericLib.cpp \
	$(top_srcdir)/marsyas/NormCut.cpp \
	$(top_srcdir)/marsyas/vmblock.cpp \
	$(top_srcdir)/marsyas/lu.cpp \
	$(top_srcdir)/marsyas/basis.cpp \
	$(top_srcdir)/marsyas/FlowThru.cpp \
	$(top_srcdir)/marsyas/PeakFeatureSelect.cpp \
	$(top_srcdir)/marsyas/SimilarityMatrix.cpp \
	$(top_srcdir)/marsyas/Metric.cpp \
	$(top_srcdir)/marsyas/MATLAB/MATLABengine.cpp \
	$(top_srcdir)/marsyas/FanOutIn.cpp \
	$(top_srcdir)/marsyas/MarSystemTemplateMedium.cpp \
	$(top_srcdir)/marsyas/HWPS.cpp \
	$(top_srcdir)/marsyas/RBF.cpp \
	$(top_srcdir)/marsyas/NormMatrix.cpp \
	$(top_srcdir)/marsyas/WHaSp.cpp \
	$(top_srcdir)/marsyas/FanOutIn.cpp \
	$(top_srcdir)/marsyas/PeakLabeler.cpp \
	$(top_srcdir)/marsyas/peakView.cpp \
	$(top_srcdir)/marsyas/PeakViewSource.cpp \
	$(top_srcdir)/marsyas/PeakViewSink.cpp \
	$(top_srcdir)/marsyas/PeakClusterSelect.cpp \
	$(top_srcdir)/marsyas/OneRClassifier.cpp \
	$(top_srcdir)/marsyas/WekaData.cpp \
	$(top_srcdir)/marsyas/TranscriberExtract.cpp \
	$(top_srcdir)/marsyas/Transcriber.cpp \
	$(top_srcdir)/marsyas/Gain.cpp
# please leave Gain.cpp at the end like this; it make scripts happy.

