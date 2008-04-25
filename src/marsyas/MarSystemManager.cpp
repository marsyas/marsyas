/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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
#include "AudioSink.h"
#include "AudioSink2.h"
#include "Mono2Stereo.h"
#include "PeakConvert.h"
#include "OverlapAdd.h"
#include "Summary.h"
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
#include "StandardDeviation.h"
#include "PlotSink.h"
#include "GaussianClassifier.h"
#include "SoundFileSource.h"
#include "SoundFileSource2.h"
#include "SoundFileSink.h" 
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
#include "DownSampler.h"
#include "PeakPeriods2BPM.h"
#include "Histogram.h"
#include "BeatHistoFeatures.h"
#include "FM.h"
#include "Annotator.h" 
#include "ZeroRClassifier.h"
#include "KNNClassifier.h"
#include "Kurtosis.h"
#include "Skewness.h"
#include "ViconFileSource.h"
#include "AudioSource.h"
#include "AudioSource2.h"
#include "ClassOutputSink.h"
#include "Filter.h" 
#include "Biquad.h"
#include "ERB.h"
#include "ClipAudioRange.h"
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
#include "Classifier.h"
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
#include "SNR.h"
#include "PCA.h"
#include "Gain.h"
// please leave Gain at the end; it makes scripts happy.

using namespace std;
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
  registerPrototype("PeakSynthOscBank", new PeakSynthOscBank("pvoscp"));
  registerPrototype("PeakSynthFFT", new PeakSynthFFT("pvfft"));
  registerPrototype("ShiftInput", new ShiftInput("sip"));
  registerPrototype("ShiftOutput", new ShiftOutput("sop"));
  registerPrototype("Shifter", new Shifter("sp"));
  registerPrototype("PvConvolve", new PvConvolve("pvconvpr"));
  registerPrototype("PeakConvert", new PeakConvert("peconvp"));
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
  registerPrototype("AudioSink", new AudioSink("audiosinkp"));
  registerPrototype("AudioSink2", new AudioSink2("audiosink2p"));
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
  registerPrototype("DownSampler", new DownSampler("ds"));
  registerPrototype("PeakPeriods2BPM", new PeakPeriods2BPM("p2bpm"));
  registerPrototype("Histogram", new Histogram("histop"));
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
  registerPrototype("Kurtosis", new Kurtosis("kurtosisp"));
  registerPrototype("Skewness", new Skewness("Skewnessp"));
  registerPrototype("ViconFileSource", new ViconFileSource("viconfilesourcep"));
  registerPrototype("AudioSource", new AudioSource("audiosourcep"));
  registerPrototype("AudioSource2", new AudioSource2("audiosource2p"));
  registerPrototype("ClassOutputSink", new ClassOutputSink("classoutputsinkp"));
  registerPrototype("Filter", new Filter("filterp"));
  registerPrototype("Biquad", new Biquad("biquadp"));
  registerPrototype("ERB", new ERB("erbp"));
  registerPrototype("ClipAudioRange", new ClipAudioRange("clpr"));
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
  registerPrototype("Summary", new Summary("summaryp"));
  registerPrototype("PhiSEMSource", new PhiSEMSource("phisemsourcepr"));
  registerPrototype("PhiSEMFilter", new PhiSEMFilter("phisemfilterpr"));
  registerPrototype("SVMClassifier", new SVMClassifier("svmclassifierpr"));
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
  registerPrototype("SNR", new SNR("snrpr"));
  registerPrototype("PCA", new PCA("pcapr"));
  registerPrototype("Gain", new Gain("gp"));
  // Please leave Gain at the end; it makes scripts happy.

  //***************************************************************************************
  //				Composite MarSystem prototypes
  //***************************************************************************************
  //--------------------------------------------------------------------------------
  // Making a prototype for a specific MidiOutput device 
  //--------------------------------------------------------------------------------
  MarSystem* devibotpr = new MidiOutput("devibotpr");
  devibotpr->linkctrl("mrs_natural/arm", "mrs_natural/byte2");
  devibotpr->linkctrl("mrs_natural/velocity", "mrs_natural/byte3");
  devibotpr->linkctrl("mrs_bool/strike", "mrs_bool/sendMessage");
  devibotpr->updctrl("mrs_natural/byte1", 144);
  registerPrototype("DeviBot", devibotpr);

  //--------------------------------------------------------------------------------
  // Stereo2Mono MarSystem 
  //--------------------------------------------------------------------------------
  MarSystem* stereo2monopr = new Sum("stereo2monopr");
  stereo2monopr->updctrl("mrs_real/weight", 0.5);
  registerPrototype("Stereo2Mono", stereo2monopr);

  //--------------------------------------------------------------------------------
  // texture window analysis composite prototype
  //--------------------------------------------------------------------------------
  MarSystem* textureStatspr = new Series("tstatspr");
  textureStatspr->addMarSystem(new Memory("mempr"));

  MarSystem* meanstdpr = new Fanout("meanstdpr");
  meanstdpr->addMarSystem(new Mean("meanpr"));
  meanstdpr->addMarSystem(new StandardDeviation("stdpr"));
  textureStatspr->addMarSystem(meanstdpr);

  textureStatspr->linkctrl("mrs_natural/memSize", "Memory/mempr/mrs_natural/memSize");
  textureStatspr->linkctrl("mrs_bool/reset", "Memory/mempr/mrs_bool/reset");
  registerPrototype("TextureStats", textureStatspr);

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
  LPCnetpr->updctrl("Filter/preEmph/mrs_realvec/ncoeffs", ncoeffs);
  LPCnetpr->updctrl("Filter/preEmph/mrs_realvec/dcoeffs", dcoeffs);
  LPCnetpr->addMarSystem(create("ShiftInput", "si"));
  LPCnetpr->addMarSystem(create("Windowing", "ham"));
  LPCnetpr->addMarSystem(create("LPC", "lpc"));
  LPCnetpr->linkctrl("mrs_realvec/preEmphFIR","Filter/preEmph/mrs_realvec/ncoeffs");
  LPCnetpr->linkctrl("mrs_natural/winSize","ShiftInput/si/mrs_natural/winSize");
  LPCnetpr->linkctrl("mrs_natural/order","LPC/lpc/mrs_natural/order");
  LPCnetpr->linkctrl("mrs_real/lambda","LPC/lpc/mrs_real/lambda");
  LPCnetpr->linkctrl("mrs_real/gamma","LPC/lpc/mrs_real/gamma");
  registerPrototype("LPCnet", LPCnetpr);

  //--------------------------------------------------------------------------------
  // Power spectrum composite prototype
  //--------------------------------------------------------------------------------
  MarSystem* pspectpr = create("Series", "pspectpr");
  pspectpr->addMarSystem(create("ShiftInput", "si"));
  pspectpr->addMarSystem(create("Windowing", "hamming"));
  pspectpr->addMarSystem(create("Spectrum","spk"));
  pspectpr->updctrl("Spectrum/spk/mrs_real/cutoff", 1.0);
  pspectpr->addMarSystem(create("PowerSpectrum", "pspk"));
  pspectpr->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType","power");
  pspectpr->linkctrl("mrs_real/cutoff","Spectrum/spk/mrs_real/cutoff");
  pspectpr->linkctrl("mrs_natural/winSize","ShiftInput/si/mrs_natural/winSize");  registerPrototype("PowerSpectrumNet", pspectpr);
  
  MarSystem* pspectpr1 = create("Series", "pspectpr1");
  pspectpr1->addMarSystem(create("Spectrum","spk"));
  pspectpr1->updctrl("Spectrum/spk/mrs_real/cutoff", 1.0);
  pspectpr1->addMarSystem(create("PowerSpectrum", "pspk"));
  pspectpr1->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType","power");
  pspectpr1->linkctrl("mrs_real/cutoff","Spectrum/spk/mrs_real/cutoff");
  registerPrototype("PowerSpectrumNet1", pspectpr1);

  // STFT_features prototype 
  MarSystem* stft_features_pr = create("Fanout", "stft_features_pr");
  stft_features_pr->addMarSystem(create("Centroid", "cntrd"));
  stft_features_pr->addMarSystem(create("Rolloff", "rlf"));
  stft_features_pr->addMarSystem(create("Flux", "flux"));
  stft_features_pr->addMarSystem(create("MFCC", "mfcc"));	
  stft_features_pr->addMarSystem(create("SCF", "scf"));
  stft_features_pr->addMarSystem(create("SFM", "sfm"));
  registerPrototype("STFT_features", stft_features_pr);

  // timbre_features prototype 
  MarSystem* timbre_features_pr = new Fanout("timbre_features_pr");

  // time domain branch 
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
  lpcFeatures->updctrl("Filter/preEmph/mrs_realvec/ncoeffs", ncoeffs);
  lpcFeatures->updctrl("Filter/preEmph/mrs_realvec/dcoeffs", dcoeffs);
  lpcFeatures->addMarSystem(create("ShiftInput", "si"));
  lpcFeatures->addMarSystem(create("Windowing", "ham"));
  
  MarSystem* lpcf = create("Fanout", "lpcf");
  
  MarSystem* lspbranch = create("Series", "lspbranch");
  MarSystem* lpccbranch = create("Series","lpccbranch");
  
  lspbranch->addMarSystem(create("LPC", "lpc"));
  lspbranch->updctrl("LPC/lpc/mrs_natural/order", 18);
  lspbranch->addMarSystem(create("LSP", "lsp"));
  
  lpccbranch->addMarSystem(create("LPC", "lpc"));
  lpccbranch->updctrl("LPC/lpc/mrs_natural/order", 12);
  lpccbranch->addMarSystem(create("LPCC", "lpcc"));
  
  
  lpcf->addMarSystem(lspbranch);
  lpcf->addMarSystem(lpccbranch);
  lpcFeatures->addMarSystem(lpcf);

  timbre_features_pr->addMarSystem(lpcFeatures);
  timbre_features_pr->linkctrl("mrs_natural/winSize","Series/timeDomain/ShiftInput/si/mrs_natural/winSize");
  timbre_features_pr->linkctrl("mrs_natural/winSize","Series/spectralShape/ShiftInput/si/mrs_natural/winSize");
  timbre_features_pr->linkctrl("mrs_natural/winSize","Series/lpcFeatures/ShiftInput/si/mrs_natural/winSize");
  	timbre_features_pr->linkctrl("mrs_string/enableSPChild", "Series/spectralShape/STFT_features/spectrumFeatures/mrs_string/enableChild");
  timbre_features_pr->linkctrl("mrs_string/enableTDChild", 
			       "Series/timeDomain/Fanout/tdf/mrs_string/enableChild");
  timbre_features_pr->linkctrl("mrs_string/enableLPCChild", 
							   "Series/lpcFeatures/Fanout/lpcf/mrs_string/enableChild");
  timbre_features_pr->linkctrl("mrs_string/disableSPChild", "Series/spectralShape/STFT_features/spectrumFeatures/mrs_string/disableChild");  
  timbre_features_pr->linkctrl("mrs_string/disableTDChild", "Series/timeDomain/Fanout/tdf/mrs_string/disableChild");
  timbre_features_pr->linkctrl("mrs_string/disableLPCChild", 
							   "Series/lpcFeatures/Fanout/lpcf/mrs_string/disableChild");

    timbre_features_pr->linkctrl("mrs_string/disableBranch", 
				 "mrs_string/disableChild");

    timbre_features_pr->updctrl("mrs_string/disableBranch", "Series/lpcFeatures");
    timbre_features_pr->updctrl("mrs_string/disableSPChild", "all");
    timbre_features_pr->updctrl("mrs_string/disableTDChild", "all");
    timbre_features_pr->updctrl("mrs_string/disableLPCChild", "all");

  registerPrototype("TimbreFeatures", timbre_features_pr);


  // StereoPanningSpectrumFeatures 
  MarSystem *stereoFeatNet = create("Series", "stereoFeatNet");
  MarSystem* stereobranches = create("Parallel", "stereobranches");
  MarSystem* left = create("Series", "left");
  MarSystem* right = create("Series", "right");
  left->addMarSystem(create("Windowing", "hamleft"));
  left->addMarSystem(create("Spectrum", "spkleft"));
  right->addMarSystem(create("Windowing", "hamright"));
  right->addMarSystem(create("Spectrum", "spkright"));
  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);
  
  stereoFeatNet->addMarSystem(stereobranches);
  stereoFeatNet->addMarSystem(create("StereoSpectrum", "sspk"));
  stereoFeatNet->addMarSystem(create("StereoSpectrumFeatures", "sspkf"));
  registerPrototype("StereoPanningSpectrumFeatures", stereoFeatNet);
  


    MarSystem* stereoFeatures = create("Fanout", "stereoFeatures");
    MarSystem* stereoTimbreFeatures = create("Parallel", "stereoTimbreFeatures");
    MarSystem* featExtractorLeft = create("TimbreFeatures", "featExtractorLeft");
    MarSystem* featExtractorRight = create("TimbreFeatures", "featExtractorRight");
    stereoTimbreFeatures->addMarSystem(featExtractorLeft);
    stereoTimbreFeatures->addMarSystem(featExtractorRight);
    stereoFeatures->addMarSystem(stereoTimbreFeatures);
    stereoFeatures->addMarSystem(create("StereoPanningSpectrumFeatures", "SPSFeatures"));
    registerPrototype("StereoFeatures", stereoFeatures);



  //--------------------------------------------------------------------------------
  // Phase Vocoder composite prototype
  //--------------------------------------------------------------------------------
  MarSystem* pvocpr = new Series("pvocpr");
  pvocpr->addMarSystem(new ShiftInput("si"));
  pvocpr->addMarSystem(new PvFold("fo"));
  pvocpr->addMarSystem(new Spectrum("spk"));
  pvocpr->addMarSystem(new PvConvert("conv"));
  pvocpr->addMarSystem(new PvOscBank("ob"));
  pvocpr->addMarSystem(new ShiftOutput("so"));
  pvocpr->addMarSystem(new Gain("gt")); 
  pvocpr->linkctrl("mrs_natural/winSize", 
		   "ShiftInput/si/mrs_natural/winSize");
  pvocpr->linkctrl("mrs_natural/Decimation", 
		   "PvFold/fo/mrs_natural/Decimation");
  pvocpr->linkctrl("mrs_natural/Decimation", 
		   "PvConvert/conv/mrs_natural/Decimation");
  pvocpr->linkctrl("mrs_natural/Sinusoids", 
		   "PvConvert/conv/mrs_natural/Sinusoids");
  pvocpr->linkctrl("mrs_natural/FFTSize", 
		   "PvFold/fo/mrs_natural/FFTSize");
  pvocpr->linkctrl("mrs_natural/Interpolation", 
		   "PvOscBank/ob/mrs_natural/Interpolation");
  pvocpr->linkctrl("mrs_natural/Interpolation", 
		   "ShiftOutput/so/mrs_natural/Interpolation");
  pvocpr->linkctrl("mrs_real/PitchShift", 
		   "PvOscBank/ob/mrs_real/PitchShift");
  pvocpr->linkctrl("mrs_real/gain", 
		   "Gain/gt/mrs_real/gain");
  registerPrototype("PhaseVocoder", pvocpr);


  //--------------------------------------------------------------------------------
  // prototype for pitch Extraction using SACF
  //--------------------------------------------------------------------------------
  MarSystem* pitchSACF = new Series("pitchSACF");
  //pitchSACF->addMarSystem(create("Windowing", "wi"));
  pitchSACF->addMarSystem(create("AutoCorrelation", "acr"));
  pitchSACF->updctrl("AutoCorrelation/acr/mrs_real/magcompress", .67);
  // pitchSACF->updctrl("AutoCorrelation/acr/mrs_natural/normalize", 1);
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
  pitchSACF->updctrl("mrs_natural/inSamples", 512);
  pitchSACF->updctrl("Fanout/fanout/TimeStretch/tsc/mrs_real/factor", 0.5);  
  // pitchSACF->updctrl("Windowing/wi/mrs_string/type", "Hanning");
  pitchSACF->updctrl("Peaker/pkr/mrs_real/peakSpacing", 0.00);
  // pitchSACF->updctrl("Peaker/pkr/mrs_natural/interpolation", 1);
  pitchSACF->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.4);
  pitchSACF->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 1);
  // pitchSACF->updctrl("MaxArgMax/mxr/mrs_natural/interpolation", 1);
  pitchSACF->linkctrl("mrs_natural/lowSamples", "Peaker/pkr/mrs_natural/peakStart");
  pitchSACF->linkctrl("mrs_natural/highSamples", "Peaker/pkr/mrs_natural/peakEnd");
  // set default values
  mrs_real lowPitch = 36;
  mrs_real highPitch = 79;
  mrs_real lowFreq = pitch2hertz(lowPitch);
  mrs_real highFreq = pitch2hertz(highPitch);
  mrs_natural lowSamples = 
    hertz2samples(highFreq, pitchSACF->getctrl("mrs_real/osrate")->to<mrs_real>());
  mrs_natural highSamples = 
    hertz2samples(lowFreq, pitchSACF->getctrl("mrs_real/osrate")->to<mrs_real>());
  pitchSACF->updctrl("mrs_natural/lowSamples", lowSamples);
  pitchSACF->updctrl("mrs_natural/highSamples", highSamples);
  registerPrototype("PitchSACF", pitchSACF);

  //--------------------------------------------------------------------------------
  // prototype for pitch Extraction using Praat-Like implementation
  // see details and discussion in
  // http://www.fon.hum.uva.nl/paul/papers/Proceedings_1993.pdf
  //--------------------------------------------------------------------------------
  MarSystem* pitchPraat = new Series("pitchPraat");
  pitchPraat->addMarSystem(create("Windowing", "wi"));
  pitchPraat->addMarSystem(create("AutoCorrelation", "acr"));
  pitchPraat->updctrl("AutoCorrelation/acr/mrs_natural/normalize", 1);
  pitchPraat->updctrl("AutoCorrelation/acr/mrs_real/octaveCost", 0.01); // 0.01
  pitchPraat->updctrl("AutoCorrelation/acr/mrs_real/voicingThreshold", 0.3);
  pitchPraat->addMarSystem(create("Peaker", "pkr"));
  pitchPraat->addMarSystem(create("MaxArgMax", "mxr"));
  // should be adapted to the sampling frequency !!
  // The window should be just long
  //  enough to contain three periods (for pitch detection) 
  //  of MinimumPitch. E.g. if MinimumPitch is 75 Hz, the window length
  //  is 40 ms  and padded with zeros to reach a power of two.
  pitchPraat->updctrl("mrs_natural/inSamples", 1024);
  pitchPraat->updctrl("Windowing/wi/mrs_string/type", "Hanning");
  pitchPraat->updctrl("Peaker/pkr/mrs_real/peakSpacing", 0.00);
  pitchPraat->updctrl("Peaker/pkr/mrs_natural/interpolation", 1);
  // pitchPraat->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.4);
  pitchPraat->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 1);
  pitchPraat->updctrl("MaxArgMax/mxr/mrs_natural/interpolation", 1);
  pitchPraat->linkctrl("mrs_natural/lowSamples", "Peaker/pkr/mrs_natural/peakStart");
  pitchPraat->linkctrl("mrs_natural/highSamples", "Peaker/pkr/mrs_natural/peakEnd");
  // set default values
  lowPitch = 36;
  highPitch = 79;
  lowFreq = pitch2hertz(lowPitch);
  highFreq = pitch2hertz(highPitch);
  lowSamples = 
    hertz2samples(highFreq, pitchPraat->getctrl("mrs_real/osrate")->to<mrs_real>());
  highSamples = 
    hertz2samples(lowFreq, pitchPraat->getctrl("mrs_real/osrate")->to<mrs_real>());
  pitchPraat->updctrl("mrs_natural/lowSamples", lowSamples);
  pitchPraat->updctrl("mrs_natural/highSamples", highSamples);
  registerPrototype("PitchPraat", pitchPraat);


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
	peAnalysePr->linkctrl("mrs_natural/winSize", 
		"ShiftInput/si/mrs_natural/winSize");
	peAnalysePr->linkctrl("mrs_natural/FFTSize", 
		"Windowing/wi/mrs_natural/size");
	peAnalysePr->linkctrl("mrs_string/WindowType", 
		"Windowing/wi/mrs_string/type");
	peAnalysePr->linkctrl("mrs_bool/zeroPhasing", 
		"Windowing/wi/mrs_bool/zeroPhasing");
	peAnalysePr->linkctrl("mrs_natural/frameMaxNumPeaks", 
		"PeakConvert/conv/mrs_natural/frameMaxNumPeaks");
	//peAnalysePr->linkctrl("mrs_natural/Decimation", 
	//	"PeakConvert/conv/mrs_natural/Decimation");
	peAnalysePr->updctrl("Shifter/sh/mrs_natural/shift", 1);
	registerPrototype("PeakAnalyse", peAnalysePr);

	//--------------------------------------------------------------------------------
	// prototype for WHaSp calculation
	//--------------------------------------------------------------------------------
	MarSystem* WHaSpnetpr = new Series("WHaSpnetpr");
	WHaSpnetpr->addMarSystem(create("PeakAnalyse", "analyse"));
	WHaSpnetpr->addMarSystem(create("WHaSp", "whasp"));
	//
	WHaSpnetpr->linkctrl("WHaSp/whasp/mrs_natural/totalNumPeaks",
		"PeakAnalyse/analyse/PeakConvert/conv/mrs_natural/totalNumPeaks");
	WHaSpnetpr->linkctrl("WHaSp/whasp/mrs_natural/frameMaxNumPeaks",
		"PeakAnalyse/analyse/PeakConvert/conv/mrs_natural/frameMaxNumPeaks");
	//
	WHaSpnetpr->linkctrl("mrs_natural/frameMaxNumPeaks", 
		"PeakAnalyse/analyse/mrs_natural/frameMaxNumPeaks");
	WHaSpnetpr->updctrl("mrs_natural/frameMaxNumPeaks", 20);
	//
	registerPrototype("WHaSpnet", WHaSpnetpr);

  //--------------------------------------------------------------------------------
  // prototype for Stereo Features
  //--------------------------------------------------------------------------------
  MarSystem* stereoFeats2 = new Fanout("StereoFeatures2pr");
  stereoFeats2->addMarSystem(create("StereoSpectrumFeatures","stereospkfeats"));
  stereoFeats2->addMarSystem(create("StereoSpectrumSources","stereospksources"));
  registerPrototype("StereoFeatures2", stereoFeats2);

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
	
  classifierpr->linkctrl("ZeroRClassifier/zerorcl/mrs_natural/nClasses", 
			 "mrs_natural/nClasses");
  classifierpr->linkctrl("GaussianClassifier/gaussiancl/mrs_natural/nClasses", 
			 "mrs_natural/nClasses");
  classifierpr->linkctrl("SVMClassifier/svmcl/mrs_natural/nClasses", 
			 "mrs_natural/nClasses");

  classifierpr->linkctrl("ZeroRClassifier/zerorcl/mrs_string/mode", 
			 "mrs_string/mode");
  classifierpr->linkctrl("GaussianClassifier/gaussiancl/mrs_string/mode", 
			 "mrs_string/mode");
  classifierpr->linkctrl("SVMClassifier/svmcl/mrs_string/mode", 
			 "mrs_string/mode");

  classifierpr->updctrl("mrs_string/disableChild", "all");
  // Indirect way 
  /* classifierpr->linkctrl("ZeroRClassifier/zerorcl/mrs_natural/nClasses", 
     "GaussianClassifier/gaussiancl/mrs_natural/nClasses");
     classifierpr->linkctrl("ZeroRClassifier/zerorcl/mrs_string/mode", 
     "GaussianClassifier/gaussiancl/mrs_string/mode");
     classifierpr->linkctrl("mrs_natural/nClasses", 
     "GaussianClassifier/gaussiancl/mrs_natural/nClasses");
     classifierpr->linkctrl("mrs_string/mode", 
     "GaussianClassifier/gaussiancl/mrs_string/mode");
  */ 
	
  registerPrototype("Classifier", classifierpr);
	
}

MarSystemManager::~MarSystemManager()
{
  map<string, MarSystem *>::const_iterator iter;

  for (iter=registry_.begin(); iter != registry_.end(); ++iter)
    {
      delete iter->second;
    }
  registry_.clear();
}

void 
MarSystemManager::registerPrototype(string type, MarSystem *marsystem)
{
  //change type_ of composite to the user specified one
  marsystem->setType(type);
  //and register it
  registry_[type] = marsystem;
}

MarSystem* 
MarSystemManager::getPrototype(string type) 
{

  if (registry_.find(type) != registry_.end())
    return (MarSystem *)(registry_[type])->clone();
  else 
    {
      MRSWARN("MarSystemManager::getPrototype No prototype found for " + type);
      return 0;
    }
}

MarSystem* 
MarSystemManager::create(string type, string name) 
{
  if (registry_.find(type) != registry_.end())
    {
      MarSystem* m = (MarSystem *)(registry_[type])->clone();
      m->setName(name);
      return m;
    }

  else 
    {
      MRSWARN("MarSystemManager::getPrototype No prototype found for " + type);
      return NULL;
    }
}

MarSystem* 
MarSystemManager::getMarSystem(istream& is, MarSystem *parent)
{
  string skipstr;
  mrs_natural i;
  is >> skipstr;
  string mcomposite;
  bool   isComposite;

  is >> mcomposite;
  string marSystem = "MarSystem";
  string marSystemComposite = "MarSystemComposite";  
  if (mcomposite == marSystem)
    isComposite = false;
  else if (mcomposite == marSystemComposite)
    isComposite = true;
  else
    return 0;

  is >> skipstr >> skipstr >> skipstr;
  string mtype;
  is >> mtype;

  is >> skipstr >> skipstr >> skipstr;
  string mname;
  is >> mname;

  MarSystem* msys = getPrototype(mtype);

  if (msys == 0)
    {
      MRSWARN("MarSystem::getMarSystem - MarSystem not supported");
      return 0;
    }
  else
    {
      msys->setName(mname);
      msys->setParent(parent);
		
      //delete all children MarSystems in a (prototype) Composite 
      //and read and link (as possible) local controls
      is >> *msys; 

      msys->update();

      workingSet_[msys->getName()] = msys; // add to workingSet

      //recreate the Composite destroyed above, relinking all
      //linked controls in its way
      if (isComposite == true)
	{
	  is >> skipstr >> skipstr >> skipstr;
	  mrs_natural nComponents;
	  is >> nComponents;
	  for (i=0; i < nComponents; i++)
	    {
	      MarSystem* cmsys = getMarSystem(is, msys);
	      if (cmsys == 0)
		return 0;
	      msys->addMarSystem(cmsys);
	    }
	  msys->update();
	}
    }

  return msys;
}

//
// Added by Adam Parkin, Jul 2005, invoked by MslModel
//
// Returns a list of all registered prototypes
//
vector <string> MarSystemManager::registeredPrototypes()
{
  vector <string> retVal;

  map<string, MarSystem *>::const_iterator iter;

  for (iter=registry_.begin(); iter != registry_.end(); ++iter)
    {
      retVal.push_back (iter->first);
    }

  return retVal;
} 

// Added by Stuart Bray Dec 2004. invoked by MslModel
map<string, MarSystem*> MarSystemManager::getWorkingSet(istream& is) 
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
bool MarSystemManager::isRegistered (string name)
{
  return (registry_.find(name) != registry_.end());
}
