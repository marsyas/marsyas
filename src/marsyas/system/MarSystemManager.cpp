/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "../common_source.h"

#include <marsyas/marsystems/ADRess.h>
#include <marsyas/marsystems/ADRessSpectrum.h>
#include <marsyas/marsystems/ADRessStereoSpectrum.h>
#include <marsyas/marsystems/ADSR.h>
#include <marsyas/marsystems/AMDF.h>
#include <marsyas/marsystems/AbsMax.h>
#include <marsyas/marsystems/AccentFilterBank.h>
#include <marsyas/marsystems/Accumulator.h>
#include <marsyas/marsystems/AdditiveOsc.h>
#include <marsyas/marsystems/AimBoxes.h>
#include <marsyas/marsystems/AimGammatone.h>
#include <marsyas/marsystems/AimHCL.h>
#include <marsyas/marsystems/AimHCL2.h>
#include <marsyas/marsystems/AimLocalMax.h>
#include <marsyas/marsystems/AimPZFC.h>
#include <marsyas/marsystems/AimPZFC2.h>
#include <marsyas/marsystems/AimSAI.h>
#include <marsyas/marsystems/AimSSI.h>
#include <marsyas/marsystems/AimVQ.h>
#include <marsyas/marsystems/Annotator.h>
#include <marsyas/marsystems/ArffFileSink.h>
#include <marsyas/marsystems/AuFileSink.h>
#include <marsyas/marsystems/AuFileSource.h>
#include <marsyas/marsystems/AubioYin.h>
#include <marsyas/marsystems/AutoCorrelation.h>
#include <marsyas/marsystems/AutoCorrelationFFT.h>
#include <marsyas/marsystems/BICchangeDetector.h>
#include <marsyas/marsystems/BeatAgent.h>
#include <marsyas/marsystems/BeatHistoFeatures.h>
#include <marsyas/marsystems/BeatHistogram.h>
#include <marsyas/marsystems/BeatHistogramFromPeaks.h>
#include <marsyas/marsystems/BeatPhase.h>
#include <marsyas/marsystems/BeatReferee.h>
#include <marsyas/marsystems/BeatTimesSink.h>
#include <marsyas/marsystems/Biquad.h>
#include <marsyas/marsystems/BlitOsc.h>
#include <marsyas/marsystems/CARFAC.h>
#include <marsyas/marsystems/Cartesian2Polar.h>
#include <marsyas/marsystems/Cascade.h>
#include <marsyas/marsystems/Centroid.h>
#include <marsyas/marsystems/Chroma.h>
#include <marsyas/marsystems/ClassOutputSink.h>
#include <marsyas/marsystems/ClassificationReport.h>
#include <marsyas/marsystems/Clip.h>
#include <marsyas/marsystems/Combinator.h>
#include <marsyas/marsystems/CompExp.h>
#include <marsyas/marsystems/Compressor.h>
#include <marsyas/marsystems/Confidence.h>
#include <marsyas/marsystems/ConstQFiltering.h>
#include <marsyas/marsystems/CrossCorrelation.h>
#include <marsyas/marsystems/DTW.h>
#include <marsyas/marsystems/DeInterleaveSizecontrol.h>
#include <marsyas/marsystems/Deinterleave.h>
#include <marsyas/marsystems/Delay.h>
#include <marsyas/marsystems/DelaySamples.h>
#include <marsyas/marsystems/Delta.h>
#include <marsyas/marsystems/DeltaFirstOrderRegression.h>
#include <marsyas/marsystems/Differentiator.h>
#include <marsyas/marsystems/DownSampler.h>
#include <marsyas/marsystems/ERB.h>
#include <marsyas/marsystems/EnhADRess.h>
#include <marsyas/marsystems/EnhADRessStereoSpectrum.h>
#include <marsyas/marsystems/FM.h>
#include <marsyas/marsystems/FMeasure.h>
#include <marsyas/marsystems/FanOutIn.h>
#include <marsyas/marsystems/Fanin.h>
#include <marsyas/marsystems/Fanout.h>
#include <marsyas/marsystems/Filter.h>
#include <marsyas/marsystems/FlowCutSource.h>
#include <marsyas/marsystems/FlowThru.h>
#include <marsyas/marsystems/FlowToControl.h>
#include <marsyas/marsystems/Flux.h>
#include <marsyas/marsystems/FullWaveRectifier.h>
#include <marsyas/marsystems/GMMClassifier.h>
#include <marsyas/marsystems/Gain.h>
#include <marsyas/marsystems/GaussianClassifier.h>
#include <marsyas/marsystems/HWPS.h>
#include <marsyas/marsystems/HalfWaveRectifier.h>
#include <marsyas/marsystems/HarmonicEnhancer.h>
#include <marsyas/marsystems/Inject.h>
#include <marsyas/marsystems/InvSpectrum.h>
#include <marsyas/marsystems/KNNClassifier.h>
#include <marsyas/marsystems/Kurtosis.h>
#include <marsyas/marsystems/LPC.h>
#include <marsyas/marsystems/LPCC.h>
#include <marsyas/marsystems/LSP.h>
#include <marsyas/marsystems/LyonPassiveEar.h>
#include <marsyas/marsystems/MFCC.h>
#include <marsyas/marsystems/Map.h>
#include <marsyas/system/MarSystem.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/MarSystemTemplateMedium.h>
#include <marsyas/marsystems/MathPower.h>
#include <marsyas/marsystems/MaxArgMax.h>
#include <marsyas/marsystems/MaxMin.h>
#include <marsyas/marsystems/McAulayQuatieri.h>
#include <marsyas/marsystems/Mean.h>
#include <marsyas/marsystems/MeanAbsoluteDeviation.h>
#include <marsyas/marsystems/Median.h>
#include <marsyas/marsystems/MedianFilter.h>
#include <marsyas/marsystems/Memory.h>
#include <marsyas/marsystems/Metric.h>
#include <marsyas/marsystems/MidiFileSynthSource.h>
#include <marsyas/marsystems/MidiInput.h>
#include <marsyas/marsystems/MidiOutput.h>
#include <marsyas/marsystems/MinArgMin.h>
#include <marsyas/marsystems/MixToMono.h>
#include <marsyas/marsystems/Mono2Stereo.h>
#include <marsyas/marsystems/Negative.h>
#include <marsyas/marsystems/NoiseGate.h>
#include <marsyas/marsystems/NoiseSource.h>
#include <marsyas/marsystems/Norm.h>
#include <marsyas/marsystems/NormCut.h>
#include <marsyas/marsystems/NormMatrix.h>
#include <marsyas/marsystems/NormMaxMin.h>
#include <marsyas/marsystems/Normalize.h>
#include <marsyas/marsystems/NormalizeAbs.h>
#include <marsyas/marsystems/OnePole.h>
#include <marsyas/marsystems/OneRClassifier.h>
#include <marsyas/marsystems/OnsetTimes.h>
#include <marsyas/marsystems/OrcaSnip.h>
#include <marsyas/marsystems/OverlapAdd.h>
#include <marsyas/marsystems/PCA.h>
#include <marsyas/marsystems/Panorama.h>
#include <marsyas/marsystems/Parallel.h>
#include <marsyas/marsystems/ParallelMatrixWeight.h>
#include <marsyas/marsystems/PatchMatrix.h>
#include <marsyas/marsystems/Peak2Rms.h>
#include <marsyas/marsystems/PeakClusterSelect.h>
#include <marsyas/marsystems/PeakConvert.h>
#include <marsyas/marsystems/PeakConvert2.h>
#include <marsyas/marsystems/PeakDistanceHorizontality.h>
#include <marsyas/marsystems/PeakEnhancer.h>
#include <marsyas/marsystems/PeakFeatureSelect.h>
#include <marsyas/marsystems/PeakLabeler.h>
#include <marsyas/marsystems/PeakPeriods2BPM.h>
#include <marsyas/marsystems/PeakRatio.h>
#include <marsyas/marsystems/PeakResidual.h>
#include <marsyas/marsystems/PeakSynthFFT.h>
#include <marsyas/marsystems/PeakSynthOsc.h>
#include <marsyas/marsystems/PeakSynthOscBank.h>
#include <marsyas/marsystems/PeakViewMerge.h>
#include <marsyas/marsystems/PeakViewSink.h>
#include <marsyas/marsystems/PeakViewSource.h>
#include <marsyas/marsystems/Peaker.h>
#include <marsyas/marsystems/PeakerAdaptive.h>
#include <marsyas/marsystems/PeakerOnset.h>
#include <marsyas/marsystems/PhaseLock.h>
#include <marsyas/marsystems/PhiSEMFilter.h>
#include <marsyas/marsystems/PhiSEMSource.h>
#include <marsyas/marsystems/PitchDiff.h>
#include <marsyas/marsystems/PlotSink.h>
#include <marsyas/marsystems/Plucked.h>
#include <marsyas/marsystems/Polar2Cartesian.h>
#include <marsyas/marsystems/Power.h>
#include <marsyas/marsystems/PowerSpectrum.h>
#include <marsyas/marsystems/PowerToAverageRatio.h>
#include <marsyas/marsystems/Product.h>
#include <marsyas/marsystems/PvConvert.h>
#include <marsyas/marsystems/PvConvolve.h>
#include <marsyas/marsystems/PvFold.h>
#include <marsyas/marsystems/PvOscBank.h>
#include <marsyas/marsystems/PvOverlapadd.h>
#include <marsyas/marsystems/PvUnconvert.h>
#include <marsyas/marsystems/RBF.h>
#include <marsyas/marsystems/RadioDrumInput.h>
#include <marsyas/marsystems/RealvecSink.h>
#include <marsyas/marsystems/RealvecSource.h>
#include <marsyas/marsystems/Reassign.h>
#include <marsyas/marsystems/Reciprocal.h>
#include <marsyas/marsystems/RemoveObservations.h>
#include <marsyas/marsystems/Resample.h>
#include <marsyas/marsystems/ResampleBezier.h>
#include <marsyas/marsystems/ResampleLinear.h>
#include <marsyas/marsystems/ResampleNearestNeighbour.h>
#include <marsyas/marsystems/ResampleSinc.h>
#include <marsyas/marsystems/Reverse.h>
#include <marsyas/marsystems/Rms.h>
#include <marsyas/marsystems/Rolloff.h>
#include <marsyas/marsystems/RunningAutocorrelation.h>
#include <marsyas/marsystems/RunningStatistics.h>
#include <marsyas/marsystems/SCF.h>
#include <marsyas/marsystems/SFM.h>
#include <marsyas/marsystems/SMO.h>
#include <marsyas/marsystems/SNR.h>
#include <marsyas/marsystems/SOM.h>
#include <marsyas/marsystems/SVMClassifier.h>
#include <marsyas/marsystems/SVMLinearClassifier.h>
#include <marsyas/marsystems/Selector.h>
#include <marsyas/marsystems/SelfSimilarityMatrix.h>
#include <marsyas/marsystems/Series.h>
#include <marsyas/marsystems/ShiftInput.h>
#include <marsyas/marsystems/ShiftOutput.h>
#include <marsyas/marsystems/Shifter.h>
#include <marsyas/marsystems/Shredder.h>
#include <marsyas/marsystems/SilenceRemove.h>
#include <marsyas/marsystems/SimilarityMatrix.h>
#include <marsyas/marsystems/SimulMaskingFft.h>
#include <marsyas/marsystems/SineSource.h>
#include <marsyas/marsystems/Skewness.h>
#include <marsyas/marsystems/SliceDelta.h>
#include <marsyas/marsystems/SliceShuffle.h>
#include <marsyas/marsystems/SoundFileSink.h>
#include <marsyas/marsystems/SoundFileSource.h>
#include <marsyas/marsystems/SoundFileSource2.h>
#include <marsyas/marsystems/SoundFileSourceHopper.h>
#include <marsyas/marsystems/SpectralFlatnessAllBands.h>
#include <marsyas/marsystems/SpectralSNR.h>
#include <marsyas/marsystems/SpectralTransformations.h>
#include <marsyas/marsystems/Spectrum.h>
#include <marsyas/marsystems/Spectrum2Chroma.h>
#include <marsyas/marsystems/Spectrum2Mel.h>
#include <marsyas/marsystems/Square.h>
#include <marsyas/marsystems/StandardDeviation.h>
#include <marsyas/marsystems/StereoSpectrum.h>
#include <marsyas/marsystems/StereoSpectrumFeatures.h>
#include <marsyas/marsystems/StereoSpectrumSources.h>
#include <marsyas/marsystems/StretchLinear.h>
#include <marsyas/marsystems/Subtract.h>
#include <marsyas/marsystems/SubtractMean.h>
#include <marsyas/marsystems/Sum.h>
#include <marsyas/marsystems/SVFilter.h>
#include <marsyas/marsystems/TempoHypotheses.h>
#include <marsyas/marsystems/TimeFreqPeakConnectivity.h>
#include <marsyas/marsystems/TimeStretch.h>
#include <marsyas/marsystems/TimelineLabeler.h>
#include <marsyas/marsystems/Transposer.h>
#include <marsyas/marsystems/TriangularFilterBank.h>
#include <marsyas/marsystems/Unfold.h>
#include <marsyas/marsystems/Vibrato.h>
#include <marsyas/marsystems/ViconFileSource.h>
#include <marsyas/marsystems/WHaSp.h>
#include <marsyas/marsystems/WavFileSink.h>
#include <marsyas/marsystems/WavFileSource.h>
#include <marsyas/marsystems/WavFileSource2.h>
#include <marsyas/marsystems/WaveletBands.h>
#include <marsyas/marsystems/WaveletPyramid.h>
#include <marsyas/marsystems/WaveguideOsc.h>
#include <marsyas/WekaData.h>
#include <marsyas/marsystems/WekaSink.h>
#include <marsyas/marsystems/WekaSource.h>
#include <marsyas/marsystems/Windowing.h>
#include <marsyas/marsystems/Yin.h>
#include <marsyas/marsystems/ZeroCrossings.h>
#include <marsyas/marsystems/ZeroRClassifier.h>
#include <marsyas/common_source.h>
// #include <marsyas/marsystems/BinauralCARFAC.h>
#include <marsyas/marsystems/HarmonicStrength.h>
#include <marsyas/marsystems/CsvFileSource.h>
#include <marsyas/marsystems/Krumhansl_key_finder.h>
#include <marsyas/marsystems/PWMSource.h>
#include <marsyas/marsystems/DCSource.h>
#include <marsyas/marsystems/AliasingOsc.h>
#include <marsyas/marsystems/APDelayOsc.h>
#include <marsyas/marsystems/Upsample.h>
#include <marsyas/marsystems/Whitening.h>
#include <marsyas/marsystems/Energy.h>
#include <marsyas/marsystems/DPWOsc.h>
#include <marsyas/marsystems/SpectralCentroidBandNorm.h>
#include <marsyas/marsystems/ChromaFilter.h>
#include <marsyas/marsystems/Sidechain.h>
#include <marsyas/marsystems/CsvSink.h>
#include <marsyas/marsystems/Ratio.h>
#include <marsyas/marsystems/Threshold.h>
//modifyHeader

#ifdef MARSYAS_AUDIOIO
#include <marsyas/marsystems/AudioSource.h>
#include <marsyas/marsystems/AudioSourceBlocking.h>
#include <marsyas/marsystems/AudioSink.h>
#include <marsyas/marsystems/AudioSinkBlocking.h>
#endif
#ifdef MARSYAS_GSTREAMER
#include <marsyas/marsystems/GStreamerSource.h>
#endif
#ifdef MARSYAS_LAME
#include <marsyas/marsystems/MP3FileSink.h>
#endif

using std::ostringstream;
using std::map;
using std::istream;
using std::ostream;
using std::vector;

using namespace Marsyas;

MarSystemManager::MarSystemManager()
{
  // registerPrototype("BinauralCARFAC", new BinauralCARFAC("binauralcarfac"));
  registerPrototype("ADRess", new ADRess("adresspr"));
  registerPrototype("ADRessSpectrum", new ADRessSpectrum("adressspectrumpr"));
  registerPrototype("ADRessStereoSpectrum", new ADRessStereoSpectrum("adressstereospectrumpr"));
  registerPrototype("ADSR", new ADSR("adsrpr"));
  registerPrototype("AMDF", new AMDF("amdfpr"));
  registerPrototype("AbsMax", new AbsMax("absmaxpr"));
  registerPrototype("AccentFilterBank", new AccentFilterBank("afb"));
  registerPrototype("Accumulator", new Accumulator("acc"));
  registerPrototype("AdditiveOsc", new AdditiveOsc("additiveoscpr"));
  registerPrototype("AimBoxes", new AimBoxes("aimboxes"));
  registerPrototype("AimGammatone", new AimGammatone("aimgammatone"));
  registerPrototype("AimHCL", new AimHCL("aimhcl"));
  registerPrototype("AimHCL2", new AimHCL2("aimhcl2"));
  registerPrototype("AimLocalMax", new AimLocalMax("aimlocalmax"));
  registerPrototype("AimPZFC", new AimPZFC("aimpzfc"));
  registerPrototype("AimPZFC2", new AimPZFC2("aimpzfc2"));
  registerPrototype("AimSAI", new AimSAI("aimsai"));
  registerPrototype("AimSSI", new AimSSI("aimssi"));
  registerPrototype("AimVQ", new AimVQ("aimvq"));
  registerPrototype("AliasingOsc", new AliasingOsc("aliasingoscpr"));
  registerPrototype("Annotator", new Annotator("anonp"));
  registerPrototype("ArffFileSink", new ArffFileSink("arfffilesinkpr"));
  registerPrototype("AuFileSink", new AuFileSink("ausinkp"));
  registerPrototype("AuFileSource", new AuFileSource("aufp"));
  registerPrototype("AubioYin", new AubioYin("aubioyin"));
  registerPrototype("AutoCorrelation", new AutoCorrelation("acrp"));
  registerPrototype("AutoCorrelationFFT", new AutoCorrelationFFT("autocorfftpr"));
  registerPrototype("BICchangeDetector", new BICchangeDetector("bicchgdetpr"));
  registerPrototype("BeatAgent", new BeatAgent("beatagent"));
  registerPrototype("BeatHistoFeatures", new BeatHistoFeatures("bhfp"));
  registerPrototype("BeatHistogram", new BeatHistogram("beathistopr"));
  registerPrototype("BeatHistogramFromPeaks", new BeatHistogramFromPeaks("beathistofrompeakspr"));
  registerPrototype("BeatPhase", new BeatPhase("beatphase"));
  registerPrototype("BeatReferee", new BeatReferee("beatreferee"));
  registerPrototype("BeatTimesSink", new BeatTimesSink("beattimessink"));
  registerPrototype("Biquad", new Biquad("biquadp"));
  registerPrototype("BlitOsc", new BlitOsc("blitoscpr"));
  registerPrototype("CARFAC", new CARFAC("carfac"));
  registerPrototype("Cartesian2Polar", new Cartesian2Polar("c2p"));
  registerPrototype("Cascade", new Cascade("cascadep"));
  registerPrototype("Centroid", new Centroid("cntrp"));
  registerPrototype("Chroma", new Chroma("chromapr"));
  registerPrototype("ClassOutputSink", new ClassOutputSink("classoutputsinkp"));
  registerPrototype("ClassificationReport", new ClassificationReport("classificationreportpr"));
  registerPrototype("Clip", new Clip("clpr"));
  registerPrototype("Combinator", new Combinator("combinator"));
  registerPrototype("CompExp", new CompExp("compexppr"));
  registerPrototype("Compressor", new Compressor("compressor"));
  registerPrototype("Confidence", new Confidence("confp"));
  registerPrototype("ConstQFiltering", new ConstQFiltering("cqfpr"));
  registerPrototype("CrossCorrelation",new CrossCorrelation("crossCorrelationpr"));
  registerPrototype("CsvFileSource", new CsvFileSource("csvfilesourcepr"));
  registerPrototype("DCSource", new DCSource("dcsourcepr"));
  registerPrototype("DeInterleaveSizecontrol", new DeInterleaveSizecontrol("deintszctrlpr"));
  registerPrototype("Deinterleave", new Deinterleave("deintpr"));
  registerPrototype("Delay", new Delay("delaypr"));
  registerPrototype("DelaySamples", new DelaySamples("delay"));
  registerPrototype("Delta", new Delta("deltapr"));
  registerPrototype("DeltaFirstOrderRegression", new DeltaFirstOrderRegression("delta"));
  registerPrototype("Differentiator", new Differentiator("differentiator"));
  registerPrototype("DownSampler", new DownSampler("ds"));
  registerPrototype("ERB", new ERB("erbp"));
  registerPrototype("EnhADRess", new EnhADRess("Enhadresspr"));
  registerPrototype("EnhADRessStereoSpectrum", new EnhADRessStereoSpectrum("enhadressstereospectrumpr"));
  registerPrototype("FM", new FM("fmp"));
  registerPrototype("FMeasure", new FMeasure("F-Measure"));
  registerPrototype("FanOutIn", new FanOutIn("fanoutinpr"));
  registerPrototype("Fanin", new Fanin("finp"));
  registerPrototype("Fanout", new Fanout("fonp"));
  registerPrototype("Filter", new Filter("filterp"));
  registerPrototype("FlowCutSource", new FlowCutSource("fcs"));
  registerPrototype("FlowThru", new FlowThru("flowthrupr"));
  registerPrototype("FlowToControl", new FlowToControl("flowtocontrolpr"));
  registerPrototype("Flux", new Flux("fluxp"));
  registerPrototype("FullWaveRectifier", new FullWaveRectifier("fwr"));
  registerPrototype("GMMClassifier", new GMMClassifier("gmmsp"));
  registerPrototype("Gain", new Gain("gp"));
  registerPrototype("GaussianClassifier", new GaussianClassifier("gaussp"));
  registerPrototype("HWPS", new HWPS("hwpspr"));
  registerPrototype("HalfWaveRectifier", new HalfWaveRectifier("hwrp"));
  registerPrototype("HarmonicEnhancer", new HarmonicEnhancer("hepr"));
  registerPrototype("HarmonicStrength", new HarmonicStrength("harmonicstrengthpr"));
  registerPrototype("Inject", new Inject("injectpr"));
  registerPrototype("InvSpectrum", new InvSpectrum("ispkp"));
  registerPrototype("KNNClassifier", new KNNClassifier("knnp"));
  registerPrototype("Krumhansl_key_finder", new Krumhansl_key_finder("krumhansl_key_finderpr"));
  registerPrototype("Kurtosis", new Kurtosis("kurtosisp"));
  registerPrototype("LPC", new LPC("lpcwarppr"));
  registerPrototype("LPCC", new LPCC("lpccpr"));
  registerPrototype("LSP", new LSP("lsppr"));
  registerPrototype("LyonPassiveEar", new LyonPassiveEar("lyonp"));
  registerPrototype("MFCC", new MFCC("mfcc"));
  registerPrototype("Map", new Map("proto_map"));
  registerPrototype("MarSystemTemplateMedium", new MarSystemTemplateMedium("marsystemtemplatemediumpr"));
  registerPrototype("MathPower", new MathPower("mathpowerpr"));
  registerPrototype("MaxArgMax", new MaxArgMax("mxrp"));
  registerPrototype("MaxMin", new MaxMin("maxminpr"));
  registerPrototype("Mean", new Mean("meanp"));
  registerPrototype("MeanAbsoluteDeviation", new MeanAbsoluteDeviation("meanabsolutedeviationpr"));
  registerPrototype("Median", new Median("median"));
  registerPrototype("MedianFilter", new MedianFilter("medianfilterpr"));
  registerPrototype("Memory", new Memory("memp"));
  registerPrototype("Metric", new Metric("metricpr"));
  registerPrototype("MidiFileSynthSource", new MidiFileSynthSource("midifilesynthsourcepr"));
  registerPrototype("MidiInput", new MidiInput("midiinpr"));
  registerPrototype("MidiOutput", new MidiOutput("midioutpr"));
  registerPrototype("MinArgMin", new MinArgMin("mnrp"));
  registerPrototype("MixToMono", new MixToMono("mixtomono"));
  registerPrototype("Mono2Stereo", new Mono2Stereo("mono2stereop"));
  registerPrototype("Negative", new Negative("negp"));
  registerPrototype("NoiseGate", new NoiseGate("noisegatep"));
  registerPrototype("NoiseSource", new NoiseSource("noisesrcsp"));
  registerPrototype("Norm", new Norm("norm"));
  registerPrototype("NormCut", new NormCut("normcutpr"));
  registerPrototype("NormMatrix", new NormMatrix("normmatrixpr"));
  registerPrototype("NormMaxMin", new NormMaxMin("normmaxminpr"));
  registerPrototype("Normalize", new Normalize("normalizepr"));
  registerPrototype("NormalizeAbs", new NormalizeAbs("normalizeabspr"));
  registerPrototype("OnePole", new OnePole("lpf"));
  registerPrototype("OneRClassifier", new OneRClassifier("onerclassifierpr"));
  registerPrototype("OnsetTimes", new OnsetTimes("OnsetTimes"));
  registerPrototype("OrcaSnip", new OrcaSnip("GetSnipBounds"));
  registerPrototype("OverlapAdd", new OverlapAdd("oa"));
  registerPrototype("PCA", new PCA("pcapr"));
  registerPrototype("PWMSource", new PWMSource("pwmsourcepr"));
  registerPrototype("Panorama", new Panorama("panoramapr"));
  registerPrototype("Parallel", new Parallel("parallelp"));
  registerPrototype("ParallelMatrixWeight", new ParallelMatrixWeight("parallelmatrixweight"));
  registerPrototype("PatchMatrix", new PatchMatrix("patmatpr"));
  registerPrototype("Peak2Rms", new Peak2Rms("peakrms"));
  registerPrototype("PeakClusterSelect", new PeakClusterSelect("peclusterselectpr"));
  registerPrototype("PeakConvert", new PeakConvert("peconvp"));
  registerPrototype("PeakConvert2", new PeakConvert2("peconvp2"));
  registerPrototype("PeakDistanceHorizontality", new PeakDistanceHorizontality("horizontality"));
  registerPrototype("PeakEnhancer", new PeakEnhancer("peakenhpr"));
  registerPrototype("PeakFeatureSelect", new PeakFeatureSelect("pefeatselectpr"));
  registerPrototype("PeakLabeler", new PeakLabeler("pelabelerpr"));
  registerPrototype("PeakPeriods2BPM", new PeakPeriods2BPM("p2bpm"));
  registerPrototype("PeakRatio", new PeakRatio("perap"));
  registerPrototype("PeakResidual", new PeakResidual("peres"));
  registerPrototype("PeakSynthFFT", new PeakSynthFFT("pvfft"));
  registerPrototype("PeakSynthOsc", new PeakSynthOsc("pso"));
  registerPrototype("PeakSynthOscBank", new PeakSynthOscBank("pvoscp"));
  registerPrototype("PeakViewMerge", new PeakViewMerge("PeakViewMerge"));
  registerPrototype("PeakViewSink", new PeakViewSink("peakviewsinkpr"));
  registerPrototype("PeakViewSource", new PeakViewSource("peakviewsourcepr"));
  registerPrototype("Peaker", new Peaker("pkrp"));
  registerPrototype("PeakerAdaptive", new PeakerAdaptive("pkr1pr"));
  registerPrototype("PeakerOnset", new PeakerOnset("pkronsetpr"));
  registerPrototype("PhaseLock", new PhaseLock("phaselock"));
  registerPrototype("PhiSEMFilter", new PhiSEMFilter("phisemfilterpr"));
  registerPrototype("PhiSEMSource", new PhiSEMSource("phisemsourcepr"));
  registerPrototype("PitchDiff", new PitchDiff("pitchdiffpr"));
  registerPrototype("PlotSink", new PlotSink("plotsp"));
  registerPrototype("Plucked", new Plucked("pluckedpr"));
  registerPrototype("Polar2Cartesian", new Polar2Cartesian("p2c"));
  registerPrototype("Power", new Power("pow"));
  registerPrototype("PowerSpectrum", new PowerSpectrum("pspkp"));
  registerPrototype("PowerToAverageRatio", new PowerToAverageRatio("crestpr"));
  registerPrototype("Product", new Product("product"));
  registerPrototype("PvConvert", new PvConvert("pvconvp"));
  registerPrototype("PvConvolve", new PvConvolve("pvconvpr"));
  registerPrototype("PvFold", new PvFold("pvfp"));
  registerPrototype("PvOscBank", new PvOscBank("pvoscp"));
  registerPrototype("PvOverlapadd", new PvOverlapadd("pvovlfp"));
  registerPrototype("PvUnconvert", new PvUnconvert("pvuconvp"));
  registerPrototype("RBF", new RBF("rbfpr"));
  registerPrototype("RadioDrumInput", new RadioDrumInput("radiodrump"));
  registerPrototype("RealvecSink", new RealvecSink("realvecSink"));
  registerPrototype("RealvecSource", new RealvecSource("realvecSrc"));
  registerPrototype("Reassign", new Reassign("reassignpr"));
  registerPrototype("Reciprocal", new Reciprocal("reciprocal"));
  registerPrototype("RemoveObservations", new RemoveObservations("removeobservationspr"));
  registerPrototype("Resample", new Resample("resampabspr"));
  registerPrototype("ResampleBezier", new ResampleBezier("resampberpr"));
  registerPrototype("ResampleLinear", new ResampleLinear("resamplinpr"));
  registerPrototype("ResampleNearestNeighbour", new ResampleNearestNeighbour("resampnnpr"));
  registerPrototype("ResampleSinc", new ResampleSinc("resampsincpr"));
  registerPrototype("Reverse", new Reverse("reversepr"));
  registerPrototype("Rms", new Rms("rms"));
  registerPrototype("Rolloff", new Rolloff("rlfp"));
  registerPrototype("RunningAutocorrelation", new RunningAutocorrelation("runningautocorrelation"));
  registerPrototype("RunningStatistics", new RunningStatistics("runningstatisticspr"));
  registerPrototype("SCF", new SCF("scf"));
  registerPrototype("SFM", new SFM("sfm"));
  registerPrototype("SMO", new SMO("smopr"));
  registerPrototype("SNR", new SNR("snrpr"));
  registerPrototype("SOM", new SOM("sompr"));
  registerPrototype("SVMClassifier", new SVMClassifier("svmclassifierpr"));
  registerPrototype("SVMLinearClassifier", new SVMClassifier("svmlinearclassifierpr"));
  registerPrototype("Selector", new Selector("selectorpr"));
  registerPrototype("SelfSimilarityMatrix", new SelfSimilarityMatrix("selfsimilaritymatrixpr"));
  registerPrototype("Series", new Series("srp"));
  registerPrototype("ShiftInput", new ShiftInput("sip"));
  registerPrototype("ShiftOutput", new ShiftOutput("sop"));
  registerPrototype("Shifter", new Shifter("sp"));
  registerPrototype("Shredder", new Shredder("shred"));
  registerPrototype("SilenceRemove", new SilenceRemove("silenceremovepr"));
  registerPrototype("SimilarityMatrix", new SimilarityMatrix("similaritymatrixpr"));
  registerPrototype("SimulMaskingFft", new SimulMaskingFft("simulmask"));
  registerPrototype("SineSource", new SineSource("sinesp"));
  registerPrototype("Skewness", new Skewness("Skewnessp"));
  registerPrototype("SliceDelta", new SliceDelta("slicedeltapr"));
  registerPrototype("SliceShuffle", new SliceShuffle("sliceshuffle"));
  registerPrototype("SoundFileSink", new SoundFileSink("sfsp"));
  registerPrototype("SoundFileSource", new SoundFileSource("sfp"));
  registerPrototype("SoundFileSource2", new SoundFileSource2("sf2p"));
  registerPrototype("SoundFileSourceHopper", new SoundFileSourceHopper("soundfilesourcehopper"));
  registerPrototype("SpectralFlatnessAllBands", new SpectralFlatnessAllBands("spectralflatnessallbandspr"));
  registerPrototype("SpectralSNR", new SpectralSNR("ssnrpr"));
  registerPrototype("SpectralTransformations", new SpectralTransformations("spectraltransformationspr"));
  registerPrototype("Spectrum", new Spectrum("spkp"));
  registerPrototype("Spectrum2Chroma", new Spectrum2Chroma("spectrum2chromapr"));
  registerPrototype("Spectrum2Mel", new Spectrum2Mel("spectrum2melpr"));
  registerPrototype("Square", new Square("square"));
  registerPrototype("StandardDeviation", new StandardDeviation("stdp"));
  registerPrototype("StereoSpectrum", new StereoSpectrum("stereopr"));
  registerPrototype("StereoSpectrumFeatures", new StereoSpectrumFeatures("stereospkfpr"));
  registerPrototype("StereoSpectrumSources", new StereoSpectrumSources("stereospectrumsourcespr"));
  registerPrototype("StretchLinear", new StretchLinear("stretchlinearpr"));
  registerPrototype("Subtract", new Subtract("subtract"));
  registerPrototype("SubtractMean", new SubtractMean("subtractmean"));
  registerPrototype("Sum", new Sum("sum"));
  registerPrototype("SVFilter", new SVFilter("svfilter"));
  registerPrototype("TempoHypotheses", new TempoHypotheses("tempohyp"));
  registerPrototype("TimeFreqPeakConnectivity", new TimeFreqPeakConnectivity("tfpeakconn"));
  registerPrototype("TimeStretch", new TimeStretch("tscp"));
  registerPrototype("TimelineLabeler", new TimelineLabeler("timelinelabelerpr"));
  registerPrototype("Transposer", new Transposer("transposer"));
  registerPrototype("TriangularFilterBank", new TriangularFilterBank("triangularfilterbank_pr"));

  registerPrototype("Unfold", new Unfold("unfold"));
  registerPrototype("Vibrato", new Vibrato("vibratopr"));
  registerPrototype("ViconFileSource", new ViconFileSource("viconfilesourcep"));
  registerPrototype("WHaSp", new WHaSp("whasppr"));
  registerPrototype("WavFileSink", new WavFileSink("wavsinkp"));
  registerPrototype("WavFileSource", new WavFileSource("wavfp"));
  registerPrototype("WavFileSource2", new WavFileSource2("wavf2p"));
  registerPrototype("WaveletBands",   new WaveletBands("wvbands"));
  registerPrototype("WaveletPyramid", new WaveletPyramid("wvpyramid"));
  registerPrototype("WaveguideOsc", new WaveguideOsc("waveguideoscpr"));
  registerPrototype("WekaSink", new WekaSink("wsink"));
  registerPrototype("WekaSource", new WekaSource("wsource"));
  registerPrototype("Windowing", new Windowing("win"));
  registerPrototype("Yin", new Yin("yin"));
  registerPrototype("ZeroCrossings", new ZeroCrossings("zcrsp"));
  registerPrototype("ZeroRClassifier", new ZeroRClassifier("zerorp"));
  registerPrototype("DTW", new DTW("dtwpr"));
  registerPrototype("APDelayOsc", new APDelayOsc("apdelayoscpr"));
  registerPrototype("Upsample", new Upsample("upsamplepr"));
  registerPrototype("Whitening", new Whitening("whiteningpr"));
  registerPrototype("Energy", new Energy("energypr"));
  registerPrototype("DPWOsc", new DPWOsc("dpwoscpr"));
  registerPrototype("SpectralCentroidBandNorm", new SpectralCentroidBandNorm("spectralcentroidbandnormpr"));
  registerPrototype("ChromaFilter", new ChromaFilter("chromafilterpr"));
  registerPrototype("Sidechain", new Sidechain("sidechain_prototype"));
  registerPrototype("CsvSink", new CsvSink("csvsink_prototype"));
  registerPrototype("Ratio", new Ratio("ratio_prototype"));
  registerPrototype("Threshold", new Threshold("threshold_prototype"));
  //modifyRegister

  // optional MarSystems
#ifdef MARSYAS_AUDIOIO
  registerPrototype("AudioSink", new AudioSink("audiosinkp"));
  registerPrototype("AudioSinkBlocking", new AudioSinkBlocking("AudioSinkBlocking"));
  registerPrototype("AudioSource", new AudioSource("audiosourcepr"));
  registerPrototype("AudioSourceBlocking", new AudioSourceBlocking("audiosourceblockingpr"));
#endif

#ifdef MARSYAS_GSTREAMER
  registerPrototype("GStreamerSource", new GStreamerSource("gstp"));
#endif
#ifdef MARSYAS_LAME
  registerPrototype("MP3FileSink", new MP3FileSink("sfsp"));
#endif

  //***************************************************************************************
  //				Composite MarSystem prototypes
  //***************************************************************************************
  // Delay instantiation
  compositesMap_["DeviBot"                      ] = DEVIBOT;
  compositesMap_["Stereo2Mono"                  ] = STEREO2MONO;
  compositesMap_["TextureStats"                 ] = TEXTURESTATS;
  compositesMap_["LPCnet"                       ] = LPCNET;
  compositesMap_["PowerSpectrumNet"             ] = POWERSPECTRUMNET;
  compositesMap_["PowerSpectrumNet1"            ] = POWERSPECTRUMNET1;
  compositesMap_["STFT_features"                ] = STFT_FEATURES;
  compositesMap_["TimbreFeatures"               ] = TIMBREFEATURES;
  compositesMap_["StereoPanningSpectrumFeatures"] = STEREOPANNINGSPECTRUMFEATURES;
  compositesMap_["StereoFeatures"               ] = STEREOFEATURES;
  compositesMap_["PhaseVocoder"                 ] = PHASEVOCODER;
  compositesMap_["PhaseVocoderOscBank"          ] = PHASEVOCODEROSCBANK;
  compositesMap_["PitchSACF"                    ] = PITCHSACF;
  compositesMap_["PitchPraat"                   ] = PITCHPRAAT;
  compositesMap_["PeakAnalyse"                  ] = PEAKANALYSE;
  compositesMap_["WHaSpnet"                     ] = WHASPNET;
  compositesMap_["StereoFeatures2"              ] = STEREOFEATURES2;
  compositesMap_["Classifier"                   ] = CLASSIFIER;
  compositesMap_["Pipe_Block"                   ] = PIPE_BLOCK;
  compositesMap_["AFB_Block_A"                  ] = AFB_BLOCK_A;
  compositesMap_["AFB_Block_B"                  ] = AFB_BLOCK_B;
  compositesMap_["AFB_Block_C"                  ] = AFB_BLOCK_C;
  compositesMap_["Decimating_QMF"               ] = DECIMATING_QMF;
  compositesMap_["MultiPitch"                   ] = MULTIPITCH;
}

void MarSystemManager::registerComposite(std::string prototype)
{
  if (compositesMap_.find(prototype) == compositesMap_.end())
    return;


  switch (compositesMap_[prototype])
  {
  case STUB:
    break;


  case MULTIPITCH:
  {

    MarSystem* lpf1;
    MarSystem* lpf2;
    MarSystem* hpf1;
    MarSystem* hwr;
    MarSystem* hwr2;
    MarSystem* hwr3;
    MarSystem* autocorhi;
    MarSystem* autocorlo;
    MarSystem* sum;
    MarSystem* pe2;
    MarSystem* pe3;
    MarSystem* pe4;

    MarSystem* fan;
    MarSystem* hinet;
    MarSystem* lonet;

    MarSystem* multipitpr;

    realvec numlow, denomlow;
    realvec numhigh, denomhigh;

    numlow.create(3);
    denomlow.create(3);
    numhigh.create(3);
    denomhigh.create(3);

    //coefs are for butter(2,1000) and butter(2,1000,'high')
    numlow(0)=0.1207f; numlow(1)=0.2415f; numlow(2)=0.1207f;
    denomlow(0)=1.0f; denomlow(1)=-0.8058f; denomlow(2)=0.2888f;

    numhigh(0)=0.5236f; numhigh(1)=-1.0473f; numhigh(2)=0.5236f;
    denomhigh(0)=1.0f; denomhigh(1)=-0.8058f; denomhigh(2)=0.2888f;

    lpf1 = new Filter("lpf1");
    lpf1->updControl("mrs_realvec/ncoeffs", numlow);
    lpf1->updControl("mrs_realvec/dcoeffs", denomlow);

    lpf2 = new Filter("lpf2");
    lpf2->updControl("mrs_realvec/ncoeffs", numlow);
    lpf2->updControl("mrs_realvec/dcoeffs", denomlow);

    hpf1 = new Filter("hpf1");
    hpf1->updControl("mrs_realvec/ncoeffs", numhigh);
    hpf1->updControl("mrs_realvec/dcoeffs", denomhigh);

    hwr = new HalfWaveRectifier("hwr");
    hwr2 = new HalfWaveRectifier("hwr2");
    hwr3 = new HalfWaveRectifier("hwr3");
    autocorhi = new AutoCorrelationFFT("autocorhi");
    autocorlo = new AutoCorrelationFFT("autocorlo");

    pe2 = new PeakEnhancer("pe2");
    pe3 = new PeakEnhancer("pe3");
    pe4 = new PeakEnhancer("pe4");

    multipitpr = new Series("multipitpr");
    fan = new Fanout("fan");
    hinet = new Series("hinet");
    lonet = new Series("lonet");
    sum = new Sum("sum");

    lonet->addMarSystem(lpf1);
    lonet->addMarSystem(hwr);
    lonet->addMarSystem(autocorlo);
    hinet->addMarSystem(hpf1);
    hinet->addMarSystem(hwr2);
    hinet->addMarSystem(lpf2);
    hinet->addMarSystem(autocorhi);

    fan->addMarSystem(hinet);
    fan->addMarSystem(lonet);

    multipitpr->addMarSystem(fan);
    multipitpr->addMarSystem(sum);
    multipitpr->addMarSystem(pe2);
    multipitpr->addMarSystem(pe3);
    multipitpr->addMarSystem(pe4);
    multipitpr->addMarSystem(hwr3);
    registerPrototype("MultiPitch", multipitpr);
  }
  break;

  case DEVIBOT:
  {
    //--------------------------------------------------------------------------------
    // Making a prototype for a specific MidiOutput device
    //--------------------------------------------------------------------------------
    MarSystem* devibotpr = new MidiOutput("devibotpr");
    devibotpr->linkControl("mrs_natural/byte2", "mrs_natural/arm");
    devibotpr->linkControl("mrs_natural/byte3", "mrs_natural/velocity");
    devibotpr->linkControl("mrs_bool/sendMessage", "mrs_bool/strike");
    devibotpr->updControl("mrs_natural/byte1", 144);
    registerPrototype("DeviBot", devibotpr);
  }
  break;

  case STEREO2MONO:
  {
    //--------------------------------------------------------------------------------
    // Stereo2Mono MarSystem
    //--------------------------------------------------------------------------------
    MarSystem* stereo2monopr = new Sum("stereo2monopr");
    stereo2monopr->updControl("mrs_real/weight", 0.5);
    registerPrototype("Stereo2Mono", stereo2monopr);
  }
  break;

  case TEXTURESTATS:
  {
    //--------------------------------------------------------------------------------
    // texture window analysis composite prototype
    //--------------------------------------------------------------------------------
    MarSystem* textureStatspr = new Series("tstatspr");
    textureStatspr->addMarSystem(new Memory("mempr"));

    MarSystem* meanstdpr = new Fanout("meanstdpr");
    meanstdpr->addMarSystem(new Mean("meanpr"));
    meanstdpr->addMarSystem(new StandardDeviation("stdpr"));
    textureStatspr->addMarSystem(meanstdpr);

    textureStatspr->linkControl("Memory/mempr/mrs_natural/memSize", "mrs_natural/memSize");
    textureStatspr->linkControl("Memory/mempr/mrs_bool/reset", "mrs_bool/reset");
    registerPrototype("TextureStats", textureStatspr);
  }
  break;

  case POWERSPECTRUMNET:
  {
    //--------------------------------------------------------------------------------
    // Power spectrum composite prototype
    //--------------------------------------------------------------------------------
    MarSystem* pspectpr = create("Series", "pspectpr");
    pspectpr->addMarSystem(create("ShiftInput", "si"));
    pspectpr->addMarSystem(create("Windowing", "hamming"));
    pspectpr->addMarSystem(create("Spectrum","spk"));
    pspectpr->updControl("Spectrum/spk/mrs_real/cutoff", 1.0);
    pspectpr->addMarSystem(create("PowerSpectrum", "pspk"));
    pspectpr->updControl("PowerSpectrum/pspk/mrs_string/spectrumType","power");
    pspectpr->linkControl("Spectrum/spk/mrs_real/cutoff", "mrs_real/cutoff");
    pspectpr->linkControl("ShiftInput/si/mrs_natural/winSize", "mrs_natural/winSize");
    registerPrototype("PowerSpectrumNet", pspectpr);
  }
  break;

  case POWERSPECTRUMNET1:
  {
    MarSystem* pspectpr1 = create("Series", "pspectpr1");
    pspectpr1->addMarSystem(create("Spectrum","spk"));
    pspectpr1->updControl("Spectrum/spk/mrs_real/cutoff", 1.0);
    pspectpr1->addMarSystem(create("PowerSpectrum", "pspk"));
    pspectpr1->updControl("PowerSpectrum/pspk/mrs_string/spectrumType","power");
    pspectpr1->linkControl("Spectrum/spk/mrs_real/cutoff", "mrs_real/cutoff");
    registerPrototype("PowerSpectrumNet1", pspectpr1);
  }
  break;

  case STFT_FEATURES:
  {
    // STFT_features prototype
    MarSystem* stft_features_pr = create("Fanout", "stft_features_pr");
    stft_features_pr->addMarSystem(create("Centroid", "cntrd"));
    stft_features_pr->addMarSystem(create("Rolloff", "rlf"));
    stft_features_pr->addMarSystem(create("Flux", "flux"));
    stft_features_pr->addMarSystem(create("MFCC", "mfcc"));

    MarSystem* chromaPrSeries =  create("Series", "chromaPrSeries");

    chromaPrSeries->addMarSystem(create("Spectrum2Chroma", "chroma"));
    chromaPrSeries->addMarSystem(create("PeakRatio","pr"));

    stft_features_pr->addMarSystem(chromaPrSeries);
    stft_features_pr->addMarSystem(create("SCF", "scf"));
    stft_features_pr->addMarSystem(create("SFM", "sfm"));
    registerPrototype("STFT_features", stft_features_pr);
  }
  break;

  // Variables are shared, so instantiate together
  case LPCNET:
  case TIMBREFEATURES:
  {
    //--------------------------------------------------------------------------------
    // LPC composite prototype
    //--------------------------------------------------------------------------------
    MarSystem* LPCnetpr = new Series("lpcnetpr");
    // create and configure the pre-emphasis filter as a FIR:
    // H(z) = 1 + aZ-1 ; a = -0.97
    LPCnetpr->addMarSystem(create("Filter", "preEmph"));
    realvec ncoeffs(2);
    realvec dcoeffs(1);
    ncoeffs(0) = 1.0;
    ncoeffs(1) = -0.97;
    dcoeffs(0) = 1.0;
    LPCnetpr->updControl("Filter/preEmph/mrs_realvec/ncoeffs", ncoeffs);
    LPCnetpr->updControl("Filter/preEmph/mrs_realvec/dcoeffs", dcoeffs);
    LPCnetpr->addMarSystem(create("ShiftInput", "si"));
    LPCnetpr->addMarSystem(create("Windowing", "ham"));
    LPCnetpr->addMarSystem(create("LPC", "lpc"));
    LPCnetpr->linkControl("Filter/preEmph/mrs_realvec/ncoeffs", "mrs_realvec/preEmphFIR");
    LPCnetpr->linkControl("ShiftInput/si/mrs_natural/winSize","mrs_natural/winSize");
    LPCnetpr->linkControl("LPC/lpc/mrs_natural/order", "mrs_natural/order");
    LPCnetpr->linkControl("LPC/lpc/mrs_real/lambda", "mrs_real/lambda");
    LPCnetpr->linkControl("LPC/lpc/mrs_real/gamma", "mrs_real/gamma");
    registerPrototype("LPCnet", LPCnetpr);

    ////////////////////////////////////////////////////////////////////
    // timbre_features prototype
    ////////////////////////////////////////////////////////////////////
    MarSystem* timbre_features_pr = new Fanout("timbre_features_pr");
    // TD branch
    MarSystem* timeDomainFeatures = create("Series", "timeDomain");
    timeDomainFeatures->addMarSystem(create("ShiftInput", "si"));
    MarSystem* tdf = create("Fanout", "tdf");
    tdf->addMarSystem(create("ZeroCrossings", "zcrs"));
    timeDomainFeatures->addMarSystem(tdf);
    timbre_features_pr->addMarSystem(timeDomainFeatures);
    // FFT branch
    MarSystem* spectralShape = create("Series", "spectralShape");
    spectralShape->addMarSystem(create("ShiftInput", "si"));
    spectralShape->addMarSystem(create("Windowing", "hamming"));
    spectralShape->addMarSystem(create("PowerSpectrumNet1", "powerSpect1"));
    MarSystem* spectrumFeatures = create("STFT_features", "spectrumFeatures");
    spectralShape->addMarSystem(spectrumFeatures);
    timbre_features_pr->addMarSystem(spectralShape);
    // LPC branch
    MarSystem* lpcFeatures = create("Series", "lpcFeatures");
    lpcFeatures->addMarSystem(create("Filter", "preEmph"));
    lpcFeatures->updControl("Filter/preEmph/mrs_realvec/ncoeffs", ncoeffs);
    lpcFeatures->updControl("Filter/preEmph/mrs_realvec/dcoeffs", dcoeffs);
    lpcFeatures->addMarSystem(create("ShiftInput", "si"));
    lpcFeatures->addMarSystem(create("Windowing", "ham"));
    MarSystem* lpcf = create("Fanout", "lpcf");
    MarSystem* lspbranch = create("Series", "lspbranch");
    MarSystem* lpccbranch = create("Series","lpccbranch");
    lspbranch->addMarSystem(create("LPC", "lpc"));
    lspbranch->updControl("LPC/lpc/mrs_natural/order", 18);
    lspbranch->addMarSystem(create("LSP", "lsp"));
    lpccbranch->addMarSystem(create("LPC", "lpc"));
    lpccbranch->updControl("LPC/lpc/mrs_natural/order", 12);
    lpccbranch->addMarSystem(create("LPCC", "lpcc"));
    lpcf->addMarSystem(lspbranch);
    lpcf->addMarSystem(lpccbranch);
    lpcFeatures->addMarSystem(lpcf);
    timbre_features_pr->addMarSystem(lpcFeatures);

    timbre_features_pr->linkControl("Series/timeDomain/ShiftInput/si/mrs_natural/winSize", "mrs_natural/winSize");
    timbre_features_pr->linkControl("Series/spectralShape/ShiftInput/si/mrs_natural/winSize", "mrs_natural/winSize");
    timbre_features_pr->linkControl("Series/lpcFeatures/ShiftInput/si/mrs_natural/winSize", "mrs_natural/winSize");

    timbre_features_pr->linkControl("Series/spectralShape/STFT_features/spectrumFeatures/mrs_string/enableChild", "mrs_string/enableSPChild");
    timbre_features_pr->linkControl("Series/spectralShape/STFT_features/spectrumFeatures/mrs_string/disableChild","mrs_string/disableSPChild");

    timbre_features_pr->linkControl("Series/timeDomain/Fanout/tdf/mrs_string/enableChild", "mrs_string/enableTDChild");
    timbre_features_pr->linkControl("Series/timeDomain/Fanout/tdf/mrs_string/disableChild", "mrs_string/disableTDChild");

    timbre_features_pr->linkControl("Series/lpcFeatures/Fanout/lpcf/mrs_string/enableChild", "mrs_string/enableLPCChild");
    timbre_features_pr->linkControl("Series/lpcFeatures/Fanout/lpcf/mrs_string/disableChild", "mrs_string/disableLPCChild");

    timbre_features_pr->updControl("mrs_string/disableSPChild", "all");
    timbre_features_pr->updControl("mrs_string/disableTDChild", "all");
    timbre_features_pr->updControl("mrs_string/disableLPCChild", "all");

    registerPrototype("TimbreFeatures", timbre_features_pr);
  }
  break;

  case STEREOPANNINGSPECTRUMFEATURES:
  {
    ///////////////////////////////////////////////////////////////////
    // StereoPanningSpectrumFeatures
    ///////////////////////////////////////////////////////////////////
    MarSystem *stereoFeatNet = create("Series", "stereoFeatNet");
    MarSystem* stereobranches = create("Parallel", "stereobranches");
    MarSystem* left = create("Series", "left");
    MarSystem* right = create("Series", "right");
    left->addMarSystem(create("ShiftInput", "sileft"));
    left->addMarSystem(create("Windowing", "hamleft"));
    left->addMarSystem(create("Spectrum", "spkleft"));
    right->addMarSystem(create("ShiftInput", "siright"));
    right->addMarSystem(create("Windowing", "hamright"));
    right->addMarSystem(create("Spectrum", "spkright"));

    stereobranches->addMarSystem(left);
    stereobranches->addMarSystem(right);

    stereoFeatNet->addMarSystem(stereobranches);
    stereoFeatNet->addMarSystem(create("StereoSpectrum", "sspk"));
    stereoFeatNet->addMarSystem(create("StereoSpectrumFeatures", "sspkf"));
    registerPrototype("StereoPanningSpectrumFeatures", stereoFeatNet);
  }
  break;

  case STEREOFEATURES:
  {

    /////////////////////////////////////////////////////////////////
    // combined stereo features
    /////////////////////////////////////////////////////////////////
    MarSystem* stereoFeatures = create("Fanout", "stereoFeatures");
    MarSystem* stereoTimbreFeatures = create("Parallel", "stereoTimbreFeatures");
    MarSystem* featExtractorLeft = create("TimbreFeatures", "featExtractorLeft");
    MarSystem* featExtractorRight = create("TimbreFeatures", "featExtractorRight");
    stereoTimbreFeatures->addMarSystem(featExtractorLeft);
    stereoTimbreFeatures->addMarSystem(featExtractorRight);
    stereoFeatures->addMarSystem(stereoTimbreFeatures);
    stereoFeatures->addMarSystem(create("StereoPanningSpectrumFeatures", "SPSFeatures"));

    //link winSize controls
    stereoFeatures->linkControl("Parallel/stereoTimbreFeatures/TimbreFeatures/featExtractorLeft/mrs_natural/winSize", "mrs_natural/winSize");
    stereoFeatures->linkControl("Parallel/stereoTimbreFeatures/TimbreFeatures/featExtractorRight/mrs_natural/winSize", "mrs_natural/winSize");
    stereoFeatures->linkControl("StereoPanningSpectrumFeatures/SPSFeatures/Parallel/stereobranches/Series/left/ShiftInput/sileft/mrs_natural/winSize", "mrs_natural/winSize");
    stereoFeatures->linkControl("StereoPanningSpectrumFeatures/SPSFeatures/Parallel/stereobranches/Series/right/ShiftInput/siright/mrs_natural/winSize", "mrs_natural/winSize");

    //link enable controls
    stereoFeatures->linkControl("Parallel/stereoTimbreFeatures/TimbreFeatures/featExtractorLeft/mrs_string/enableSPChild", "mrs_string/enableSPChild");
    stereoFeatures->linkControl("Parallel/stereoTimbreFeatures/TimbreFeatures/featExtractorRight/mrs_string/enableSPChild", "mrs_string/enableSPChild");

    stereoFeatures->linkControl("Parallel/stereoTimbreFeatures/TimbreFeatures/featExtractorLeft/mrs_string/enableTDChild", "mrs_string/enableTDChild");
    stereoFeatures->linkControl("Parallel/stereoTimbreFeatures/TimbreFeatures/featExtractorRight/mrs_string/enableTDChild", "mrs_string/enableTDChild");


    registerPrototype("StereoFeatures", stereoFeatures);
  }
  break;

  case PHASEVOCODER:
  {
    //--------------------------------------------------------------------------------
    // Phase Vocoder composite prototype
    //--------------------------------------------------------------------------------
    MarSystem* pvocpr = new Series("pvocpr");
    pvocpr->addMarSystem(new ShiftInput("si"));
    pvocpr->addMarSystem(new PvFold("fo"));
    pvocpr->addMarSystem(new Spectrum("spk"));
    pvocpr->addMarSystem(new PvConvert("conv"));
    pvocpr->addMarSystem(new PvUnconvert("uconv"));
    pvocpr->addMarSystem(new InvSpectrum("ispectrum"));
    pvocpr->addMarSystem(new PvOverlapadd("pover"));
    pvocpr->addMarSystem(new ShiftOutput("so"));

    pvocpr->linkControl("ShiftInput/si/mrs_natural/winSize",
                        "mrs_natural/winSize");
    pvocpr->linkControl("PvOverlapadd/pover/mrs_natural/winSize",
                        "mrs_natural/winSize");


    pvocpr->linkControl("PvConvert/conv/mrs_natural/Decimation",
                        "mrs_natural/Decimation");
    pvocpr->linkControl("PvUnconvert/uconv/mrs_natural/Decimation",
                        "mrs_natural/Decimation");
    pvocpr->linkControl("PvOverlapadd/pover/mrs_natural/Decimation",
                        "mrs_natural/Decimation");

    pvocpr->linkControl("PvUnconvert/uconv/mrs_natural/Interpolation",
                        "mrs_natural/Interpolation");
    pvocpr->linkControl("PvOverlapadd/pover/mrs_natural/Interpolation",
                        "mrs_natural/Interpolation");
    pvocpr->linkControl("ShiftOutput/so/mrs_natural/Interpolation",
                        "mrs_natural/Interpolation");

    pvocpr->linkControl("PvFold/fo/mrs_natural/FFTSize",
                        "mrs_natural/FFTSize");
    pvocpr->linkControl("PvOverlapadd/pover/mrs_natural/FFTSize",
                        "mrs_natural/FFTSize");

    pvocpr->linkControl("PvConvert/conv/mrs_realvec/phases",
                        "PvUnconvert/uconv/mrs_realvec/analysisphases");

    pvocpr->linkControl("PvUnconvert/uconv/mrs_realvec/regions",
                        "PvConvert/conv/mrs_realvec/regions");

    pvocpr->linkControl("PvConvert/conv/mrs_natural/Sinusoids",
                        "mrs_natural/Sinusoids");
    pvocpr->linkControl("PvConvert/conv/mrs_string/mode",
                        "mrs_string/convertMode");
    pvocpr->linkControl("PvUnconvert/uconv/mrs_string/mode",
                        "mrs_string/unconvertMode");
    pvocpr->linkControl("PvUnconvert/uconv/mrs_bool/phaselock",
                        "mrs_bool/phaselock");


    pvocpr->linkControl("PvFold/fo/mrs_real/rmsIn",
                        "PvOverlapadd/pover/mrs_real/rmsIn");


    registerPrototype("PhaseVocoder", pvocpr);
  }
  break;

  case PHASEVOCODEROSCBANK:
  {
    MarSystem* pvocpr1 = new Series("pvocpr1");
    pvocpr1->addMarSystem(new ShiftInput("si"));
    pvocpr1->addMarSystem(new PvFold("fo"));
    pvocpr1->addMarSystem(new Spectrum("spk"));
    pvocpr1->addMarSystem(new PvConvert("conv"));
    pvocpr1->addMarSystem(new PvOscBank("pob"));
    pvocpr1->addMarSystem(new ShiftOutput("so"));

    pvocpr1->linkControl("ShiftInput/si/mrs_natural/winSize",
                         "mrs_natural/winSize");

    pvocpr1->linkControl("PvOscBank/pob/mrs_natural/winSize",
                         "mrs_natural/winSize");

    pvocpr1->linkControl("PvFold/fo/mrs_real/rmsIn",
                         "PvOscBank/pob/mrs_real/rmsIn");

    pvocpr1->linkControl("PvConvert/conv/mrs_natural/Decimation",
                         "mrs_natural/Decimation");
    pvocpr1->linkControl("ShiftOutput/so/mrs_natural/Interpolation",
                         "mrs_natural/Interpolation");
    pvocpr1->linkControl("PvFold/fo/mrs_natural/FFTSize",
                         "mrs_natural/FFTSize");
    pvocpr1->linkControl("PvConvert/conv/mrs_natural/Sinusoids",
                         "mrs_natural/Sinusoids");
    pvocpr1->linkControl("PvConvert/conv/mrs_string/mode",
                         "mrs_string/convertMode");
    pvocpr1->linkControl("PvOscBank/pob/mrs_natural/Interpolation",
                         "mrs_natural/Interpolation");
    pvocpr1->linkControl("PvOscBank/pob/mrs_real/PitchShift",
                         "mrs_real/PitchShift");

    pvocpr1->linkControl("PvOscBank/pob/mrs_bool/phaselock",
                         "mrs_bool/phaselock");

    pvocpr1->linkControl("PvConvert/conv/mrs_realvec/phases",
                         "PvOscBank/pob/mrs_realvec/analysisphases");




    registerPrototype("PhaseVocoderOscBank", pvocpr1);
  }
  break;

  // Variables are shared, so instantiate together
  case PITCHSACF:
  case PITCHPRAAT:
  {
    //--------------------------------------------------------------------------------
    // prototype for pitch Extraction using SACF
    //--------------------------------------------------------------------------------
    MarSystem* pitchSACF = new Series("pitchSACF");
    //pitchSACF->addMarSystem(create("Windowing", "wi"));
    pitchSACF->addMarSystem(create("AutoCorrelation", "acr"));
    pitchSACF->updControl("AutoCorrelation/acr/mrs_real/magcompress", .67);
    // pitchSACF->updControl("AutoCorrelation/acr/mrs_natural/normalize", 1);
    pitchSACF->addMarSystem(create("HalfWaveRectifier", "hwr"));
    MarSystem* fanout = create("Fanout", "fanout");
    fanout->addMarSystem(create("Gain", "id1"));
    fanout->addMarSystem(create("TimeStretch", "tsc"));
    pitchSACF->addMarSystem(fanout);
    MarSystem* fanin = create("Fanin", "fanin");
    fanin->addMarSystem(create("Gain", "id2"));
    fanin->addMarSystem(create("Negative", "nid"));
    pitchSACF->addMarSystem(fanin);
    pitchSACF->addMarSystem(create("HalfWaveRectifier", "hwr"));
    pitchSACF->addMarSystem(create("Peaker", "pkr"));
    pitchSACF->addMarSystem(create("MaxArgMax", "mxr"));
    // should be adapted to the sampling frequency !!
    pitchSACF->updControl("mrs_natural/inSamples", 512);
    pitchSACF->updControl("Fanout/fanout/TimeStretch/tsc/mrs_real/factor", 0.5);
    // pitchSACF->updControl("Windowing/wi/mrs_string/type", "Hanning");
    pitchSACF->updControl("Peaker/pkr/mrs_real/peakSpacing", 0.00);
    // pitchSACF->updControl("Peaker/pkr/mrs_natural/interpolation", 1);
    pitchSACF->updControl("Peaker/pkr/mrs_real/peakStrength", 0.4);
    pitchSACF->updControl("MaxArgMax/mxr/mrs_natural/nMaximums", 1);
    // pitchSACF->updControl("MaxArgMax/mxr/mrs_natural/interpolation", 1);
    pitchSACF->linkControl("mrs_natural/lowSamples", "Peaker/pkr/mrs_natural/peakStart");
    pitchSACF->linkControl("mrs_natural/highSamples", "Peaker/pkr/mrs_natural/peakEnd");
    // set default values
    mrs_real lowPitch = 69;
    mrs_real highPitch = 93;
    mrs_real lowFreq = pitch2hertz(lowPitch);
    mrs_real highFreq = pitch2hertz(highPitch);
    mrs_natural lowSamples =
      hertz2samples(highFreq, pitchSACF->getctrl("mrs_real/osrate")->to<mrs_real>());
    mrs_natural highSamples =
      hertz2samples(lowFreq, pitchSACF->getctrl("mrs_real/osrate")->to<mrs_real>());
    pitchSACF->updControl("mrs_natural/lowSamples", lowSamples);
    pitchSACF->updControl("mrs_natural/highSamples", highSamples);
    registerPrototype("PitchSACF", pitchSACF);

    //--------------------------------------------------------------------------------
    // prototype for pitch Extraction using Praat-Like implementation
    // see details and discussion in
    // http://www.fon.hum.uva.nl/paul/papers/Proceedings_1993.pdf
    //--------------------------------------------------------------------------------
    MarSystem* pitchPraat = new Series("pitchPraat");
    pitchPraat->addMarSystem(create("Windowing", "wi"));
    pitchPraat->addMarSystem(create("AutoCorrelation", "acr"));
    pitchPraat->updControl("AutoCorrelation/acr/mrs_natural/normalize", 1);
    pitchPraat->updControl("AutoCorrelation/acr/mrs_real/octaveCost", 0.0);
    pitchPraat->updControl("AutoCorrelation/acr/mrs_real/voicingThreshold", 0.3);
    pitchPraat->linkControl("mrs_real/voicingThreshold", "AutoCorrelation/acr/mrs_real/voicingThreshold");
    pitchPraat->addMarSystem(create("Peaker", "pkr"));
    pitchPraat->addMarSystem(create("MaxArgMax", "mxr"));
    // should be adapted to the sampling frequency !!
    // The window should be just long
    //  enough to contain three periods (for pitch detection)
    //  of MinimumPitch. E.g. if MinimumPitch is 75 Hz, the window length
    //  is 40 ms  and padded with zeros to reach a power of two.
    pitchPraat->updControl("mrs_natural/inSamples", 1024);
    pitchPraat->updControl("Windowing/wi/mrs_string/type", "Hanning");
    pitchPraat->updControl("Peaker/pkr/mrs_real/peakSpacing", 0.00);
    pitchPraat->updControl("Peaker/pkr/mrs_natural/interpolation", 1);
    // pitchPraat->updControl("Peaker/pkr/mrs_real/peakStrength", 0.4);
    pitchPraat->updControl("MaxArgMax/mxr/mrs_natural/nMaximums", 1);
    pitchPraat->updControl("MaxArgMax/mxr/mrs_natural/interpolation", 1);
    pitchPraat->linkControl("mrs_natural/lowSamples", "Peaker/pkr/mrs_natural/peakStart");
    pitchPraat->linkControl("mrs_natural/highSamples", "Peaker/pkr/mrs_natural/peakEnd");
    // set default values
    lowPitch = 36;
    highPitch = 79;
    lowFreq = pitch2hertz(lowPitch);
    highFreq = pitch2hertz(highPitch);

    lowSamples =
      hertz2samples(highFreq, pitchPraat->getctrl("mrs_real/osrate")->to<mrs_real>());
    highSamples =
      hertz2samples(lowFreq, pitchPraat->getctrl("mrs_real/osrate")->to<mrs_real>());
    pitchPraat->updControl("mrs_natural/lowSamples", lowSamples);
    pitchPraat->updControl("mrs_natural/highSamples", highSamples);
    registerPrototype("PitchPraat", pitchPraat);
  }
  break;

  case PIPE_BLOCK:
  {
    MarSystem* pipeBlock = new Series("Pipe_Block");

    pipeBlock->addMarSystem(create("AFB_Block_A", "blockA"));

    pipeBlock->addMarSystem(create("AFB_Block_B", "blockB"));
    pipeBlock->linkControl("mrs_natural/factor", "AFB_Block_B/blockB/mrs_natural/factor");

    pipeBlock->addMarSystem(create("AFB_Block_C", "blockC"));

    registerPrototype("Pipe_Block", pipeBlock);
  }
  break;

  case AFB_BLOCK_A:
  {
    MarSystem* afbBlockA = new Series("AFB_Block_A");

    afbBlockA->addMarSystem(create("Decimating_QMF", "qmf1"));

    MarSystem* parallel = create("Parallel", "parallel");

    parallel->addMarSystem(create("DownSampler", "ds"));
    parallel->updControl("DownSampler/ds/mrs_natural/factor", 2);

    MarSystem* ser = new Series("ser");

    ser->addMarSystem(create("Decimating_QMF", "qmf2"));

    ser->addMarSystem(create("Selector", "selector"));

    parallel->addMarSystem(ser);

    afbBlockA->addMarSystem(parallel);

    afbBlockA->updControl("Parallel/parallel/Series/ser/Selector/selector/mrs_natural/enable", 0); // Arian: this one is to be fed to the next pipeline
    afbBlockA->updControl("Parallel/parallel/Series/ser/Selector/selector/mrs_natural/disable", 1);

    afbBlockA->addMarSystem(create("Sum", "sum"));


    registerPrototype("AFB_Block_A", afbBlockA);
  }
  break;

  case AFB_BLOCK_B:
  {
    MarSystem* afbBlockB = new Series("AFB_Block_B");

    afbBlockB->addMarSystem(create("Square", "sq"));
    afbBlockB->addMarSystem(create("Biquad", "bq"));

    afbBlockB->updControl("Biquad/bq/mrs_string/type", "lowpass");
    afbBlockB->updControl("Biquad/bq/mrs_real/resonance", 1.0);
    afbBlockB->updControl("Biquad/bq/mrs_real/frequency", 10.0);

    afbBlockB->addMarSystem(create("DownSampler", "ds"));

    afbBlockB->linkControl("mrs_natural/factor", "DownSampler/ds/mrs_natural/factor");

    registerPrototype("AFB_Block_B", afbBlockB);
  }
  break;

  case AFB_BLOCK_C:
  {
    MarSystem* afbBlockC = new Series("AFB_Block_C");

    MarSystem* fanout = create("Fanout", "fanout");


    MarSystem* ser = create("Series", "ser");

    ser->addMarSystem(create("Compressor", "comp"));
    ser->addMarSystem(create("Differentiator", "diff"));
    ser->addMarSystem(create("HalfWaveRectifier", "rect"));
    ser->addMarSystem(create("Gain", "g2"));

    fanout->addMarSystem(create("Gain", "g1"));
    fanout->addMarSystem(ser);

    afbBlockC->addMarSystem(fanout);

    afbBlockC->updControl("Fanout/fanout/Series/ser/Gain/g2/mrs_real/gain", 0.8);
    afbBlockC->updControl("Fanout/fanout/Gain/g1/mrs_real/gain", 0.2);

    afbBlockC->addMarSystem(create("Sum", "sum"));

    afbBlockC->updControl("Sum/sum/mrs_real/weight", 1.0);

    registerPrototype("AFB_Block_C", afbBlockC);
  }
  break;

  case DECIMATING_QMF:
  {
    MarSystem* decimating_QMF = new Series("Decimating_QMF");

    decimating_QMF->addMarSystem(create("Gain", "g"));

    decimating_QMF->updControl("Gain/g/mrs_real/gain", 0.5);

    MarSystem* fanout0 = create("Fanout", "fanout0");

    MarSystem* ser0 = create("Series", "ser0");
    MarSystem* ser1 = create("Series", "ser1");

    ser0->addMarSystem(create("DownSampler", "ds0"));
    ser0->updControl("DownSampler/ds0/mrs_natural/factor", 2);

    ser0->addMarSystem(create("Biquad", "a0"));
    ser0->updControl("Biquad/a0/mrs_string/type", "allpass");
    ser0->updControl("Biquad/a0/mrs_real/frequency", 3000.0);
    ser0->updControl("Biquad/a0/mrs_real/resonance", 1.0);

    ser1->addMarSystem(create("Delay", "d"));
    ser1->updControl("Delay/d/mrs_natural/delaySamples", 1);
    ser1->updControl("Delay/d/mrs_real/feedback", 0.0); // not implemented though
    ser1->updControl("Delay/d/mrs_real/gain", 1.0); // not implemented though

    ser1->addMarSystem(create("DownSampler", "ds1"));
    ser1->updControl("DownSampler/ds1/mrs_natural/factor", 2);

    ser1->addMarSystem(create("Biquad", "a1"));
    ser1->updControl("Biquad/a1/mrs_string/type", "allpass");
    ser1->updControl("Biquad/a1/mrs_real/frequency", 500.0);
    ser1->updControl("Biquad/a1/mrs_real/resonance", 1.0);

    fanout0->addMarSystem(ser0);
    fanout0->addMarSystem(ser1);

    decimating_QMF->addMarSystem(fanout0);

    MarSystem* fanout1 = create("Fanout", "fanout1");

    fanout1->addMarSystem(create("Sum", "sum"));
    fanout1->addMarSystem(create("Subtract", "sub"));

    decimating_QMF->addMarSystem(fanout1);

    registerPrototype("Decimating_QMF", decimating_QMF);
  }
  break;


  case PEAKANALYSE:
  {
    //--------------------------------------------------------------------------------
    // prototype for Peak Extraction stuff
    //--------------------------------------------------------------------------------
    MarSystem* peAnalysePr = new Series("PeakAnalysePr");
    peAnalysePr->addMarSystem(create("ShiftInput", "si"));
    peAnalysePr->addMarSystem(create("Shifter", "sh"));
    peAnalysePr->addMarSystem(create("Windowing", "wi"));
    MarSystem *parallel = create("Parallel", "par");
    parallel->addMarSystem(create("Spectrum", "spk1"));
    parallel->addMarSystem(create("Spectrum", "spk2"));
    peAnalysePr->addMarSystem(parallel);
    peAnalysePr->addMarSystem(create("PeakConvert", "conv"));
    peAnalysePr->linkControl("mrs_natural/winSize",
                             "ShiftInput/si/mrs_natural/winSize");
    peAnalysePr->linkControl("mrs_natural/FFTSize",
                             "Windowing/wi/mrs_natural/size");
    peAnalysePr->linkControl("mrs_string/WindowType",
                             "Windowing/wi/mrs_string/type");
    peAnalysePr->linkControl("mrs_bool/zeroPhasing",
                             "Windowing/wi/mrs_bool/zeroPhasing");
    peAnalysePr->linkControl("mrs_natural/frameMaxNumPeaks",
                             "PeakConvert/conv/mrs_natural/frameMaxNumPeaks");
    //peAnalysePr->linkControl("mrs_natural/Decimation",
    //	"PeakConvert/conv/mrs_natural/Decimation");
    peAnalysePr->updControl("Shifter/sh/mrs_natural/shift", 1);
    registerPrototype("PeakAnalyse", peAnalysePr);
  }
  break;

  case WHASPNET:
  {
    //--------------------------------------------------------------------------------
    // prototype for WHaSp calculation
    //--------------------------------------------------------------------------------
    MarSystem* WHaSpnetpr = new Series("WHaSpnetpr");
    WHaSpnetpr->addMarSystem(create("PeakAnalyse", "analyse"));
    WHaSpnetpr->addMarSystem(create("WHaSp", "whasp"));
    //
    WHaSpnetpr->linkControl("WHaSp/whasp/mrs_natural/totalNumPeaks",
                            "PeakAnalyse/analyse/PeakConvert/conv/mrs_natural/totalNumPeaks");
    WHaSpnetpr->linkControl("WHaSp/whasp/mrs_natural/frameMaxNumPeaks",
                            "PeakAnalyse/analyse/PeakConvert/conv/mrs_natural/frameMaxNumPeaks");
    //
    WHaSpnetpr->linkControl("mrs_natural/frameMaxNumPeaks",
                            "PeakAnalyse/analyse/mrs_natural/frameMaxNumPeaks");
    WHaSpnetpr->updControl("mrs_natural/frameMaxNumPeaks", 20);
    //
    registerPrototype("WHaSpnet", WHaSpnetpr);
  }
  break;

  case STEREOFEATURES2:
  {
    //--------------------------------------------------------------------------------
    // prototype for Stereo Features
    //--------------------------------------------------------------------------------
    MarSystem* stereoFeats2 = new Fanout("StereoFeatures2pr");
    stereoFeats2->addMarSystem(create("StereoSpectrumFeatures","stereospkfeats"));
    stereoFeats2->addMarSystem(create("StereoSpectrumSources","stereospksources"));
    registerPrototype("StereoFeatures2", stereoFeats2);
  }
  break;

  case CLASSIFIER:
  {
    //--------------------------------------------------------------------------------
    // prototype for Classifier
    //--------------------------------------------------------------------------------
    MarSystem* classifierpr = create("Fanout", "Classifierpr");
    classifierpr->addMarSystem(create("ZeroRClassifier", "zerorcl"));
    classifierpr->addMarSystem(create("GaussianClassifier", "gaussiancl"));
    classifierpr->addMarSystem(create("SVMClassifier", "svmcl"));

    // Direct way with creating control
    classifierpr->addctrl("mrs_natural/nClasses", 1);
    classifierpr->addctrl("mrs_string/mode", "train");
    classifierpr->setctrlState("mrs_string/mode", true);
    classifierpr->addctrl("mrs_realvec/classProbabilities", realvec());
    // classifierpr->setctrlState("mrs_realvec/classProbabilities",true);

    classifierpr->linkControl("ZeroRClassifier/zerorcl/mrs_natural/nClasses",
                              "mrs_natural/nClasses");
    classifierpr->linkControl("GaussianClassifier/gaussiancl/mrs_natural/nClasses",
                              "mrs_natural/nClasses");
    classifierpr->linkControl("SVMClassifier/svmcl/mrs_natural/nClasses",
                              "mrs_natural/nClasses");

    classifierpr->linkControl("ZeroRClassifier/zerorcl/mrs_string/mode",
                              "mrs_string/mode");
    classifierpr->linkControl("GaussianClassifier/gaussiancl/mrs_string/mode",
                              "mrs_string/mode");
    classifierpr->linkControl("SVMClassifier/svmcl/mrs_string/mode",
                              "mrs_string/mode");

    classifierpr->linkControl("ZeroRClassifier/zerorcl/mrs_realvec/classProbabilities",
                              "mrs_realvec/classProbabilities");
    classifierpr->linkControl("GaussianClassifier/gaussiancl/mrs_realvec/classProbabilities",
                              "mrs_realvec/classProbabilities");
    classifierpr->linkControl("SVMClassifier/svmcl/mrs_realvec/classProbabilities",
                              "mrs_realvec/classProbabilities");

    classifierpr->updControl("mrs_string/disableChild", "all");
    registerPrototype("Classifier", classifierpr);
  }
  break;

  }
  // Future calls should not re-instantiate the prototype
  compositesMap_[prototype] = STUB;
}

MarSystemManager::~MarSystemManager()
{
  map<mrs_string, MarSystem *>::const_iterator iter;

  for (iter=registry_.begin(); iter != registry_.end(); ++iter)
  {
    delete iter->second;
  }
  registry_.clear();
}

void
MarSystemManager::registerPrototype(mrs_string type, MarSystem *marsystem)
{
  //change type_ of composite to the user specified one
  marsystem->setType(type);
  // check and dispose old prototype
  std::map<std::string,MarSystem*>::iterator iter = registry_.find(type);
  if (iter != registry_.end())
  {
    MarSystem* m = iter->second;
    delete m;
  }
  //and register it
  registry_[type] = marsystem;
}


MarSystem*
MarSystemManager::getPrototype(mrs_string type)
{
  if (registry_.find(type) != registry_.end())
    return (MarSystem *)(registry_[type])->clone();
  else
  {
    MRSWARN("MarSystemManager::getPrototype: No prototype found for " + type);
    return NULL;
  }
}

MarSystem*
MarSystemManager::create(mrs_string type, mrs_string name)
{

  registerComposite(type);

  if (registry_.find(type) != registry_.end())
  {
    MarSystem* m = (MarSystem *)(registry_[type])->clone();
    m->setName(name);
    return m;
  }
  else
  {
    MRSWARN("MarSystemManager::create: No prototype found for " + type);
    return NULL;
  }
}

MarSystem*
MarSystemManager::create(std::string marsystemname)
{
  mrs_string::size_type loc = marsystemname.rfind("/", marsystemname.length()-1);
  if (loc != mrs_string::npos)
    return create(marsystemname.substr(0,loc),
                  marsystemname.substr(loc +1));
  else
    return create(marsystemname, marsystemname + "_unknown");
}


MarSystem*
MarSystemManager::loadFromFile(std::string filename)
{
  ifstream ifs(filename.c_str());
  MarSystem* net;
  net = getMarSystem(ifs);
  return net;
}



MarSystem*
MarSystemManager::getMarSystem(istream& is, MarSystem *parent)
{
  /* Create a MarSystem object from an input stream in .mpl format
   * ( this is the format created by MarSystem::put(ostream& o) )
   */
  mrs_string skipstr;
  mrs_string mcomposite;
  mrs_natural i;
  bool isComposite;
  mrs_string marSystem = "MarSystem";
  mrs_string marSystemComposite = "MarSystemComposite";

  /* first line looks like:
   * # marSystem(Composite)
   */
  is >> skipstr;
  is >> mcomposite;
  if (mcomposite == marSystem)
    isComposite = false;
  else if (mcomposite == marSystemComposite)
    isComposite = true;
  else
  {
    MRSERR("Unknown MarSystemType" << mcomposite);
    MRSERR("skipstr = " << skipstr);
    return 0;
  }

  /* next line looks like:
   * # Type = MarSystemSubclass
   */
  is >> skipstr >> skipstr >> skipstr;
  mrs_string mtype;
  is >> mtype;


  /* next line looks like:
   * # Name = mname
   */
  is >> skipstr >> skipstr >> skipstr;
  mrs_string mname;
  is >> mname;

  MarSystem* msys = getPrototype(mtype);

  if (msys == 0)
  {
    if (compositesMap_.find(mtype) == compositesMap_.end())
    {
      MRSERR("MarSystem::getMarSystem - MarSystem " << mtype << " is not yet part of Marsyas");
      return 0;
    }
    else
    {
      // lazy composite registration
      registerComposite(mtype);
      msys = getPrototype(mtype);
    }
  }

  msys->setName(mname);
  msys->setParent(parent);

  //delete all children MarSystems in a (prototype) Composite
  //and read and link (as possible) local controls
  is >> *msys;

  msys->update();

  workingSet_[msys->getName()] = msys; // add to workingSet

  //recreate the Composite destroyed above, relinking all
  //linked controls in its way
  if (isComposite)
  {
    /* If this is a composite system, we may have subcomponents
     * the number of subcomponents will be listed like this:
     * (blank line)
     * # nComponents = 3
     * (blank line)
     * Here, we read in the nComponents, then instantiate each component
     */
    is >> skipstr >> skipstr >> skipstr;
    mrs_natural nComponents;
    is >> nComponents;
    for (i=0; i < nComponents; ++i)
    {
      MarSystem* cmsys = getMarSystem(is, msys);
      if (cmsys == 0)
        return 0;
      msys->addMarSystem(cmsys);
    }
    msys->update();
  }
  return msys;
}

//
// Added by Adam Parkin, Jul 2005, invoked by MslModel
//
// Returns a list of all registered prototypes
//
vector <mrs_string> MarSystemManager::registeredPrototypes()
{
  vector <mrs_string> retVal;

  map<mrs_string, MarSystem *>::const_iterator iter;

  for (iter=registry_.begin(); iter != registry_.end(); ++iter)
  {
    retVal.push_back (iter->first);
  }

  return retVal;
}

// Added by Stuart Bray Dec 2004. invoked by MslModel
map<mrs_string, MarSystem*> MarSystemManager::getWorkingSet(istream& is)
{
  getMarSystem(is);
  return workingSet_;
}

//
// Added by Adam Parkin Jul 2005, invoked by MslModel
//
// Takes as an argument a string, if the string corresponds
// to a registered MarSystem, the function returns true,
// otherwise returns false
//
bool MarSystemManager::isRegistered (mrs_string name)
{
  return (registry_.find(name) != registry_.end());
}
