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

#include "MarSystemManager.h"
#include "MarSystem.h"
#include "HalfWaveRectifier.h"
#include "common.h"
#include "Series.h"
#include "Cascade.h"
#include "Parallel.h"
#include "Fanin.h"
#include "Fanout.h"
#include "TimeStretch.h"
#include "PatchMatrix.h"
#include "Peaker.h"
#include "PeakerAdaptive.h"
#include "MaxArgMax.h"
#include "MinArgMin.h"
#include "AutoCorrelation.h"
#include "Spectrum.h"
#include "InvSpectrum.h"
#include "Negative.h"
#include "PvConvert.h"
#include "PvFold.h"
#include "PvOscBank.h"
#include "ShiftInput.h"
#include "ShiftOutput.h"
#include "Shifter.h"
#include "PvUnconvert.h"
#include "PvOverlapadd.h"
#include "PvConvolve.h"
#include "AuFileSource.h"
#include "WavFileSource.h"
#include "WavFileSource2.h"
#include "SineSource.h"
#include "NoiseSource.h"
#include "AudioSinkBlocking.h"
#include "AudioSink.h"
#include "Mono2Stereo.h"
#include "PeakConvert.h"
#include "PeakConvert2.h"
#include "OverlapAdd.h"
#include "ClassificationReport.h"
#include "PeakRatio.h"
#include "PeakSynthOsc.h"
#include "PeakSynthOscBank.h"
#include "PeakSynthFFT.h"
#include "PeakResidual.h"
#include "RealvecSource.h"
#include "RealvecSink.h"
#include "Power.h"
#include "Cartesian2Polar.h"
#include "Polar2Cartesian.h"
#include "Windowing.h"
#include "AuFileSink.h"
#include "WavFileSink.h"
#include "PowerSpectrum.h"
#include "Centroid.h"
#include "Rolloff.h"
#include "Flux.h"
#include "ZeroCrossings.h"
#include "Memory.h"
#include "Mean.h"
#include "DTW.h"
#include "StandardDeviation.h"
#include "PlotSink.h"
#include "GaussianClassifier.h"
#include "GMMClassifier.h"
#include "SoundFileSource.h"
#include "SoundFileSource2.h"
#include "SoundFileSink.h"
#include "MP3FileSink.h"
#include "Confidence.h"
#include "Rms.h"
#include "Peak2Rms.h"
#include "WekaSink.h"
#include "WekaSource.h"
#include "MFCC.h"
#include "SCF.h"
#include "SFM.h"
#include "Accumulator.h"
#include "Shredder.h"
#include "WaveletPyramid.h"
#include "WaveletBands.h"
#include "FullWaveRectifier.h"
#include "OnePole.h"
#include "Norm.h"
#include "Sum.h"
#include "Product.h"
#include "Reciprocal.h"
#include "AccentFilterBank.h"
#include "ConstQFiltering.h"
#include "Compressor.h"
#include "Differentiator.h"
#include "Delta.h"
#include "Square.h"
#include "Subtract.h"
#include "Median.h"
#include "MedianFilter.h"
#include "AubioYin.h"
#include "Yin.h"
#include "DownSampler.h"
#include "PeakPeriods2BPM.h"
#include "BeatHistogramFromPeaks.h"
#include "BeatHistogram.h"
#include "BeatHistoFeatures.h"
#include "BeatPhase.h"
#include "FM.h"
#include "Annotator.h"
#include "ZeroRClassifier.h"
#include "KNNClassifier.h"
#include "Kurtosis.h"
#include "Skewness.h"
#include "ViconFileSource.h"
#include "AudioSource.h"
#include "ClassOutputSink.h"
#include "Filter.h"
#include "Biquad.h"
#include "ERB.h"
#include "LyonPassiveEar.h"
#include "Clip.h"
#include "HarmonicEnhancer.h"
#include "Reassign.h"
#include "SilenceRemove.h"
#include "NormMaxMin.h"
#include "Normalize.h"
#include "SMO.h"
#include "Plucked.h"
#include "Delay.h"
#include "LPC.h"
#include "LPCC.h"
#include "LSP.h"
#include "SOM.h"
#include "FlowCutSource.h"
#include "MidiInput.h"
#include "MidiOutput.h"
#include "BICchangeDetector.h"
#include "SpectralSNR.h"
#include "StereoSpectrum.h"
#include "StereoSpectrumFeatures.h"
#include "Vibrato.h"
#include "Panorama.h"
#include "FlowThru.h"
#include "FanOutIn.h"
#include "CompExp.h"
#include "MarSystemTemplateMedium.h"
#include "PeakFeatureSelect.h"
#include "SimilarityMatrix.h"
#include "SelfSimilarityMatrix.h"
#include "Metric.h"
#include "HWPS.h"
#include "RBF.h"
#include "NormMatrix.h"
#include "WHaSp.h"
#include "PeakLabeler.h"
#include "PeakClusterSelect.h"
#include "PeakViewSink.h"
#include "NormCut.h"
#include "PeakViewSource.h"
#include "OneRClassifier.h"
#include "WekaData.h"
#include "PhiSEMSource.h"
#include "PhiSEMFilter.h"
#include "SVMClassifier.h"
#include "Chroma.h"
#include "Spectrum2Chroma.h"
#include "Spectrum2Mel.h"
#include "ADRess.h"
#include "ADRessSpectrum.h"
#include "ADRessStereoSpectrum.h"
#include "EnhADRess.h"
#include "StereoSpectrumSources.h"
#include "EnhADRessStereoSpectrum.h"
#include "McAulayQuatieri.h"
#include "PeakerOnset.h"
#include "ADSR.h"
#include "Reverse.h"
#include "Deinterleave.h"
#include "DeInterleaveSizecontrol.h"
#include "SNR.h"
#include "PCA.h"
#include "AbsMax.h"
#include "MaxMin.h"
#include "TimelineLabeler.h"
#include "RadioDrumInput.h"
#include "NoiseGate.h"
#include "ResampleBezier.h"
#include "ResampleLinear.h"
#include "Resample.h"
#include "ResampleSinc.h"
#include "ResampleNearestNeighbour.h"
#include "StretchLinear.h"
#include "MidiFileSynthSource.h"
#include "PvMultiResolution.h"
#include "Gain.h"
#include "Selector.h"
#include "SpectralTransformations.h"
#include "RunningStatistics.h"
#include "SliceDelta.h"
#include "DeltaFirstOrderRegression.h"
#include "ArffFileSink.h"
#include "MixToMono.h"
#include "SoundFileSourceHopper.h"
#include "TempoHypotheses.h"
#include "OnsetTimes.h"
#include "BeatAgent.h"
#include "BeatReferee.h"
#include "PhaseLock.h"
#include "BeatTimesSink.h"
#include "CrossCorrelation.h"
#include "SliceShuffle.h"
#include "RunningAutocorrelation.h"
#include "SubtractMean.h"
#include "AutoCorrelationFFT.h"
#include "PeakEnhancer.h"
#include "Transposer.h"
#include "SimulMaskingFft.h"
#include "AimPZFC.h"
#include "AimPZFC2.h"
#include "AimGammatone.h"
#include "AimHCL.h"
#include "AimHCL2.h" 
#include "AimLocalMax.h"
#include "AimSAI.h"
#include "AimSSI.h"
#include "AimBoxes.h"
#include "AimVQ.h"
#include "Unfold.h"
#include "TimeFreqPeakConnectivity.h"
#include "Combinator.h"
#include "ParallelMatrixWeight.h"
#include "PeakDistanceHorizontality.h"
#include "PeakViewMerge.h"
#include "FMeasure.h"
#include "DelaySamples.h"
#include "OrcaSnip.h"
#include "AMDF.h"
#include "Inject.h"
#include "PowerToAverageRatio.h"
#include "MeanAbsoluteDeviation.h"
#include "NormalizeAbs.h"
#include "RemoveObservations.h"
#include "SpectralFlatnessAllBands.h"
#include "MathPower.h"
#include "PitchDiff.h"
#include "CARFAC.h"
// #include "BinauralCARFAC.h"
#include "HarmonicStrength.h"
#include "CsvFileSource.h"
#include "Krumhansl_key_finder.h"
#include "PWMSource.h"
#include "DCSource.h"
//modifyHeader


using std::ostringstream;
using std::map;
using std::istream;
using std::ostream;
using std::vector;

using namespace Marsyas;

MarSystemManager::MarSystemManager()
{
	registerPrototype("SoundFileSource", new SoundFileSource("sfp"));
	registerPrototype("SoundFileSource2", new SoundFileSource2("sf2p"));
	registerPrototype("HalfWaveRectifier", new HalfWaveRectifier("hwrp"));
	registerPrototype("AutoCorrelation", new AutoCorrelation("acrp"));
	registerPrototype("Series", new Series("srp"));
	registerPrototype("Fanin", new Fanin("finp"));
	registerPrototype("Fanout", new Fanout("fonp"));
	registerPrototype("Parallel", new Parallel("parallelp"));
	registerPrototype("Cascade", new Cascade("cascadep"));
	registerPrototype("TimeStretch", new TimeStretch("tscp"));
	registerPrototype("Peaker", new Peaker("pkrp"));
	registerPrototype("PatchMatrix", new PatchMatrix("patmatpr"));
	registerPrototype("PeakerAdaptive", new PeakerAdaptive("pkr1pr"));
	registerPrototype("MaxArgMax", new MaxArgMax("mxrp"));
	registerPrototype("MinArgMin", new MinArgMin("mnrp"));
	registerPrototype("Spectrum", new Spectrum("spkp"));
	registerPrototype("InvSpectrum", new InvSpectrum("ispkp"));
	registerPrototype("Negative", new Negative("negp"));
	registerPrototype("PvConvert", new PvConvert("pvconvp"));
	registerPrototype("PvUnconvert", new PvUnconvert("pvuconvp"));
	registerPrototype("PvFold", new PvFold("pvfp"));
	registerPrototype("PvOverlapadd", new PvOverlapadd("pvovlfp"));
	registerPrototype("PvOscBank", new PvOscBank("pvoscp"));
	registerPrototype("PeakRatio", new PeakRatio("perap"));
	registerPrototype("PeakSynthOscBank", new PeakSynthOscBank("pvoscp"));
	registerPrototype("PeakSynthFFT", new PeakSynthFFT("pvfft"));
	registerPrototype("ShiftInput", new ShiftInput("sip"));
	registerPrototype("ShiftOutput", new ShiftOutput("sop"));
	registerPrototype("Shifter", new Shifter("sp"));
	registerPrototype("PvConvolve", new PvConvolve("pvconvpr"));
	registerPrototype("PeakConvert", new PeakConvert("peconvp"));
	registerPrototype("PeakConvert2", new PeakConvert2("peconvp2"));
	registerPrototype("OverlapAdd", new OverlapAdd("oa"));
	registerPrototype("PeakSynthOsc", new PeakSynthOsc("pso"));
	registerPrototype("PeakResidual", new PeakResidual("peres"));
	registerPrototype("RealvecSource", new RealvecSource("realvecSrc"));
	registerPrototype("RealvecSink", new RealvecSink("realvecSink"));
	registerPrototype("Power", new Power("pow"));
	registerPrototype("Cartesian2Polar", new Cartesian2Polar("c2p"));
	registerPrototype("Polar2Cartesian", new Polar2Cartesian("p2c"));
	registerPrototype("FlowCutSource", new FlowCutSource("fcs"));
	registerPrototype("AuFileSource", new AuFileSource("aufp"));
	registerPrototype("WavFileSource", new WavFileSource("wavfp"));
	registerPrototype("WavFileSource2", new WavFileSource2("wavf2p"));
	registerPrototype("SoundFileSink", new SoundFileSink("sfsp"));
	registerPrototype("MP3FileSink", new MP3FileSink("sfsp"));
	registerPrototype("AudioSink", new AudioSink("audiosinkp"));
	registerPrototype("AuFileSink", new AuFileSink("ausinkp"));
	registerPrototype("WavFileSink", new WavFileSink("wavsinkp"));
	registerPrototype("Mono2Stereo", new Mono2Stereo("mono2stereop"));
	registerPrototype("Windowing", new Windowing("win"));
	registerPrototype("PowerSpectrum", new PowerSpectrum("pspkp"));
	registerPrototype("Centroid", new Centroid("cntrp"));
	registerPrototype("Rolloff", new Rolloff("rlfp"));
	registerPrototype("Flux", new Flux("fluxp"));
	registerPrototype("ZeroCrossings", new ZeroCrossings("zcrsp"));
	registerPrototype("Memory", new Memory("memp"));
	registerPrototype("Mean", new Mean("meanp"));
	registerPrototype("StandardDeviation", new StandardDeviation("stdp"));
	registerPrototype("PlotSink", new PlotSink("plotsp"));
	registerPrototype("GaussianClassifier", new GaussianClassifier("gaussp"));
	registerPrototype("GMMClassifier", new GMMClassifier("gmmsp"));
	registerPrototype("Confidence", new Confidence("confp"));
	registerPrototype("Rms", new Rms("rms"));
	registerPrototype("Peak2Rms", new Peak2Rms("peakrms"));
	registerPrototype("WekaSink", new WekaSink("wsink"));
	registerPrototype("WekaSource", new WekaSource("wsource"));
	registerPrototype("MFCC", new MFCC("mfcc"));
	registerPrototype("SCF", new SCF("scf"));
	registerPrototype("SFM", new SFM("sfm"));
	registerPrototype("Accumulator", new Accumulator("acc"));
	registerPrototype("Shredder", new Shredder("shred"));
	registerPrototype("WaveletPyramid", new WaveletPyramid("wvpyramid"));
	registerPrototype("WaveletBands",   new WaveletBands("wvbands"));
	registerPrototype("FullWaveRectifier", new FullWaveRectifier("fwr"));
	registerPrototype("OnePole", new OnePole("lpf"));
	registerPrototype("Norm", new Norm("norm"));
	registerPrototype("Sum", new Sum("sum"));
	registerPrototype("Product", new Product("product"));
	registerPrototype("Reciprocal", new Reciprocal("reciprocal"));
	registerPrototype("AccentFilterBank", new AccentFilterBank("afb"));
	registerPrototype("ConstQFiltering", new ConstQFiltering("cqfpr"));
	registerPrototype("Compressor", new Compressor("compressor"));
	registerPrototype("Differentiator", new Differentiator("differentiator"));
	registerPrototype("Delta", new Delta("deltapr"));
	registerPrototype("Square", new Square("square"));
	registerPrototype("Subtract", new Subtract("subtract"));
	registerPrototype("Median", new Median("median"));
	registerPrototype("MedianFilter", new MedianFilter("medianfilterpr"));
	registerPrototype("AMDF", new AMDF("amdfpr"));
	registerPrototype("AubioYin", new AubioYin("aubioyin"));
	registerPrototype("Yin", new Yin("yin"));
	registerPrototype("DownSampler", new DownSampler("ds"));
	registerPrototype("PeakPeriods2BPM", new PeakPeriods2BPM("p2bpm"));
	registerPrototype("BeatHistogramFromPeaks", new BeatHistogramFromPeaks("beathistofrompeakspr"));
	registerPrototype("BeatPhase", new BeatPhase("beatphase"));
	registerPrototype("BeatHistogram", new BeatHistogram("beathistopr"));
	registerPrototype("BeatHistoFeatures", new BeatHistoFeatures("bhfp"));
	registerPrototype("SineSource", new SineSource("sinesp"));
	registerPrototype("NoiseSource", new NoiseSource("noisesrcsp"));
	registerPrototype("FM", new FM("fmp"));
	registerPrototype("Annotator", new Annotator("anonp"));
	registerPrototype("ZeroRClassifier", new ZeroRClassifier("zerorp"));
	registerPrototype("KNNClassifier", new KNNClassifier("knnp"));
	registerPrototype("SMO", new SMO("smopr"));
	registerPrototype("Plucked", new Plucked("pluckedpr"));
	registerPrototype("Delay", new Delay("delaypr"));
	registerPrototype("DelaySamples", new DelaySamples("delay"));
	registerPrototype("Kurtosis", new Kurtosis("kurtosisp"));
	registerPrototype("Skewness", new Skewness("Skewnessp"));
	registerPrototype("ViconFileSource", new ViconFileSource("viconfilesourcep"));
	registerPrototype("AudioSource", new AudioSource("audiosourcep"));
	registerPrototype("ClassOutputSink", new ClassOutputSink("classoutputsinkp"));
	registerPrototype("Filter", new Filter("filterp"));
	registerPrototype("Biquad", new Biquad("biquadp"));
	registerPrototype("RadioDrumInput", new RadioDrumInput("radiodrump"));
	registerPrototype("NoiseGate", new NoiseGate("noisegatep"));
	registerPrototype("ERB", new ERB("erbp"));
	registerPrototype("LyonPassiveEar", new LyonPassiveEar("lyonp"));
	registerPrototype("Clip", new Clip("clpr"));
	registerPrototype("HarmonicEnhancer", new HarmonicEnhancer("hepr"));
	registerPrototype("Reassign", new Reassign("reassignpr"));
	registerPrototype("SilenceRemove", new SilenceRemove("silenceremovepr"));
	registerPrototype("NormMaxMin", new NormMaxMin("normmaxminpr"));
	registerPrototype("Normalize", new Normalize("normalizepr"));
	registerPrototype("LPC", new LPC("lpcwarppr"));
	registerPrototype("LPCC", new LPCC("lpccpr"));
	registerPrototype("LSP", new LSP("lsppr"));
	registerPrototype("SOM", new SOM("sompr"));
	registerPrototype("MidiInput", new MidiInput("midiinpr"));
	registerPrototype("MidiOutput", new MidiOutput("midioutpr"));
	registerPrototype("BICchangeDetector", new BICchangeDetector("bicchgdetpr"));
	registerPrototype("SpectralSNR", new SpectralSNR("ssnrpr"));
	registerPrototype("StereoSpectrum", new StereoSpectrum("stereopr"));
	registerPrototype("StereoSpectrumFeatures", new StereoSpectrumFeatures("stereospkfpr"));
	registerPrototype("Vibrato", new Vibrato("vibratopr"));
	registerPrototype("Panorama", new Panorama("panoramapr"));
	registerPrototype("FlowThru", new FlowThru("flowthrupr"));
	registerPrototype("FanOutIn", new FanOutIn("fanoutinpr"));
	registerPrototype("CompExp", new CompExp("compexppr"));
	registerPrototype("MarSystemTemplateMedium", new MarSystemTemplateMedium("marsystemtemplatemediumpr"));
	registerPrototype("PeakFeatureSelect", new PeakFeatureSelect("pefeatselectpr"));
	registerPrototype("SimilarityMatrix", new SimilarityMatrix("similaritymatrixpr"));
	registerPrototype("SelfSimilarityMatrix", new SelfSimilarityMatrix("selfsimilaritymatrixpr"));
	registerPrototype("Metric", new Metric("metricpr"));
	registerPrototype("HWPS", new HWPS("hwpspr"));
	registerPrototype("RBF", new RBF("rbfpr"));
	registerPrototype("NormMatrix", new NormMatrix("normmatrixpr"));
	registerPrototype("WHaSp", new WHaSp("whasppr"));
	registerPrototype("PeakLabeler", new PeakLabeler("pelabelerpr"));
	registerPrototype("PeakClusterSelect", new PeakClusterSelect("peclusterselectpr"));
	registerPrototype("PeakViewSink", new PeakViewSink("peakviewsinkpr"));
	registerPrototype("NormCut", new NormCut("normcutpr"));
	registerPrototype("PeakViewSource", new PeakViewSource("peakviewsourcepr"));
	registerPrototype("OneRClassifier", new OneRClassifier("onerclassifierpr"));
	registerPrototype("ClassificationReport", new ClassificationReport("classificationreportpr"));
	registerPrototype("PhiSEMSource", new PhiSEMSource("phisemsourcepr"));
	registerPrototype("PhiSEMFilter", new PhiSEMFilter("phisemfilterpr"));
	registerPrototype("SVMClassifier", new SVMClassifier("svmclassifierpr"));
	registerPrototype("Chroma", new Chroma("chromapr"));
	registerPrototype("Spectrum2Chroma", new Spectrum2Chroma("spectrum2chromapr"));

	registerPrototype("Spectrum2Mel", new Spectrum2Mel("spectrum2melpr"));
	registerPrototype("ADRess", new ADRess("adresspr"));
	registerPrototype("ADRessSpectrum", new ADRessSpectrum("adressspectrumpr"));
	registerPrototype("ADRessStereoSpectrum", new ADRessStereoSpectrum("adressstereospectrumpr"));
	registerPrototype("EnhADRess", new EnhADRess("Enhadresspr"));
	registerPrototype("StereoSpectrumSources", new StereoSpectrumSources("stereospectrumsourcespr"));
	registerPrototype("EnhADRessStereoSpectrum", new EnhADRessStereoSpectrum("enhadressstereospectrumpr"));
	registerPrototype("PeakerOnset", new PeakerOnset("pkronsetpr"));
	registerPrototype("ADSR", new ADSR("adsrpr"));
	registerPrototype("Reverse", new Reverse("reversepr"));
	registerPrototype("DeInterleaveSizecontrol", new DeInterleaveSizecontrol("deintszctrlpr"));
 	registerPrototype("DTW", new DTW("dtwpr"));
	registerPrototype("Deinterleave", new Deinterleave("deintpr"));
	registerPrototype("SNR", new SNR("snrpr"));
	registerPrototype("PCA", new PCA("pcapr"));
	registerPrototype("AbsMax", new AbsMax("absmaxpr"));
	registerPrototype("MaxMin", new MaxMin("maxminpr"));
	registerPrototype("TimelineLabeler", new TimelineLabeler("timelinelabelerpr"));
	registerPrototype("StretchLinear", new StretchLinear("stretchlinearpr"));
	registerPrototype("ResampleBezier", new ResampleBezier("resampberpr"));
	registerPrototype("ResampleLinear", new ResampleLinear("resamplinpr"));
	registerPrototype("Resample", new Resample("resampabspr"));
	registerPrototype("ResampleSinc", new ResampleSinc("resampsincpr"));
	registerPrototype("ResampleNearestNeighbour", new ResampleNearestNeighbour("resampnnpr"));
	registerPrototype("MidiFileSynthSource", new MidiFileSynthSource("midifilesynthsourcepr"));
	registerPrototype("PvMultiResolution", new PvMultiResolution("PvMultiResolution"));
	registerPrototype("AudioSinkBlocking", new AudioSinkBlocking("AudioSinkBlocking"));


	registerPrototype("Gain", new Gain("gp"));
	registerPrototype("Selector", new Selector("selectorpr"));
	registerPrototype("SpectralTransformations", new SpectralTransformations("spectraltransformationspr"));
	registerPrototype("RunningStatistics", new RunningStatistics("runningstatisticspr"));
	registerPrototype("SliceDelta", new SliceDelta("slicedeltapr"));
	registerPrototype("DeltaFirstOrderRegression", new DeltaFirstOrderRegression("delta"));
	registerPrototype("ArffFileSink", new ArffFileSink("arfffilesinkpr"));
	registerPrototype("MixToMono", new MixToMono("mixtomono"));
	registerPrototype("SoundFileSourceHopper", new SoundFileSourceHopper("soundfilesourcehopper"));
	registerPrototype("TempoHypotheses", new TempoHypotheses("tempohyp"));
	registerPrototype("OnsetTimes", new OnsetTimes("OnsetTimes"));
	registerPrototype("BeatAgent", new BeatAgent("beatagent"));
	registerPrototype("BeatReferee", new BeatReferee("beatreferee"));

	registerPrototype("AutoCorrelationFFT", new AutoCorrelationFFT("autocorfftpr"));
	registerPrototype("PeakEnhancer", new PeakEnhancer("peakenhpr"));
	registerPrototype("PhaseLock", new PhaseLock("phaselock"));
	registerPrototype("BeatTimesSink", new BeatTimesSink("beattimessink"));
	registerPrototype("CrossCorrelation",new CrossCorrelation("crossCorrelationpr"));
	registerPrototype("SliceShuffle", new SliceShuffle("sliceshuffle"));
	registerPrototype("RunningAutocorrelation", new RunningAutocorrelation("runningautocorrelation"));
	registerPrototype("SubtractMean", new SubtractMean("subtractmean"));

	registerPrototype("Transposer", new Transposer("transposer"));
	registerPrototype("SimulMaskingFft", new SimulMaskingFft("simulmask"));
	registerPrototype("TimeFreqPeakConnectivity", new TimeFreqPeakConnectivity("tfpeakconn"));
	registerPrototype("Combinator", new Combinator("combinator"));
	registerPrototype("ParallelMatrixWeight", new ParallelMatrixWeight("parallelmatrixweight"));
	registerPrototype("PeakDistanceHorizontality", new PeakDistanceHorizontality("horizontality"));
	registerPrototype("PeakViewMerge", new PeakViewMerge("PeakViewMerge"));
	registerPrototype("FMeasure", new FMeasure("F-Measure"));
	registerPrototype("OrcaSnip", new OrcaSnip("GetSnipBounds"));


	registerPrototype("AimPZFC", new AimPZFC("aimpzfc"));
	registerPrototype("AimPZFC2", new AimPZFC2("aimpzfc2"));

	registerPrototype("AimGammatone", new AimGammatone("aimgammatone"));
	registerPrototype("AimHCL", new AimHCL("aimhcl"));
	registerPrototype("AimHCL2", new AimHCL2("aimhcl2"));
	registerPrototype("AimLocalMax", new AimLocalMax("aimlocalmax"));
	registerPrototype("AimSAI", new AimSAI("aimsai"));
	registerPrototype("AimSSI", new AimSSI("aimssi"));
	registerPrototype("AimBoxes", new AimBoxes("aimboxes"));
	registerPrototype("AimVQ", new AimVQ("aimvq"));
	registerPrototype("Unfold", new Unfold("unfold"));

	registerPrototype("Inject", new Inject("injectpr"));
	registerPrototype("PowerToAverageRatio", new PowerToAverageRatio("crestpr"));
	registerPrototype("MeanAbsoluteDeviation", new MeanAbsoluteDeviation("meanabsolutedeviationpr"));
	registerPrototype("NormalizeAbs", new NormalizeAbs("normalizeabspr"));
	registerPrototype("RemoveObservations", new RemoveObservations("removeobservationspr"));
	registerPrototype("SpectralFlatnessAllBands", new SpectralFlatnessAllBands("spectralflatnessallbandspr"));
	registerPrototype("MathPower", new MathPower("mathpowerpr"));
	registerPrototype("PitchDiff", new PitchDiff("pitchdiffpr"));
	registerPrototype("CARFAC", new CARFAC("carfac"));
	// registerPrototype("BinauralCARFAC", new BinauralCARFAC("binauralcarfac"));
	registerPrototype("HarmonicStrength", new HarmonicStrength("harmonicstrengthpr"));
	registerPrototype("CsvFileSource", new CsvFileSource("csvfilesourcepr"));
	registerPrototype("Krumhansl_key_finder", new Krumhansl_key_finder("krumhansl_key_finderpr"));
	registerPrototype("PWMSource", new PWMSource("pwmsourcepr"));
	registerPrototype("DCSource", new DCSource("dcsourcepr"));
	//modifyRegister

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
			mrs_real lowPitch = 36;
			mrs_real highPitch = 79;
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
