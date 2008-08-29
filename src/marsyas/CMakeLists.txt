
# I've put the most OS-specific files at the top, to test the
# library support before compiling the un-problematic files
set( MarSystem_SOURCES
  Thread.cpp
  vmblock.cpp
  SVMClassifier.cpp
  PhiSEMSource.cpp
  AudioSink.cpp
  AudioSource.cpp
  ADSR.cpp
  AMDF.cpp
  ANN_node.cpp
  AbsSoundFileSink.cpp
  AbsSoundFileSource.cpp
  Accumulator.cpp
  Annotator.cpp
  AuFileSink.cpp
  AuFileSource.cpp
  AutoCorrelation.cpp
  BaseAudioSink.cpp
  BeatHistoFeatures.cpp
  Cascade.cpp
  Centroid.cpp
  ClassOutputSink.cpp
  ClipAudioRange.cpp
  Collection.cpp
  CollectionFileSource.cpp
  Color.cpp
  CommandLineOptions.cpp
  Communicator.cpp
  CompExp.cpp
  Confidence.cpp
  Conversions.cpp
  Daub4.cpp
  Delay.cpp
  DownSampler.cpp
  ERB.cpp
  Envelope.cpp
  Esitar.cpp
  EvExpr.cpp
  EvGetUpd.cpp
  EvValUpd.cpp
  ExCommon.cpp
  ExNode.cpp
  ExParser.cpp
  ExScanner.cpp
  ExSymTbl.cpp
  ExVal.cpp
  Expr.cpp
  FM.cpp
  Fanin.cpp
  Fanout.cpp
  FileName.cpp
  Filter.cpp
  Flux.cpp
  FullWaveRectifier.cpp
  GaussianClassifier.cpp
  HalfWaveRectifier.cpp
  HarmonicEnhancer.cpp
  Histogram.cpp
  InvSpectrum.cpp
  KNNClassifier.cpp
  Kurtosis.cpp
  LPC.cpp
  LPCC.cpp
  LSP.cpp
  Limiter.cpp
  MFCC.cpp
  MP3FileSource.cpp
  MarControl.cpp
  MarControlManager.cpp
  MarControlValue.cpp
  MarEvent.cpp
  MarFileSink.cpp
  MarSystem.cpp
  MarSystemManager.cpp
  MarSystemTemplateAdvanced.cpp
  MarSystemTemplateBasic.cpp
  MaxArgMax.cpp
  Mean.cpp
  MeddisHairCell.cpp
  Memory.cpp
  MemorySource.cpp
  MidiInput.cpp
  MidiOutput.cpp
  MinArgMin.cpp
  Mono2Stereo.cpp
  MrsLog.cpp
  MslModel.cpp
  Negative.cpp
  NoiseGate.cpp
  NoiseSource.cpp
  Norm.cpp
  NormCut.cpp
  NormMaxMin.cpp
  Normalize.cpp
  NumericLib.cpp
  OggFileSource.cpp
  OnePole.cpp
  OverlapAdd.cpp
  Parallel.cpp
  PeakConvert.cpp
  PeakResidual.cpp
  PeUtilities.cpp
  Peak2Rms.cpp
  PeakPeriods2BPM.cpp
  Peaker.cpp
  PeakerAdaptive.cpp
  PlotSink.cpp
  Plucked.cpp
  Power.cpp
  PowerSpectrum.cpp
  Product.cpp
  PvConvert.cpp
  PvConvolve.cpp
  PvFold.cpp
  PvOscBank.cpp
  PvOverlapadd.cpp
  PvUnconvert.cpp
  RawFileSource.cpp
  RealvecSink.cpp
  RealvecSource.cpp
  Reassign.cpp
  Repeat.cpp
  Rms.cpp
  Rolloff.cpp
  SCF.cpp
  SFM.cpp
  SMO.cpp
  SOM.cpp
  ScheduledEvent.cpp
  Scheduler.cpp
  SchedulerManager.cpp
  SeneffEar.cpp
  Series.cpp
  ShiftInput.cpp
  ShiftOutput.cpp
  Shifter.cpp
  Shredder.cpp
  SilenceRemove.cpp
  SineSource.cpp
  Skewness.cpp
  SoundFileSink.cpp
  SoundFileSource.cpp
  Spectrum.cpp
  StandardDeviation.cpp
  StereoSpectrum.cpp
  StereoSpectrumFeatures.cpp
  Stk.cpp
  Sum.cpp
  Summary.cpp
  Talk.cpp
  TimeLine.cpp
  TimeStretch.cpp
  TimerFactory.cpp
  TmControlValue.cpp
  TmGetTime.cpp
  TmSampleCount.cpp
  TmTime.cpp
  TmTimer.cpp
  ViconFileSource.cpp
  WavFileSink.cpp
  WavFileSource.cpp
  WaveletBands.cpp
  WaveletPyramid.cpp
  WaveletStep.cpp
  WekaSink.cpp
  WekaSource.cpp
  Windowing.cpp
  ZeroCrossings.cpp
  ZeroRClassifier.cpp
  basis.cpp
  fft.cpp
  lu.cpp
  realvec.cpp
  PeakSynthOsc.cpp
  Cartesian2Polar.cpp
  PeakSynthFFT.cpp
  Polar2Cartesian.cpp
  FlowCutSource.cpp
  AbsSoundFileSource2.cpp
  AudioSink2.cpp
  AudioSource2.cpp
  SoundFileSource2.cpp
  WavFileSource2.cpp
  BICchangeDetector.cpp
  QGMMModel.cpp
  PeakSynthOscBank.cpp
  SpectralSNR.cpp
  Vibrato.cpp
  Panorama.cpp
  Biquad.cpp
  FlowThru.cpp
  MarSystemTemplateMedium.cpp
  PeakFeatureSelect.cpp
  SimilarityMatrix.cpp
  Metric.cpp
  HWPS.cpp
  RBF.cpp
  NormMatrix.cpp
  WHaSp.cpp
  FanOutIn.cpp
  PeakLabeler.cpp
  PeakClusterSelect.cpp
  peakView.cpp
  PeakViewSink.cpp
  PeakViewSource.cpp
  OneRClassifier.cpp
  WekaData.cpp
  TranscriberExtract.cpp
  Transcriber.cpp
  PhiSEMFilter.cpp
  Classifier.cpp
  Spectrum2Chroma.cpp
  Spectrum2Mel.cpp
  ADRess.cpp
  ADRessSpectrum.cpp
  ADRessStereoSpectrum.cpp
  EnhADRess.cpp
  StereoSpectrumSources.cpp
  EnhADRessStereoSpectrum.cpp
  McAulayQuatieri.cpp
  PeakerOnset.cpp
  Reverse.cpp
  SNR.cpp
  PCA.cpp
  AbsMax.cpp
  TimelineLabeler.cpp
  RadioDrumInput.cpp
  Gain.cpp
)#modifySources

set(Marsyas_HEADERS
  MarSystemManager.h
  MarSystem.h
  common.h
  Conversions.h
  MarEvent.h
  TmTime.h
  TmControlValue.h
  MrsLog.h
  MarControl.h
  realvec.h
  TmSampleCount.h
  MarControlValue.h
  Communicator.h
  TmTimer.h
  Repeat.h
  ScheduledEvent.h
  Scheduler.h
  MarControlAccessor.h
  Heap.h
  AudioSink.h
)

## maybe -- necessary to make the .h files show up in MSVC
## I don't think it's worth listing them all.  -gp
#if (WIN32)
#  list (APPEND MarSystem_SOURCES
#    common.h
#  )
#endif (WIN32)


## OTHERLIBS
if (MARSYAS_MAD)
	include_directories(${mad_INCLUDE_DIR})
endif (MARSYAS_MAD)

if (MARSYAS_VORBIS)
	include_directories(${VORBIS_INCLUDE_DIR})
endif (MARSYAS_VORBIS)

if (GSTREAMER_FOUND)
	include_directories(${GSTREAMER_INCLUDE_DIRS})
endif (GSTREAMER_FOUND)

if (MARSYAS_MATLAB) 
   include_directories(${MATLAB_INCLUDE_DIR})
endif (MARSYAS_MATLAB)  


list(APPEND MarSystem_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/libsvm/svm.cpp)

if (MARSYAS_AUDIOIO)
	list(APPEND MarSystem_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/RtAudio/RtAudio.cpp)
	list(APPEND MarSystem_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/RtAudio/RtAudio3.cpp)
endif (MARSYAS_AUDIOIO)

if (MARSYAS_MIDIIO)
	list(APPEND MarSystem_SOURCES ${CMAKE_SOURCE_DIR}/otherlibs/RtMidi/RtMidi.cpp)
endif (MARSYAS_MIDIIO)

if (GSTREAMER_FOUND)
	list(APPEND MarSystem_SOURCES GStreamerSource.cpp gst-decode.cpp)
endif (GSTREAMER_FOUND)


if (MARSYAS_MATLAB)
	list(APPEND MarSystem_SOURCES MATLABengine.cpp)
endif (MARSYAS_MATLAB)





## ACTUAL BUILD
add_library(marsyas STATIC ${MarSystem_SOURCES} )

## LINKING
if (MARSYAS_MAD)
	TARGET_LINK_LIBRARIES(marsyas ${mad_LIBRARY})
endif (MARSYAS_MAD)

if (MARSYAS_VORBIS)
	TARGET_LINK_LIBRARIES(marsyas ${OGGVORBIS_LIBRARIES})
endif (MARSYAS_VORBIS)

if (MARSYAS_MATLAB) 
	TARGET_LINK_LIBRARIES(marsyas ${MATLAB_LIBRARIES})
endif (MARSYAS_MATLAB)

if (GSTREAMER_FOUND)
	TARGET_LINK_LIBRARIES(marsyas ${GSTREAMER_LIBRARIES})
endif (GSTREAMER_FOUND)


## INSTALLING
install(FILES ${Marsyas_HEADERS} DESTINATION include/marsyas)
install(TARGETS marsyas LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

