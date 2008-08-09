
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
  VScheduler.cpp
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

)

set(Marsyas_HEADERS
  MarSystemManager.h
  MarSystem.h
  common.h
  Conversions.h
  MarEvent.h
  TmTime.h
  VScheduler.h
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


## OTHERLIBS
include (marsyas-osincludes)

## maybe -- necessary to make the .h files show up in MSVC
## I don't think it's worth listing them all.  -gp
if (WIN32)
  list (APPEND MarSystem_SOURCES
    common.h
  )
endif (WIN32)


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


## INSTALLING
install(FILES ${Marsyas_HEADERS} DESTINATION include/marsyas)
install(TARGETS marsyas LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

