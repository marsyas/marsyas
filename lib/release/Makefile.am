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

if QT_PRESENT
lib_LIBRARIES = libmarsyas.a libmarsyasqt.a

libmarsyasqt.a: libmarsyasqt_Makefile
	cd $(top_srcdir)/src/marsyasqt/ && make
# libmarsyasqt.a is placed in lib/release/  from the qmake .pro file

libmarsyasqt_Makefile:
	cd $(top_srcdir)/src/marsyasqt/ && chmod u+w ../marsyasqt/ && qmake

libmarsyasqt_a_SOURCES =

install-data-local:
	cp $(top_srcdir)/lib/release/libmarsyasqt.a $(DESTDIR)$(libdir)
	cp $(top_srcdir)/src/marsyasqt/*.h $(DESTDIR)$(includedir)/marsyas/
else
lib_LIBRARIES         = libmarsyas.a
endif

pkginclude_HEADERS = \
			  $(top_srcdir)/otherlibs/libsvm/svm.h \
			  $(top_srcdir)/otherlibs/RtAudio/RtAudio3.h \
			  $(top_srcdir)/otherlibs/RtAudio/RtError3.h \
			  $(top_srcdir)/otherlibs/RtAudio/RtAudio.h \
			  $(top_srcdir)/otherlibs/RtAudio/RtError.h \
			  $(top_srcdir)/otherlibs/RtMidi/RtMidi.h \
			  $(top_srcdir)/otherlibs/RtMidi/RtError.h

INCLUDES += -I$(top_srcdir)/src/marsyas -I$(top_srcdir)/otherlibs/RtAudio -I$(top_srcdir)/otherlibs/RtMidi -I$(top_srcdir)/otherlibs/libsvm -I$(top_srcdir)/src/marsyas/MATLAB 

libmarsyas_a_SOURCES    = \
			  $(top_srcdir)/otherlibs/libsvm/svm.cpp \
			  $(top_srcdir)/otherlibs/RtAudio/RtAudio3.cpp \
			  $(top_srcdir)/otherlibs/RtAudio/RtAudio.cpp \
			  $(top_srcdir)/otherlibs/RtMidi/RtMidi.cpp \
			  $(top_srcdir)/src/marsyas/MrsLog.cpp \
			  $(top_srcdir)/src/marsyas/VScheduler.cpp \
			  $(top_srcdir)/src/marsyas/Scheduler.cpp \
			  $(top_srcdir)/src/marsyas/ScheduledEvent.cpp \
			  $(top_srcdir)/src/marsyas/SchedulerManager.cpp \
			  $(top_srcdir)/src/marsyas/TmTimer.cpp \
			  $(top_srcdir)/src/marsyas/TmTime.cpp \
			  $(top_srcdir)/src/marsyas/TmSampleCount.cpp \
			  $(top_srcdir)/src/marsyas/TmGetTime.cpp \
			  $(top_srcdir)/src/marsyas/TmControlValue.cpp \
			  $(top_srcdir)/src/marsyas/TimerFactory.cpp \
			  $(top_srcdir)/src/marsyas/MarEvent.cpp \
			  $(top_srcdir)/src/marsyas/EvGetUpd.cpp \
			  $(top_srcdir)/src/marsyas/EvValUpd.cpp \
			  $(top_srcdir)/src/marsyas/Repeat.cpp \
			  $(top_srcdir)/src/marsyas/EvExpr.cpp \
			  $(top_srcdir)/src/marsyas/ExScanner.cpp \
			  $(top_srcdir)/src/marsyas/ExParser.cpp \
			  $(top_srcdir)/src/marsyas/ExSymTbl.cpp \
			  $(top_srcdir)/src/marsyas/ExNode.cpp \
			  $(top_srcdir)/src/marsyas/ExVal.cpp \
			  $(top_srcdir)/src/marsyas/Expr.cpp \
			  $(top_srcdir)/src/marsyas/ExCommon.cpp \
			  $(top_srcdir)/src/marsyas/realvec.cpp \
			  $(top_srcdir)/src/marsyas/SoundFileSource.cpp \
			  $(top_srcdir)/src/marsyas/SoundFileSource2.cpp \
			  $(top_srcdir)/src/marsyas/AuFileSource.cpp \
			  $(top_srcdir)/src/marsyas/WavFileSource.cpp \
			  $(top_srcdir)/src/marsyas/WavFileSource2.cpp \
			  $(top_srcdir)/src/marsyas/BaseAudioSink.cpp \
			  $(top_srcdir)/src/marsyas/Mono2Stereo.cpp \
			  $(top_srcdir)/src/marsyas/AudioSource.cpp \
			  $(top_srcdir)/src/marsyas/AudioSource2.cpp \
			  $(top_srcdir)/src/marsyas/AudioSink.cpp \
			  $(top_srcdir)/src/marsyas/AudioSink2.cpp \
			  $(top_srcdir)/src/marsyas/SoundFileSink.cpp \
			  $(top_srcdir)/src/marsyas/AuFileSink.cpp \
			  $(top_srcdir)/src/marsyas/WavFileSink.cpp \
			  $(top_srcdir)/src/marsyas/FileName.cpp \
			  $(top_srcdir)/src/marsyas/MarSystem.cpp \
			  $(top_srcdir)/src/marsyas/MarSystemTemplateAdvanced.cpp \
			  $(top_srcdir)/src/marsyas/MarSystemTemplateBasic.cpp \
			  $(top_srcdir)/src/marsyas/MarControlManager.cpp \
			  $(top_srcdir)/src/marsyas/MarControl.cpp \
			  $(top_srcdir)/src/marsyas/MarControlValue.cpp \
			  $(top_srcdir)/src/marsyas/Spectrum.cpp \
			  $(top_srcdir)/src/marsyas/InvSpectrum.cpp \
			  $(top_srcdir)/src/marsyas/Collection.cpp \
			  $(top_srcdir)/src/marsyas/fft.cpp \
			  $(top_srcdir)/src/marsyas/Rms.cpp \
			  $(top_srcdir)/src/marsyas/Peak2Rms.cpp \
			  $(top_srcdir)/src/marsyas/PlotSink.cpp \
			  $(top_srcdir)/src/marsyas/AutoCorrelation.cpp \
			  $(top_srcdir)/src/marsyas/Peaker.cpp \
			  $(top_srcdir)/src/marsyas/PeakerAdaptive.cpp \
			  $(top_srcdir)/src/marsyas/HalfWaveRectifier.cpp \
			  $(top_srcdir)/src/marsyas/TimeStretch.cpp \
			  $(top_srcdir)/src/marsyas/Conversions.cpp \
			  $(top_srcdir)/src/marsyas/MaxArgMax.cpp \
			  $(top_srcdir)/src/marsyas/MinArgMin.cpp \
			  $(top_srcdir)/src/marsyas/Series.cpp \
			  $(top_srcdir)/src/marsyas/ShiftInput.cpp \
			  $(top_srcdir)/src/marsyas/ShiftOutput.cpp \
			  $(top_srcdir)/src/marsyas/Shifter.cpp \
			  $(top_srcdir)/src/marsyas/PvFold.cpp \
			  $(top_srcdir)/src/marsyas/PvConvert.cpp \
			  $(top_srcdir)/src/marsyas/PeakConvert.cpp \
			  $(top_srcdir)/src/marsyas/PeakSynthOsc.cpp  \
			  $(top_srcdir)/src/marsyas/PeakSynthOscBank.cpp \
			  $(top_srcdir)/src/marsyas/Polar2Cartesian.cpp  \
			  $(top_srcdir)/src/marsyas/Cartesian2Polar.cpp  \
			  $(top_srcdir)/src/marsyas/FlowCutSource.cpp \
			  $(top_srcdir)/src/marsyas/PeakSynthFFT.cpp \
			  $(top_srcdir)/src/marsyas/OverlapAdd.cpp \
			  $(top_srcdir)/src/marsyas/PeakResidual.cpp \
			  $(top_srcdir)/src/marsyas/PeUtilities.cpp \
			  $(top_srcdir)/src/marsyas/PvConvolve.cpp \
			  $(top_srcdir)/src/marsyas/PvOscBank.cpp \
			  $(top_srcdir)/src/marsyas/PvUnconvert.cpp \
			  $(top_srcdir)/src/marsyas/PvOverlapadd.cpp \
			  $(top_srcdir)/src/marsyas/Windowing.cpp \
			  $(top_srcdir)/src/marsyas/Fanout.cpp \
			  $(top_srcdir)/src/marsyas/Fanin.cpp \
			  $(top_srcdir)/src/marsyas/Negative.cpp \
			  $(top_srcdir)/src/marsyas/SineSource.cpp \
			  $(top_srcdir)/src/marsyas/MarSystemManager.cpp \
			  $(top_srcdir)/src/marsyas/PowerSpectrum.cpp \
			  $(top_srcdir)/src/marsyas/Centroid.cpp \
			  $(top_srcdir)/src/marsyas/Rolloff.cpp \
			  $(top_srcdir)/src/marsyas/Flux.cpp \
			  $(top_srcdir)/src/marsyas/Memory.cpp \
			  $(top_srcdir)/src/marsyas/Mean.cpp \
			  $(top_srcdir)/src/marsyas/StandardDeviation.cpp \
			  $(top_srcdir)/src/marsyas/ZeroCrossings.cpp \
			  $(top_srcdir)/src/marsyas/Thread.cpp \
			  $(top_srcdir)/src/marsyas/Socket.cpp \
			  $(top_srcdir)/src/marsyas/GaussianClassifier.cpp \
			  $(top_srcdir)/src/marsyas/AbsSoundFileSink.cpp \
			  $(top_srcdir)/src/marsyas/AbsSoundFileSource.cpp \
			  $(top_srcdir)/src/marsyas/AbsSoundFileSource2.cpp \
			  $(top_srcdir)/src/marsyas/Confidence.cpp \
			  $(top_srcdir)/src/marsyas/MarFileSink.cpp \
			  $(top_srcdir)/src/marsyas/WekaSink.cpp \
			  $(top_srcdir)/src/marsyas/MFCC.cpp \
			  $(top_srcdir)/src/marsyas/SCF.cpp \
			  $(top_srcdir)/src/marsyas/SFM.cpp \
			  $(top_srcdir)/src/marsyas/WaveletPyramid.cpp \
			  $(top_srcdir)/src/marsyas/WaveletStep.cpp \
			  $(top_srcdir)/src/marsyas/Daub4.cpp \
			  $(top_srcdir)/src/marsyas/OnePole.cpp \
			  $(top_srcdir)/src/marsyas/Accumulator.cpp \
			  $(top_srcdir)/src/marsyas/Shredder.cpp \
			  $(top_srcdir)/src/marsyas/Messager.cpp \
			  $(top_srcdir)/src/marsyas/Talk.cpp \
			  $(top_srcdir)/src/marsyas/TimeLine.cpp \
			  $(top_srcdir)/src/marsyas/Color.cpp \
			  $(top_srcdir)/src/marsyas/Communicator.cpp \
			  $(top_srcdir)/src/marsyas/Server.cpp \
			  $(top_srcdir)/src/marsyas/FullWaveRectifier.cpp \
			  $(top_srcdir)/src/marsyas/Norm.cpp \
			  $(top_srcdir)/src/marsyas/DownSampler.cpp \
			  $(top_srcdir)/src/marsyas/WaveletBands.cpp \
			  $(top_srcdir)/src/marsyas/Sum.cpp \
			  $(top_srcdir)/src/marsyas/Product.cpp \
			  $(top_srcdir)/src/marsyas/PeakPeriods2BPM.cpp \
			  $(top_srcdir)/src/marsyas/Histogram.cpp \
			  $(top_srcdir)/src/marsyas/BeatHistoFeatures.cpp \
			  $(top_srcdir)/src/marsyas/MslModel.cpp \
			  $(top_srcdir)/src/marsyas/CommandLineOptions.cpp \
			  $(top_srcdir)/src/marsyas/RawFileSource.cpp \
			  $(top_srcdir)/src/marsyas/FM.cpp \
			  $(top_srcdir)/src/marsyas/ANN_node.cpp \
			  $(top_srcdir)/src/marsyas/Annotator.cpp \
			  $(top_srcdir)/src/marsyas/Summary.cpp \
			  $(top_srcdir)/src/marsyas/ZeroRClassifier.cpp \
			  $(top_srcdir)/src/marsyas/KNNClassifier.cpp \
			  $(top_srcdir)/src/marsyas/Filter.cpp \
			  $(top_srcdir)/src/marsyas/Biquad.cpp \
			  $(top_srcdir)/src/marsyas/Kurtosis.cpp \
			  $(top_srcdir)/src/marsyas/Skewness.cpp \
			  $(top_srcdir)/src/marsyas/AMDF.cpp \
			  $(top_srcdir)/src/marsyas/SMO.cpp \
			  $(top_srcdir)/src/marsyas/SOM.cpp \
			  $(top_srcdir)/src/marsyas/LPC.cpp \
			  $(top_srcdir)/src/marsyas/LSP.cpp \
			  $(top_srcdir)/src/marsyas/BICchangeDetector.cpp \
			  $(top_srcdir)/src/marsyas/QGMMModel.cpp \
			  $(top_srcdir)/src/marsyas/LPCC.cpp \
			  $(top_srcdir)/src/marsyas/ERB.cpp \
			  $(top_srcdir)/src/marsyas/CollectionFileSource.cpp \
			  $(top_srcdir)/src/marsyas/MeddisHairCell.cpp \
			  $(top_srcdir)/src/marsyas/ViconFileSource.cpp \
			  $(top_srcdir)/src/marsyas/Cascade.cpp \
			  $(top_srcdir)/src/marsyas/Parallel.cpp \
			  $(top_srcdir)/src/marsyas/SeneffEar.cpp \
			  $(top_srcdir)/src/marsyas/MP3FileSource.cpp \
			  $(top_srcdir)/src/marsyas/OggFileSource.cpp \
			  $(top_srcdir)/src/marsyas/ClassOutputSink.cpp \
			  $(top_srcdir)/src/marsyas/ClipAudioRange.cpp \
			  $(top_srcdir)/src/marsyas/SilenceRemove.cpp \
			  $(top_srcdir)/src/marsyas/SpectralSNR.cpp \
			  $(top_srcdir)/src/marsyas/Stk.cpp \
			  $(top_srcdir)/src/marsyas/StereoSpectrum.cpp \
			  $(top_srcdir)/src/marsyas/StereoSpectrumFeatures.cpp \
			  $(top_srcdir)/src/marsyas/Limiter.cpp \
			  $(top_srcdir)/src/marsyas/NoiseSource.cpp \
			  $(top_srcdir)/src/marsyas/CompExp.cpp \
			  $(top_srcdir)/src/marsyas/NoiseGate.cpp \
			  $(top_srcdir)/src/marsyas/Plucked.cpp \
			  $(top_srcdir)/src/marsyas/ADSR.cpp \
			  $(top_srcdir)/src/marsyas/Envelope.cpp \
			  $(top_srcdir)/src/marsyas/Delay.cpp \
			  $(top_srcdir)/src/marsyas/Vibrato.cpp \
			  $(top_srcdir)/src/marsyas/NormMaxMin.cpp \
			  $(top_srcdir)/src/marsyas/HarmonicEnhancer.cpp \
			  $(top_srcdir)/src/marsyas/Reassign.cpp \
			  $(top_srcdir)/src/marsyas/Normalize.cpp \
			  $(top_srcdir)/src/marsyas/WekaSource.cpp \
			  $(top_srcdir)/src/marsyas/RealvecSource.cpp \
			  $(top_srcdir)/src/marsyas/RealvecSink.cpp \
			  $(top_srcdir)/src/marsyas/Panorama.cpp \
			  $(top_srcdir)/src/marsyas/Power.cpp \
			  $(top_srcdir)/src/marsyas/MemorySource.cpp \
			  $(top_srcdir)/src/marsyas/MidiInput.cpp \
			  $(top_srcdir)/src/marsyas/Esitar.cpp \
			  $(top_srcdir)/src/marsyas/MidiOutput.cpp \
			  $(top_srcdir)/src/marsyas/NumericLib.cpp \
			  $(top_srcdir)/src/marsyas/NormCut.cpp \
			  $(top_srcdir)/src/marsyas/vmblock.cpp \
			  $(top_srcdir)/src/marsyas/lu.cpp \
			  $(top_srcdir)/src/marsyas/basis.cpp \
			  $(top_srcdir)/src/marsyas/FlowThru.cpp \
			  $(top_srcdir)/src/marsyas/PeakFeatureSelect.cpp \
			  $(top_srcdir)/src/marsyas/SimilarityMatrix.cpp \
			  $(top_srcdir)/src/marsyas/Metric.cpp \
			  $(top_srcdir)/src/marsyas/MATLAB/MATLABengine.cpp \
			  $(top_srcdir)/src/marsyas/FanOutIn.cpp \
			  $(top_srcdir)/src/marsyas/MarSystemTemplateMedium.cpp \
			  $(top_srcdir)/src/marsyas/HWPS.cpp \
			  $(top_srcdir)/src/marsyas/RBF.cpp \
			  $(top_srcdir)/src/marsyas/NormMatrix.cpp \
			  $(top_srcdir)/src/marsyas/WHaSp.cpp \
			  $(top_srcdir)/src/marsyas/FanOutIn.cpp \
			  $(top_srcdir)/src/marsyas/PeakLabeler.cpp \
			  $(top_srcdir)/src/marsyas/peakView.cpp \
			  $(top_srcdir)/src/marsyas/PeakViewSource.cpp \
			  $(top_srcdir)/src/marsyas/PeakViewSink.cpp \
			  $(top_srcdir)/src/marsyas/PeakClusterSelect.cpp \
			  $(top_srcdir)/src/marsyas/OneRClassifier.cpp \
			  $(top_srcdir)/src/marsyas/WekaData.cpp \
			  $(top_srcdir)/src/marsyas/TranscriberExtract.cpp \
			  $(top_srcdir)/src/marsyas/Transcriber.cpp \
			  $(top_srcdir)/src/marsyas/PhiSEMSource.cpp \
			  $(top_srcdir)/src/marsyas/PhiSEMFilter.cpp \
			$(top_srcdir)/src/marsyas/SVMClassifier.cpp \
			$(top_srcdir)/src/marsyas/Classifier.cpp \
			$(top_srcdir)/src/marsyas/Spectrum2Chroma.cpp \
	$(top_srcdir)/src/marsyas/Spectrum2Mel.cpp \
	$(top_srcdir)/src/marsyas/ADRess.cpp \
	$(top_srcdir)/src/marsyas/ADRessSpectrum.cpp \
	$(top_srcdir)/src/marsyas/ADRessStereoSpectrum.cpp \
	$(top_srcdir)/src/marsyas/EnhADRess.cpp \
	$(top_srcdir)/src/marsyas/StereoSpectrumSources.cpp \
	$(top_srcdir)/src/marsyas/EnhADRessStereoSpectrum.cpp \
	$(top_srcdir)/src/marsyas/McAulayQuatieri.cpp \
	$(top_srcdir)/src/marsyas/PeakerOnset.cpp \
	$(top_srcdir)/src/marsyas/Reverse.cpp \
	$(top_srcdir)/src/marsyas/SNR.cpp \
	$(top_srcdir)/src/marsyas/PCA.cpp \
	$(top_srcdir)/src/marsyas/AbsMax.cpp \
	$(top_srcdir)/src/marsyas/TimelineLabeler.cpp \
	$(top_srcdir)/src/marsyas/RadioDrumInput.cpp \
	$(top_srcdir)/src/marsyas/AudioSourceRt4.cpp \
			$(top_srcdir)/src/marsyas/Gain.cpp
# please leave Gain.cpp at the end like this; it make scripts happy.

