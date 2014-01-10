/*
** Copyright (C) 2000-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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



// mudbox is a container executable for various simple
// functions/applications that typically test drive
// a single MarSystem or type of processing. It can
// either be viewed as repository of simple (but sometimes
// broken) examples or as an incubator for more complicated
// applications that deserve a separate executable. This
// is the best place to experiment with Marsyas without
// adding your own application and having to change
// the build process.


#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iomanip>

#include <marsyas/common_source.h>
#include <marsyas/Collection.h>
#include <marsyas/NumericLib.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/FileName.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/marsystems/MarSystemTemplateBasic.h>
#include <marsyas/marsystems/MarSystemTemplateAdvanced.h>
#include <marsyas/sched/EvValUpd.h>

#include <marsyas/marsystems/Spectrum2ACMChroma.h>

#include <time.h>

#ifdef MARSYAS_MIDIIO
#include <RtMidi.h>
#endif

#ifdef MARSYAS_AUDIOIO
#include <RtAudio.h>
#endif

#ifdef MARSYAS_PNG
#include <pngwriter.h>
#endif


using namespace std;
using namespace Marsyas;

CommandLineOptions cmd_options;
string toy_withName;
int helpopt;
int usageopt;
int verboseopt;

void
printUsage(string progName)
{
  MRSDIAG("marsyasToy_Withs.cpp - printUsage");
  cerr << "Usage : " << progName << " -t toy_withName file1 [file2]" << endl;
  cerr << endl;
  exit(1);
}

void
printHelp(string progName)
{
  MRSDIAG("marsyasTests.cpp - printHelp");
  cerr << "marsyasTests, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Various tests " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << " -t toy_withName file1 file2 file3" << endl;
  cerr << endl;
  cerr << "Supported toy_withs:" << endl;
  cerr << "audiodevices    : enumerate audio devices " << endl;
  cerr << "beats		: causal real-time beat tracking (score_function:['regular', 'correlation', 'squareCorr'] wav_input.wav)" << endl;
  cerr << "cascade         : check cascade composite " << endl;
  cerr << "collection      : using collection file1 as a SoundFileSource " << endl;
  cerr << "confidence      : toy_with confidence calculation" << endl;
  cerr << "drumclassify    : drumclassify (mplfile argument)" << endl;
  cerr << "duplex          : duplex audio input/output" << endl;
  cerr << "duplex2         : more duplex functionality" << endl;
  cerr << "fanoutswitch    : toy_with disabling fanout branches " << endl;
  cerr << "filter          : toy_with filter MarSystem " << endl;
  cerr << "fft             : toy_with fft analysis/resynthesis " << endl;
  cerr << "inSamples       : changing inSamples at runtime " << endl;
  cerr << "knn             : toy_with K-NearestNeighbor classifier " << endl;
  cerr << "LPC_LSP         : toy_with LPC and LSP routines" << endl;
  cerr << "MarControls     : toy_with MarControl API" << endl;
  cerr << "marsystemIO     : toy_with marsystem IO " << endl;
  cerr << "matlab          : toy_with matlab" << endl;
  cerr << "MATLABengine    : toy_with MATLABengine" << endl;
  cerr << "mixer           : toy_with fanout for mixing " << endl;
  cerr << "mp3convert      : toy_with convertion of a collection of .mp3 files to .wav files" << endl;
  cerr << "multichannel_merge : toy with multichannel merge" << endl;
  cerr << "normMaxMin      : toy_with of normalize marsSystem " << endl;
  cerr << "onsets          : toy_with onsets" << endl;
  cerr << "panorama        : toy_with Panorama amplitude panning " << endl;
  cerr << "parallel        : toy_with Parallel composite " << endl;
  cerr << "phase           : toy_with phase manipulation " << endl;

  cerr << "pngwrite        : toy_with png writer " << endl;

  cerr << "plucked        : toy_with plucked " << endl;
  cerr << "pluckedBug     : toy_with plucked Bug " << endl;
  cerr << "pluckedLive    : toy_with plucked Live" << endl;

  cerr << "stereoFeaturesVisualization : toy_with stereo features visualization" << endl;
  cerr << "phisem          : toy_with physem" << endl;
  cerr << "pitch           : toy_with pitch" << endl;
  cerr << "power           : toy_with power" << endl;
  cerr << "probe           : toy_with Probe functionality " << endl;
  cerr << "radiodrum       : toy_with radiodrum" << endl;
  cerr << "realvec         : toy_with realvec functions " << endl;
  cerr << "realvecCtrl     : toy_with realvecCtrl" << endl;
  cerr << "reverb          : toy_with reverb " << endl;
  cerr << "rmsilence       : toy_with removing silences " << endl;
  cerr << "scheduler       : toy_with scheduler " << endl;
  cerr << "schedulerExpr   : toy_with scheduler with expressions " << endl;
  cerr << "shredder        : toy_with shredder" << endl;
  cerr << "SNR             : toy_with Siganl-to-Noise Ratio" << endl;
  cerr << "SOM             : toy_with support vector machine " << endl;
  cerr << "spectralSNR     : toy_with spectral SNR " << endl;
  cerr << "stereoFeatures  : toy_with stereo features " << endl;
  cerr << "stereoMFCC      : toy_with stereo MFCC " << endl;
  cerr << "stereoFeaturesMFCC : toy_with stereo features and MFCCs" << endl;
  cerr << "swipe           : toy_with_swipe (F0 estimator)" << endl;
  cerr << "stretchLinear   : toy_with_stretchLinear time stretching with linear interpolation" << endl;

  cerr << "stereo2mono     : toy_with stereo to mono conversion " << endl;
  cerr << "ADRess          : toy_with stereo ADRess algorithm " << endl;
  cerr << "tempo           : toy_with tempo estimation " << endl;
  cerr << "train_predict   : toy_with train and predict" << endl;
  cerr << "vibrato         : toy_with vibrato using time-varying delay line" << endl;
  cerr << "vicon           : toy_with processing of vicon motion capture data" << endl;
  cerr << "windowedsource  : toy_with windowed source" << endl;
  cerr << "Windowing       : toy_with different window functions of Windowing marsystem" << endl;
  cerr << "weka            : toy_with weka source and sink functionality" << endl;
  cerr << "updControl         : toy_with updating control with pointers " << endl;

  cerr << "sfplay          : plays a sound file" << endl;
  cerr << "SFPlay          : plays only labelled regions in an audio file" << endl;
  cerr << "getControls     : toy_with getControls functionality " << endl;
  cerr << "mono2stereo     : toy_with mono2stereo MarSystem " << endl;

  cerr << "marostring      : toy_with marostring [xml|svg|html|json] " << endl;

  cerr << "accent_filter_bank	: toy_with AccentFilterBank " << endl;
  cerr << "ExtractChroma   : toy_with chroma (file1 = in wav file, file 2 = out text file)" << endl;
  exit(1);
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addStringOption("toy_withName", "t", EMPTYSTRING);
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  verboseopt = cmd_options.getBoolOption("verbose");
  toy_withName = cmd_options.getStringOption("toy_withName");
}


void
toy_with_audiodevices()
{
#ifdef MARSYAS_AUDIOIO
  std::cout << "Testing audio devices" << endl;

  // Create an api map.
  std::map<int, std::string> apiMap;
  apiMap[RtAudio::MACOSX_CORE] = "OS-X Core Audio";
  apiMap[RtAudio::WINDOWS_ASIO] = "Windows ASIO";
  apiMap[RtAudio::WINDOWS_DS] = "Windows Direct Sound";
  apiMap[RtAudio::UNIX_JACK] = "Jack Client";
  apiMap[RtAudio::LINUX_ALSA] = "Linux ALSA";
  apiMap[RtAudio::LINUX_OSS] = "Linux OSS";
  apiMap[RtAudio::RTAUDIO_DUMMY] = "RtAudio Dummy";

  std::vector< RtAudio::Api > apis;
  RtAudio :: getCompiledApi( apis );

  std::cout << "\nCompiled APIs:\n";
  for ( unsigned int i=0; i<apis.size(); i++ )
    std::cout << "  " << apiMap[ apis[i] ] << std::endl;

  RtAudio audio;
  RtAudio::DeviceInfo info;

  std::cout << "\nCurrent API: " << apiMap[ audio.getCurrentApi() ] << std::endl;

  unsigned int devices = audio.getDeviceCount();
  std::cout << "\nFound " << devices << " device(s) ...\n";

  for (unsigned int i=0; i<devices; i++) {
    info = audio.getDeviceInfo(i);

    std::cout << "\nDevice Name = " << info.name << '\n';
    if ( info.probed == false )
      std::cout << "Probe Status = UNsuccessful\n";
    else {
      std::cout << "Probe Status = Successful\n";
      std::cout << "Output Channels = " << info.outputChannels << '\n';
      std::cout << "Input Channels = " << info.inputChannels << '\n';
      std::cout << "Duplex Channels = " << info.duplexChannels << '\n';
      if ( info.isDefaultOutput ) std::cout << "This is the default output device.\n";
      else std::cout << "This is NOT the default output device.\n";
      if ( info.isDefaultInput ) std::cout << "This is the default input device.\n";
      else std::cout << "This is NOT the default input device.\n";
      if ( info.nativeFormats == 0 )
        std::cout << "No natively supported data formats(?)!";
      else {
        std::cout << "Natively supported data formats:\n";
        if ( info.nativeFormats & RTAUDIO_SINT8 )
          std::cout << "  8-bit int\n";
        if ( info.nativeFormats & RTAUDIO_SINT16 )
          std::cout << "  16-bit int\n";
        if ( info.nativeFormats & RTAUDIO_SINT24 )
          std::cout << "  24-bit int\n";
        if ( info.nativeFormats & RTAUDIO_SINT32 )
          std::cout << "  32-bit int\n";
        if ( info.nativeFormats & RTAUDIO_FLOAT32 )
          std::cout << "  32-bit float\n";
        if ( info.nativeFormats & RTAUDIO_FLOAT64 )
          std::cout << "  64-bit float\n";
      }
      if ( info.sampleRates.size() < 1 )
        std::cout << "No supported sample rates found!";
      else {
        std::cout << "Supported sample rates = ";
        for (unsigned int j=0; j<info.sampleRates.size(); j++)
          std::cout << info.sampleRates[j] << " ";
      }
      std::cout << std::endl;
    }
  }
  std::cout << std::endl;



#endif


}



void
toy_with_matlab(string sfName)
{
  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("Gain", "gain"));
  net->addMarSystem(mng.create("AudioSink", "dest"));

  net->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  net->updControl("Gain/gain/mrs_real/gain", 8.0);
  net->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  net->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
  mrs_real srate = net->getctrl("mrs_real/osrate")->to<mrs_real>();
  mrs_natural inSamples = net->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  mrs_real tstep = inSamples / srate;
  mrs_real time = 0.0;

  mrs_realvec src_data;
  mrs_realvec filter_data;

  src_data = net->getctrl("SoundFileSource/src/mrs_realvec/processedData")->to<mrs_realvec>();
  MATLAB_PUT(src_data, "src_data");
  MATLAB_EVAL("a = 20 * log10(abs(fft(src_data)));");
  MATLAB_EVAL("plot(a(1:200))");;
  mrs_real maxp = 0.0, maxl = 0.0;

  for (int i=0; i < 500; ++i)
  {
    net->tick();
    src_data = net->getctrl("SoundFileSource/src/mrs_realvec/processedData")->to<mrs_realvec>();
    if (i % 10 == 0)
    {
      MATLAB_PUT(src_data, "src_data");
      MATLAB_EVAL("a = 20 * log10(abs(fft(src_data)));");
      MATLAB_EVAL("plot(a(1:200))");
      MATLAB_EVAL("[maxp,maxl] = max(a(1:200))");
      MATLAB_GET("maxp", maxp);
      MATLAB_GET("maxl", maxl);
      cout << maxl << "-" << maxp << endl;
    }
    time += tstep;
  }
  MATLAB_CLOSE();
}


void
toy_with_cascade()
{
  cout << "Toy with Cascade Composite" << endl;
  MarSystemManager mng;
  MarSystem* cascade = mng.create("Cascade", "cascade");

  realvec a(3),b(3);
  MarSystem* f0 = mng.create("Filter", "f0");
  a(0) = 1.0f;
  a(1) = 0.0f;
  a(2) = 0.0f;
  b(0) = 1.0f;
  b(1) = -0.9f;
  b(2) = 0.0f;
  f0->setctrl("mrs_realvec/ncoeffs", a);
  f0->setctrl("mrs_realvec/dcoeffs", b);
  f0->setctrl("mrs_natural/inSamples", mrs_natural(5));
  f0->setctrl("mrs_natural/inObservations", mrs_natural(1));
  f0->setctrl("mrs_real/israte", 44100.0f);

  MarSystem* f1 = mng.create("Filter", "f1");
  a(0) = 1.0f;
  a(1) = 1.0f;
  a(2) = 0.0f;
  b(0) = 1.0f;
  b(1) = 0.0f;
  b(2) = 0.0f;
  f1->setctrl("mrs_realvec/ncoeffs", a);
  f1->setctrl("mrs_realvec/dcoeffs", b);

  cout << "After creating Filters" << endl;

  cascade->addMarSystem(f0);
  cascade->addMarSystem(f1);

  realvec in, out;
  in.create(mrs_natural(1),mrs_natural(5));
  in(0,0) = 1.0f;
  out.create(mrs_natural(2),mrs_natural(5));

  cout << "Before processing" << endl;

  // TOCHECK: memory leak when calling process directly
  cascade->process(in, out);
}


void
toy_with_CollectionFileSource(string sfName)
{
  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  playbacknet->linkControl("mrs_bool/hasData",
                           "SoundFileSource/src/mrs_bool/hasData");
  playbacknet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");

  // mrs_bool isEmpty;
  // int cindex = 0;
  int index = 0;

  playbacknet->updControl("SoundFileSource/src/mrs_real/repetitions", 1.0);

// 	while (isEmpty = playbacknet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  for (int i=0; i < 2000; i++)
  {
    playbacknet->tick();
    // playbacknet->updControl("SoundFileSource/src/mrs_natural/cindex", cindex);
    cout << playbacknet->getctrl("SoundFileSource/src/mrs_string/currentlyPlaying")->to<mrs_string>() << endl;
    cout << playbacknet->getctrl("SoundFileSource/src/mrs_natural/pos")->to<mrs_natural>() << endl;
    cout << playbacknet->getctrl("SoundFileSource/src/mrs_real/repetitions")->to<mrs_real>() << endl;

    // cindex++;
    // cout << "cindex = " << cindex << endl;
    //toy_with if setting "mrs_natural/pos" to 0 for rewinding is working
    if(playbacknet->getctrl("mrs_natural/pos")->to<mrs_natural>() > 800000)
      playbacknet->updControl("mrs_natural/pos", 0);
    cout << playbacknet->getctrl("mrs_bool/hasData")->to<mrs_bool>() << endl;

    index++;
  }
  cout << "index = " << index << endl;

  delete playbacknet;
}




void
toy_with_RadioDrumInput()
{
  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("AudioSource", "src"));
  playbacknet->addMarSystem(mng.create("RadioDrumInput", "rd"));

  playbacknet->updControl("mrs_natural/inSamples", 512);
  playbacknet->updControl("mrs_real/osrate", 44100.0);
  playbacknet->updControl("mrs_real/israte", 44100.0);
  playbacknet->updControl("AudioSource/src/mrs_bool/initAudio", true);
  playbacknet->updControl("RadioDrumInput/rd/mrs_bool/initmidi", true);
  while (true)
  {
    playbacknet->tick();

    /*
      printf("right stick x position:  %li left stick x position: %li \r",
      playbacknet->getctrl("RadioDrumInput/rd/mrs_natural/rightstickx")->to<mrs_natural>(),
      playbacknet->getctrl("RadioDrumInput/rd/mrs_natural/leftstickx")->to<mrs_natural>() );
    */
    cout << "rightstick= " << playbacknet->getctrl("RadioDrumInput/rd/mrs_natural/rightstickx")->to<mrs_natural>() << endl;
    cout << "leftstick= " <<  playbacknet->getctrl("RadioDrumInput/rd/mrs_natural/leftstickx")->to<mrs_natural>() << endl;
  }
  delete playbacknet;
}






/*
  This code works by loading an mpl file with a trained classifier.

  First run bextract to analyze examples and train a classifier
  bextract -e DRUMEXTRACT -f outputfile.mpl -w 512 -sr 44100.0

  Then run
  drumextract outputfile.mpl
*/
void drumClassify( string drumFile) {
  int windowsize = 512;
  int numberOfCoefficients = 67;

  MarSystemManager mng;

  MarSystem* TimeLoop= mng.create("Series", "TimeLoop");
  TimeLoop->addMarSystem(mng.create("AudioSource", "src"));
  TimeLoop->addMarSystem(mng.create("PeakerAdaptive", "peak"));

  TimeLoop->updControl("PeakerAdaptive/peak/mrs_natural/peakEnd", 512);
  TimeLoop->updControl("PeakerAdaptive/peak/mrs_real/peakSpacing", 0.5);
  TimeLoop->updControl("PeakerAdaptive/peak/mrs_real/peakStrength", 0.7);
  TimeLoop->updControl("PeakerAdaptive/peak/mrs_natural/peakStart", 0);
  TimeLoop->updControl("PeakerAdaptive/peak/mrs_natural/peakStrengthReset", 2);
  TimeLoop->updControl("PeakerAdaptive/peak/mrs_real/peakDecay", 0.9);
  TimeLoop->updControl("PeakerAdaptive/peak/mrs_real/peakGain", 0.5);

  //======================================
  // Features
  //======================================
  MarSystem* extractNet = mng.create("Series", "extractNet");

  MarSystem* spectimeFanout = mng.create("Fanout", "spectimeFanout");
  spectimeFanout->addMarSystem(mng.create("ZeroCrossings", "zcrs"));
  spectimeFanout->addMarSystem(mng.create("Rms", "rms"));

  MarSystem* spectralNet = mng.create("Series", "spectralNet");
  spectralNet->addMarSystem(mng.create("Windowing", "ham"));
  spectralNet->addMarSystem(mng.create("Spectrum", "spk"));
  spectralNet->addMarSystem(mng.create("PowerSpectrum", "pspk"));

  MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
  featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
  featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));
  featureFanout->addMarSystem(mng.create("Kurtosis", "kurtosis"));
  featureFanout->addMarSystem(mng.create("Skewness", "skewness"));
  featureFanout->addMarSystem(mng.create("SFM", "sfm"));
  featureFanout->addMarSystem(mng.create("SCF", "scf"));


  spectralNet->addMarSystem(featureFanout);
  spectimeFanout->addMarSystem(spectralNet);
  extractNet->addMarSystem(spectimeFanout);

  // Our Classifier
  extractNet->addMarSystem(mng.create("GaussianClassifier", "classifier"));
  extractNet->updControl("GaussianClassifier/classifier/mrs_string/mode","predict");
  extractNet->updControl("GaussianClassifier/classifier/mrs_natural/nClasses",2);

  realvec means;
  realvec vars;

  means.create(numberOfCoefficients);
  vars.create(numberOfCoefficients);

  //=============================
  //Read trained values from mpl value into realvecs to be loaded into the classifier
  //============================

  vector<string> words;
  ifstream in(drumFile.c_str());
  string word;
  while(in >> word)
    words.push_back(word);

  for ( mrs_natural i = 0; i < (mrs_natural) words.size(); ++i)
  {

    if ( words[i] == "means" )
      for ( int p = 0; p < numberOfCoefficients; p++)
      {
        // get he current word from the file
        word = words[++i];
        // cast the string to a float
        float readval;
        istringstream mystream(word);
        mystream >> readval;
        means.setval( i,i,readval);
      }
    if ( words[i] == "covars" )
      for ( int p = 0; p < numberOfCoefficients; p++)
      {
        // get he current word from the file
        word = words[++i];
        // cast the string to a float
        float readval;
        istringstream mystream(word);
        mystream >> readval;
        means.setval( i,i,readval);
      }
  }

  cout << means << endl;
  cout << vars << endl;

  extractNet->updControl("GaussianClassifier/classifier/realvec/means", means);
  extractNet->updControl("GaussianClassifier/classifier/realvec/covars", vars);
  extractNet->updControl("mrs_natural/inSamples",512);
  extractNet->updControl("mrs_natural/onSamples",512);
  extractNet->updControl("mrs_real/israte", 44100.0);
  extractNet->updControl("mrs_real/osrate", 44100.0);

  realvec in1;
  realvec out1;
  realvec out2;

  in1.create(TimeLoop->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
             TimeLoop->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  out1.create(TimeLoop->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
              TimeLoop->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  out2.create(extractNet->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
              extractNet->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

#ifdef MARSYAS_MIDIIO
  RtMidiOut *midiout = new RtMidiOut();
  midiout->openPort(0);
#endif

  vector<unsigned char> message;
  message.push_back(144);
  message.push_back(60);
  message.push_back(0);

  // PeakerAdaptive looks for hits and then if it finds one reports a non-zero value
  // When a non-zero value is found extractNet is ticked
  while ( TimeLoop->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() )
  {
    TimeLoop->process(in1,out1);
    for (int i = 0; i < windowsize; ++i)
    {
      if ( out1(i) > 0)
      {
        extractNet->process(out1, out2);

        cout << *extractNet << endl;
        cout << out2 << endl;

        mrs_real class1 = out2(0);
        mrs_real class2 = out2(1);

        int decision;
        decision =  (int) ((class1/class2) * 127);
        message[3]=decision;
        break;
      }
    }

  }
}




void
toy_with_onsets(string sfName)
{
  // cout << "toying with onsets" << endl;
  MarSystemManager mng;

  // assemble the processing network
  MarSystem* onsetnet = mng.create("Series", "onsetnet");
  MarSystem* onsetaccum = mng.create("Accumulator", "onsetaccum");
  MarSystem* onsetseries= mng.create("Series","onsetseries");
  onsetseries->addMarSystem(mng.create("SoundFileSource", "src"));
  onsetseries->addMarSystem(mng.create("Stereo2Mono", "src")); //replace by a "Monofier" MarSystem (to be created) [!]
  //onsetseries->addMarSystem(mng.create("ShiftInput", "si"));
  //onsetseries->addMarSystem(mng.create("Windowing", "win"));
  MarSystem* onsetdetector = mng.create("FlowThru", "onsetdetector");
  onsetdetector->addMarSystem(mng.create("ShiftInput", "si")); //<---
  onsetdetector->addMarSystem(mng.create("Windowing", "win")); //<---
  onsetdetector->addMarSystem(mng.create("Spectrum","spk"));
  onsetdetector->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  onsetdetector->addMarSystem(mng.create("Flux", "flux"));
  //onsetdetector->addMarSystem(mng.create("Memory","mem"));
  onsetdetector->addMarSystem(mng.create("ShiftInput","sif"));
  onsetdetector->addMarSystem(mng.create("Filter","filt1"));
  onsetdetector->addMarSystem(mng.create("Reverse","rev1"));
  onsetdetector->addMarSystem(mng.create("Filter","filt2"));
  onsetdetector->addMarSystem(mng.create("Reverse","rev2"));
  onsetdetector->addMarSystem(mng.create("PeakerOnset","peaker"));
  onsetseries->addMarSystem(onsetdetector);
  onsetaccum->addMarSystem(onsetseries);
  onsetnet->addMarSystem(onsetaccum);
  //onsetnet->addMarSystem(mng.create("ShiftOutput","so"));
  MarSystem* onsetmix = mng.create("Fanout","onsetmix");
  onsetmix->addMarSystem(mng.create("Gain","gainaudio"));
  MarSystem* onsetsynth = mng.create("Series","onsetsynth");
  onsetsynth->addMarSystem(mng.create("NoiseSource","noisesrc"));
  onsetsynth->addMarSystem(mng.create("ADSR","env"));
  onsetsynth->addMarSystem(mng.create("Gain", "gainonsets"));
  onsetmix->addMarSystem(onsetsynth);
  onsetnet->addMarSystem(onsetmix);

  //onsetnet->addMarSystem(mng.create("AudioSink", "dest"));
  onsetnet->addMarSystem(mng.create("SoundFileSink", "fdest"));


  ///////////////////////////////////////////////////////////////////////////////////////
  //link controls
  ///////////////////////////////////////////////////////////////////////////////////////
  onsetnet->linkControl("mrs_bool/hasData",
                        "Accumulator/onsetaccum/Series/onsetseries/SoundFileSource/src/mrs_bool/hasData");
  //onsetnet->linkControl("ShiftOutput/so/mrs_natural/Interpolation","mrs_natural/inSamples");
  onsetnet->linkControl("Accumulator/onsetaccum/mrs_bool/flush",
                        "Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_bool/onsetDetected");
  //onsetnet->linkControl("Fanout/onsetmix/Series/onsetsynth/Gain/gainonsets/mrs_real/gain",
  //	"Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_real/confidence");

  //onsetnet->linkControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Memory/mem/mrs_bool/reset",
  //	"Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_bool/onsetDetected");

  //link FILTERS coeffs
  onsetnet->linkControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt2/mrs_realvec/ncoeffs",
                        "Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/ncoeffs");
  onsetnet->linkControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt2/mrs_realvec/dcoeffs",
                        "Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/dcoeffs");

  ///////////////////////////////////////////////////////////////////////////////////////
  // update controls
  ///////////////////////////////////////////////////////////////////////////////////////
  FileName outputFile(sfName);
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/SoundFileSource/src/mrs_string/filename", sfName);
  onsetnet->updControl("SoundFileSink/fdest/mrs_string/filename", outputFile.nameNoExt() + "_onsets.wav");
  mrs_real fs = onsetnet->getctrl("mrs_real/osrate")->to<mrs_real>();

  mrs_natural winSize = 4096;//2048;
  mrs_natural hopSize = 1024;//411;
  mrs_natural lookAheadSamples = 6;
  mrs_real thres = 0.75;

  mrs_real textureWinMinLen = 0.050; //secs
  mrs_natural minTimes = (mrs_natural) (textureWinMinLen*fs/hopSize); //12;//onsetWinSize+1;//15;
  // cout << "MinTimes = " << minTimes << " (i.e. " << textureWinMinLen << " secs)" << endl;
  mrs_real textureWinMaxLen = 22.0; //secs
  mrs_natural maxTimes = (mrs_natural) (textureWinMaxLen*fs/hopSize);//1000; //whatever... just a big number for now...
  // cout << "MaxTimes = " << maxTimes << " (i.e. " << textureWinMaxLen << " secs)" << endl;

  //best result till now are using dB power Spectrum!
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PowerSpectrum/pspk/mrs_string/spectrumType",
                       "wrongdBonsets");

  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Flux/flux/mrs_string/mode",
                       "DixonDAFX06");

  //configure zero-phase Butterworth filter of Flux time series (from J.P.Bello TASLP paper)
  // Coefficients taken from MATLAB butter(2, 0.28)
  realvec bcoeffs(1,3);
  bcoeffs(0) = 0.1174;
  bcoeffs(1) = 0.2347;
  bcoeffs(2) = 0.1174;
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/ncoeffs",
                       bcoeffs);
  realvec acoeffs(1,3);
  acoeffs(0) = 1.0;
  acoeffs(1) = -0.8252;
  acoeffs(2) = 0.2946;
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/dcoeffs",
                       acoeffs);

  onsetnet->updControl("mrs_natural/inSamples", hopSize);
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/ShiftInput/si/mrs_natural/winSize", winSize);

  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_natural/lookAheadSamples", lookAheadSamples);
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_real/threshold", thres); //!!!

  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/ShiftInput/sif/mrs_natural/winSize", 4*lookAheadSamples+1);

  mrs_natural winds = 1+lookAheadSamples+mrs_natural(ceil(mrs_real(winSize)/hopSize/2.0));
  // cout << "timesToKeep = " << winds << endl;
  onsetnet->updControl("Accumulator/onsetaccum/mrs_natural/timesToKeep", winds);
  onsetnet->updControl("Accumulator/onsetaccum/mrs_string/mode","explicitFlush");
  onsetnet->updControl("Accumulator/onsetaccum/mrs_natural/maxTimes", maxTimes);
  onsetnet->updControl("Accumulator/onsetaccum/mrs_natural/minTimes", minTimes);

  //set audio/onset resynth balance and ADSR params for onset sound
  onsetnet->updControl("Fanout/onsetmix/Gain/gainaudio/mrs_real/gain", 1.0);
  onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/Gain/gainonsets/mrs_real/gain", 0.8);
  onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/aTarget", 1.0);
  onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/aTime", winSize/80/fs); //!!!
  onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/susLevel", 0.0);
  onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/dTime", winSize/4/fs); //!!!

  //onsetnet->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  //MATLAB Engine inits
  //used for toy_with_onsets.m
  MATLAB_EVAL("clear;");
  MATLAB_PUT(winSize, "winSize");
  MATLAB_PUT(hopSize, "hopSize");
  MATLAB_PUT(lookAheadSamples, "lookAheadSamples");
  MATLAB_EVAL("srcAudio = [];");
  MATLAB_EVAL("onsetAudio = [];");
  MATLAB_EVAL("FluxTS = [];");
  MATLAB_EVAL("segmentData = [];");
  MATLAB_EVAL("onsetTS = [];");

  ///////////////////////////////////////////////////////////////////////////////////////
  //process input file (till EOF)
  ///////////////////////////////////////////////////////////////////////////////////////
  mrs_natural timestamps_samples = 0;
  //mrs_real sampling_rate;
  //sampling_rate = onsetnet->getctrl("mrs_real/osrate")->to<mrs_real>();
  // cout << "Sampling rate = " << sampling_rate << endl;

  while(onsetnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/nton", 1.0); //note on
    onsetnet->tick();
    timestamps_samples += onsetnet->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
    // cout << timestamps_samples / sampling_rate << endl;
    cout << timestamps_samples << endl;;
    onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/ntoff", 0.0); //note off
  }
}


void
toy_with_train_predict(string trainFileName, string testFileName)
{
  cout << "Toying with train: " << trainFileName
       << " and predict: " << testFileName << endl;


  MarSystemManager mng;

  ////////////////////////////////////////////////////////////
  //
  // The network that we will use to train and predict
  //
  MarSystem* net = mng.create("Series", "series");

  ////////////////////////////////////////////////////////////
  //
  // The WekaSource we read the train and test .arf files into
  //
  net->addMarSystem(mng.create("WekaSource", "wsrc"));

  ////////////////////////////////////////////////////////////
  //
  // The classifier
  //
  MarSystem* classifier = mng.create("Classifier", "cl");
  net->addMarSystem(classifier);

  ////////////////////////////////////////////////////////////
  //
  // Which classifier function to use
  //
  string classifier_ = "GS";
  if (classifier_ == "GS")
    net->updControl("Classifier/cl/mrs_string/enableChild", "GaussianClassifier/gaussiancl");
  if (classifier_ == "ZEROR")
    net->updControl("Classifier/cl/mrs_string/enableChild", "ZeroRClassifier/zerorcl");
  if (classifier_ == "SVM")
    net->updControl("Classifier/cl/mrs_string/enableChild", "SVMClassifier/svmcl");

  ////////////////////////////////////////////////////////////
  //
  // The training file we are feeding into the WekaSource
  //
  net->updControl("WekaSource/wsrc/mrs_string/filename", trainFileName);
  net->updControl("mrs_natural/inSamples", 1);

  ////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////
  //
  // Set the classes of the Summary and Classifier to be
  // the same as the WekaSource
  //
  net->updControl("Classifier/cl/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  net->updControl("Classifier/cl/mrs_string/mode", "train");

  ////////////////////////////////////////////////////////////
  //
  // Tick over the training WekaSource until all lines in the
  // training file have been read.
  //
  while (!net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>()) {
    string mode = net->getctrl("WekaSource/wsrc/mrs_string/mode")->to<mrs_string>();
    net->tick();
    net->updControl("Classifier/cl/mrs_string/mode", mode);
  }


  ////////////////////////////////////////////////////////////
  //
  // Predict the classes of the test data
  //
  net->updControl("WekaSource/wsrc/mrs_string/filename", testFileName);
  net->updControl("Classifier/cl/mrs_string/mode", "predict");

  ////////////////////////////////////////////////////////////
  //
  // Tick over the test WekaSource until all lines in the
  // test file have been read.
  //
  realvec data;

  int correct_instances = 0;
  int total_instances = 0;

  while (!net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>()) {
    net->tick();
    data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    cout << (int)data(0,0) << "-" << (int)data(1,0) << endl;
    if ((int)data(0,0) == (int)data(1,0))
      correct_instances++;
    total_instances++;
  }

  cout << "Correct instancs = " << correct_instances << " out of a total of " << total_instances << endl;


  // sness - hmm, I really should be able to delete net, but I get a
  // coredump when I do.  Maybe I need to destroy something else first?
  delete net;


}




void
toy_with_sfplay(string sfName)
{
  cout << "Toy_with: sfplay" << endl;

  MarSystemManager mng;


  MarSystem* playbacknet = mng.create("Series", "playbacknet");

  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  // playbacknet->addMarSystem(mng.create("NoiseSource", "src"));
  // MarSystem* mix = mng.create("Fanout/mix");
  // mix->addMarSystem(mng.create("Biquad", "f1"));
  // mix->addMarSystem(mng.create("Biquad", "f2"));

  // playbacknet->addMarSystem(mix);
  // playbacknet->addMarSystem(mng.create("Reverse", "rev"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);



  playbacknet->linkControl("mrs_bool/hasData",
                           "SoundFileSource/src/mrs_bool/hasData");
  playbacknet->linkControl("mrs_natural/pos",
                           "SoundFileSource/src/mrs_natural/pos");

  // while (isEmpty = playbacknet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  for (int i=0; i < 1000; ++i)
  {
    // playbacknet->updControl("Fanout/mix/Biquad/f1/mrs_real/frequency", fabs(2000.0 - frequency)+10.0);
    // playbacknet->updControl("Fanout/mix/Biquad/f2/mrs_real/frequency", frequency);

    // playbacknet->updControl("mrs_natural/inSamples", win);
    // frequency += 10.0;
    // win += 12;
    // cout << win << endl;
    playbacknet->tick();
  }
  delete playbacknet;
}






void
toy_with_sine()
{
  cout << "Toying with sine" << endl;
  MarSystemManager mng;

  MarSystem* snet = mng.create("Series/snet");
  MarSystem* smix = mng.create("Fanout/smix");
  smix->addMarSystem(mng.create("SineSource/src1"));
  smix->addMarSystem(mng.create("SineSource/src2"));
  snet->addMarSystem(smix);
  snet->addMarSystem(mng.create("AudioSink/dest"));

  snet->updControl("mrs_natural/inSamples", 2048);
  snet->updControl("AudioSink/dest/mrs_bool/initAudio", true);


  for (int i=0; i < 500; ++i)
  {
    snet->updControl("Fanout/smix/SineSource/src1/mrs_real/frequency", 440.0);
    snet->updControl("Fanout/smix/SineSource/src2/mrs_real/frequency", 445.0);
    snet->tick();
  }

}


void
toy_with_sizes(mrs_string fname)
{

  MarSystemManager mng;
  MarSystem* snet = mng.create("Series/snet");
  MarSystem* src = mng.create("SoundFileSource/src");
  MarSystem* gain = mng.create("Gain/gain");
  snet->addMarSystem(src);
  snet->addMarSystem(gain);


  snet->updControl("SoundFileSource/src/mrs_string/filename", fname);


}


void
toy_with_scales()
{
  MarSystemManager mng;

  MarSystem* snet = mng.create("Series/snet");
  MarSystem* smix = mng.create("Fanout/smix");

  MarSystem* branch1 = mng.create("Series/branch1");
  MarSystem* branch2 = mng.create("Series/branch2");
  branch1->addMarSystem(mng.create("SineSource/src"));
  branch1->addMarSystem(mng.create("Gain/gain"));
  branch2->addMarSystem(mng.create("SineSource/src"));
  branch2->addMarSystem(mng.create("Gain/gain"));

  smix->addMarSystem(branch1);
  smix->addMarSystem(branch2);
  snet->addMarSystem(smix);
  snet->addMarSystem(mng.create("MixToMono/m2m"));

  snet->addMarSystem(mng.create("AudioSink/dest"));

  snet->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  MarControlPtr f1, f2;
  MarControlPtr g1, g2;

  f1 = snet->getctrl("Fanout/smix/Series/branch1/SineSource/src/mrs_real/frequency");
  f2 = snet->getctrl("Fanout/smix/Series/branch2/SineSource/src/mrs_real/frequency");
  g1 = snet->getctrl("Fanout/smix/Series/branch1/Gain/gain/mrs_real/gain");
  g2 = snet->getctrl("Fanout/smix/Series/branch2/Gain/gain/mrs_real/gain");

//  	cout << "Beating" << endl;

//  	for (int i=0; i < 200; ++i)
//  	{
//  		f1->setValue(440.0);
//  		f2->setValue(442.0);
// 		snet->tick();
// 	}


// 	// play with ratios
// 	vector<mrs_real> ratios;
// 	ratios.push_back(2.0);
// 	ratios.push_back(3.0/2.0);
// 	ratios.push_back(4.0/3.0);
// 	ratios.push_back(5.0/4.0);
// 	ratios.push_back(6.0/5.0);

// 	cout << "Ratios as chords" << endl;

// 	for (int i=0; i<5; ++i)
// 	{
// 		f1->setValue(440.0);
// 		f2->setValue(ratios[i] * 440.0);
// 		for (int j=0; j<100; j++)
// 			snet->tick();
// 	}


// 	cout << "Ratios as intervals" << endl;
// 	g2->setValue(0.0);
// 	for (int i=0; i<5; ++i)
// 	{
// 		f1->setValue(440.0);
// 		for (int j=0; j<50; j++)
// 			snet->tick();
// 		f1->setValue(ratios[i] * 440.0);
// 		for (int j=0; j<50; j++)
// 			snet->tick();
// 	}

  cout << "Dividing the octave" << endl;
  mrs_natural divisions;

  g1->setValue(0.0);
  g2->setValue(0.0);

  cin >> divisions;

  g1->setValue(1.0);
  g2->setValue(1.0);


  mrs_real c = pow(2, 1.0/divisions);
  cout << "c = " << c << endl;

  for (int i=0; i < divisions+1; ++i)
  {
    f1->setValue(440.0 * pow(c, i));
    cout << 440.0 * pow(c,i) << " - " << pow(c,i) << endl;

    for (int j=0; j<30; j++)
      snet->tick();
  }

  g1->setValue(0.0);
  g2->setValue(0.0);

  for (int j=0; j<10; j++)
    snet->tick();





  cout << "Dividing the 3.0 (Bohlen-Pierce" << endl;
  cin >> divisions;

  g1->setValue(1.0);
  g2->setValue(1.0);


  c = pow(3.0, 1.0/divisions);
  cout << "c = " << c << endl;

  for (int i=0; i < divisions+1; ++i)
  {
    f1->setValue(440.0 * pow(c, i));
    cout << 440.0 * pow(c,i) << " - " << pow(c,i) << endl;

    for (int j=0; j<30; j++)
      snet->tick();
  }




}




void
toy_with_labelsfplay(string sfName)
{
  MarSystemManager mng;


  MarSystem* playbacknet = mng.create("Series", "playbacknet");

  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
  playbacknet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");

  playbacknet->addMarSystem(mng.create("TimelineLabeler", "tll"));
  playbacknet->linkControl("TimelineLabeler/tll/mrs_string/labelFiles", "SoundFileSource/src/mrs_string/labelNames");
  playbacknet->linkControl("TimelineLabeler/tll/mrs_natural/currentLabelFile", "SoundFileSource/src/mrs_natural/currentLabel");
  playbacknet->linkControl("TimelineLabeler/tll/mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  playbacknet->linkControl("TimelineLabeler/tll/mrs_bool/advance", "SoundFileSource/src/mrs_bool/advance");

  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  //TimelineLabeler Config!! (should be set before setting the SoundFileSource filename control...)
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  playbacknet->updControl("TimelineLabeler/tll/mrs_bool/playRegionsOnly", false); //play whole file
  //playbacknet->updControl("TimelineLabeler/tll/mrs_bool/playRegionsOnly", true); //play only the defined regions
  //
  //playbacknet->updControl("TimelineLabeler/tll/mrs_string/selectLabel", ""); //consider any labeled region (audio outside regions will get a -1 label)
  playbacknet->updControl("TimelineLabeler/tll/mrs_string/selectLabel", "sing"); //consider only regions labeled as "sing" (all other regions and audio outside of any region will get a -1 label)
  //playbacknet->updControl("TimelineLabeler/tll/mrs_string/selectLabel", "guitar"); //consider only regions labeled as "guitar" (all other regions and audio outside of any region will get a -1 label)
  /////////////////////////////////////////////////////////////////////////////////////////////////////

  //SoundFileSource config
  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  while (playbacknet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {

    playbacknet->tick();

    //cout << "pos " << playbacknet->getctrl("mrs_natural/pos")->to<mrs_natural>() << endl;
    cout << "TimelineLabeler \"mrs_natural/current_label\" = " << playbacknet->getctrl("TimelineLabeler/tll/mrs_natural/currentLabel")->to<mrs_natural>() << endl;

    //toy_with if setting "mrs_natural/pos" to 0 for rewinding is working
    //if(playbacknet->getctrl("mrs_natural/pos")->to<mrs_natural>() > 100000)
    //	playbacknet->updControl("mrs_natural/pos", 0);
  }

  delete playbacknet;
}

void
toy_with_getControls(string sfName)
{
  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));


  MarSystem* newseries = mng.create("Series", "newseries");
  newseries->addMarSystem(mng.create("Gain", "g1"));
  newseries->addMarSystem(mng.create("Gain", "g2"));

  playbacknet->addMarSystem(newseries);


  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");


  cout << *playbacknet << endl;

  std::map<std::string, MarControlPtr> mycontrols = playbacknet->getControls();
  std::map<std::string, MarControlPtr>::iterator myc;

  for (myc = mycontrols.begin(); myc != mycontrols.end(); ++myc)
    cout << myc->first << endl;


  delete playbacknet;
}

void
toy_with_mono2stereo(string sfName)
{
  cout << "Mono2Stereo toy_with" << endl;

  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");

  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("Mono2Stereo", "m2s"));
  playbacknet->addMarSystem(mng.create("SoundFileSink", "dest"));

  string outName = "m2s.wav";

  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updControl("SoundFileSink/dest/mrs_string/filename", outName);


  while (playbacknet->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    playbacknet->tick();
  }

  delete playbacknet;
}

void
toy_with_fanoutswitch()
{
  cout << "Toy_Withing fanout switch" << endl;

  MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "src");
  MarSystem* src = mng.create("SoundFileSource", "src");
  src->updControl("mrs_string/filename", "/AudioDataBase/Music/musica1.wav");//"/Users/gtzan/data/sound/music_speech/music/gravity.au");

  pnet->addMarSystem(src);
  pnet->addMarSystem(mng.create("PlotSink", "psink1"));
  pnet->updControl("PlotSink/psink1/mrs_string/filename", "in");

  MarSystem* mix = mng.create("Fanout", "mix");
  MarSystem* g1 = mng.create("Gain", "g1");
  MarSystem* g2 = mng.create("Gain", "g2");
  MarSystem* g3 = mng.create("Gain", "g3");
  MarSystem* g4 = mng.create("Gain", "g4");

  g1->updControl("mrs_real/gain", 1.5);
  g2->updControl("mrs_real/gain", 2.5);
  g3->updControl("mrs_real/gain", 3.0);
  g4->updControl("mrs_real/gain", 4.0);

  mix->addMarSystem(g1);
  mix->addMarSystem(g2);
  mix->addMarSystem(g3);
  mix->addMarSystem(g4);

  pnet->addMarSystem(mix);
  pnet->addMarSystem(mng.create("PlotSink", "psink2"));
  pnet->updControl("PlotSink/psink2/mrs_string/filename", "out");

  //sedn to stdout
  pnet->updControl("PlotSink/psink2/mrs_bool/messages", true);

  //just a sample for easier visual inspection
  pnet->updControl("mrs_natural/inSamples", 1);

  // test mute
  realvec mute;
  mute.create(4);
  mute.setval(0.0);
  mute(2) = 1.0; //mute 3rd child
  cout << mute << endl;
  pnet->updControl("Fanout/mix/mrs_realvec/muted", mute);

  // test disabling/enabling a subset of Fanout branches
  //
  //pnet->updControl("Fanout/mix/mrs_natural/disable", 0);
  //pnet->updControl("Fanout/mix/mrs_natural/disable", 1);
  //pnet->updControl("Fanout/mix/mrs_natural/disable", 2);
  //pnet->updControl("Fanout/mix/mrs_natural/disable", 3);

  //pnet->updControl("Fanout/mix/mrs_string/enableChild", "Gain/g2");
  //pnet->updControl("Fanout/mix/mrs_string/enableChild", "Gain/g4");

  //pnet->updControl("Fanout/mix/mrs_realvec/enabled", tmp); //writting to this control makes nothing!

  // tick to check the result
  // PlotSinks are used for output
  pnet->tick();

  delete pnet;
}

void
toy_with_rmsilence(string sfName)
{
  cout << "Removing silences from: " << sfName << endl;
  MarSystemManager mng;

  MarSystem* rmnet = mng.create("Series", "rmnet");

  MarSystem* srm = mng.create("SilenceRemove", "srm");
  MarSystem* src = mng.create("SoundFileSource", "src");
  src->updControl("mrs_string/filename", sfName);
  srm->addMarSystem(src);


  rmnet->addMarSystem(srm);
  rmnet->addMarSystem(mng.create("SoundFileSink", "dest"));

  FileName fname(sfName);
  rmnet->updControl("SoundFileSink/dest/mrs_string/filename", "srm.wav");

  cout << *rmnet << endl;
  while (rmnet->getctrl("SilenceRemove/srm/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    rmnet->tick();
  }

  cout << "Finished removing silences. Output is " << "srm.wav" << endl;

  delete rmnet;
}

void
toy_with_multiple(mrs_string file1, mrs_string file2)
{
  MarSystemManager mng;
  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  MarSystem* fanout = mng.create("Fanout", "fanout");

  // Create the SoundFileSources
  fanout->addMarSystem(mng.create("SoundFileSource", "src1"));
  fanout->addMarSystem(mng.create("SoundFileSource", "src2"));

  // Assign filenames to the SoundFileSources
  fanout->updControl("SoundFileSource/src1/mrs_string/filename",file1);
  fanout->updControl("SoundFileSource/src2/mrs_string/filename",file2);

  // Add the fanout to the main network
  playbacknet->addMarSystem(fanout);

  // Sum up all of the fanouts
  playbacknet->addMarSystem(mng.create("Sum", "sum"));

  // Create the output file which is a SoundFileSink
  playbacknet->addMarSystem(mng.create("SoundFileSink", "dest"));
  playbacknet->updControl("SoundFileSink/dest/mrs_string/filename", "multiple.wav");

  while (playbacknet->getControl("Fanout/fanout/SoundFileSource/src1/mrs_bool/hasData")->to<mrs_bool>())
  {
    playbacknet->tick();
  }

}


void
toy_with_marsystemIO()
{
  cout << "Toy_Withing IO of MarSystems" << endl;
  MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");

  MarSystem* src = mng.create("SoundFileSource", "src");
  MarSystem* dest = mng.create("AudioSink", "dest");


  pnet->addMarSystem(src);
  pnet->addMarSystem(dest);

  pnet->updControl("mrs_natural/inSamples", 1024);


  ofstream oss;
  oss.open("marsystemIO.mpl");
  oss << *pnet << endl;
  ifstream iss;
  iss.open("marsystemIO.mpl");
  MarSystem* rsrc = mng.getMarSystem(iss);

  cout << *rsrc << endl;

  delete pnet;
}

void
toy_with_mixer(string sfName0, string sfName1, string sfName2)
{
  cout << "Mixing" << endl;
  cout << "File0 = " << sfName0 << endl;
  cout << "File1 = " << sfName1 << endl;

  MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");
  MarSystem* mix = mng.create("Fanout", "mix");

  MarSystem* branch0 = mng.create("Series", "branch0");
  branch0->addMarSystem(mng.create("SoundFileSource", "src0"));
  branch0->addMarSystem(mng.create("Gain", "gain0"));

  MarSystem* branch1 = mng.create("Series", "branch1");
  branch1->addMarSystem(mng.create("SoundFileSource", "src1"));
  branch1->addMarSystem(mng.create("Gain", "gain1"));

  mix->addMarSystem(branch0);
  mix->addMarSystem(branch1);

  pnet->addMarSystem(mix);
  pnet->addMarSystem(mng.create("Sum", "sum"));

  pnet->updControl("Fanout/mix/Series/branch0/SoundFileSource/src0/mrs_string/filename", sfName0);
  pnet->updControl("Fanout/mix/Series/branch1/SoundFileSource/src1/mrs_string/filename", sfName1);

  if (sfName2 == EMPTYSTRING)
    pnet->addMarSystem(mng.create("AudioSink", "dest"));
  else
  {
    pnet->addMarSystem(mng.create("SoundFileSink", "dest"));
    pnet->updControl("SoundFileSink/dest/mrs_string/filename", sfName2);
  }

  //pnet->updControl("Fanout/mix/Series/branch0/Gain/gain0/mrs_real/gain", 0.5);
  //pnet->updControl("Fanout/mix/Series/branch1/Gain/gain1/mrs_real/gain", 0.5);
  pnet->updControl("Sum/sum/mrs_string/mode","sum_samples");

  while(pnet->getctrl("Fanout/mix/Series/branch0/SoundFileSource/src0/mrs_bool/hasData")->to<mrs_bool>())
  {
    pnet->tick();
  }

  delete pnet;
}

void
toy_with_inSamples(string sfName)
{
  cout << "toy_with_inSamples: sfName = " << sfName << endl;
  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("Gain", "gain"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));
  // playbacknet->addMarSystem(mng.create("SoundFileSink", "dest"));


  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  //  playbacknet->updControl("SoundFileSink/dest/mrs_string/filename", "foo.wav");

  int i=1;
  // increment inSamples by 1 at every iteration
  while(playbacknet->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    playbacknet->updControl("mrs_natural/inSamples", i);
    playbacknet->tick();
    ++i;
    cout << "inSamples = " << i << endl;

  }

}

void
toy_with_fft(string sfName, int size)
{
  cout << "Toying with fft" << endl;
  MarSystemManager mng;

  MarSystem* series = mng.create("Series","network");
  series->addMarSystem(mng.create("SoundFileSource","src"));
  series->addMarSystem(mng.create("Spectrum", "spk"));
  // series->addMarSystem(mng.create("SpectralTransformations", "spectrans"));
  series->addMarSystem(mng.create("InvSpectrum", "ispk"));
  series->addMarSystem(mng.create("AudioSink", "dest"));

  // the name of the input file
  series->updControl("SoundFileSource/src/mrs_string/filename",
                     sfName);

  // the name of the output file
  // series->updControl("SoundFileSink/dest/mrs_string/filename",
  // "processed.wav");



  // number of samples to process each tick
  series->updControl("mrs_natural/inSamples",  size * 512);
  series->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  // cout << *series << endl;

  while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    series->tick();
  }

}


void
toy_with_rats(string sfName)
{
  cout << "Ultrasound rat vocalization detector " << sfName << endl;

  MarSystemManager mng;

  MarSystem* series = mng.create("Series","network");
  series->addMarSystem(mng.create("SoundFileSource","src"));
  series->addMarSystem(mng.create("Spectrum", "spk"));
  series->addMarSystem(mng.create("InvSpectrum", "ispk"));
  MarSystem* dest = mng.create("SoundFileSink", "dest");
  MarSystem* dest1 = mng.create("SoundFileSink", "dest1");

  mrs_natural i =0;

  series->updControl("SoundFileSource/src/mrs_string/filename",
                     sfName);




  series->updControl("mrs_natural/inSamples", 8192);

  series->updControl("Spectrum/spk/mrs_real/cutoff", 0.55);
  series->updControl("Spectrum/spk/mrs_real/lowcutoff", 0.45);

  mrs_realvec input(series->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                    series->getctrl("mrs_natural/inSamples")->to<mrs_natural>());


  mrs_realvec output(series->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                     series->getctrl("mrs_natural/onSamples")->to<mrs_natural>());


  mrs_realvec zero(series->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                   series->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  zero.setval(0.0);


  dest->updControl("mrs_natural/inSamples", 8192);
  dest->updControl("mrs_real/israte", 192000.0);
  dest->updControl("mrs_string/filename",  "detected_rats.wav");

  dest1->updControl("mrs_natural/inSamples", 8192);
  dest1->updControl("mrs_real/israte", 192000.0);
  dest1->updControl("mrs_string/filename",  "only_rats.wav");

  int sample_count =0;

  ofstream oss;
  oss.open("rat_log.txt");

  while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    series->process(input, output);

    mrs_realvec v =
      series->getctrl("SoundFileSource/src/mrs_realvec/processedData")->to<mrs_realvec>();
    mrs_real rms = 0.0;
    int count = 0;

    for (int t = 0; t < 8192; t++)
    {
      rms += (output(0,t) * output(0,t));
      count++;
    }
    rms /= count;
    rms = sqrt(rms);
    if (rms > 0.001)
    {
      oss << fixed << setprecision(2) << (sample_count / 192000.0) << "\trats" << endl;
      dest->process(v, output);
      dest1->process(v,output);
    }

    else
    {
      oss << fixed << setprecision(2) << (sample_count / 192000.0) << "\tbackground" << endl;
      dest->process(zero, zero);
    }


    ++i;
    sample_count += 8192;
  }

  cout << "File with silenced background : " << "detected_rats.wav" << endl;
  cout << "File with background removed  : " << "only_rats.wav" << endl;
  cout << "Log file with rat/background times in seconds : " << "rat_log.txt" << endl;


  delete series;
}

void
toy_with_parallel()
{
  MarSystemManager mng;
  MarSystem *parallel = mng.create("Parallel", "parallel");

  realvec in;
  in.create(mrs_natural(10), mrs_natural(10));
  for (mrs_natural i = 0; i < in.getRows(); ++i) {
    for (mrs_natural j = 0; j < in.getCols(); j++) {
      in(i,j) = i*j;
    }
  }

  realvec out;
  out.create(in.getRows(),in.getCols());

  MarSystem* g0 = mng.create("Gain", "g0");
  g0->setctrl("mrs_natural/inObservations", mrs_natural(3));
  g0->setctrl("mrs_natural/inSamples", in.getCols());
  g0->setctrl("mrs_real/gain", 3.0f);

  MarSystem* g1 = mng.create("Gain", "g1");
  g1->setctrl("mrs_natural/inObservations", mrs_natural(2));
  g1->setctrl("mrs_real/gain", 2.0f);

  MarSystem* g2 = mng.create("Gain", "g2");
  g2->setctrl("mrs_natural/inObservations", mrs_natural(5));
  g2->setctrl("mrs_real/gain", 5.0f);

  parallel->addMarSystem(g0);
  parallel->addMarSystem(g1);
  parallel->addMarSystem(g2);

  parallel->process(in, out);

  cout << out << endl;

  delete parallel;
}


void toy_with_phase(string fname, int size)
{
  cout << "Toying with phase" << endl;

  MarSystemManager mng;


  MarSystem* net = mng.create("Series/net");
  net->addMarSystem(mng.create("SoundFileSource/src"));
  net->addMarSystem(mng.create("ShiftInput/si"));
  net->addMarSystem(mng.create("PvFold/fo"));
  net->addMarSystem(mng.create("Spectrum/spk"));
  net->addMarSystem(mng.create("PhaseRandomize/prandom"));
  net->addMarSystem(mng.create("InvSpectrum/ispk"));
  net->addMarSystem(mng.create("PvOverlapadd/pover"));
  net->addMarSystem(mng.create("ShiftOutput/so"));
  // net->addMarSystem(mng.create("AudioSink/dest"));
  net->addMarSystem(mng.create("SoundFileSink/dest"));


  mrs_natural winSize;
  mrs_natural hopSize;


  winSize = (mrs_natural)pow(2.0, size);
  hopSize = (mrs_natural)pow(2.0, size-1);

  cout << "winSize = " << winSize << endl;
  cout << "hopSize = " << hopSize << endl;

  net->updControl("ShiftInput/si/mrs_natural/winSize", winSize);
  net->updControl("ShiftOutput/so/mrs_natural/Interpolation", hopSize);
  net->updControl("mrs_natural/inSamples", hopSize);
  net->updControl("PvFold/fo/mrs_natural/FFTSize", winSize);
  net->updControl("PvOverlapadd/pover/mrs_natural/FFTSize", winSize);
  net->updControl("PvOverlapadd/pover/mrs_natural/winSize", winSize);
  net->updControl("PvOverlapadd/pover/mrs_natural/Decimation", hopSize);
  net->updControl("PvOverlapadd/pover/mrs_natural/Interpolation", hopSize);

  net->updControl("SoundFileSource/src/mrs_string/filename", fname);
  // net->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  net->updControl("SoundFileSink/dest/mrs_string/filename", "processed.wav");

  int sample_count = 0;


  while(net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    winSize = (mrs_natural)pow(2.0, size);
    hopSize = (mrs_natural)pow(2.0, size-1);

    cout << "winSize = " << winSize << endl;
    cout << "hopSize = " << hopSize << endl;
    net->updControl("ShiftInput/si/mrs_natural/winSize", winSize);
    net->updControl("ShiftOutput/so/mrs_natural/Interpolation", hopSize);
    net->updControl("PvFold/fo/mrs_natural/FFTSize", winSize);
    net->updControl("PvOverlapadd/pover/mrs_natural/FFTSize", winSize);
    net->updControl("PvOverlapadd/pover/mrs_natural/winSize", winSize);
    net->updControl("PvOverlapadd/pover/mrs_natural/Decimation", hopSize);
    net->updControl("PvOverlapadd/pover/mrs_natural/Interpolation", hopSize);
    net->updControl("mrs_natural/inSamples", hopSize);

    sample_count += hopSize;

    net->tick();
    if (sample_count >= 2 * 44100)
    {
      size ++;
      sample_count = 0;
    }

  }


}


void toy_with_pngwriter(string fname)
{
  cout << "Toying with png writer" << endl;
  cout << "Will only work if Marsyas is successfully compiled with PNG support" << endl;



#ifdef MARSYAS_PNG
  cout << "starting PNG processing " << endl;

  MarSystemManager mng;

  // A series to contain everything
  MarSystem* series = mng.create("Series", "series");

  // The sound file
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->updControl("SoundFileSource/src/mrs_string/filename", fname);

  // Compute the AbsMax of this window
  series->addMarSystem(mng.create("AbsMax","absmax"));


  realvec processedData;
  double normalizedItem;

  int length = 0;
  double min = 99999999999.9;
  double max = -99999999999.9;


  while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())  {
    series->tick();
    length++;

    processedData = series->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    if (processedData(0) < min)
      min = processedData(0);
    if (processedData(0) > max)
      max = processedData(0);
  }

  int height = 128;
  int middle = height/2;

  cout << "min = " << min << endl;
  cout << "max = " << max << endl;
  cout << "length = " << length << endl;


  pngwriter png(length,height,0, "waveform.png");

  series->updControl("SoundFileSource/src/mrs_natural/pos", 0);
  series->tick();

  // series->updControl("SoundFileSource/src/mrs_string/filename", "foo.wav");
  // series->updControl("SoundFileSource/src/mrs_string/filename", fname);

  double normalizedData;

  // Give it a white background

  png.invert();

  // A line across the middle of the plot
  png.line(0,middle,length,middle,0.5,0.5,1.0);

  double x = 0;
  double y = 0;
  while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())  {

    series->tick();
    processedData = series->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    normalizedData = (processedData(0) - min) / (max - min);

//  	cout << "p=" << processedData(0) << " n=" << normalizedData << endl;

    y = processedData(0) / 2.0 * height;
    // Dark blue full
    png.line(x,middle,x,middle+y,0.0,0.0,1.0);
    png.line(x,middle,x,middle-y,0.0,0.0,1.0);

    // Light blue 50%
    png.line(x,middle,x,middle+y*0.5,0.5,0.5,1.0);
    png.line(x,middle,x,middle-y*0.5,0.5,0.5,1.0);

    x++;

  }

  png.close();

#else
  (void) fname;
#endif
}

void
toy_with_probe()
{
  cout << "Toy_Withing probe functionality" << endl;

  // create the Marsyas
  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");
  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("Gain", "gain"));
  pnet->addMarSystem(mng.create("Gain", "gain2"));

  cout << "BEFORE PROBE " << endl;
  cout << "pnet = " << *pnet << endl;
  pnet->updControl("mrs_bool/probe", true);
  cout << "AFTER PROBE" << endl;

  cout << "AFTER ONE TICK" << endl;
  pnet->tick();
  cout << "pnet = " << *pnet << endl;
}






void
toy_with_knn()
{
  MarSystemManager mng;
  MarSystem *knn = mng.create("KNNClassifier", "knn");

  // ---- TOY_WITH TRAIN ---------------------

  knn->updControl("mrs_string/mode", "train");

  mrs_natural inS = 9;
  mrs_natural inO = 3;

  realvec input(inO, inS);

  input(0,0) = 0.0;
  input(0,1) = 1.0;
  input(0,2) = 2.0;
  input(0,3) = 3.0;
  input(0,4) = 4.0;
  input(0,5) = 5.0;
  input(0,6) = 6.0;
  input(0,7) = 7.0;
  input(0,8) = 8.0;
  input(1,0) = 0.0;
  input(1,1) = 1.1;
  input(1,2) = 2.1;
  input(1,3) = 3.1;
  input(1,4) = 4.1;
  input(1,5) = 5.1;
  input(1,6) = 6.1;
  input(1,7) = 7.1;
  input(1,8) = 8.1;

  // annotate :)
  input(2,0) = 0.0;
  input(2,1) = 0.0;
  input(2,2) = 0.0;
  input(2,3) = 0.0;
  input(2,4) = 1.0;
  input(2,5) = 1.0;
  input(2,6) = 1.0;
  input(2,7) = 1.0;
  input(2,8) = 1.0;

  knn->updControl("mrs_natural/inSamples", inS);
  knn->updControl("mrs_natural/inObservations", inO);

  realvec output(knn->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), knn->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  cout << "INPUT: " << input << endl;

  knn->process(input, output);
  cout << "TOY_WITH: " << output << endl;

  // IMPORTANT updcontrol done and then process to indicate to KNN to finish
  knn->updControl("mrs_bool/done", true);
  knn->tick();

  // --------------- TOY_WITH PREDICT -----------------
  knn->updControl("mrs_string/mode", "predict");
  knn->updControl("mrs_natural/k", 3);
  knn->updControl("mrs_natural/nLabels", 2);
  inS = 1;
  inO = 3;

  realvec input2(inO, inS);

  input2(0,0) = 3.0;
  input2(1,0) = 3.2;
  input2(2,0) = 1.0;

  knn->updControl("mrs_natural/inSamples", inS);
  knn->updControl("mrs_natural/inObservations", inO);

  realvec output2(knn->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), knn->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  cout << "Predict" << endl;
  knn->process(input2, output2);

  cout << "PREDICT: " << output2 << endl;

  delete knn;
}

// toy_with filter
void
toy_with_filter()
{
  // Toy_With 1

  /* realvec a(3),b(3);
     Filter* f = new Filter("f");
     a(0) = 1.0f;
     a(1) = 0.0f;
     a(2) = 0.0f;
     b(0) = 1.0f;
     b(1) = -0.9f;
     b(2) = 0.0f;

     f->setctrl("mrs_realvec/ncoeffs", a);
     f->setctrl("mrs_realvec/dcoeffs", b);
     f->setctrl("mrs_natural/inSamples", mrs_natural(5));
     f->setctrl("mrs_natural/inObservations", mrs_natural(2));
     f->setctrl("mrs_real/israte", 44100.0f);
     f->update();

     realvec in, out;
     in.create(mrs_natural(2),mrs_natural(5));
     in(0,0) = 1.0f;
     in(1,0) = 1.0f;
     out.create(mrs_natural(2),mrs_natural(5));

     f->process(in, out);
     cout << out << endl;
  */

  // Toy_With 2

  MarSystemManager mng;

  MarSystem* f = mng.create("Filter", "f");

  realvec al(5),bl(5);

  al(0) = 1.0;
  al(1) = -3.9680;
  al(2) = 5.9062;
  al(3) = -3.9084;
  al(4) = 0.9702;

  bl(0) = 0.0001125;
  bl(1) = 0.0;
  bl(2) = -0.0002250;
  bl(3) = 0.0;
  bl(4) = 0.0001125;

  realvec ah(5),bh(5);
  ah(0) = 1.0;
  ah(1) = -3.5797;
  ah(2) = 4.9370;
  ah(3) = -3.1066;
  ah(4) = 0.7542;

  bh(0) = 0.0087;
  bh(1) = 0.0;
  bh(2) = -0.0174;
  bh(3) = 0;
  bh(4) = 0.0087;

  f->setctrl("mrs_realvec/ncoeffs", bl);
  f->setctrl("mrs_realvec/dcoeffs", al);
  f->setctrl("mrs_natural/inSamples", mrs_natural(20));
  f->setctrl("mrs_natural/inObservations", mrs_natural(2));
  f->setctrl("mrs_real/israte", 44100.0f);
  f->update();

  realvec in, out;
  in.create(mrs_natural(2),mrs_natural(20));
  in(0,0) = 1.0f;
  in(1,0) = 1.0f;
  out.create(mrs_natural(2),mrs_natural(20));

  f->process(in, out);
  cout << out << endl;

  delete f;
}

void
toy_with_panorama(string sfName)
{
  cout << "Toy_Withing panorama amplitude panning" << endl;
  MarSystemManager mng;
  MarSystem* playbacknet = mng.create("Series", "playbacknet");

  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("Panorama", "pan"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));
  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
  playbacknet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  mrs_bool isEmpty;
  mrs_natural t = 0;
  mrs_real angle = -PI/4.0;
  playbacknet->updControl("Panorama/pan/mrs_real/angle", angle);
  while ((isEmpty = playbacknet->getctrl("mrs_bool/hasData")->to<mrs_bool>()) != true)
  {
    playbacknet->tick();
    t++;
    if (t % 10 == 0)
    {
      angle += (PI/32);
      playbacknet->updControl("Panorama/pan/mrs_real/angle", angle);
    }
  }
}


void
toy_with_reverb(string sfName)
{
  cout << "Toy-with reverb" << endl;
  MarSystemManager mng;
  MarSystem* playbacknet = mng.create("Series", "playbacknet");

  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  MarSystem* filterbank = mng.create("Fanout", "filterbank");
  filterbank->addMarSystem(mng.create("Filter", "cf8"));
  filterbank->addMarSystem(mng.create("Filter", "cf12"));
  filterbank->addMarSystem(mng.create("Filter", "cf18"));
  filterbank->addMarSystem(mng.create("Filter", "cf20"));

  realvec cf8_a(8);
  realvec cf8_b(9);
  cf8_a.setval(0.0);
  cf8_b.setval(0.0);
  cf8_a(0) = 1.0;
  cf8_b(0) = 1.0;
  cf8_b(8) = -0.9227;

  realvec cf12_a(12);
  realvec cf12_b(13);
  cf12_a.setval(0.0);
  cf12_b.setval(0.0);
  cf12_a(0) = 1.0;
  cf12_b(0) = 1.0;
  cf12_b(12) = -0.8864;

  realvec cf18_a(18);
  realvec cf18_b(19);
  cf18_a.setval(0.0);
  cf18_b.setval(0.0);
  cf18_a(0) = 1.0;
  cf18_b(0) = 1.0;
  cf18_b(18) = -0.9137;

  realvec cf20_a(20);
  realvec cf20_b(21);
  cf20_a.setval(0.0);
  cf20_b.setval(0.0);
  cf20_a(0) = 1.0;
  cf20_b(0) = 1.0;
  cf20_b(20) = -0.8866;

  filterbank->updControl("Filter/cf8/mrs_realvec/ncoeffs", cf8_a);
  filterbank->updControl("Filter/cf8/mrs_realvec/dcoeffs", cf8_b);
  filterbank->updControl("Filter/cf12/mrs_realvec/ncoeffs", cf12_a);
  filterbank->updControl("Filter/cf12/mrs_realvec/dcoeffs", cf12_b);
  filterbank->updControl("Filter/cf18/mrs_realvec/ncoeffs", cf18_a);
  filterbank->updControl("Filter/cf18/mrs_realvec/dcoeffs", cf18_b);
  filterbank->updControl("Filter/cf20/mrs_realvec/ncoeffs", cf20_a);
  filterbank->updControl("Filter/cf20/mrs_realvec/dcoeffs", cf20_b);



  playbacknet->addMarSystem(mng.create("Gain", "gain"));
  playbacknet->addMarSystem(filterbank);
  playbacknet->addMarSystem(mng.create("Filter", "allpass"));
  playbacknet->addMarSystem(mng.create("Filter", "allpass1"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  realvec apf_a(12);
  realvec apf_b(12);
  apf_a.setval(0.0);
  apf_b.setval(0.0);
  apf_a(0) = -0.1989;
  apf_a(11) = 1.0;
  apf_b(0) = 1.0;
  apf_b(11) = -0.1989;

  playbacknet->updControl("Filter/allpass/mrs_realvec/ncoeffs", apf_a);
  playbacknet->updControl("Filter/allpass/mrs_realvec/dcoeffs", apf_b);
  playbacknet->updControl("Filter/allpass1/mrs_realvec/ncoeffs", apf_a);
  playbacknet->updControl("Filter/allpass1/mrs_realvec/dcoeffs", apf_b);
  playbacknet->updControl("Gain/gain/mrs_real/gain", 0.25);
  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  cout << "sfName " << sfName << endl;
  mrs_bool isEmpty;

  playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

  while ((isEmpty = playbacknet->getctrl("mrs_bool/hasData")->to<mrs_bool>()) != true)
  {
    playbacknet->tick();
  }



}

// moved into regressionChecks on July 14.  -gp
void
toy_with_vibrato(string sfName)
{
  cout << "Toy_Withing vibrato" << endl;

  MarSystemManager mng;
  MarSystem* playbacknet = mng.create("Series", "playbacknet");

  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("Vibrato", "vib"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
  playbacknet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");

  playbacknet->updControl("Vibrato/vib/mrs_real/mod_freq", 10.0);


  mrs_bool isEmpty;
  while ((isEmpty = playbacknet->getctrl("mrs_bool/hasData")->to<mrs_bool>()) != true)
  {
    playbacknet->tick();
  }
}


// toy_with input,processing and sonification
// of Vicon (motion capture system)

void
toy_with_vicon(string vfName)
{
  if (vfName != EMPTYSTRING)
    cout << "Toy_Withing Vicon file: " << vfName << endl;
  else
  {
    cout << "No vicon file specified" << endl;
    return;
  }

  MarSystemManager mng;
  MarSystem *viconNet = mng.create("Series", "viconNet");
  viconNet->addMarSystem(mng.create("ViconFileSource", "vsrc"));

  // viconNet->addMarSystem(mng.create("Vicon2Ctrl", "vctrl"));
  // viconNet->addMarSystem(mng.create("PlotSink", "vsrc"));

  viconNet->updControl("mrs_natural/inSamples", 1);
  viconNet->updControl("ViconFileSource/vsrc/mrs_string/filename", vfName);
  viconNet->updControl("mrs_real/israte", 120.0);

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  MarSystem* sinebank = mng.create("Fanout", "sinebank");
  sinebank->addMarSystem(mng.create("SineSource", "ssrc1"));
  sinebank->addMarSystem(mng.create("SineSource", "ssrc2"));
  sinebank->addMarSystem(mng.create("SineSource", "ssrc3"));
  sinebank->addMarSystem(mng.create("SineSource", "ssrc4"));
  sinebank->addMarSystem(mng.create("SineSource", "ssrc5"));
  sinebank->addMarSystem(mng.create("SineSource", "ssrc6"));
  sinebank->addMarSystem(mng.create("SineSource", "ssrc7"));
  sinebank->addMarSystem(mng.create("SineSource", "ssrc8"));
  sinebank->addMarSystem(mng.create("SineSource", "ssrc9"));
  sinebank->addMarSystem(mng.create("SineSource", "ssrc10"));
  sinebank->addMarSystem(mng.create("SineSource", "ssrc11"));
  sinebank->addMarSystem(mng.create("SineSource", "ssrc12"));

  playbacknet->addMarSystem(sinebank);
  playbacknet->addMarSystem(mng.create("Sum", "sum"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  realvec in(viconNet->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
             viconNet->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec out(viconNet->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
              viconNet->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  playbacknet->updControl("mrs_natural/inSamples", 184);

  // set message to STK
  /* cout << "ControlChange    0.0  1  44 24.000000" << endl;
     cout << "AfterTouch       0.0 1 64.000000" << endl;
     cout << "PitchChange      0.0 1 64.000" << endl;
     cout << "ControlChange    0.0 1  2 20.000000" << endl;
     cout << "ControlChange    0.0 1  4 64.000000" << endl;
     cout << "ControlChange    0.0 1  11 64.000000" << endl;
     cout << "ControlChange    0.0 1  1 0.000000" << endl;
     cout << "NoteOn           0.0 1 64.000000 64.000000" << endl;
  */

  while (viconNet->getctrl("ViconFileSource/vsrc/mrs_bool/hasData")->to<mrs_bool>())
  {
    viconNet->process(in,out);

    // out(1,0) = 0.0;
    // out(2,0) = 0.0;
    // out(3,0) = 0.0;

    out(7,0) = 0.0;
    out(8,0) = 0.0;
    out(9,0) = 0.0;

    out(10,0) = 0.0;
    out(11,0) = 0.0;
    out(12,0) = 0.0;

    out(13,0) = 0.0;
    out(14,0) = 0.0;
    out(15,0) = 0.0;

    out(16,0) = 0.0;
    out(17,0) = 0.0;
    out(18,0) = 0.0;

    // playbacknet->updControl("Fanout/sinebank/SineSource/ssrc1/mrs_real/frequency", fabs(out(13,0)));
    // playbacknet->updControl("Fanout/sinebank/SineSource/ssrc1/mrs_real/frequency", fabs(fabs(out(1.0))));

    playbacknet->updControl("Fanout/sinebank/SineSource/ssrc2/mrs_real/frequency", fabs(out(2,0)));
    playbacknet->updControl("Fanout/sinebank/SineSource/ssrc3/mrs_real/frequency", fabs(out(3,0)));

    playbacknet->updControl("Fanout/sinebank/SineSource/ssrc4/mrs_real/frequency", fabs(out(7,0)));
    playbacknet->updControl("Fanout/sinebank/SineSource/ssrc5/mrs_real/frequency", fabs(out(8,0)));
    playbacknet->updControl("Fanout/sinebank/SineSource/ssrc6/mrs_real/frequency", fabs(out(9,0)));

    playbacknet->updControl("Fanout/sinebank/SineSource/ssrc7/mrs_real/frequency", fabs(out(10,0)));
    playbacknet->updControl("Fanout/sinebank/SineSource/ssrc8/mrs_real/frequency", fabs(out(11,0)));
    playbacknet->updControl("Fanout/sinebank/SineSource/ssrc9/mrs_real/frequency", fabs(out(12,0)));

    playbacknet->updControl("Fanout/sinebank/SineSource/ssrc10/mrs_real/frequency", fabs(out(16,0)));
    playbacknet->updControl("Fanout/sinebank/SineSource/ssrc11/mrs_real/frequency", fabs(out(17,0)));
    playbacknet->updControl("Fanout/sinebank/SineSource/ssrc12/mrs_real/frequency", fabs(out(18,0)));

    // cout << out(13,0) << endl;

    // cout << "AfterTouch       -1.0 1 " << fabs((out(1,0) / 18.0)) << endl;
    // usleep(50000);

    // cout << "AfterTouch       -1.0 1 " << fabs((out(1,0) / 18.0)) << endl;
    // cout << "PitchChange       0.0 1 " << fabs((out(1,0) / 18.0)) << endl;

    playbacknet->tick();
  }

  // cout << viconNet->getctrl("ViconFileSource/vsrc/mrs_string/markers") << endl;
  // cout << "Sample Rate: " << viconNet->getctrl("ViconFileSource/vsrc/mrs_real/israte") << endl;

  delete viconNet;
  delete playbacknet;
}

void
toy_with_MidiFileSynthSource(string sfName)
{
  cout << "Playing " << sfName << endl;

  MarSystemManager mng;

  // Create a series Composite
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("MidiFileSynthSource", "src"));
  series->addMarSystem(mng.create("AudioSink", "dest"));

  // only update controls from Composite level
  series->updControl("mrs_natural/inSamples", 512);
  series->updControl("mrs_real/israte", 44100.0);
  series->updControl("MidiFileSynthSource/src/mrs_real/start", 0.0);//in seconds
  // MIDIToolbox prompts for user confirmation if the MIDI file is too long [!]
  // Must find a way to avoid it otherwise it	breaks C++ execution!
  series->updControl("MidiFileSynthSource/src/mrs_real/end", 10.0);

  series->updControl("MidiFileSynthSource/src/mrs_string/filename", sfName);

  series->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  while(series->getctrl("MidiFileSynthSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    series->tick();
    cout << "Number of playing notes: " << series->getctrl("MidiFileSynthSource/src/mrs_natural/nActiveNotes")->to<mrs_natural>() << endl;
    //cout << "Pos: " << series->getctrl("MidiFileSynthSource/src/mrs_natural/pos")->to<mrs_natural>() << endl;
  }

  delete series;

}

void
toy_with_MATLABengine()
{
  //In order to toy_with the MATLABengine class
  // the following define must be set:
  //	  MARSYAS_MATLAB
  //
  // To build this toy_with with MATLAB engine support, please consult the following site
  // for detailed info:
  //
  // http://www.mathworks.com/access/helpdesk/help/techdoc/matlab_external/f39903.html
  //
  // <lmartins@inescporto.pt> - 17.06.2006

#ifdef MARSYAS_MATLAB

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TOY_WITH evaluate MATLAB command" << endl;
  cout << "******************************************************" << endl;
  cout << endl << "Run MATLAB benchmark utility..." << endl;
  MATLAB_EVAL("bench;");
  cout << endl << "Press any key to continue..." << endl;
  cout << endl << "Run other MATLAB stuff..." << endl;
  MATLAB_EVAL("a = magic(10);");
  MATLAB_EVAL("figure(3)");
  MATLAB_EVAL("imagesc(a);");
  MATLAB_EVAL("clear a;");
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TOY_WITH mrs_natural" << endl;
  cout << "******************************************************" << endl;
  mrs_natural Marsyas_natural = 123456789;
  cout << "Send a mrs_natural to MATLAB: " << Marsyas_natural << endl;
  MATLAB_PUT(Marsyas_natural,"Marsyas_natural");
  cout << endl << "Variable sent. Check MATLAB variable 'Marsyas_natural' and compare values..." << endl;
  getchar();
  Marsyas_natural = 0;
  if(MATLAB_GET("Marsyas_natural", Marsyas_natural) == 0)
    cout << "Get it from MATLAB back to Marsyas: " << Marsyas_natural << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TOY_WITH mrs_real" << endl;
  cout << "******************************************************" << endl;
  mrs_real Marsyas_real = 3.123456789;
  cout << "Send a mrs_real to MATLAB: " << Marsyas_real << endl;
  MATLAB_PUT(Marsyas_real,"Marsyas_real");
  cout << endl << "Variable sent: check MATLAB variable 'Marsyas_real' and compare values..." << endl;
  getchar();
  Marsyas_real = 0.0;
  if(MATLAB_GET("Marsyas_real", Marsyas_real)== 0)
    cout << "Get it from MATLAB back to Marsyas: " << Marsyas_real << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TOY_WITH mrs_complex" << endl;
  cout << "******************************************************" << endl;
  mrs_complex Marsyas_complex = mrs_complex(1.123456789, 2.123456789);
  cout << "Send a mrs_complex to MATLAB: " << Marsyas_complex.real() << " + j" << Marsyas_complex.imag() << endl;
  MATLAB_PUT(Marsyas_complex,"Marsyas_complex");
  cout << endl << "Variable sent: check MATLAB variable 'Marsyas_complex' and compare values..." << endl;
  getchar();
  Marsyas_complex = mrs_complex(0.0, 0.0);
  if(MATLAB_GET("Marsyas_complex", Marsyas_complex) == 0)
    cout << "Get it from MATLAB back to Marsyas: " << Marsyas_complex.real() << " + j" << Marsyas_complex.imag() << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TOY_WITH realvec (row vector)" << endl;
  cout << "******************************************************" << endl;
  realvec marRow_realvec1D(4);//Marsyas row vector
  marRow_realvec1D(0) = 1.123456789;
  marRow_realvec1D(1) = 2.123456789;
  marRow_realvec1D(2) = 3.123456789;
  marRow_realvec1D(3) = 4.123456789;
  cout << "Send a realvec to MATLAB: " << endl;
  cout << endl << marRow_realvec1D  << endl;
  MATLAB_PUT(marRow_realvec1D,"marRow_realvec1D");
  cout << endl << "Variable sent: check MATLAB variable 'marRow_realvec1D' and compare values..." << endl;
  getchar();
  marRow_realvec1D.setval(0.0);
  if(MATLAB_GET("marRow_realvec1D", marRow_realvec1D) == 0)
    cout << "Get it from MATLAB back to Marsyas: " << endl << endl << marRow_realvec1D << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TOY_WITH realvec (col vector)" << endl;
  cout << "******************************************************" << endl;
  realvec marCol_realvec1D(4,1);//Marsyas col vector
  marCol_realvec1D(0) = 1.123456789;
  marCol_realvec1D(1) = 2.123456789;
  marCol_realvec1D(2) = 3.123456789;
  marCol_realvec1D(3) = 4.123456789;
  cout << "Send a realvec to MATLAB: " << endl;
  cout << endl << marCol_realvec1D  << endl;
  MATLAB_PUT(marCol_realvec1D,"marCol_realvec1D");
  cout << endl << "Variable sent: check MATLAB variable 'marCol_realvec1D' and compare values..." << endl;
  getchar();
  marCol_realvec1D.setval(0.0);
  if(MATLAB_GET("marCol_realvec1D", marCol_realvec1D) == 0)
    cout << "Get it from MATLAB back to Marsyas: " << endl << endl << marCol_realvec1D << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TOY_WITH realvec (2D array)" << endl;
  cout << "******************************************************" << endl;
  realvec marsyas_realvec2D(2,3);//Marsyas matrix: 2 rows; 3 columns
  marsyas_realvec2D(0,0) = 0.0;
  marsyas_realvec2D(0,1) = 0.1;
  marsyas_realvec2D(0,2) = 0.2;
  marsyas_realvec2D(1,0) = 1.0;
  marsyas_realvec2D(1,1) = 1.1;
  marsyas_realvec2D(1,2) = 1.2;
  cout << "Send a realvec to MATLAB: " << endl;
  cout << endl << marsyas_realvec2D  << endl;
  MATLAB_PUT(marsyas_realvec2D,"marsyas_realvec2D");
  cout << endl << "Variable sent: check MATLAB variable 'marsyas_realvec2D' and compare values..." << endl;
  getchar();
  marsyas_realvec2D.setval(0.0);
  if(MATLAB_GET("marsyas_realvec2D", marsyas_realvec2D)==0)
    cout << "Get it from MATLAB back to Marsyas: " << endl << endl << marsyas_realvec2D << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TOY_WITH std::vector<mrs_natural>" << endl;
  cout << "******************************************************" << endl;
  vector<mrs_natural> vector_natural(4);
  vector_natural[0] = 1;
  vector_natural[1] = 2;
  vector_natural[2] = 3;
  vector_natural[3] = 4;
  cout << "Send a std::vector<mrs_natural> to MATLAB: " << endl;
  cout << "vector_natural[0] = " << vector_natural[0] << endl;
  cout << "vector_natural[1] = " << vector_natural[1] << endl;
  cout << "vector_natural[2] = " << vector_natural[2] << endl;
  cout << "vector_natural[3] = " << vector_natural[3] << endl;

  MATLAB_PUT(vector_natural,"vector_natural");
  cout << endl << "Variable sent: check MATLAB variable 'vector_natural' and compare values..." << endl;
  getchar();
  vector_natural.clear();
  if(MATLAB_GET("vector_natural", vector_natural)==0)
  {
    cout << "Get it from MATLAB back to Marsyas: " << endl;
    cout << "vector_natural[0] = " << vector_natural[0] << endl;
    cout << "vector_natural[1] = " << vector_natural[1] << endl;
    cout << "vector_natural[2] = " << vector_natural[2] << endl;
    cout << "vector_natural[3] = " << vector_natural[3] << endl;
  }
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TOY_WITH std::vector<mrs_real>" << endl;
  cout << "******************************************************" << endl;
  vector<mrs_real> vector_real(4);
  vector_real[0] = 1.123456789;
  vector_real[1] = 2.123456789;
  vector_real[2] = 3.123456789;
  vector_real[3] = 4.123456789;
  cout << "Send a std::vector<mrs_real> to MATLAB: " << endl;
  cout << "vector_real[0] = " << vector_real[0] << endl;
  cout << "vector_real[1] = " << vector_real[1] << endl;
  cout << "vector_real[2] = " << vector_real[2] << endl;
  cout << "vector_real[3] = " << vector_real[3] << endl;

  MATLAB_PUT(vector_real,"vector_real");
  cout << endl << "Variable sent: check MATLAB variable 'vector_real' and compare values..." << endl;
  getchar();
  vector_real.clear();
  if(MATLAB_GET("vector_real", vector_real)==0)
  {
    cout << "Get it from MATLAB back to Marsyas: " << endl;
    cout << "vector_real[0] = " << vector_real[0] << endl;
    cout << "vector_real[1] = " << vector_real[1] << endl;
    cout << "vector_real[2] = " << vector_real[2] << endl;
    cout << "vector_real[3] = " << vector_real[3] << endl;
  }
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TOY_WITH std::vector<mrs_complex>" << endl;
  cout << "******************************************************" << endl;
  vector<mrs_complex> vector_complex(4);
  vector_complex[0] = mrs_complex(1.123456789, 2.123456789);
  vector_complex[1] = mrs_complex(3.123456789, 4.123456789);
  vector_complex[2] = mrs_complex(5.123456789, 6.123456789);
  vector_complex[3] = mrs_complex(7.123456789, 8.123456789);
  cout << "Send a std::vector<mrs_complex> to MATLAB: " << endl;
  cout << "vector_complex[0] = " << vector_complex[0].real() << " + j" << vector_complex[0].imag() << endl;
  cout << "vector_complex[1] = " << vector_complex[1].real() << " + j" << vector_complex[1].imag() << endl;
  cout << "vector_complex[2] = " << vector_complex[2].real() << " + j" << vector_complex[2].imag() << endl;
  cout << "vector_complex[3] = " << vector_complex[3].real() << " + j" << vector_complex[3].imag() << endl;

  MATLAB_PUT(vector_complex,"vector_complex");
  cout << endl << "Variable sent: check MATLAB variable 'vector_complex' and compare values..." << endl;
  getchar();
  vector_complex.clear();
  if(MATLAB_GET("vector_complex", vector_complex)==0)
  {
    cout << "Get it from MATLAB back to Marsyas: " << endl;
    cout << "vector_complex[0] = " << vector_complex[0].real() << " + j" << vector_complex[0].imag() << endl;
    cout << "vector_complex[1] = " << vector_complex[1].real() << " + j" << vector_complex[1].imag() << endl;
    cout << "vector_complex[2] = " << vector_complex[2].real() << " + j" << vector_complex[2].imag() << endl;
    cout << "vector_complex[3] = " << vector_complex[3].real() << " + j" << vector_complex[3].imag() << endl;
  }
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

#else
  cout << endl << "MATLAB Engine not configured! Not possible to run toy_with..." << endl;
  cout << "To build this toy_with with MATLAB engine support, check:" << endl << endl;
  cout << "http://www.mathworks.com/access/helpdesk/help/techdoc/matlab_external/f39903.html" << endl;
  getchar();

#endif
}

void
toy_with_LPC_LSP(string sfName)
{
  // In order to toy_with the LPC and LSP routines using the MATLABengine class
  // for numeric validation of the routines and graphical plots of the results,
  // the following defines must be set:
  //
  //	  MARSYAS_MATLAB
  //    _MATLAB_LPC_ (in LPC.cpp)
  //    _MATLAB_LSP_ (in LSP.cpp)
  //
  // Additionally, inside MATLAB, the /marsyasMATLAB directory should be in the path
  // so the LPC_toy_with.m and LSP_toy_with.m mfiles (included in /marsyasMATLAB) in can be
  // called directly from the C++ code for toy_withing and plotting purposes.
  //
  // <lmartins@inescporto.pt> - 17.06.2006

  cout << "TOY_WITH: LPC and LSP calculation and validation using MATLAB (engine)" << endl;
  cout << "Sound to analyze: " << sfName << endl;

  mrs_natural lpcOrder = 10;
  mrs_natural hopSize = 256;

  cout<<"LPC and LSP order: " <<lpcOrder <<endl;
  cout<<"hopeSize: " <<hopSize <<endl;

  /*
    MarSystemManager mng;

    //LPC network
    MarSystem* input = mng.create("Series", "input");

    input->addMarSystem(mng.create("SoundFileSource","src"));
    input->updControl("SoundFileSource/src/mrs_string/filename", sfName);
    input->updControl("mrs_natural/inSamples", hopSize);

    input->addMarSystem(mng.create("ShiftInput", "si"));

    input->updControl("ShiftInput/si/mrs_natural/winSize", hopSize);

    input->addMarSystem(mng.create("LPC", "LPC"));
    input->updControl("LPC/LPC/mrs_natural/order",lpcOrder);
    input->updControl("LPC/LPC/mrs_real/lambda",0.0);
    input->updControl("LPC/LPC/mrs_real/gamma",1.0);

    input->addMarSystem(mng.create("LSP", "lsp"));
    input->updControl("LSP/lsp/mrs_natural/order",lpcOrder);
    input->updControl("LSP/lsp/mrs_real/gamma",1.0);

    int i = 0;
    while(input->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
    {
    input->tick();
    cout << "Processed frame " << i << endl;
    ++i;
    }

    cout << endl << "LPC and LSP processing finished!";

    delete input;
  */

  MarSystemManager mng;

  MarSystem* input = mng.create("Series", "input");

  input->addMarSystem(mng.create("SoundFileSource","src"));
  input->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  input->updControl("mrs_natural/inSamples", hopSize);

  //input->addMarSystem(mng.create("ShiftInput", "si"));

  //input->updControl("ShiftInput/si/mrs_natural/winSize", hopSize);

  MarSystem* lspS = mng.create("Series","lspS");
  lspS->addMarSystem(mng.create("LPC", "LPC"));
  lspS->addMarSystem(mng.create("LSP", "lsp"));
  input->addMarSystem(lspS);
  // 	input->updControl("Series/lspS/LPC/LPC/mrs_natural/order",lpcOrder);
  // 	input->updControl("Series/lspS/LPC/LPC/mrs_real/lambda",0.0);
  // 	input->updControl("Series/lspS/LPC/LPC/mrs_real/gamma",1.0);
  // 	input->updControl("Series/lspS/LSP/lsp/mrs_natural/order",lpcOrder);
  // 	input->updControl("Series/lspS/LSP/lsp/mrs_real/gamma",1.0);

  int i = 0;
  while(input->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    input->tick();
    cout << "Processed frame " << i << endl;
    ++i;
  }

  cout << endl << "LPC and LSP processing finished!";

  delete input;
}

void
toy_with_realvec()
{
  //Toy_With new operator= implementation:
  //attributions are now performed even if the realvec arguments
  //have different sizes => left hand realvec is deleted before
  //performing attribution.
  realvec a;
  realvec b;

  a.create(3);
  a(0) = 1;
  a(1) = 2;
  a(2) = 3;

  //b.create(a.getSize()); //although a and b have diferent sizes, there is no need for this now!
  b = a;

  cout << endl << ">>>>>>>> realvec::operator=() : "<< endl;
  cout << "attributions are now performed even if the realvec arguments have different sizes" << endl;
  cout << "=> left hand realvec is deleted and then recreated during attribution." << endl << endl;

  cout << a << endl << endl;
  cout << b << endl << endl;

  cout << "When toy_withs stops - press a key to continue" << endl;

  getchar();

  // check the functions getRow/Col, setRow/Col, getSubMatrix/setSubMatrix
  cout << endl << ">>>>>>>> getRow/Col, setRow/Col, getSubMatrix/setSubMatrix" << endl;
  a.create (3,4);
  for (mrs_natural i = 0; i < 3; ++i)
    for (mrs_natural j = 0; j < 4; j++)
      a(i,j)	= (i*4)+j;
  cout << a << endl << endl;

  b.create (2,2);
  a.getSubMatrix (0,0, b);
  cout << b << endl;
  b.create (2,3);
  a.getSubMatrix (1,1, b);
  cout << b  << endl;
  b.create(5,5);
  a.getSubMatrix (1,1, b);
  cout << b  << endl;

  b.create(2,2);
  a.getSubMatrix (0,0, b);
  a.setSubMatrix (1,1,b);
  cout << a << endl << endl;

  cout << ">>>>>>>> ...done." << endl;
  getchar();

#ifdef MARSYAS_MATLAB

  realvec matrixA, matrixB;
  realvec meanobs;
  realvec stdobs;
  realvec varobs;
  realvec normobs;
  realvec invMatrix;
  realvec covmatrix;
  realvec corrmatrix;

  cout << endl << ">>>>>>>> Create a simple matrix:" << endl << endl;
  matrixA.create(2,3);
  matrixA(0,0) = 1.0;
  matrixA(0,1) = 2.0;
  matrixA(0,2) = 3.0;
  matrixA(1,0) = 4.0;
  matrixA(1,1) = 5.0;
  matrixA(1,2) = 6.0;
  cout << matrixA << endl;
  getchar();
  cout << ">>>>>>>> realvec::getRow(1,...):" << endl << endl;
  realvec tmpVec;
  matrixA.getRow(1, tmpVec);
  cout << tmpVec << endl;
  getchar();
  cout << ">>>>>>>> realvec::getCol(2,...):" << endl << endl;
  matrixA.getCol(2, tmpVec);
  cout << tmpVec << endl << endl;
  getchar();

  cout << ">>>>>>>> Sending matrix to MATLAB..." << endl;
  MATLAB_PUT(matrixA, "matrixA");
  cout << ">>>>>>>> ...complete! Press a key to continue." << endl;
  getchar();

  cout << endl<< ">>>>>>>> calculate means of each row using MATLAB:" << endl << endl;
  MATLAB_EVAL("meanobs = mean(matrixA')'");
  MATLAB_GET("meanobs", meanobs);
  cout << meanobs << endl;
  getchar();
  cout << ">>>>>>>> compare with realvec::meanObs(): " << endl << endl;
  matrixA.meanObs(tmpVec);
  cout << tmpVec << endl;
  getchar();

  cout << endl<< ">>>>>>>> calculate stdev of each row using MATLAB:" << endl << endl;
  //marsyas uses the biased estimator for the stdev calculation
  MATLAB_EVAL("stdobs = std(matrixA',1)'");
  MATLAB_GET("stdobs", stdobs);
  cout << stdobs << endl;
  getchar();
  cout << ">>>>>>>> compare with realvec::stdObs(): " << endl << endl;
  matrixA.stdObs(tmpVec);
  cout << tmpVec << endl;
  getchar();

  cout << endl<< ">>>>>>>> calculate variance of each row using MATLAB:" << endl << endl;
  //marsyas uses the biased estimator for the var calculation
  MATLAB_EVAL("varobs = var(matrixA',1)'");
  MATLAB_GET("varobs", varobs);
  cout << varobs << endl;
  getchar();
  cout << ">>>>>>>> compare with realvec::varObs(): " << endl << endl;
  matrixA.varObs(tmpVec);
  cout <<  tmpVec << endl;
  getchar();

  cout << endl<< ">>>>>>>> Standardize observation's matrix using MATLAB:" << endl << endl;
  MATLAB_EVAL("matrixA = matrixA'");
  MATLAB_EVAL("clear normobs");
  MATLAB_EVAL("normobs(1,:) = matrixA(1,:) - mean(matrixA)");
  MATLAB_EVAL("normobs(1,:) = normobs(1,:) / std(matrixA,1)");
  MATLAB_EVAL("normobs(2,:) = matrixA(2,:) - mean(matrixA)");
  MATLAB_EVAL("normobs(2,:) = normobs(2,:) / std(matrixA,1)");
  MATLAB_EVAL("normobs(3,:) = matrixA(3,:) - mean(matrixA)");
  MATLAB_EVAL("normobs(3,:) = normobs(3,:) / std(matrixA,1)");
  MATLAB_EVAL("normobs = normobs'");
  MATLAB_GET("normobs", normobs);
  cout << normobs << endl;
  getchar();
  cout << ">>>>>>>> compare with realvec::normObs(): " << endl << endl;
  matrixA.normObs();
  cout << matrixA << endl;
  getchar();

  cout << ">>>>>>>> Creating a new random matrix in MATLAB..." << endl;
  cout << ">>>>>>>> ... and get it into a realvec: " << endl << endl;
  MATLAB_EVAL("matrixA = rand(2,30)");
  MATLAB_GET("matrixA", matrixA);
  cout << matrixA << endl;
  getchar();

  cout << endl<< ">>>>>>>> calculate COVARIANCE matrix using MATLAB (unbiased estimator):" << endl << endl;
  MATLAB_EVAL("covmatrix = cov(matrixA')'");
  MATLAB_GET("covmatrix", covmatrix);
  cout << covmatrix << endl;
  getchar();
  cout << ">>>>>>>> compare with realvec::covariance(): " << endl << endl;
  //realvec::covariance() uses the unbiased estimator for the covar calculation
  //matrixB.create(matrixA.getRows(),matrixA.getCols()); //no need for this anymore!:-)
  matrixB = matrixA;
  matrixB.covariance(tmpVec);
  cout <<  tmpVec << endl;
  getchar();

  cout << endl<< ">>>>>>>> calculate COVARIANCE matrix using MATLAB (biased estimator):" << endl << endl;
  MATLAB_EVAL("covmatrix = cov(matrixA',1)'");
  MATLAB_GET("covmatrix", covmatrix);
  cout << covmatrix << endl;
  getchar();
  cout << ">>>>>>>> compare with realvec::covariance2(): " << endl << endl;
  //realvec::covariance2() uses the biased estimator for the covar calculation
  matrixB.create(matrixA.getRows(),matrixA.getCols());
  matrixB = matrixA;
  matrixB.covariance2(tmpVec);
  cout <<  tmpVec << endl;
  cout << ">>>>>>>> Results are different because realvec::covariance2() does not remove the mean from input data before estimating the cov matrix... " << endl << endl;
  getchar();

  cout << endl<< ">>>>>>>> calculate CORRELATION matrix using MATLAB:" << endl << endl;
  MATLAB_EVAL("corrmatrix = corrcoef(matrixA')'");
  MATLAB_GET("corrmatrix", corrmatrix);
  cout << corrmatrix << endl;
  getchar();
  cout << ">>>>>>>> compare with realvec::correlation(): " << endl << endl;
  matrixA.correlation(tmpVec);
  cout <<  tmpVec << endl;
  getchar();

  //-----------------------

  cout << ">>>>>>>> Creating a random matrix in MATLAB..." << endl;
  cout << ">>>>>>>> ... and get it into a realvec: " << endl << endl;
  MATLAB_EVAL("matrixA = rand(4)");
  MATLAB_GET("matrixA", matrixA);
  cout << matrixA << endl;
  getchar();

  cout << endl << ">>>>>>>> Calculate TRACE using MATLAB: " << endl;
  MATLAB_EVAL("traceval = trace(matrixA)");
  mrs_real traceval;
  MATLAB_GET("traceval", traceval);
  cout << traceval << endl << endl;
  cout << endl << ">>>>>>>> Calculate TRACE using realvec::trace(): " << endl;
  cout << matrixA.trace() << endl << endl;
  getchar();

  cout << endl << ">>>>>>>> Calculate matrix DETERMINANT using: " << endl;
  cout << "realvec::det() = " << matrixA.det() << endl;
  MATLAB_EVAL("determinant = det(matrixA)");
  mrs_real determinant;
  MATLAB_GET("determinant", determinant);
  cout << "MATLAB det() = " << determinant << endl << endl;
  getchar();

  cout << ">>>>>>>> Invert the matrix using realvec::invert()... " << endl;
  invMatrix.create(matrixA.getRows(),matrixA.getCols());
  int res = matrixA.invert(invMatrix);
  cout << ">>>>>>>> ...done! invert() returned: "<< res << endl << endl;
  cout << invMatrix << endl;
  getchar();
  cout << ">>>>>>>> Invert the matrix using MATLAB... " << endl;
  MATLAB_EVAL("invMatrix = inv(matrixA)");
  cout << ">>>>>>>> ...done! Get it to a realvec." << endl;
  getchar();
  matrixA.setval(0.0);
  MATLAB_GET("invMatrix", matrixA);
  cout << matrixA << endl;
  getchar();
  cout << "Compare results: difference should be a zero (or infinitesimal) valued matrix: " << endl << endl;
  cout << matrixA - invMatrix << endl;
  cout << "Maximum absolute error = " << (matrixA - invMatrix).maxval() << endl;
  getchar();

  //toy_with DivergenceShape metrics
  cout << ">>>>>>>> Creating two random matrices in MATLAB..." << endl;
  cout << ">>>>>>>> ... and get them into realvecs: " << endl << endl;
  MATLAB_EVAL("matrixA = cov(rand(40,4))");
  MATLAB_EVAL("matrixB = cov(rand(40,4))");
  MATLAB_GET("matrixA", matrixA);
  MATLAB_GET("matrixB", matrixB);
  cout << ">>>>>>>> Done!" << endl << endl;
  getchar();
  cout << ">>>>>>>> Calculate Divergence Shape between the two matrices:" << endl;
  cout << "realvec::divShape(Ci, Cj) = " << NumericLib::divergenceShape(matrixA,matrixB) << endl << endl;
  cout << ">>>>>>>> Calculate Bhattacharyya Shape between the two matrices:" << endl;
  cout << "realvec::battShape(Ci, Cj) = " << NumericLib::bhattacharyyaShape(matrixA,matrixB) << endl;
  getchar();

#endif
}

void
toy_with_MarControls(string sfName)
{
  cout << "TOY_WITH: new MarControl API" << endl;
  cout << "Using input audio file: " << sfName << endl;
  MarSystemManager mng;
  mng.registerPrototype("MarSystemTemplateBasic", new MarSystemTemplateBasic("mtbp"));
  mng.registerPrototype("MarSystemTemplateAdvanced", new MarSystemTemplateAdvanced("mtap"));

  MarSystem* pnet = mng.create("Series", "net");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("MarSystemTemplateBasic", "basic"));
  pnet->addMarSystem(mng.create("MarSystemTemplateAdvanced", "advanced"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));

  //set sound file to be opened
  pnet->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  //the new way to use controls (using a "smart" pointer MarControlPtr object)
  //in fact only makes sense to use this new form of controls' access if the
  //control is to be accessed intensively (e.g. in a for loop), as is this case.
  //(this control pointer can be somehow seen as an "efficient" link!)
  MarControlPtr ctrl_hasData = pnet->getctrl("SoundFileSource/src/mrs_bool/hasData");
  MarControlPtr ctrl_repeats = pnet->getctrl("MarSystemTemplateBasic/basic/mrs_natural/repeats");

  //Custom Controls
  MarControlPtr ctrl_hdr = pnet->getctrl("MarSystemTemplateAdvanced/advanced/mrs_myheader/hdrname");
  MyHeader hdr;
  hdr.someString = "myHeader.txt";
  hdr.someFlag = true;
  hdr.someValue = 666;
  hdr.someVec.create(2);
  ctrl_hdr->setValue(hdr);

  pnet->updControl("MarSystemTemplateBasic/basic/mrs_natural/repeats", 2);

  mrs_natural tmp;
  while(ctrl_hasData->isTrue())
  {
    pnet->tick();
    // just as an example, let's update the number of repeats in
    // a cyclic manner (1,2,3,4,5,1,2,3,...)
    tmp = (ctrl_repeats->to<mrs_natural>() % 5) +1;
    ctrl_repeats->setValue(tmp);
    // Since this changes the onSamples of MarSystemTemplateBasic/basic
    // we must call the Composite update() method, so the size of the
    // corresponding slice can be updated accordingly!
    pnet->update();
  }

  cout << "Finished MarControls toy_with!";

  delete pnet;
}

double
randD(double max)
{
  return max  *  (double)rand() / ((double)(RAND_MAX)+(double)(1.0)) ;
}






void
toy_with_stereoFeaturesMFCC(string fname0, string fname1)
{



  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));

  MarSystem* ffanout = mng.create("Fanout", "ffanout");

  MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));
  left->addMarSystem(mng.create("PowerSpectrum", "leftpspk"));
  left->addMarSystem(mng.create("MFCC", "leftMFCC"));
  left->addMarSystem(mng.create("TextureStats", "leftTextureStats"));

  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));
  right->addMarSystem(mng.create("PowerSpectrum", "rightpspk"));
  right->addMarSystem(mng.create("MFCC", "rightMFCC"));
  right->addMarSystem(mng.create("TextureStats", "rightTextureStats"));

  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);

  MarSystem* secondbranch = mng.create("Series", "secondbranch");
  MarSystem* stereobranches1 = mng.create("Parallel", "stereobranches1");
  MarSystem* left1 = mng.create("Series", "left1");
  MarSystem* right1 = mng.create("Series", "right1");

  left1->addMarSystem(mng.create("Windowing", "hamleft1"));
  left1->addMarSystem(mng.create("Spectrum", "spkleft1"));
  right1->addMarSystem(mng.create("Windowing", "hamright1"));
  right1->addMarSystem(mng.create("Spectrum", "spkright1"));

  stereobranches1->addMarSystem(left1);
  stereobranches1->addMarSystem(right1);

  secondbranch->addMarSystem(stereobranches1);
  secondbranch->addMarSystem(mng.create("StereoSpectrum", "sspk"));
  secondbranch->addMarSystem(mng.create("StereoSpectrumFeatures", "sspkf"));
  secondbranch->addMarSystem(mng.create("TextureStats", "texturests"));

  playbacknet->addMarSystem(ffanout);
  ffanout->addMarSystem(stereobranches);
  ffanout->addMarSystem(secondbranch);

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->addMarSystem(playbacknet);

  MarSystem* statistics2 = mng.create("Fanout", "statistics2");
  statistics2->addMarSystem(mng.create("Mean", "mn"));
  statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->updControl("Accumulator/acc/mrs_natural/nTimes", 1000);
  total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));


  total->updControl("WekaSink/wsink/mrs_natural/nLabels", 3);
  total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
  total->updControl("WekaSink/wsink/mrs_string/labelNames", "garage,grunge,jazz,");
  total->updControl("WekaSink/wsink/mrs_string/filename", "stereoFeaturesMFCC.arff");

  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", fname0);
  playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");


  total->updControl("mrs_natural/inSamples", 1024);

  Collection l;
  l.read(fname0);

  total->updControl("Annotator/ann/mrs_natural/label", 0);
  for (mrs_natural i=0; i < l.size(); ++i)
  {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
    /* if (i==0)
       total->updControl("Accumulator/acc/Series/playbacknet/AudioSink/dest/mrs_bool/initAudio", true);
    */
    cout << "Processing " << l.entry(i) << endl;
    total->tick();
    cout << "i = " << i << endl;

  }

  Collection m;
  m.read(fname1);

  total->updControl("Annotator/ann/mrs_natural/label", 1);


  for (mrs_natural i=0; i < m.size(); ++i)
  {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
    cout << "Processing " << m.entry(i) << endl;
    total->tick();
    cout << "i=" << i << endl;
  }


  Collection n;
  n.read("j.mf");

  total->updControl("Annotator/ann/mrs_natural/label", 2);


  for (mrs_natural i=0; i < n.size(); ++i)
  {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", n.entry(i));
    cout << "Processing " << n.entry(i) << endl;
    total->tick();
    cout << "i=" << i << endl;
  }
}

void
toy_with_stereoMFCC(string fname0, string fname1)
{
  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));

  MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));
  left->addMarSystem(mng.create("PowerSpectrum", "leftpspk"));
  left->addMarSystem(mng.create("MFCC", "leftMFCC"));
  left->addMarSystem(mng.create("TextureStats", "leftTextureStats"));

  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));
  right->addMarSystem(mng.create("PowerSpectrum", "rightpspk"));
  right->addMarSystem(mng.create("MFCC", "rightMFCC"));
  right->addMarSystem(mng.create("TextureStats", "rightTextureStats"));

  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);

  playbacknet->addMarSystem(stereobranches);

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->addMarSystem(playbacknet);

  MarSystem* statistics2 = mng.create("Fanout", "statistics2");
  statistics2->addMarSystem(mng.create("Mean", "mn"));
  statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->updControl("Accumulator/acc/mrs_natural/nTimes", 1000);
  total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));


  total->updControl("WekaSink/wsink/mrs_natural/nLabels", 3);
  total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
  total->updControl("WekaSink/wsink/mrs_string/labelNames", "garage,grunge,jazz,");
  total->updControl("WekaSink/wsink/mrs_string/filename", "stereoMFCC.arff");

  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", fname0);
  playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");


  total->updControl("mrs_natural/inSamples", 1024);


  Collection l;
  l.read(fname0);
  total->updControl("Annotator/ann/mrs_natural/label", 0);

  for (mrs_natural i=0; i < l.size(); ++i)
  {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
    /* if (i==0)
       total->updControl("Accumulator/acc/Series/playbacknet/AudioSink/dest/mrs_bool/initAudio", true);
    */
    cout << "Processing " << l.entry(i) << endl;
    total->tick();
    cout << "i = " << i << endl;

  }

  Collection m;
  m.read(fname1);

  total->updControl("Annotator/ann/mrs_natural/label", 1);


  for (mrs_natural i=0; i < m.size(); ++i)
  {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
    cout << "Processing " << m.entry(i) << endl;
    total->tick();
    cout << "i=" << i << endl;
  }

  Collection n;
  n.read("j.mf");

  total->updControl("Annotator/ann/mrs_natural/label", 2);


  for (mrs_natural i=0; i < n.size(); ++i)
  {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", n.entry(i));
    cout << "Processing " << n.entry(i) << endl;
    total->tick();
    cout << "i=" << i << endl;
  }
}

void
toy_with_mp3convert(string fname0)
{
  MarSystemManager mng;
  MarSystem* convertNet = mng.create("Series", "convertNet");

  convertNet->addMarSystem(mng.create("SoundFileSource", "src"));
  convertNet->addMarSystem(mng.create("SoundFileSink", "dest"));

  Collection l;
  l.read(fname0);

  for (mrs_natural  i=0; i < l.size(); ++i)
  {
    convertNet->updControl("SoundFileSource/src/mrs_string/filename", l.entry(i));
    string::size_type pos = l.entry(i).rfind(".", l.entry(i).length());
    string ext;
    if (pos == string::npos) ext = "";
    else
      ext = l.entry(i).substr(pos, l.entry(i).length());
    string extension;
    if (ext == ".mp3")
      extension = ".wav";
    else
      extension = ".mp3";
    ostringstream oss;
    oss << "ojazz" << i << extension;
    cout << "Converting " << l.entry(i) << " to " << oss.str() << endl;
    convertNet->updControl("SoundFileSink/dest/mrs_string/filename", oss.str());

    while(convertNet->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() == true)
    {
      convertNet->tick();
    }

  }
}

void
toy_with_stereoFeaturesVisualization(string fname0)
{
  MarSystemManager mng;

  MarSystem* total = mng.create("Series", "total");
  MarSystem* acc = mng.create("Accumulator", "acc");

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));
  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));

  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);

  playbacknet->addMarSystem(stereobranches);
  playbacknet->addMarSystem(mng.create("StereoSpectrum", "sspk"));
  // playbacknet->addMarSystem(mng.create("Memory", "mem"));
  // playbacknet->addMarSystem(mng.create("PlotSink", "psink"));

  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", fname0);
  playbacknet->updControl("mrs_natural/inSamples", 1024);
  playbacknet->updControl("mrs_bool/initAudio", true);

  acc->addMarSystem(playbacknet);
  acc->updControl("mrs_natural/nTimes", 2500);

  total->addMarSystem(acc);
  total->addMarSystem(mng.create("PlotSink", "psink"));

  total->tick();
}

void
toy_with_stereoFeatures(string fname0, string fname1)
{
  cout << "TOY_WITHING STEREO FEATURES" << endl;

  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  // playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));
  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));

  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);

  playbacknet->addMarSystem(stereobranches);
  playbacknet->addMarSystem(mng.create("StereoSpectrum", "sspk"));
  playbacknet->addMarSystem(mng.create("StereoSpectrumFeatures", "sspkf"));
  playbacknet->addMarSystem(mng.create("TextureStats", "texturests"));

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->addMarSystem(playbacknet);

  MarSystem* statistics2 = mng.create("Fanout", "statistics2");
  statistics2->addMarSystem(mng.create("Mean", "mn"));
  statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->updControl("Accumulator/acc/mrs_natural/nTimes", 1000);
  total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));

  total->updControl("WekaSink/wsink/mrs_natural/nLabels", 4);
  total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
  total->updControl("WekaSink/wsink/mrs_string/labelNames", "garage,grunge,jazz,ojazz");
  total->updControl("WekaSink/wsink/mrs_string/filename", "stereoFeatures.arff");

  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", fname0);
  playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

  total->updControl("mrs_natural/inSamples", 1024);

  Collection l;
  l.read(fname0);

  total->updControl("Annotator/ann/mrs_natural/label", 0);
  for (mrs_natural i=0; i < l.size(); ++i)
  {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
    /* if (i==0)
       total->updControl("Accumulator/acc/Series/playbacknet/AudioSink/dest/mrs_bool/initAudio", true);
    */
    cout << "Processing " << l.entry(i) << endl;
    total->tick();
    cout << "i = " << i << endl;

  }

  Collection n;
  n.read(fname1);

  total->updControl("Annotator/ann/mrs_natural/label", 1);

  for (mrs_natural i=0; i < n.size(); ++i)
  {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", n.entry(i));
    cout << "Processing " << n.entry(i) << endl;
    total->tick();
    cout << "i=" << i << endl;
  }


  Collection m;
  m.read("j.mf");

  total->updControl("Annotator/ann/mrs_natural/label", 2);


  for (mrs_natural i=0; i < m.size(); ++i)
  {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
    cout << "Processing " << m.entry(i) << endl;
    total->tick();
    cout << "i=" << i << endl;
  }
  Collection w;
  w.read("oj.mf");
  total->updControl("Annotator/ann/mrs_natural/label", 3);

  for (mrs_natural i=0; i < w.size(); ++i)
  {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", w.entry(i));
    cout << "Processing " << w.entry(i) << endl;
    total->tick();
    cout << "i=" << i << endl;
  }
}

void
toy_with_ADRess(string fname0, string fname1)
{
  (void) fname1;
  cout << "TOY_WITHING ADRess STEREO FEATURES" << endl;

  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  // playbacknet->addMarSystem(mng.create("AudioSink", "dest"));


  MarSystem* stereobranches = mng.create("Parallel", "stereobranches");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));
  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));

  stereobranches->addMarSystem(left);
  stereobranches->addMarSystem(right);
  playbacknet->addMarSystem(stereobranches);

  //playbacknet->addMarSystem(mng.create("StereoSpectrum", "sspk"));
  //playbacknet->addMarSystem(mng.create("StereoSpectrumFeatures", "sspkf"));
  //playbacknet->addMarSystem(mng.create("TextureStats", "texturests"));
  playbacknet->addMarSystem(mng.create("EnhADRess", "adress"));
  playbacknet->addMarSystem(mng.create("EnhADRessStereoSpectrum", "adressstereospec"));

  //playbacknet->addMarSystem(mng.create("StereoSpectrumSources", "sspks"));
  playbacknet->addMarSystem(mng.create("StereoFeatures", "sfeats"));


  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->addMarSystem(playbacknet);

  //MarSystem* statistics2 = mng.create("Fanout", "statistics2");
  //statistics2->addMarSystem(mng.create("Mean", "mn"));
  //statistics2->addMarSystem(mng.create("StandardDeviation", "std"));

  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  total->updControl("Accumulator/acc/mrs_natural/nTimes", 1000);
  //total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));


  total->updControl("WekaSink/wsink/mrs_natural/nLabels", 4);
  total->updControl("WekaSink/wsink/mrs_natural/downsample", 1);
  total->updControl("WekaSink/wsink/mrs_string/labelNames", "garage,grunge,jazz,ojazz");
  total->updControl("WekaSink/wsink/mrs_string/filename", "stereoFeatures.arff");

  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", fname0);
  playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");


  total->updControl("mrs_natural/inSamples", 1024);

  // cout << *total << endl;

  //	Collection l;
  //	l.read(fname0);


  total->updControl("Annotator/ann/mrs_natural/label", 0);
  //for (i=0; i < l.size(); ++i)
  //{
  total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", fname0);//l.entry(i));
  /* if (i==0)
     total->updControl("Accumulator/acc/Series/playbacknet/AudioSink/dest/mrs_bool/initAudio", true);
  */
  //cout << "Processing " << l.entry(i) << endl;
  total->tick();

  //	}

  /*
    Collection n;
    n.read(fname1);

    total->updControl("Annotator/ann/mrs_natural/label", 1);


    for (i=0; i < n.size(); ++i)
    {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", n.entry(i));
    cout << "Processing " << n.entry(i) << endl;
    total->tick();
    cout << "i=" << i << endl;
    }


    Collection m;
    m.read("j.mf");

    total->updControl("Annotator/ann/mrs_natural/label", 2);


    for (i=0; i < m.size(); ++i)
    {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
    cout << "Processing " << m.entry(i) << endl;
    total->tick();
    cout << "i=" << i << endl;
    }



    Collection w;
    w.read("oj.mf");

    total->updControl("Annotator/ann/mrs_natural/label", 3);


    for (i=0; i < w.size(); ++i)
    {
    total->updControl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", w.entry(i));
    cout << "Processing " << w.entry(i) << endl;
    total->tick();
    cout << "i=" << i << endl;
    }
  */
}






void
toy_with_stereo2mono(string fname)
{
  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  playbacknet->addMarSystem(mng.create("SoundFileSink", "dest"));

  playbacknet->updControl("SoundFileSource/src/mrs_string/filename", fname);
  playbacknet->updControl("SoundFileSink/dest/mrs_string/filename", "monoFromStereo.wav");
  playbacknet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");


  mrs_bool isEmpty;

  while ((isEmpty = playbacknet->getctrl("mrs_bool/hasData")->to<mrs_bool>()) != true)
  {
    playbacknet->tick();
  }

  delete playbacknet;
}



void
toy_with_lyons(string fname)
{


  const mrs_natural   numIrSamples    = 256;
  mrs_bool            dataMismatch    = false;
  MarSystemManager    mng;
  mrs_realvec         srcData (numIrSamples),
                      destData;
  mrs_realvec			mtlb_destData;


  ///////////////////////////////////////////////////////////////
  cout << ">>>>>>>> compute example audio output for Lyon's Passive Ear" << endl;
  cout << "(test only with short mono audio files)" << endl << endl;


  mrs_bool    isEmpty;
  mrs_natural sampleCount = 0;

  dataMismatch    = false;

  // new network
  MarSystem* lyonTestNet = mng.create("Series", "lyonTestNet");
  lyonTestNet->addMarSystem(mng.create("SoundFileSource", "src2"));
  lyonTestNet->addMarSystem(mng.create("LyonPassiveEar", "lyonsear"));

  //lyonTestNet->updControl("mrs_natural/inSamples", 1);
  lyonTestNet->updControl("SoundFileSource/src2/mrs_string/filename", fname);
  lyonTestNet->updControl("LyonPassiveEar/lyonsear/mrs_natural/decimFactor", 100);
  lyonTestNet->updControl("LyonPassiveEar/lyonsear/mrs_bool/agcActive", true);

  lyonTestNet->linkControl("mrs_bool/hasData", "SoundFileSource/src2/mrs_bool/hasData");

  // first compute the matlab result (no block based processing there with lyon filterbank)
#ifdef MARSYAS_MATLAB
  const mrs_bool      doMatlabPlots   = true;
  const mrs_real      simSampleRate   = 16000.0F;
  const mrs_real		floatTolerance  = 1e-6F;

  // empty workspace
  MATLAB_EVAL ("clear;");

  // set parameters
  MATLAB_PUT(lyonTestNet->getctrl("LyonPassiveEar/lyonsear/mrs_real/israte")->to<mrs_real>(), "fs");
  // only use short audio files because matlab will hold both input and fb output in memory
  MATLAB_PUT(lyonTestNet->getctrl("SoundFileSource/src2/mrs_string/filename")->to<mrs_string>(), "fname");
  // only use short audio files because matlab will hold both input and fb output in memory
  MATLAB_PUT(lyonTestNet->getctrl("LyonPassiveEar/lyonsear/mrs_natural/decimFactor")->to<mrs_natural>(), "decimFactor");

  // load audio file (only wav ATM!)
  MATLAB_EVAL("[audioIn, filefs] = wavread(fname);");
  // compute lyon output
  MATLAB_EVAL("mtlbOut = LyonPassiveEar(audioIn,filefs,decimFactor);");
#endif

  // do processing until eof
  while ((isEmpty = lyonTestNet->getctrl("mrs_bool/hasData")->to<mrs_bool>()) != true)
  {
    // calculate filterbank output
    lyonTestNet->tick();
    mrs_realvec outData = lyonTestNet->getControl ("mrs_realvec/processedData")->to<mrs_realvec>();

#ifdef MARSYAS_MATLAB

    // keep matlab up-to-date with the current position
    MATLAB_PUT((sampleCount + 1), "currSampleCount");
    MATLAB_PUT(lyonTestNet->getctrl("mrs_natural/onSamples")->to<mrs_natural>(), "inSamples");

    // get matlab data for the current block
    MATLAB_PUT(outData, "mrsOut");
    MATLAB_EVAL("currOutData = mtlbOut (:,currSampleCount:min(end,currSampleCount + inSamples-1));");
    MATLAB_GET ("currOutData", mtlb_destData);
    MATLAB_PUT(outData, "mrsOut");

    // compare output
    for (int i = 0; i < mtlb_destData.getRows (); ++i)
    {
      for (int j = 0; j < mtlb_destData.getCols (); j++)
      {
        if (std::abs (mtlb_destData(i,j) - outData(i,j)) > floatTolerance)
        {
          dataMismatch    = true;
          cout << "Block@Sample: " << sampleCount << ", Row: " << i << ", Col: " << j << ", Diff: " << mtlb_destData(i,j) - outData(i,j) << endl;
        }
      }
    }
#endif
    sampleCount += outData.getCols ();
  }
#ifdef MARSYAS_MATLAB
  if (!dataMismatch && doMatlabPlots )
  {
    MATLAB_EVAL("imagesc(mtlbOut);colorbar");
    MATLAB_EVAL("title('filter bank output');xlabel('Time (frames)');ylabel('Filter Band (Idx)');");
  }
#endif
  cout << "Results (Matlab, Marsyas): " << ((dataMismatch)? " not identical!" : "identical") << endl << endl;
  cout << "Lyons Passive Ear Audio test done..." << endl;
  cout << "Hit Enter to continue." << endl;
  getchar ();

#ifdef MARSYAS_MATLAB
  MATLAB_CLOSE ();
#endif


}

/*!
 * compares ERB implementations (matlab marsyas),
 * requires Auditory Toolbox (http://cobweb.ecn.purdue.edu/~malcolm/interval/1998-010/)
 *
 * \param fname wave file name
 */
void
toy_with_auditorytbx(string fname)
{

  const mrs_real      simSampleRate   = 16000.0F;
  const mrs_natural   numIrSamples    = 512;
  mrs_real            lowFreq         = 100.0F;
  mrs_natural         numChan         = 10;
  mrs_bool            dataMismatch    = false;
  MarSystemManager    mng;
  mrs_realvec         srcData,
                      destData;

  ///////////////////////////////////////////////////////////////
  cout << ">>>>>>>> compute example IR of ERB filterbank" << endl << endl;

  // network
  MarSystem* erbSimulNet = mng.create("Series", "erbSimulNet");
  erbSimulNet->addMarSystem (mng.create("RealvecSource", "rvsrc"));
  erbSimulNet->addMarSystem(mng.create("ERB", "erb"));

  // initialization of buffers and controls
  srcData.create (numIrSamples);
  srcData(0) = 1;

  erbSimulNet->updControl("mrs_real/israte", simSampleRate);
  erbSimulNet->updControl("ERB/erb/mrs_natural/numChannels", numChan);
  erbSimulNet->updControl("ERB/erb/mrs_real/lowFreq", lowFreq);
  erbSimulNet->updControl("RealvecSource/rvsrc/mrs_realvec/data", srcData);

  // compute IR of erb filterbank
  erbSimulNet->tick();

  // get IR
  destData = erbSimulNet->getctrl ("mrs_realvec/processedData")->to<mrs_realvec>();

#ifdef MARSYAS_MATLAB
  const mrs_bool      doMatlabPlots   = true;
  mrs_realvec     mtlb_destData;
  const mrs_real  floatTolerance  = 1e-6F;

  // set parameters
  MATLAB_PUT(erbSimulNet->getctrl("ERB/erb/mrs_real/israte")->to<mrs_real>(), "fs");
  MATLAB_PUT(numChan, "numChan");
  MATLAB_PUT(lowFreq, "lowFreq");
  MATLAB_PUT(erbSimulNet->getctrl("mrs_natural/inSamples")->to<mrs_natural>(), "inSamples");

  // compute matlab filter coeffs
  MATLAB_EVAL("fcoefs = MakeERBFilters(fs,numChan,lowFreq);");
  // compute matlab IR
  MATLAB_EVAL("mtlbIR = ERBFilterBank([1 zeros(1,inSamples-1)], fcoefs);");

  // set output data
  MATLAB_PUT(destData, "mrsIR");

  if (doMatlabPlots)
  {
    // plot output data
    MATLAB_EVAL("resp = 20*log10(abs(fft(mrsIR')));");
    MATLAB_EVAL("freqScale = (0:(inSamples-1))/inSamples*fs;");
    MATLAB_EVAL("figure,semilogx(freqScale(1:(inSamples*.5-1)),resp(1:(inSamples*.5-1),:));");
    MATLAB_EVAL("axis([lowFreq fs -60 0]);title('Frequency Response');xlabel('Frequency (Hz)');ylabel('Filterbank Transfer Function (dB)');grid on;");

    // compare IRs
    MATLAB_EVAL("figure,imagesc (mrsIR-mtlbIR);colorbar;");
    MATLAB_EVAL("title('IR: Difference between Implementations');xlabel('Time (frames)');ylabel('Filter Band (Idx)');");
  }

  MATLAB_GET ("mtlbIR", mtlb_destData);
  mtlb_destData  -= destData;
  for (int i = 0; i < destData.getRows (); ++i)
  {
    for (int j = 0; j < destData.getCols (); j++)
    {
      if (std::abs (mtlb_destData(i,j)) > floatTolerance)
      {
        dataMismatch    = true;
        break;
      }
    }
  }
  cout << "Results (Matlab, Marsyas): " << ((dataMismatch)? " not identical!" : "identical") << endl;

#endif

  cout << "ERB IR test done..." << endl;
  cout << "Hit Enter to continue." << endl;
  getchar ();

  ///////////////////////////////////////////////////////////////
  cout << ">>>>>>>> compute example audio output for ERB filterbank" << endl;
  cout << "(only use with short mono audio files)" << endl << endl;


  mrs_bool    isEmpty;
  mrs_natural sampleCount = 0;

  lowFreq         = 100.0F;
  numChan         = 40;
  dataMismatch    = false;

  // new network
  MarSystem* erbTestNet = mng.create("Series", "erbTestNet");
  erbTestNet->addMarSystem(mng.create("SoundFileSource", "src"));
  erbTestNet->addMarSystem(mng.create("ERB", "erb2"));

  erbTestNet->updControl("ERB/erb2/mrs_natural/numChannels", numChan);
  erbTestNet->updControl("ERB/erb2/mrs_real/lowFreq", lowFreq);

  erbTestNet->updControl("SoundFileSource/src/mrs_string/filename", fname);
  erbTestNet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

  // first compute the matlab result (no block based processing there with ERB filterbank)
#ifdef MARSYAS_MATLAB
  // empty workspace
  MATLAB_EVAL ("clear;");

  // set parameters
  MATLAB_PUT(erbTestNet->getctrl("ERB/erb2/mrs_real/israte")->to<mrs_real>(), "fs");
  MATLAB_PUT(numChan, "numChan");
  MATLAB_PUT(lowFreq, "lowFreq");
  // only use short audio files because matlab will hold both input and fb output in memory
  MATLAB_PUT(erbTestNet->getctrl("SoundFileSource/src/mrs_string/filename")->to<mrs_string>(), "fname");

  // compute matlab filter coeffs
  MATLAB_EVAL("fcoefs = MakeERBFilters(fs,numChan,lowFreq);");
  // load audio file (only wav ATM!)
  MATLAB_EVAL("[audioIn, filefs] = wavread(fname);");
  // compute ERB output
  MATLAB_EVAL("mtlbOut = ERBFilterBank(audioIn, fcoefs);");
#endif

  // do processing until eof
  while ((isEmpty = erbTestNet->getctrl("mrs_bool/hasData")->to<mrs_bool>()) != true)
  {
    // calculate filterbank output
    erbTestNet->tick();
    mrs_realvec outData = erbTestNet->getControl ("mrs_realvec/processedData")->to<mrs_realvec>();

#ifdef MARSYAS_MATLAB

    // keep matlab up-to-date with the current position
    MATLAB_PUT((sampleCount + 1), "currSampleCount");
    MATLAB_PUT(erbTestNet->getctrl("mrs_natural/inSamples")->to<mrs_natural>(), "inSamples");

    // get matlab data for the current block
    MATLAB_EVAL("currOutData = mtlbOut (:,currSampleCount:min(end,currSampleCount + inSamples-1));");
    MATLAB_GET ("currOutData", mtlb_destData);

    // compare output
    for (int i = 0; i < mtlb_destData.getRows (); ++i)
    {
      for (int j = 0; j < mtlb_destData.getCols (); j++)
      {
        if (std::abs (mtlb_destData(i,j) - outData(i,j)) > floatTolerance)
        {
          dataMismatch    = true;
          cout << "Block@Sample: " << sampleCount << ", Row: " << i << ", Col: " << j << ", Diff: " << mtlb_destData(i,j) - outData(i,j) << endl;
        }
      }
    }
#endif
    sampleCount += outData.getCols ();
  }
#ifdef MARSYAS_MATLAB
  if (!dataMismatch && doMatlabPlots )
  {
    MATLAB_EVAL("for j=1:size(mtlbOut,1) c=max(mtlbOut(j,:),0);  c=filter([1],[1 -.99],c); mtlbOut(j,:)=c; end;");
    MATLAB_EVAL("imagesc(mtlbOut);");
  }
#endif
  cout << "Results (Matlab, Marsyas): " << ((dataMismatch)? " not identical!" : "identical") << endl << endl;
  cout << "ERB Audio test done..." << endl;
  cout << "Hit Enter to continue." << endl;
  getchar ();

#ifdef MARSYAS_MATLAB
  MATLAB_CLOSE ();
#endif
}



void toy_with_swipe(string sfname) {
  (void) sfname;
// commented out because log2() and exp2() aren't part of the
// C++ standard and don't exist on win32.   -gp, 15 Aug 2008.
  /*
  // std::cout << "Hello, World!!!" <<endl;

  #ifdef TEST_ERB_CONVERSION
  double foo = 12345.6;
  double bar = hertz2erb(foo);
  std::cout << "Hz->ERB->Hz:" << endl;
  std::cout << foo << " " << hertz2erb(foo) << " " << erb2hertz(hertz2erb(foo)) << endl;
  #endif

  #define VERSUS_MATLAB

  #ifdef VERSUS_MATLAB
  cout << "Starting Matlab..." << endl;
  MATLAB_EVAL("clear;");
  #endif


  //////////////////////////////////////////////////
  // Sampling rate
  mrs_real fs = 44100.0;
  #ifdef VERSUS_MATLAB
  MATLAB_PUT(fs, "fs");
  #endif


  //////////////////////////////////////////////////
  // Pitch min and max parameters
  mrs_real f0_min = 30;
  mrs_real f0_max = 5000;
  #ifdef VERSUS_MATLAB
  MATLAB_PUT(f0_min, "f0_min");
  MATLAB_PUT(f0_max, "f0_max");
  MATLAB_EVAL("plim = [f0_min f0_max];");
  #endif



  //////////////////////////////////////////////////
  // Find ws, all the window sizes we'll use, and pO, the optimal pitch for each one

  #ifdef VERSUS_MATLAB
  MATLAB_EVAL("K = 2; % Parameter k for Hann window");
  MATLAB_EVAL("logWs = round( log2( 4*K * fs ./ plim ) );");
  MATLAB_EVAL("ws = 2.^[ logWs(1): -1: logWs(2) ]; % P2-WSs");
  MATLAB_EVAL("pO = 4*K * fs ./ ws; % Optimal pitches for P2-WSs");
  #endif

  mrs_natural K = 2;  // "Parameter k for Hann window"
  mrs_natural logWs_big = ((int) (0.5 + (log2(4*K*fs/f0_min))));
  mrs_natural logWs_small = ((int) (0.5 + (log2(4*K*fs/f0_max))));
  mrs_natural num_Ws = 1+logWs_big-logWs_small;

  realvec ws(num_Ws);
  realvec pO(num_Ws);

  for (int i=0; i<num_Ws; ++i) {
  ws(i) = exp2(logWs_big-i);
  pO(i) = 4*K*fs/ws(i);
  }


  #ifdef VERSUS_MATLAB
  MATLAB_PUT(ws, "ws_marsyas");
  // MATLAB_EVAL("figure(); plot(ws(:) - ws_marsyas(:)); title('ws-ws_marsyas');");
  MATLAB_PUT(pO, "pO_marsyas");
  // MATLAB_EVAL("figure(); plot(pO(:) - pO_marsyas(:)); title('pO-pO_marsyas');");
  #endif




  //////////////////////////////////////////////////
  // Find candidate pitches ("pc") and the (log2 of the) window size for each pitch ("d")

  mrs_real dlog2p = 1./96.;
  #ifdef VERSUS_MATLAB
  MATLAB_PUT(dlog2p, "dlog2p");
  MATLAB_EVAL("log2pc = [ log2(plim(1)): dlog2p: log2(plim(end)) ]';");
  MATLAB_EVAL("pc = 2 .^ log2pc;");
  MATLAB_EVAL("d = 1 + log2pc - log2( 4*K*fs./ws(1) );");
  #endif

  // cout << "dlog2p: " << dlog2p << endl;
  double num_pc_d = (log2(f0_max)-log2(f0_min)) / dlog2p;
  // cout << "num_pc_d: " << num_pc_d << endl;

  int num_pc = 1+ ((int) num_pc_d);
  // cout << "num_pc: " << num_pc << endl;

  realvec pc(num_pc);
  realvec d(num_pc);

  mrs_real log2_f0min = log2(f0_min);
  for (int i=0; i<num_pc; ++i) {
  mrs_real log2pc_i = log2_f0min + (i*dlog2p);
  pc(i) = exp2(log2pc_i);
  d(i) = 1 + log2pc_i -log2(4*K*fs / ws(0));
  }
  // cout << "pc: " << pc << endl;

  #ifdef VERSUS_MATLAB
  MATLAB_PUT(pc, "pc_marsyas");
  MATLAB_PUT(d, "d_marsyas");
  // MATLAB_EVAL("figure(); plot(pc(:) - pc_marsyas(:)); title('pc-pc_marsyas');");
  // MATLAB_EVAL("figure(); plot(d(:) - d_marsyas(:)); title('d-d_marsyas');");
  #endif



  //////////////////////////////////////////////////
  // Find "ERBs spaced frequencies (in Hertz)" ("fERBs")

  mrs_real dERBs = 0.1;
  mrs_real min_fERBs = hertz2erb(pc(0)/4);
  mrs_real max_fERBs = hertz2erb(fs/2);
  // cout << "max_fERBS: " << max_fERBs << endl;

  int num_fERBs = 1 + ((int) ((max_fERBs-min_fERBs)/dERBs));

  realvec fERBs(num_fERBs);
  for (int i=0; i<num_fERBs; ++i) {
  fERBs(i) = erb2hertz(min_fERBs + (i*dERBs));
  }

  // cout << "fERBs(num_fERBs-1): " << fERBs(num_fERBs-1) << endl;


  #ifdef VERSUS_MATLAB
  MATLAB_PUT(dERBs, "dERBs");
  MATLAB_PUT(fs, "fs");
  MATLAB_EVAL("fERBs = erbs2hz([ hz2erbs(pc(1)/4): dERBs: hz2erbs(fs/2) ]');");
  MATLAB_PUT(fERBs, "fERBs_marsyas");
  // MATLAB_EVAL("figure(); plot(fERBs(:) - fERBs_marsyas(:)); title('fERBs-fERBs_marsyas')");
  #endif



  //////////////////////////////////////////////////
  // File read, windowing and spectrogram


  // For now just use one window size:
  mrs_natural windowSize = 16384;
  mrs_natural overlap = 8192;
  mrs_natural hopSize = windowSize - overlap;
  (void) hopSize;


  #ifdef VERSUS_MATLAB
  MATLAB_PUT(sfname, "sfname");
  MATLAB_PUT(windowSize, "windowSize_marsyas");
  MATLAB_PUT(overlap, "overlap_marsyas");
  MATLAB_PUT(hopSize, "hop_marsyas");
  MATLAB_EVAL("audio = wavread_mono(sfname);");
  // Zero-pad
  MATLAB_EVAL("audio_zp = [ zeros(windowSize_marsyas/2,1); audio(:); zeros(hop_marsyas+windowSize_marsyas/2,1) ];");
  MATLAB_EVAL("magspec = abs(specgram(audio_zp, windowSize_marsyas, fs, hanning(windowSize_marsyas), overlap_marsyas))");
  MATLAB_EVAL("magspec_freqaxis = [0:windowSize_marsyas/2].*(fs/windowSize_marsyas);");
  #endif


  MarSystemManager mng;


  #define COMPARE_WINDOWINGxxx
  #ifdef COMPARE_WINDOWING
  #ifdef VERSUS_MATLAB
  cout << "Comparing windows..." << endl;

  // See if the window is the same between Marsyas and Matlab
  MarSystem *winTest = mng.create("Windowing", "win");
  winTest->updControl("mrs_string/type", "Hanning");
  winTest->updControl("mrs_natural/zeroPadding", 0);
  winTest->updControl("mrs_natural/inSamples", windowSize);

  mrs_realvec win_input;
  win_input.create(1, windowSize);
  win_input.setval(1);

  mrs_realvec win;
  win.create(1, windowSize);

  winTest->process(win_input, win);

  MATLAB_PUT(win, "win");
  // disp doesn't work:
  // MATLAB_EVAL("disp(['win from marsyas is size ' num2str(length(win))])");
  // MATLAB_EVAL("disp(['windowSize is ' num2str(windowSize_marsyas)])");
  // So use this kludge:
  //    MATLAB_EVAL("figure(); title(['Marsyas size: ' num2str(length(win)) ', windowSize ' num2str(windowSize_marsyas) ]);");

  MATLAB_EVAL("figure(); subplot(211); plot(win, 'b'); hold on; plot(hanning(windowSize_marsyas), 'r'); legend('marsyas hanning', 'matlab hanning'); subplot(212); plot(win' - hanning(windowSize_marsyas)); ylabel('diff');");
  #endif
  #endif




  MarSystem *net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("ShiftInput", "si"));
  net->addMarSystem(mng.create("Windowing", "win"));
  net->addMarSystem(mng.create("Spectrum", "spec"));
  net->addMarSystem(mng.create("PowerSpectrum", "pspec"));
  net->updControl("SoundFileSource/src/mrs_string/filename", sfname);
  net->updControl("mrs_natural/inSamples", overlap);
  net->updControl("ShiftInput/si/mrs_natural/winSize", windowSize);
  net->updControl("Windowing/win/mrs_string/type", "Hanning");
  net->updControl("Windowing/win/mrs_natural/zeroPadding", 0);
  net->updControl("PowerSpectrum/pspec/mrs_string/spectrumType", "magnitude");


  int num_windows = 5;

  cout << "Entering STFT loop..." << endl;


  for (int i = 0; i<num_windows; ++i) {


  net->tick();
  mrs_realvec v = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  #ifdef VERSUS_MATLAB
  // Compare STFT spectra

  if (i==0) {
  MATLAB_PUT(v, "first_spec_marsyas");
  MATLAB_EVAL("fmarsyas = first_spec_marsyas ./ max(first_spec_marsyas)");
  //MATLAB_EVAL("figure();  plot(fmarsyas, 'r'); ylabel('Marsyas');");
  //MATLAB_EVAL("title('First frame from Marsyas');");
  }

  if (i>=0) {
  MATLAB_PUT(v, "this_spec_marsyas");
  mrs_natural fn = i;
  (void) fn;
  MATLAB_PUT(fn, "frame_number");
  char cmd[100];
  sprintf(cmd, "this_spec = magspec(:,%d);", i+1);
  MATLAB_EVAL(cmd);
  MATLAB_EVAL("matlab = this_spec ./ max(this_spec)");
  MATLAB_EVAL("marsyas = this_spec_marsyas ./ max(this_spec_marsyas)");

  if (i==1 && 0) {
  MATLAB_EVAL("figure(); subplot(311); plot(db(fmarsyas), 'r'); ylabel('Marsyas #0');");
  MATLAB_EVAL("subplot(312); plot(db(marsyas), 'r'); ylabel('Marsyas #1');");
  MATLAB_EVAL("subplot(313); plot(db(marsyas-fmarsyas), 'r'); ylabel('diff');");
  MATLAB_EVAL("samexaxis();");
  }

  MATLAB_EVAL("figure(); subplot(311); plot(magspec_freqaxis,marsyas, 'r'); ylabel('Marsyas');");
  MATLAB_EVAL("title(['frame ' num2str(frame_number)])");
  MATLAB_EVAL("subplot(312); plot(magspec_freqaxis,matlab, 'b'); ylabel('Matlab');");
  MATLAB_EVAL("subplot(313); plot(magspec_freqaxis,matlab-marsyas); ylabel('diff.'); samexaxis();");
  MATLAB_EVAL("xlabel('Freq (Hz)')");
  }
  #endif


  // Interpolate at equidistant ERBs steps, and
  // call the square root of magnitude the "loudness" L
  // NB: swipe uses spline interpolation (matlab's INTERP1 with the 'spline' arg)
  // but here I'm just doing linear interpolation.  You can see in the Matlab plots
  // below that it makes almost no difference.


  realvec L(num_fERBs);
  for (int i=0; i<num_fERBs; ++i) {
  mrs_real freq = fERBs(i);
  // cout << "fERBs(" << i << "): " << fERBs(i) << endl;

  mrs_real fractional_FFT_bin = (freq/fs) * ((mrs_real) windowSize);
  mrs_natural lower_FFT_bin = (mrs_natural) fractional_FFT_bin;
  if (lower_FFT_bin < 0) {
  cout << "error: lower_FFT_bin is " << lower_FFT_bin << endl;
  }

  if (lower_FFT_bin+1 >= windowSize/2) {
  cout << "error: upper_FFT_bin is " << lower_FFT_bin+1 << endl;
  }

  mrs_real distance = fractional_FFT_bin - lower_FFT_bin;
  mrs_real interp_mag = (1-distance) * v(lower_FFT_bin) +
  distance * v(lower_FFT_bin+1);
  L(i) = sqrt(interp_mag);
  }

  #ifdef VERSUS_MATLAB
  MATLAB_PUT(L, "L_marsyas");
  MATLAB_EVAL("M_lin = interp1(magspec_freqaxis, this_spec_marsyas, fERBs);");
  MATLAB_EVAL("M = interp1(magspec_freqaxis, this_spec_marsyas, fERBs, 'spline', 0);");
  MATLAB_EVAL("L_lin = sqrt(M_lin);");
  MATLAB_EVAL("L = sqrt(M);");

  MATLAB_EVAL("figure(); subplot(411);");
  MATLAB_EVAL("plot(fERBs, L, 'g'); hold on; plot(fERBs, L_marsyas, 'r');");
  MATLAB_EVAL("legend('Matlab spline interp', 'Matt C interp')");
  MATLAB_EVAL("subplot(412);");
  MATLAB_EVAL("plot(fERBs, L' - L_marsyas)");
  MATLAB_EVAL("ylabel('matlab-marsyas')");
  MATLAB_EVAL("subplot(413);");
  MATLAB_EVAL("plot(fERBs, L_lin, 'g'); hold on; plot(fERBs, L_marsyas, 'r');");
  MATLAB_EVAL("legend('Matlab linear interp', 'Matt C interp')");
  MATLAB_EVAL("subplot(414);");
  MATLAB_EVAL("plot(fERBs, L_lin' - L_marsyas)");
  MATLAB_EVAL("ylabel('matlab-marsyas')");
  MATLAB_EVAL("samexaxis(); ylabel('Freq (Hz, ERB-spaced)');");
  #endif

  // "Select candidates that use this window size"
  // XXX Kludge


  }


  #ifdef VERSUS_MATLAB
  MATLAB_EVAL("dt = 0.01;");
  MATLAB_EVAL("t = [ 0: dt: length(audio)/fs ]';");
  #endif

  mrs_real dt = 0.01;
  (void) dt;


  #ifdef VERSUS_MATLAB
  cout << "Press enter to finish (and close Matlab)" << endl;
  getchar();
  MATLAB_CLOSE();
  #endif
  */
}




void
toy_with_windowedsource(string fname0)
{
  cout << "Toying with " << fname0 << endl;


  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");
  pnet->addMarSystem(mng.create("WindowedSoundFileSource","src"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));



  pnet->updControl("WindowedSoundFileSource/src/mrs_string/filename", fname0);
  pnet->updControl("mrs_natural/inSamples", 1024);	                          // hopSize
  pnet->updControl("WindowedSoundFileSource/src/mrs_natural/winSize", 2048);   // winSize
  pnet->updControl("AudioSink/dest/mrs_bool/initAudio", true);



  MarControlPtr ctrl_currentlyPlaying = pnet->getctrl("WindowedSoundFileSource/src/mrs_string/currentlyPlaying");

  string currentlyPlaying;

  while(1)
  {
    currentlyPlaying = ctrl_currentlyPlaying->to<mrs_string>();
    cout << "Processing : " << currentlyPlaying << endl;
    pnet->tick();
  }



}



void toy_with_spectralSNR(string fname0, string fname1)
{
  cout << "Toy_Withing spectral SNR" << endl;
  cout << "Original  signal: " << fname0 << endl;
  cout << "Extracted signal: " << fname1 << endl;

  MarSystemManager mng;

  MarSystem* total = mng.create("Series", "total");

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updControl("mrs_natural/nTimes", 100);

  MarSystem* snet = mng.create("Series", "snet");

  MarSystem* net = mng.create("Parallel", "net");

  MarSystem* branch1 = mng.create("Series", "branch1");
  MarSystem* branch2 = mng.create("Series", "branch2");

  branch1->addMarSystem(mng.create("SoundFileSource", "src1"));
  branch1->addMarSystem(mng.create("Spectrum", "spk1"));
  branch1->addMarSystem(mng.create("PowerSpectrum", "pspk1"));
  // branch1->addMarSystem(mng.create("PlotSink", "psink1"));

  branch2->addMarSystem(mng.create("SoundFileSource", "src2"));
  branch2->addMarSystem(mng.create("Spectrum", "spk2"));
  branch2->addMarSystem(mng.create("PowerSpectrum", "pspk2"));
  // branch2->addMarSystem(mng.create("PlotSink", "psink2"));

  net->addMarSystem(branch1);
  net->addMarSystem(branch2);


  net->updControl("Series/branch1/SoundFileSource/src1/mrs_string/filename",
                  fname0);
  net->updControl("Series/branch2/SoundFileSource/src2/mrs_string/filename",
                  fname1);

  net->updControl("Series/branch1/PowerSpectrum/pspk1/mrs_string/spectrumType", "magnitude");
  net->updControl("Series/branch2/PowerSpectrum/pspk2/mrs_string/spectrumType", "magnitude");

  /* net->updControl("Series/branch1/PlotSink/psink1/mrs_string/filename", "p1p");
     net->updControl("Series/branch2/PlotSink/psink2/mrs_string/filename", "p2p");

  */


  snet->addMarSystem(net);
  snet->addMarSystem(mng.create("SpectralSNR", "ssnr"));


  acc->addMarSystem(snet);
  total->addMarSystem(acc);
  // total->addMarSystem(mng.create("Mean", "mean"));
  total->addMarSystem(mng.create("PlotSink", "psink"));

  total->updControl("mrs_natural/inSamples",	2048);


  total->tick();



}


void toy_with_PeakView (string peakFile0, string peakFile1, string outputFile, string option)
{
  MarSystemManager	mng;
  MarSystem*			net			= mng.create("Series", "net");
  MarSystem*			input		= mng.create("Fanout", "input");

  mrs_natural			assFrameSize= 512;

  input->addMarSystem(mng.create("PeakViewSource", "peakFile0"));
  input->addMarSystem (mng.create("PeakViewSource", "peakFile1"));

  net->addMarSystem (input);
  net->addMarSystem (mng.create("PeakViewMerge", "merge"));
  net->addMarSystem (mng.create("PeakViewSink", "output"));

  // set file names
  net->updControl("Fanout/input/PeakViewSource/peakFile0/mrs_string/filename",
                  peakFile0);
  net->updControl("Fanout/input/PeakViewSource/peakFile1/mrs_string/filename",
                  peakFile1);
  net->updControl("PeakViewSink/output/mrs_string/filename",
                  outputFile);

  // set options
  net->linkControl("PeakViewSink/output/mrs_real/fs", "Fanout/input/PeakViewSource/peakFile0/mrs_real/osrate");
  net->updControl("PeakViewSink/output/mrs_natural/frameSize", assFrameSize);

  if (option != EMPTYSTRING)
    net->updControl("PeakViewMerge/merge/mrs_string/mode", option);

  // do the processing
  while (net->getControl("Fanout/input/PeakViewSource/peakFile0/mrs_bool/hasData")->to<mrs_bool>() == true)
    net->tick();

  net->updControl("PeakViewSink/output/mrs_bool/done", true);

  delete net;
}

void toy_with_PeakEval (string testFile, string refFile, string outTextFileName = EMPTYSTRING)
{
  std::ofstream outTextFile;
  MarSystemManager	mng;
  MarSystem*			net			= mng.create("Series", "net");
  MarSystem*			input		= mng.create("Fanout", "input");

  mrs_realvec			fMeasureResult(3,1);

  if (outTextFileName != EMPTYSTRING)
    outTextFile.open(outTextFileName.c_str ());

  cout << "Reference File: " << refFile << endl;
  cout << "Test File: " << testFile << endl;
  input->addMarSystem(mng.create("PeakViewSource", "testFile"));
  input->addMarSystem (mng.create("PeakViewSource", "refFile"));

  net->addMarSystem (input);
  net->addMarSystem (mng.create("PeakViewMerge", "merge"));

  net->addMarSystem (mng.create ("FMeasure", "fmeasure"));

  // set file names
  net->updControl("Fanout/input/PeakViewSource/testFile/mrs_string/filename",
                  testFile);
  net->updControl("Fanout/input/PeakViewSource/refFile/mrs_string/filename",
                  refFile);

  // set options
  net->updControl("PeakViewMerge/merge/mrs_string/mode", "AND");
  net->updControl("PeakViewMerge/merge/mrs_bool/discardNegativeGroups", true);
  net->updControl("Fanout/input/PeakViewSource/testFile/mrs_bool/discardNegativeGroups", true);
  net->updControl("Fanout/input/PeakViewSource/refFile/mrs_bool/discardNegativeGroups", true);

  // link controls
  net->linkControl("PeakViewMerge/merge/mrs_natural/frameMaxNumPeaks1",
                   "Fanout/input/PeakViewSource/testFile/mrs_natural/frameMaxNumPeaks");
  net->linkControl("PeakViewMerge/merge/mrs_natural/frameMaxNumPeaks2",
                   "Fanout/input/PeakViewSource/refFile/mrs_natural/frameMaxNumPeaks");
  net->linkControl("FMeasure/fmeasure/mrs_natural/numObservationsInTest",
                   "Fanout/input/PeakViewSource/testFile/mrs_natural/totalNumPeaks");
  net->linkControl("FMeasure/fmeasure/mrs_natural/numObservationsInReference",
                   "Fanout/input/PeakViewSource/refFile/mrs_natural/totalNumPeaks");
  net->linkControl("FMeasure/fmeasure/mrs_natural/numTruePositives",
                   "PeakViewMerge/merge/mrs_natural/totalNumPeaks");

  // do the processing
  while (net->getControl("Fanout/input/PeakViewSource/testFile/mrs_bool/hasData")->to<mrs_bool>() == true)
    net->tick();

  fMeasureResult	= net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  // write fmeasure outputs
  cout << endl << "F-Measure Result: " << fMeasureResult(0,0) << endl;

  if (outTextFile.good ())
    outTextFile << fMeasureResult(0,0) << "\t" << fMeasureResult(1,0) << "\t" << fMeasureResult(2,0) << std::endl;

  outTextFile.close ();

  delete net;
}

void toy_with_SNR(string fname0, string fname1, string fname2 = EMPTYSTRING, string ignoreSilence = EMPTYSTRING, int refDelay = 0)
{
  std::ofstream outTextFile;
  const mrs_natural blockSize = 1024;

  cout << "Toying with SNR" << endl;
  cout << "mudbox -t SNR testfile reffile [outtextfile] [is] [refDelayInSamples]"<< endl << endl;
  cout << "SIGNAL = "  << fname0 << endl;
  cout << "REFERENCE = " << fname1 << endl;
  cout << "Reference Delay = " << refDelay << " samples" << endl;

  mrs_realvec snrResult(2);
  MarSystemManager mng;

  if (fname2 != EMPTYSTRING)
    outTextFile.open(fname2.c_str ());

  // create network
  MarSystem* net			= mng.create("Series", "net");
  MarSystem* input		= mng.create("Fanout", "input");
  MarSystem* refSeries	= mng.create("Series", "refSeries");

  input->addMarSystem(mng.create("SoundFileSource", "signalSrc"));
  input->addMarSystem (refSeries);

  refSeries->addMarSystem(mng.create("SoundFileSource", "refSrc"));
  if (refDelay > 0)
    refSeries->addMarSystem (mng.create("Delay", "delay"));

  net->addMarSystem(input);
  net->addMarSystem(mng.create("SNR", "snr"));

  // set file names
  net->updControl("Fanout/input/SoundFileSource/signalSrc/mrs_string/filename",
                  fname0);
  net->updControl("Fanout/input/Series/refSeries/SoundFileSource/refSrc/mrs_string/filename",
                  fname1);

  // set other controls
  net->updControl("mrs_natural/inSamples", blockSize);
  net->updControl("mrs_natural/inObservations", 2);
  if (ignoreSilence == "is")
    net->updControl ("SNR/snr/mrs_string/mode", "checkRef4Silence");
  if (refDelay > 0)
  {
    net->updControl ("Fanout/input/Series/refSeries/Delay/delay/mrs_real/maxDelaySamples", 1.0*refDelay);
    net->updControl ("Fanout/input/Series/refSeries/Delay/delay/mrs_real/delaySamples", 1.0*refDelay);
  }

  // process
  while (net->getctrl("Fanout/input/SoundFileSource/signalSrc/mrs_bool/hasData")->to<mrs_bool>() == true)
  {
    net->tick();
  }

  snrResult	= net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

  cout << snrResult << endl;
  if (outTextFile.good ())
    outTextFile << snrResult(0) <<"\t" << snrResult(1) << std::endl;
  else if (ignoreSilence == "is")
    cout << "Problem writing output file!" << endl;

  outTextFile.close ();

  delete net;
}

void toy_with_orcaSnip(string fname0, string outWavFileName = EMPTYSTRING)
{
  // settings
  mrs_natural blockSize	= 2048,
              hopSize		= 256,
               accnTimes	= 0;
  // result
  mrs_realvec startStopSamples(2);

  // stuff
  MarSystemManager mng;
  FileName Sfname(fname0);
  mrs_real sampleFreq = 0;
  const mrs_real tonalityThreshInHz[2] = { 500, 3000};

  cout << "Snipping Orca Sounds" << endl;
  //startStopSamples.setval (0.);

  if (outWavFileName == EMPTYSTRING)
    outWavFileName	= Sfname.path () + Sfname.nameNoExt () + ".snip." + Sfname.ext ();

  // create network
  MarSystem* main			= mng.create("Series", "main");
  MarSystem* net			= mng.create("Accumulator", "accu");
  MarSystem* featureSeries= mng.create("Series", "featureSeries");
  MarSystem* featureFan	= mng.create("Fanout", "featureFan");
  MarSystem* featTonality	= mng.create("Series", "featTonality");
  MarSystem* featRms		= mng.create("Series", "rms");

  main->addMarSystem (net);
  // add decision making system
  main->addMarSystem (mng.create("OrcaSnip", "getBounds"));

  net->addMarSystem (featureSeries);

  // add soundfilesrc and hopsize stuff
  featureSeries->addMarSystem (mng.create("SoundFileSource", "signalSrc"));
  featureSeries->addMarSystem(mng.create("ShiftInput", "shiftSrc"));

  // add fanout
  featureSeries->addMarSystem (featureFan);

  // add fanout branches
  featureFan->addMarSystem (featTonality);
  featureFan->addMarSystem (featRms);

  // generate rms chain
  featRms->addMarSystem (mng.create("Rms", "rms"));

  // generate tonality chain
  //featTonality->addMarSystem (mng.create("HalfWaveRectifier", "Hwr"));
  featTonality->addMarSystem (mng.create("AutoCorrelation", "Acf"));
  featTonality->addMarSystem (mng.create("Peaker", "peaker"));
  featTonality->addMarSystem (mng.create("MaxArgMax", "max"));
  featTonality->addMarSystem (mng.create("DownSampler", "ds"));

  //featRms->addMarSystem(mng->create("Windowing", "wiSyn1"));
  //featRms->addMarSystem(mng->create("Spectrum", "specSyn"));
  //featRms->addMarSystem (mng.create("2BImplemented", "getBounds"));

  // set parameters
  main->updControl ("mrs_natural/inSamples", hopSize);
  featureSeries->updControl ("SoundFileSource/signalSrc/mrs_string/filename", Sfname.fullname ());
  featureSeries->updControl ("mrs_natural/onSamples", blockSize);

  featureSeries->updControl ("Fanout/featureFan/Series/featTonality/AutoCorrelation/Acf/mrs_bool/setr0to1", true);

  sampleFreq	= featureSeries->getControl ("SoundFileSource/signalSrc/mrs_real/osrate")->to<mrs_real>();
  featureSeries->updControl ("Fanout/featureFan/Series/featTonality/Peaker/peaker/mrs_real/peakStrength", .2);
  featureSeries->updControl ("Fanout/featureFan/Series/featTonality/Peaker/peaker/mrs_real/peakStrengthRelThresh", 1.);
  featureSeries->updControl ("Fanout/featureFan/Series/featTonality/Peaker/peaker/mrs_real/peakStrengthRelThresh", 1.);
  featureSeries->updControl ("Fanout/featureFan/Series/featTonality/Peaker/peaker/mrs_natural/peakStart", (mrs_natural)(sampleFreq/tonalityThreshInHz[1] + .1));
  featureSeries->updControl ("Fanout/featureFan/Series/featTonality/Peaker/peaker/mrs_natural/peakEnd", (mrs_natural)(sampleFreq/tonalityThreshInHz[0] + .1));
  featureSeries->updControl ("Fanout/featureFan/Series/featTonality/MaxArgMax/max/mrs_natural/nMaximums", 1);

  accnTimes	= (featureSeries->getControl ("SoundFileSource/signalSrc/mrs_natural/size")->to<mrs_natural>()+ 2*(blockSize-hopSize)) * 1./hopSize ;
  net->updControl ("mrs_natural/nTimes", accnTimes);
  main->linkControl("OrcaSnip/getBounds/mrs_natural/inSamples", "Accumulator/accu/mrs_natural/nTimes");

  featureSeries->updControl("ShiftInput/shiftSrc/mrs_natural/winSize", blockSize);


  // process
  main->tick ();

  // get result
  startStopSamples(0) = main->getControl ("OrcaSnip/getBounds/mrs_natural/startSnip")->to<mrs_natural>()*hopSize + (blockSize-hopSize);
  startStopSamples(1) = main->getControl ("OrcaSnip/getBounds/mrs_natural/stopSnip")->to<mrs_natural>()*hopSize + (blockSize-hopSize);

  cout << startStopSamples << endl;

  // write the region between start and stop to a snd file
  MarSystem* out		= mng.create("Series", "main");
  out->addMarSystem (mng.create("SoundFileSource", "signalSrc"));
  out->addMarSystem (mng.create("Clip", "outClip"));
  out->addMarSystem (mng.create("SoundFileSink", "signalSnk"));

  out->updControl ("mrs_natural/inSamples", hopSize);
  out->updControl ("SoundFileSource/signalSrc/mrs_string/filename", Sfname.fullname ());
  out->updControl ("SoundFileSink/signalSnk/mrs_string/filename", outWavFileName);
  out->updControl( "SoundFileSource/signalSrc/mrs_natural/pos", (mrs_natural)(startStopSamples(0) + .1));
  out->updControl("Clip/outClip/mrs_real/range", 1-1./((1<<15)-1)); // assume a 16bit signal - that should do the job.

  while (startStopSamples(0) < startStopSamples(1))
  {
    out->tick ();
    startStopSamples(0)  = startStopSamples(0) + hopSize;
  }

  delete main;
  delete out;
}

// currently produces memory errors in valgrind!
void toy_with_realvecsource_realtime()
{
  MarSystemManager mng;
  MarSystem *net = mng.create("Series", "net");

  // data we want to get these two observations into our network.
  mrs_natural a = 0.0;
  mrs_natural b = 0.0;

  realvec input(2,1);
  MarSystem *src = mng.create("RealvecSource", "src");

  // src->updControl("mrs_string/onObsNames", "a,b,");
  net->addMarSystem(src);
  net->addMarSystem(mng.create("Annotator", "ann"));
  net->addMarSystem(mng.create("WekaSink", "dest"));

  net->updControl("mrs_natural/inSamples", 1);
  net->updControl("RealvecSource/src/mrs_string/onObsNames", "a,b,");
  net->updControl("RealvecSource/src/mrs_realvec/data", input);


  cout << input << endl;
  // according to realvecsource.h "When you feed in a
  // realvec, the rows turn into observations and
  // the columns turn into samples.", so I expected
  // the onSamples to be 2.
  cout << (*src) << endl;

  // do processing
  for (int i=0; i < 2; i++) {
    // get new values from somewhere
    a = 1.0*i;
    b = 2.0*i;

    // enter values to network
    input(0,0) = a;
    input(1,0) = b;
    src->updControl("mrs_realvec/data", input);

    // process
    net->tick();
  }
  delete net; // also handles src!
}

void toy_with_SOM(string collectionName)
{
  MarSystemManager mng;

  srand(4);

  MarSystem* extractNet = mng.create("Series", "extractNet");
  extractNet->addMarSystem(mng.create("SoundFileSource", "src"));
  MarSystem* spectimeFanout = mng.create("Fanout", "spectimeFanout");
  spectimeFanout->addMarSystem(mng.create("ZeroCrossings", "zcrs"));

  MarSystem* spectralNet = mng.create("Series", "spectralNet");
  spectralNet->addMarSystem(mng.create("Windowing", "ham"));
  spectralNet->addMarSystem(mng.create("Spectrum", "spk"));
  spectralNet->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
  featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
  featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));
  spectralNet->addMarSystem(featureFanout);
  spectimeFanout->addMarSystem(spectralNet);
  extractNet->addMarSystem(spectimeFanout);
  extractNet->addMarSystem(mng.create("Memory", "mem"));

  MarSystem* stats = mng.create("Fanout", "stats");
  stats->addMarSystem(mng.create("Mean", "mn1"));
  stats->addMarSystem(mng.create("StandardDeviation", "std1"));
  extractNet->addMarSystem(stats);

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updControl("mrs_natural/nTimes", 1200);
  acc->addMarSystem(extractNet);

  MarSystem* total = mng.create("Series", "total");
  total->addMarSystem(acc);
  MarSystem* stats2 = mng.create("Fanout", "stats2");
  stats2->addMarSystem(mng.create("Mean", "mn2"));
  stats2->addMarSystem(mng.create("StandardDeviation", "std2"));
  total->addMarSystem(stats2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("SOM", "som"));


  // link top-level controls
  total->linkControl("mrs_string/filename",
                     "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/filename");

  total->linkControl("mrs_natural/pos",
                     "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/pos");


  total->linkControl("mrs_string/allfilenames",
                     "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/allfilenames");

  total->linkControl("mrs_natural/numFiles",
                     "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/numFiles");

  total->linkControl("mrs_bool/hasData",
                     "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/hasData");
  total->linkControl("mrs_bool/advance",
                     "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/advance");

  total->linkControl("mrs_bool/memReset",
                     "Accumulator/acc/Series/extractNet/Memory/mem/mrs_bool/reset");

  total->linkControl("mrs_natural/label",
                     "Annotator/ann/mrs_natural/label");

  total->updControl("mrs_natural/inSamples", 512);

  total->updControl("mrs_string/filename", collectionName);


  mrs_natural trainSize = 20000;
  mrs_natural grid_width = 10;
  mrs_natural grid_height = 10;
  mrs_natural iterations = 20;

  MarSystem* som = mng.create("SOM", "som");
  som->updControl("mrs_natural/inObservations", 2);
  som->updControl("mrs_natural/inSamples", 1);
  som->updControl("mrs_natural/grid_height", grid_height);
  som->updControl("mrs_natural/grid_width", grid_width);
  som->updControl("mrs_string/mode", "train");

  realvec train_data(trainSize);

  realvec input;
  input.create(2,1);
  realvec output;
  output.create(3,1);


  // init train data

  cout << "Initializing training data" << endl;

  for (mrs_natural i=0; i < trainSize; ++i)
  {
    train_data(i) = randD(1.0);
    cout << train_data(i) << endl;
  }

  // train map
  cout << "training " << endl;

  // output initial map
  realvec som_map;
  som_map.create((mrs_natural)grid_width,(mrs_natural)grid_height);


  for (mrs_natural k=0; k < iterations; k++)
  {
    cout << "Iteration " << k << endl;

    for (mrs_natural i=0; i < trainSize; ++i)
    {
      input(0) = train_data(i);
      input(1) = 0;
      som->process(input, output);
      som_map((mrs_natural)output(0), (mrs_natural)output(1)) = train_data(i) * 64.0;
      if (i==100)
      {
        ostringstream oss;
        oss << "map" << k << ".plot";
        som_map.write(oss.str());
      }
    }
  }

  cout << "predicting" << endl;

  som->updControl("mrs_bool/done", true);
  // predict
  som->updControl("mrs_string/mode", "predict");

  for (mrs_natural i=0; i < 100; ++i)
  {
    input(0) = train_data(i);
    cout << "input(0) = " << input(0) << endl;
    input(1) = i;
    cout << "input(1) = " << input(1) << endl;
    som->process(input,output);
    som->process(input, output);
    som_map((mrs_natural)output(0), (mrs_natural)output(1)) = train_data(i) * 64.0;
    if (i==99)
    {
      ostringstream oss;
      oss << "predictmap" << ".plot";
      som_map.write(oss.str());
    }
  }

}

void toy_with_Windowing()
{
  MarSystemManager mng;
  MarSystem* series = mng.create("Series","series");
  series->addMarSystem(mng.create("Windowing","win"));
  series->addMarSystem(mng.create("PlotSink","plot"));

  realvec in;
  realvec out;

  vector<string> winname;
  winname.push_back("Hamming");
  winname.push_back("Hanning");
  winname.push_back("Triangle");
  winname.push_back("Bartlett");
  winname.push_back("Gaussian");
  winname.push_back("Blackman");
  winname.push_back("Blackman-Harris");

  //series->updControl("Windowing/win/mrs_natural/size", 800);
  //series->updControl("Windowing/win/mrs_natural/zeroPadding", 200);
  //series->updControl("Windowing/win/mrs_bool/zeroPhasing", true);

  in.create(series->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  out.create(series->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  in.setval(1.0);

  for (size_t i = 0 ; i < winname.size(); ++i)
  {
    series->updControl("Windowing/win/mrs_string/type", winname[i]);
    series->updControl("PlotSink/plot/mrs_string/filename", "marsyas" + winname[i]);

    MRSMSG(winname[i]);
    MRSMSG("size: " << series->getctrl("Windowing/win/mrs_natural/size")->to<mrs_natural>());
    MRSMSG("zero-padding: " << series->getctrl("Windowing/win/mrs_natural/zeroPadding")->to<mrs_natural>());

    series->process(in,out);
  }
}


void
toy_with_weka(string fname)
{
  cout << "Toying with weka " << fname << endl;

  MarSystemManager mng;

  MarSystem* net;
  net = mng.create("Series", "net");
  net->addMarSystem(mng.create("WekaSource", "wsrc"));
  // net->addMarSystem(mng.create("OneRClassifier", "ocl"));
  net->addMarSystem(mng.create("GaussianClassifier", "gcl"));
  net->addMarSystem(mng.create("Summary", "summary"));
  // net->updControl("WekaSource/wsrc/mrs_string/attributesToInclude", "1,2,3");

  net->updControl("WekaSource/wsrc/mrs_string/validationMode", "PercentageSplit,50%");
  net->updControl("WekaSource/wsrc/mrs_string/filename", fname);
  net->updControl("mrs_natural/inSamples", 1);

  net->updControl("Summary/summary/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  net->updControl("Summary/summary/mrs_string/classNames",
                  net->getctrl("WekaSource/wsrc/mrs_string/classNames"));


  net->updControl("GaussianClassifier/gcl/mrs_natural/nClasses", net->getctrl("WekaSource/wsrc/mrs_natural/nClasses"));
  net->linkControl("GaussianClassifier/gcl/mrs_string/mode", "Summary/summary/mrs_string/mode");

  while(net->getctrl("WekaSource/wsrc/mrs_bool/done")->to<mrs_bool>() == false)
  {
    string mode = net->getctrl("WekaSource/wsrc/mrs_string/mode")->to<mrs_string>();
    cout << mode << endl;
    net->tick();
    net->updControl("GaussianClassifier/gcl/mrs_string/mode", mode);
    // net->updControl("Summary/summary/mrs_string/mode", mode);
  }

  net->updControl("Summary/summary/mrs_bool/done", true);
  net->tick();

}




void
toy_with_updControl(string fname)
{
  MarSystemManager mng;

  MarSystem* pnet_;

  pnet_ = mng.create("Series", "pnet_");
  pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet_->addMarSystem(mng.create("Gain", "gain"));
  pnet_->addMarSystem(mng.create("AudioSink", "dest"));

  MarControlPtr filePtr = pnet_->getctrl("SoundFileSource/src/mrs_string/filename");

  cout << filePtr->hasState() << endl;

  pnet_->updControl(filePtr, fname);
  // pnet_->updControl("SoundFileSource/src/mrs_string/filename", fname);

  cout << *pnet_ << endl;
}


void
toy_with_dtw(string fname1, string fname2)
{
  mrs_natural size1;
  mrs_natural size2;
  mrs_natural max_size;

  MarSystemManager mng;

  MarSystem* rmsnet = mng.create("Series/rmsnet");
  MarSystem *rmsfan = mng.create("Fanout/rmsfan");

  // Network to compute RMS for file 1
  MarSystem* branch1 = mng.create("Series/branch1");
  branch1->addMarSystem(mng.create("SoundFileSource/src"));
  branch1->addMarSystem(mng.create("MixToMono/mix2mono"));
  branch1->addMarSystem(mng.create("Rms/rms"));

  // Network to compute RMS for file 2
  MarSystem* branch2 = mng.create("Series/branch2");
  branch2->addMarSystem(mng.create("SoundFileSource/src"));
  branch2->addMarSystem(mng.create("MixToMono/mix2mono"));
  branch2->addMarSystem(mng.create("Rms/rms"));

  rmsfan->addMarSystem(branch1);
  rmsfan->addMarSystem(branch2);
  rmsnet->addMarSystem(rmsfan);

  // Collect the RMS contours
  rmsnet->addMarSystem(mng.create("RealvecSink/rdest"));

  rmsnet->updControl("Fanout/rmsfan/Series/branch1/SoundFileSource/src/mrs_string/filename", fname1);
  rmsnet->updControl("Fanout/rmsfan/Series/branch2/SoundFileSource/src/mrs_string/filename", fname2);
  rmsnet->updControl("mrs_natural/inSamples", 44100);

  size1 = rmsnet->getctrl("Fanout/rmsfan/Series/branch1/SoundFileSource/src/mrs_natural/size")->to<mrs_natural>() / 44100.0;
  size2 = rmsnet->getctrl("Fanout/rmsfan/Series/branch2/SoundFileSource/src/mrs_natural/size")->to<mrs_natural>() / 44100.0;

  if (size1 <= size2)
    max_size =size2;
  else
    max_size = size1;

  // RMS contour are calculated below
  for (int i = 0; i < max_size; ++i)
    rmsnet->tick();
  cout << "Done processing files" << fname1 << " and " << fname2 << endl;

  mrs_realvec rms_data = rmsnet->getctrl("RealvecSink/rdest/mrs_realvec/data")->to<mrs_realvec>();

  // make pictures of contours
#ifdef MARSYAS_PNG
  pngwriter png1(rms_data.getCols(),128, 0, "rms1.png");
  pngwriter png2(rms_data.getCols(),128, 0, "rms2.png");
  png1.invert();
  png2.invert();

  rms_data.normMaxMin();

  for (int i=0; i < rms_data.getCols(); ++i)
  {
    png1.line(i, 0, i, rms_data(0,i) * 128, 0.0, 0.0, 1.0);
    png2.line(i, 0, i, rms_data(1,i) * 128, 0.0, 0.0, 1.0);
  }

  png1.close();
  png2.close();
#endif

  // Prepare network to compute similarity Matrix and the use
  // DTW to find alignment

  mrs_realvec sizes;
  sizes.create(2);
  sizes(0) = size1;
  sizes(1) = size2;

  MarSystem* net = mng.create("Series", "series");
  net->updControl("mrs_natural/inSamples", rms_data.getCols());
  net->updControl("mrs_natural/inObservations", 2);
  net->addMarSystem(mng.create("RealvecSource", "src"));
  net->updControl("RealvecSource/src/mrs_realvec/data", rms_data);

  MarSystem* sim = mng.create("SimilarityMatrix/sim");
  sim->updControl("mrs_string/normalize","MinMax");
  sim->updControl("mrs_realvec/sizes", sizes);

  // Distance/similarity metric used
  MarSystem* met = mng.create("Metric/met");
  met->updControl("mrs_string/metric", "euclideanDistance");
  sim->addMarSystem(met);
  net->addMarSystem(sim);

  MarSystem* dtw = mng.create("DTW/dtw");
  dtw->updControl("mrs_string/lastPos","end");
  dtw->updControl("mrs_string/startPos","zero");
  dtw->updControl("mrs_bool/weight",false);
  dtw->updControl("mrs_string/mode","normal");
  net->addMarSystem(dtw);

  // Compute the similarity matrix and DTW alignment path
  net->tick();

  mrs_realvec similarity_output =
    net->getctrl("SimilarityMatrix/sim/mrs_realvec/processedData")->to<mrs_realvec>();
  mrs_realvec dtw_output =
    net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();


  // Make picture for the resulting Matrix
#ifdef MARSYAS_PNG
  pngwriter png_rms(rms_data.getCols(), rms_data.getCols(), 0, "simMatrix.png");

  // Find max and min
  double max = MINREAL;
  double min = MAXREAL;
  for (int r=0; r < similarity_output.getRows(); ++r) {
    for (int c=0; c < similarity_output.getCols(); ++c) {
      if (similarity_output(r,c) < min)
        min = similarity_output(r,c);
      if (similarity_output(r,c) > max)
        max = similarity_output(r,c);
    }
  }

  double colour;
  // Make a png of the similarity matrix
  for (int r=0; r < similarity_output.getRows(); ++r) {
    for (int c=0; c < similarity_output.getCols(); ++c) {
      colour = 1.0 - ((similarity_output(r,c) - min) / (max - min));
      png_rms.plot(c,r,colour,colour,colour);
    }
  }

// Overlay the DTW data
  for (int r=0; r < dtw_output.getRows(); ++r) {
    int x = dtw_output(r,0);
    int y = dtw_output(r,1);
    png_rms.plot(x,y,0.0,0.0,0.0);
  }


  png_rms.close();

#endif

}






void
toy_with_duplex()
{
  cout << "Toy_Withing duplex audio input and output" << endl;
  MarSystemManager mng;

  MarSystem* dnet;
  dnet = mng.create("Series", "dnet");

  dnet->addMarSystem(mng.create("AudioSource", "src"));
  dnet->addMarSystem(mng.create("Gain", "gain"));
  dnet->addMarSystem(mng.create("AudioSink", "dest"));


  dnet->updControl("mrs_real/israte", 44100.0);
  dnet->updControl("AudioSource/src/mrs_natural/nChannels", 1);
  dnet->updControl("AudioSource/src/mrs_bool/initAudio", true);
  dnet->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  for (int i=0; i < 500; ++i)
  {
    dnet->tick();
  }
}


void
toy_with_duplex2(mrs_string sfName)
{
  cout << "Toying with duplex audio input and output (2)" << endl;
  MarSystemManager mng;

  MarSystem* dnet;
  dnet = mng.create("Series", "dnet");

  dnet->addMarSystem(mng.create("SoundFileSource", "src"));
  dnet->addMarSystem(mng.create("Gain", "gain"));
  dnet->addMarSystem(mng.create("AudioSink", "adest"));
  dnet->addMarSystem(mng.create("AudioSource", "asrc"));
  dnet->addMarSystem(mng.create("SoundFileSink", "dest"));


  // dnet->updControl("mrs_real/israte", 44100.0);
  // dnet->updControl("AudioSource/asrc/mrs_natural/nChannels", 1);


  dnet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  dnet->updControl("SoundFileSink/dest/mrs_string/filename", "duplex2.wav");
  dnet->updControl("AudioSink/adest/mrs_bool/initAudio", true);
  dnet->updControl("AudioSource/asrc/mrs_bool/initAudio", true);

  while (dnet->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    dnet->tick();
  }
}











// Pluck(0,100,1.0,0.5,"Toy_WithPluckedRich0_100hz.wav");
//Pluck Karplus Strong Model Kastro.cpp output to wavfile
void
Pluck(mrs_real pos, mrs_real fre, mrs_real loz, mrs_real stret, string name)
{
  (void) loz;
  (void) stret;
  MarSystemManager mng;
  MarSystem* series = mng.create("Series", "series");

  series->addMarSystem(mng.create("Plucked", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));
  series->update();

  series->updControl("Gain/gain/mrs_real/gain", 1.0);
  series->updControl("SoundFileSink/dest/mrs_natural/nChannels",
                     series->getctrl("Plucked/src/mrs_natural/nChannels"));
  series->updControl("mrs_real/israte",
                     series->getctrl("Plucked/src/mrs_real/osrate"));
  series->updControl("SoundFileSink/dest/mrs_string/filename",name);

  series->updControl("Plucked/src/mrs_real/frequency",fre);
  series->updControl("Plucked/src/mrs_real/pluckpos",pos);
  // series->updControl("Plucked/src/mrs_real/loss",loz);
  //series->updControl("Plucked/src/mrs_real/stretch",stret);

  series->updControl("mrs_natural/inSamples", 512);
  series->update();

  cout << (*series) << endl;

  realvec in(series->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
             series->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec out(series->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
              series->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  mrs_natural t=0;

  for (t = 0; t < 400; t++)
  {
    series->process(in,out);
    t++;
  }

  //while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  //{
  //  series->tick();
  //}
}

// Pluck(0,100,1.0,0.5,"Toy_WithPluckedRich0_100hz.wav");
//Pluck Karplus Strong Model Kastro.cpp output to wavfile
//mrs_real pos, mrs_real fre, mrs_real loz, mrs_real stret, string name
void
toy_with_plucked(string sfName1,string sfName2,string sfName3,string sfName4,string sfName5,string sfName6)
{
  cout << "run using mudbox -t plucked <outputFilename> <position> <frequency> <loss> <stretch> <time played>" << endl;
  string name=sfName1;
  cout << "output file: " << name << endl;
  std::istringstream i1(sfName2);

  double dpos;
  i1 >> dpos;
  mrs_real pos=mrs_real(dpos);
  cout << "position:" << pos << endl;

  std::istringstream i2(sfName3);
  double dfre;
  i2 >> dfre;
  mrs_real fre=mrs_real(dfre);
  cout << "frequency:" << fre << endl;


  std::istringstream i3(sfName4);
  double dloz;

  i3 >> dloz;
  mrs_real loz=mrs_real(dloz);
  cout << "loss:" << loz << endl;

  double dstret;
  std::istringstream i4(sfName5);
  i4 >> dstret;
  mrs_real stret=mrs_real(dstret);
  cout << "strech:" <<  stret << endl;

  int time_played=400;
  std::istringstream i5(sfName6);
  i5 >> time_played;
  cout << "time played in ticks:" << time_played << endl;


  MarSystemManager mng;
  MarSystem* series = mng.create("Series", "series");
  cout<< "series created" << endl;
  series->addMarSystem(mng.create("Plucked", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  //series->addMarSystem(mng.create("MixToMono", "mix"));
  //series->addMarSystem(mng.create("AudioSink", "asink"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));

  cout<< "net designed" << endl;

  series->updControl("Gain/gain/mrs_real/gain", 1.0);
  series->updControl("SoundFileSink/dest/mrs_natural/nChannels",
                     series->getctrl("Plucked/src/mrs_natural/nChannels"));
  //set input sample rate
  series->updControl("mrs_real/israte",
                     series->getctrl("Plucked/src/mrs_real/osrate"));
  series->updControl("SoundFileSink/dest/mrs_string/filename",name);
  series->updControl("Plucked/src/mrs_real/frequency",fre);
  series->updControl("Plucked/src/mrs_real/pluckpos",pos);
  series->updControl("Plucked/src/mrs_real/loss",loz);
  series->updControl("Plucked/src/mrs_real/stretch",stret);

  series->updControl("mrs_natural/inSamples", 512);
  //cout << (*series) << endl;

  //realvec in(series->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
  //	series->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  //realvec out(series->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
  //	series->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  mrs_natural t=0;

  for (t = 0; t < time_played ; t++)
  {
    series->tick();
    t++;
  }

  //while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>())
  //{
  //  series->tick();
  //}
}


// PluckLive(0,100,1.0,0.5);
//Pluck Karplus Strong Model Plucked.cpp outputs to DAC
void PluckLive(mrs_real pos, mrs_real fre, mrs_real loz, mrs_real stret)
{
  (void) loz;
  (void) stret;



  MarSystemManager mng;

  MarSystem* series = mng.create("Series", "series");


  series->addMarSystem(mng.create("Plucked", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("AudioSink", "dest"));


  series->update();




  series->updControl("Gain/gain/mrs_real/gain", 1.0);

  series->updControl("AudioSink/dest/mrs_natural/nChannels",
                     series->getctrl("Plucked/src/mrs_natural/nChannels"));
  series->updControl("AudioSink/dest/mrs_real/israte",
                     series->getctrl("Plucked/src/mrs_real/osrate"));



  series->updControl("Plucked/src/mrs_real/frequency",fre);
  series->updControl("Plucked/src/mrs_real/pluckpos",pos);
  //series->updControl("Plucked/src/mrs_real/loss",loz);
  //series->updControl("Plucked/src/mrs_real/stretch",stret);

  series->updControl("mrs_natural/inSamples", 512);
  series->update();




  cout << (*series) << endl;


  realvec in(series->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
             series->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec out(series->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
              series->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  mrs_natural t=0;


  for (t = 0; t < 400; t++)
  {
    series->process(in,out);
    t++;

  }





  //while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  //{
  //  series->tick();
  //}


}


// PluckLive(0,100,1.0,0.5);
//Pluck Karplus Strong Model Plucked.cpp outputs to DAC
void toy_with_pluckedLive(string sfName2,string sfName3,string sfName4,string sfName5,string sfName6)
{

  std::istringstream i1(sfName2);

  double dpos;
  i1 >> dpos;
  mrs_real pos=mrs_real(dpos);
  cout << "position:" << pos << endl;

  std::istringstream i2(sfName3);
  double dfre;
  i2 >> dfre;
  mrs_real fre=mrs_real(dfre);
  cout << "frequency:" << fre << endl;


  std::istringstream i3(sfName4);
  double dloz;

  i3 >> dloz;
  mrs_real loz=mrs_real(dloz);
  cout << "loss:" << loz << endl;

  double dstret;
  std::istringstream i4(sfName5);
  i4 >> dstret;
  mrs_real stret=mrs_real(dstret);
  cout << "strech:" <<  stret << endl;

  int samrate=400;
  std::istringstream i5(sfName6);
  i5 >> samrate;
  cout << "samplingrate:" << samrate << endl;

  MarSystemManager mng;

  MarSystem* series = mng.create("Series", "series");
  MarSystem* fan=mng.create("Fanout","fan");


  MarSystem* intSeries1 = mng.create("Series", "intSeries1");
  MarSystem* intSeries2 = mng.create("Series", "intSeries2");


  intSeries1->addMarSystem(mng.create("Plucked", "src"));
  intSeries1->addMarSystem(mng.create("ADSR", "adsr1"));

  intSeries2->addMarSystem(mng.create("Plucked", "src2"));
  intSeries2->addMarSystem(mng.create("ADSR", "adsr2"));

  fan->addMarSystem(intSeries1);
  fan->addMarSystem(intSeries2);

  series->addMarSystem(fan);
  series->addMarSystem(mng.create("Selector","sel"));
  series->addMarSystem(mng.create("MixToMono", "mix"));
  series->addMarSystem(mng.create("AudioSink", "dest"));





  series->updControl("AudioSink/dest/mrs_natural/nChannels",
                     series->getControl("Plucked/src/mrs_natural/nChannels"));


  //	//addctrl("mrs_real/aRate", 0.001);//attack rate
  //addctrl("mrs_real/aTime", 0.2);//attack time
  //addctrl("mrs_real/aTarget", 1.0);//attack target
  ////addctrl("mrs_real/dRate", 0.001);//decay rate
  //addctrl("mrs_real/dTime", 0.1);//decay time
  //addctrl("mrs_real/susLevel", 0.85);//sustain level
  ////addctrl("mrs_real/rRate", 0.001);//release rate
  //addctrl("mrs_real/rTime", 0.2);//release time


  //double dRate=0.001;       //#decay rate -> computed from dTime : Rate_ = 1.0 / (Time_ * sampleRate_);
  //double rRate=0.001;       // #release rate -> computed from dTime : Rate_ = 1.0 / (Time_ * sampleRate_);
  double aTime=0.02;         //#attack time
  double aTarget=1.0;      // #attack target
  double dTime=0.03;         //#decay time
  double susLevel=0.75;    // #sustain level
  double rTime=0.9;

  series->updControl("Fanout/fan/Series/intSeries1/Plucked/src/mrs_real/frequency",fre);
  series->updControl("Fanout/fan/Series/intSeries1/Plucked/src/mrs_real/pluckpos",pos);
  series->updControl("Fanout/fan/Series/intSeries1/Plucked/src/mrs_real/loss",loz);
  series->updControl("Fanout/fan/Series/intSeries1/Plucked/src/mrs_real/stretch",stret);
  cout << "4 times" << endl;

  series->updControl("Fanout/fan/Series/intSeries1/ADSR/adsr1/mrs_real/aTime", aTime);         //#attack time
  series->updControl("Fanout/fan/Series/intSeries1/ADSR/adsr1/mrs_real/aTarget", aTarget);      // #attack target
  series->updControl("Fanout/fan/Series/intSeries1/ADSR/adsr1/mrs_real/dTime", dTime);         //#decay time
  series->updControl("Fanout/fan/Series/intSeries1/ADSR/adsr1/mrs_real/susLevel", susLevel);    // #sustain level
  series->updControl("Fanout/fan/Series/intSeries1/ADSR/adsr1/mrs_real/rTime", rTime);        // #release time

  //series->updControl("Fanout/fan/Series/intSeries1/Plucked/src/mrs_real/nton",1.0);
  series->updControl("Fanout/fan/Series/intSeries1/ADSR/adsr1/mrs_natural/state", 1);
  series->updControl("Fanout/fan/Series/intSeries1/ADSR/adsr1/mrs_real/nton", 1.0);


  series->updControl("Fanout/fan/Series/intSeries2/Plucked/src2/mrs_real/frequency",440.0);
  series->updControl("Fanout/fan/Series/intSeries2/Plucked/src2/mrs_real/pluckpos",pos);
  series->updControl("Fanout/fan/Series/intSeries2/Plucked/src2/mrs_real/loss",loz);
  series->updControl("Fanout/fan/Series/intSeries2/Plucked/src2/mrs_real/stretch",stret);
  cout << "4 times" << endl;

  series->updControl("Fanout/fan/Series/intSeries2/ADSR/adsr2/mrs_real/aTime", aTime);         //#attack time
  series->updControl("Fanout/fan/Series/intSeries2/ADSR/adsr2/mrs_real/aTarget", aTarget);      // #attack target
  series->updControl("Fanout/fan/Series/intSeries2/ADSR/adsr2/mrs_real/dTime", dTime);         //#decay time
  series->updControl("Fanout/fan/Series/intSeries2/ADSR/adsr2/mrs_real/susLevel", susLevel);    // #sustain level
  series->updControl("Fanout/fan/Series/intSeries2/ADSR/adsr2/mrs_real/rTime", rTime);        // #release time
  series->updControl("Fanout/fan/Series/intSeries2/ADSR/adsr2/mrs_natural/state", 1);
  series->updControl("Fanout/fan/Series/intSeries2/ADSR/adsr2/mrs_real/nton", 1.0);


  series->updControl("mrs_natural/inSamples", samrate);
  series->updControl("AudioSink/dest/mrs_bool/initAudio",true);
  //series->updControl("Selector/sel/mrs_natural/disable",1);


  int t=0;
  for (t = 0; t < 300; t++)
  {
    series->tick();
    //series->updControl("Fanout/fan/Series/intSeries1/Plucked/src/mrs_real/loss",0.0);
    cout << t << endl;
    if (t == 50)
    {
      series->updControl("Selector/sel/mrs_natural/disable",0);
    }
  }





  //while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>())
  //{
  //  series->tick();
  //}


}

// Example that demonstrates the features of the Plucked Marsystem and should further bug vs feature discussion
//Pluck Karplus Strong Model Plucked.cpp outputs to DAC
void toy_with_pluckedBug()
{


  MarSystemManager mng;

  MarSystem* series = mng.create("Series", "series");
  MarSystem* fan=mng.create("Fanout","fan");
  MarSystem* internSeries = mng.create("Series", "internSeries");
  MarSystem* internSeries2 = mng.create("Series", "internSeries2");

  internSeries->addMarSystem(mng.create("Plucked", "src"));
  //################ code starts acting strange as soon as a second plucked is in play
  internSeries2->addMarSystem(mng.create("Plucked", "src2"));

  fan->addMarSystem(internSeries);
  fan->addMarSystem(internSeries2);
  series->addMarSystem(fan);
  series->addMarSystem(mng.create("Selector","sel"));
  series->addMarSystem(mng.create("MixToMono", "mix"));
  series->addMarSystem(mng.create("AudioSink", "dest"));

  series->updControl("AudioSink/dest/mrs_natural/nChannels",
                     series->getControl("Plucked/src/mrs_natural/nChannels"));


  mrs_real pos=mrs_real(1.0);
  mrs_real fre=mrs_real(440.0);
  mrs_real loz=mrs_real(1.0);
  mrs_real stret=mrs_real(1.0);

  series->updControl("Fanout/fan/Series/internSeries/Plucked/src/mrs_real/frequency",fre);
  series->updControl("Fanout/fan/Series/internSeries/Plucked/src/mrs_real/pluckpos",pos);
  series->updControl("Fanout/fan/Series/internSeries/Plucked/src/mrs_real/loss",loz);
  series->updControl("Fanout/fan/Series/internSeries/Plucked/src/mrs_real/stretch",stret);

  series->updControl("Fanout/fan/Series/internSeries2/Plucked/src2/mrs_real/frequency",(fre+400.0));
  series->updControl("Fanout/fan/Series/internSeries2/Plucked/src2/mrs_real/pluckpos",pos);
  series->updControl("Fanout/fan/Series/internSeries2/Plucked/src2/mrs_real/loss",loz);
  series->updControl("Fanout/fan/Series/internSeries2/Plucked/src2/mrs_real/stretch",stret);


  series->updControl("AudioSink/dest/mrs_bool/initAudio",true);


  int t=0;
  for (t = 0; t < 300; t++)
  {
    series->tick();
    cout << t << endl;
    if (t == 50)
    {
      //this is when strange things happen

      series->updControl("Selector/sel/mrs_natural/disable",0);
    }
  }

}


void toy_with_normMaxMin()
{
  MarSystemManager mng;
  // MarSystem * nrm = mng.create("NormMaxMin", "nrm");
  MarSystem* nrm = mng.create("Normalize", "nrm");

  realvec in;
  realvec out;



  in.create((mrs_natural)4,(mrs_natural)3);
  out.create((mrs_natural)4,(mrs_natural)3);

  nrm->updControl("mrs_natural/inSamples", (mrs_natural)3);
  nrm->updControl("mrs_natural/inObservations", (mrs_natural)4);



  in(0,0) = 5.1;
  in(1,0) = 3.5;
  in(2,0) = 1.4;
  in(3,0) = 0.2;

  in(0,1) = 4.9;
  in(1,1) = 3.0;
  in(2,1) = 1.4;
  in(3,1) = 0.2;

  in(0,2) = 4.7;
  in(1,2) = 3.2;
  in(2,2) = 1.3;
  in(3,2) = 0.2;

  cout << "before processing " << endl;

  nrm->process(in,out);

  cout << "out = " << out << endl;
}


// take advantage of MarSystemManager
void
tempotoy_with_sfplay(string sfName)
{
  cout << "Playing " << sfName << endl;

  MarSystemManager mng;

  // Create a series Composite
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("AudioSink", "dest"));

  // only update controls from Composite level
  series->updControl("mrs_natural/inSamples", 128);
  series->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
    series->tick();

  delete series;
}



void
toy_with_tempo(string fname, mrs_natural tempo, mrs_natural rank)
{
  ifstream from(fname.c_str());

  string name;
  mrs_natural itempo;
  mrs_real strength;


  vector<string> tempo_map[11][11];
  vector<string> names;
  vector<mrs_real>  tempos;
  vector<mrs_real>  strengths;
  mrs_natural count = 0;




  mrs_real min_strength = 1000.0;
  mrs_real max_strength = 0.0;
  mrs_real min_tempo = 1000.0;
  mrs_real max_tempo = 0.0;

  while(!from.eof() )
  {
    from >> name;
    from >> itempo;
    from >> strength;

    names.push_back(name);
    tempos.push_back(itempo);
    strengths.push_back(strength);

    if (strength > max_strength)
      max_strength = strength;
    if (strength < min_strength)
      min_strength = strength;


    if (itempo > max_tempo)
      max_tempo = itempo;
    if (itempo < min_tempo)
      min_tempo = itempo;

    count++;
  }
  cout << "Min strength = " << min_strength << endl;
  cout << "Max strength = " << max_strength << endl;

  cout << "Min tempo = " << min_tempo << endl;
  cout << "Max temp = " << max_tempo << endl;


  for (mrs_natural i=0; i < count; ++i)
  {
    strengths[i] = floor(((strengths[i] - min_strength) / (max_strength - min_strength)) * 10.0 + 0.5);

    tempos[i] = floor(((tempos[i] - min_tempo) / (max_tempo - min_tempo)) * 10.0 + 0.5);

    /* cout << "Name = " << names[i] << endl;
       cout << "Tempo = " << tempos[i] << endl;
       cout << "Strength = " << strengths[i] << endl;
    */

    mrs_natural si = (mrs_natural)strengths[i];
    mrs_natural ti = (mrs_natural)tempos[i];

    /* cout << "si = " << si << endl;
       cout << "ti = " << ti << endl;
    */

    tempo_map[si][ti].push_back(names[i]);
  }








  for (mrs_natural ti = 0; ti < 11; ++ti)
    for (mrs_natural si = 0; si < 11; ++si)
    {
      cout << "Tempo = " << ti << " - " << "Strength = " << si << endl;
      vector<string> retrievedFiles = tempo_map[si][ti];

      vector<string>::iterator vi;

      for (vi = retrievedFiles.begin(); vi != retrievedFiles.end(); ++vi)
      {
        cout << (*vi) << endl;
      }



    }


  tempo = (mrs_natural)floor(((tempo - min_tempo) / (max_tempo - min_tempo)) * 10.0 + 0.5);
  vector<string> retrievedFiles = tempo_map[rank][tempo];

  if (retrievedFiles.size()  == 0)
    cout << "No file for these specs" << endl;
  else
  {
    cout << "Playing " << retrievedFiles[0] << endl;

    tempotoy_with_sfplay(retrievedFiles[0]);
  }

  return;
}



void
toy_with_stretchLinear(string sfName)
{
  MarSystemManager mng;
  MarSystem* net = mng.create("Series", "pnet");

  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("StretchLinear", "sl"));
  net->addMarSystem(mng.create("SoundFileSink", "dest"));

  net->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  net->updControl("StretchLinear/sl/mrs_real/stretch", 1.5);
  net->updControl("SoundFileSink/dest/mrs_string/filename", "foo.wav");
  mrs_natural size = net->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>();
  cout << "size = " << size << endl;

  net->updControl("mrs_natural/inSamples", size);



  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())				{
    net->tick();
  }
}




void
toy_with_pitch(string sfName)
{

  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("ShiftInput", "sfi"));
  pnet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->addMarSystem(mng.create("PitchPraat", "pitch"));
  // pnet->addMarSystem(mng.create("PitchSACF", "pitch"));
  pnet->addMarSystem(mng.create("RealvecSink", "rvSink"));

  mrs_real lowPitch = 36;
  mrs_real highPitch = 79;
  mrs_real lowFreq = pitch2hertz(lowPitch);
  mrs_real highFreq = pitch2hertz(highPitch);

  mrs_natural lowSamples =
    hertz2samples(highFreq, pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>());
  mrs_natural highSamples =
    hertz2samples(lowFreq, pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>());

  pnet->updControl("PitchPraat/pitch/mrs_natural/lowSamples", lowSamples);
  pnet->updControl("PitchPraat/pitch/mrs_natural/highSamples", highSamples);

  //  The window should be just long
  //  enough to contain three periods (for pitch detection)
  //  of MinimumPitch. E.g. if MinimumPitch is 75 Hz, the window length
  //  is 40 ms and padded with zeros to reach a power of two.
  mrs_real windowSize = 3/lowPitch*pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  pnet->updControl("mrs_natural/inSamples", 512);

  pnet->updControl("ShiftInput/sfi/mrs_natural/winSize", powerOfTwo(windowSize));
  //pnet->updControl("ShiftInput/sfi/mrs_natural/winSize", 1024);

  while (pnet->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
    pnet->tick();

  realvec data = pnet->getctrl("RealvecSink/rvSink/mrs_realvec/data")->to<mrs_realvec>();
  for (mrs_natural i=1; i<data.getSize(); i+=2)
    data(i) = samples2hertz(data(i), pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>());

  pnet->updControl("RealvecSink/rvSink/mrs_bool/done", true);

  MATLAB_PUT(data, "data");
  MATLAB_EVAL("plot(data(2:2:end))");

  cout << data ;
  // to output to a file
  ofstream dataFile;
  dataFile.open("data.txt");
  dataFile << data;
  //////////////////////////
  delete pnet;
}

void
toy_with_centroid(string sfName1)
{
  cout << "Toy with centroid " << sfName1 << endl;
  MarSystemManager mng;

  MarSystem* net = mng.create("Series/net");
  MarSystem* accum = mng.create("Accumulator/accum");

  MarSystem* cnet = mng.create("Series/cnet");
  cnet->addMarSystem(mng.create("SoundFileSource/src"));
  cnet->addMarSystem(mng.create("Gain/gain"));
  cnet->addMarSystem(mng.create("Windowing/ham"));
  cnet->addMarSystem(mng.create("Spectrum/spk"));
  cnet->addMarSystem(mng.create("PowerSpectrum/pspk"));
  cnet->addMarSystem(mng.create("Centroid/cntrd"));
  cnet->linkControl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");

  accum->addMarSystem(cnet);
  net->addMarSystem(accum);

  accum->updControl("mrs_string/mode", "explicitFlush");
  cnet->updControl("mrs_string/filename", sfName1);

  //mrs_real val = 0.0;

  ofstream ofs;
  ofs.open("centroid.mpl");
  ofs << *cnet << endl;
  ofs.close();

  net->updControl("Accumulator/accum/mrs_natural/maxTimes", 2000);
  net->updControl("Accumulator/accum/mrs_natural/timesToKeep", 1);
  net->linkControl("Accumulator/accum/mrs_bool/flush",
                   "Accumulator/accum/Series/cnet/SoundFileSource/src/mrs_bool/currentCollectionNewFile");
  net->updControl("Accumulator/accum/Series/cnet/SoundFileSource/src/mrs_real/duration", 0.5);

  while(net->getControl("Accumulator/accum/Series/cnet/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    cout << net->getControl("Accumulator/accum/Series/cnet/SoundFileSource/src/mrs_string/currentlyPlaying")->to<mrs_string>() << endl;

    net->tick();
    const mrs_realvec& src_data =
      net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    cout << src_data << endl;
  }

}



void
toy_with_confidence(string sfName)
{
  MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("Confidence", "confidence"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));

  pnet->linkControl("SoundFileSource/src/mrs_string/currentlyPlaying", "Confidence/confidence/mrs_string/fileName");

  pnet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  pnet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
  pnet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");

  mrs_bool isEmpty;
  //cout << *pnet << endl;
  while ((isEmpty = pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>()) != true)
  {
    //cout << "pos " << pnet->getctrl("mrs_natural/pos")->to<mrs_natural>() << endl;
    cout << pnet->getctrl("SoundFileSource/src/mrs_string/currentlyPlaying")->to<mrs_string>() << endl;
    cout << pnet->getctrl("Confidence/confidence/mrs_string/fileName")->to<mrs_string>() << endl;

    pnet->tick();

    //toy_with if setting "mrs_natural/pos" to 0 for rewinding is working
    //if(pnet->getctrl("mrs_natural/pos")->to<mrs_natural>() > 100000)
    //	pnet->updControl("mrs_natural/pos", 0);
  }
  cout << "tick " << isEmpty << endl;
  delete pnet;
}

void
toy_with_realvecCtrl(string sfName)
{
  MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));


  pnet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  pnet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
  pnet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");

  while ( pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {

    pnet->tick();

  }
  delete pnet;
}

void
toy_with_power(string sfName)
{
  MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->addMarSystem(mng.create("ShiftInput", "si"));
  pnet->addMarSystem(mng.create("Power", "pw"));
  pnet->addMarSystem(mng.create("RealvecSink", "rvSink"));

  pnet->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  pnet->updControl("ShiftInput/si/mrs_natural/winSize", 2048);

  pnet->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");
  pnet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");

  //  cout << *pnet;
  while ( pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    cout << pnet->getctrl("mrs_natural/pos")->to<mrs_natural>();
    pnet->tick();

  }
  realvec data = pnet->getctrl("RealvecSink/rvSink/mrs_realvec/data")->to<mrs_realvec>();

  // dB conversion
  for (mrs_natural i=0 ; i<data.getSize() ; ++i)
    data(i) = 20*log10(data(i));

  cout << data;

  delete pnet;
}

void
toy_with_shredder(string sfName)
{
  MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");
  MarSystem* acc = mng.create("Accumulator", "acc");
  MarSystem* shred = mng.create("Shredder", "shred");

  acc->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(acc);
  shred->addMarSystem(mng.create("AudioSink", "dest"));
  pnet->addMarSystem(shred);

  pnet->updControl("Accumulator/acc/SoundFileSource/src/mrs_string/filename", sfName);
  pnet->updControl("Shredder/shred/AudioSink/dest/mrs_bool/initAudio", true);

  pnet->linkControl("mrs_bool/hasData", "Accumulator/acc/SoundFileSource/src/mrs_bool/hasData");
  pnet->linkControl("mrs_natural/pos", "Accumulator/acc/SoundFileSource/src/mrs_natural/pos");

  pnet->updControl("Accumulator/acc/mrs_natural/nTimes", 10);
  pnet->updControl("Shredder/shred/mrs_natural/nTimes", 10);


  while (pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    pnet->tick();
  }
  delete pnet;
}


void
toy_with_scheduler(string sfName)
{
  cout << "Testing scheduler" << endl;

  string ipName=sfName;
  string opName="scheduled.wav";

  cout    << "Input: " << ipName
          << "\nOutput: " << opName << endl;

  MarSystemManager mng;

  // Create a series Composite
  //    type, name
  MarSystem* series1 = mng.create("Series", "series1");
  series1->addMarSystem(mng.create("SoundFileSource", "src"));
  series1->addMarSystem(mng.create("Gain", "gain"));
  series1->addMarSystem(mng.create("SoundFileSink", "dest"));

  // only update controls from Composite level
  series1->updControl("mrs_natural/inSamples", 256);
  series1->updControl("SoundFileSource/src/mrs_string/filename", ipName);
  series1->updControl("SoundFileSink/dest/mrs_string/filename", opName);

  // post events to the scheduler using updControl(..)
  series1->updControl("Gain/gain/mrs_real/gain", 1.0);
  series1->updControl(TmTime("TmVirtualTime/Virtual","1s"), Repeat("2s", 3), new EvValUpd(series1,"Gain/gain/mrs_real/gain", 0.0));
  series1->updControl(TmTime("TmVirtualTime/Virtual","2s"), Repeat("2s", 3), new EvValUpd(series1,"Gain/gain/mrs_real/gain", 1.0));

  for (int i=0; i<1000; ++i) {
    series1->tick();
  }

  // Composite deletes the added MarSystems
  // so you must not delete them
  delete series1;
}


mrs_natural randomInt(mrs_natural max) {
  return (mrs_natural)((float)max * rand() / (RAND_MAX + 1.0));
}

mrs_real randomFloat(mrs_real max) {
  return (mrs_real)(max * rand() / (RAND_MAX + 1.0));
}



void toy_phisem()
{
  mrs_real israte;
  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("PhiSEMSource", "src"));
  playbacknet->addMarSystem(mng.create("PhiSEMFilter", "filter"));
  playbacknet->addMarSystem(mng.create("Gain", "gain"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  playbacknet->updControl("Gain/gain/mrs_real/gain", 0.05);
  playbacknet->updControl("AudioSink/dest/mrs_natural/bufferSize", 256);
  playbacknet->updControl("AudioSink/dest/mrs_bool/initAudio", true);
  playbacknet->updControl("mrs_natural/inSamples", 1024);

  realvec resonances;
  realvec frequencies;
  resonances.create(10);
  frequencies.create(10);

  israte = playbacknet->getctrl("mrs_real/israte")->to<mrs_real>();
  cout << "Sample Rate: " << israte << endl;
  for (;;) {

    mrs_natural numObjects = 1 + randomInt(10);
    mrs_real systemDecay = 0.5 + randomFloat(0.5); //0.999
    mrs_real soundDecay = 0.5 + randomFloat(0.5); //0.95;
    mrs_real baseGain = 10.0 + randomFloat(10.0);
    mrs_natural numFilters = 1 + randomInt(5);

    cout << "===========================" << endl;
    cout << "numObjects:  " << numObjects << endl
         << "baseGain:    " << baseGain << endl
         << "systemDecay: " << systemDecay << endl
         << "soundDecay:  " << soundDecay << endl
         << "numFilters:  " << numFilters << endl;

    for(int i=0; i < numFilters; ++i) {
      resonances(i) = 0.9 + randomFloat(0.1);
      frequencies(i) = 200.0 * (1 + randomInt(30));
      cout << "  filter(" << i << ") freq=" << frequencies(i)
           << " res=" << resonances(i) << endl;
    }

    playbacknet->updControl("PhiSEMSource/src/mrs_natural/numObjects", numObjects);
    playbacknet->updControl("PhiSEMSource/src/mrs_real/systemDecay", systemDecay);
    playbacknet->updControl("PhiSEMSource/src/mrs_real/soundDecay", soundDecay);
    playbacknet->updControl("PhiSEMSource/src/mrs_real/baseGain", baseGain);

    playbacknet->updControl("PhiSEMFilter/filter/mrs_realvec/resonances", resonances);
    playbacknet->updControl("PhiSEMFilter/filter/mrs_realvec/frequencies", frequencies);
    playbacknet->updControl("PhiSEMFilter/filter/mrs_natural/numFilters", numFilters);

    for(int i=0; i < 2*israte/1024; ++i) {
      playbacknet->tick();
    }
  }
  delete playbacknet;

}

void
toy_with_margrid(string sfName)
{
  MarSystemManager mng;
  MarSystem* total_;

  string trainFname = sfName;



  cout << "Toying with MarGrid" << endl;
  // Build network for feature extraction
  MarSystem* extractNet = mng.create("Series", "extractNet");
  extractNet->addMarSystem(mng.create("SoundFileSource", "src"));
  extractNet->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  MarSystem* spectralNet = mng.create("Series", "spectralNet");
  spectralNet->addMarSystem(mng.create("Windowing", "ham"));
  spectralNet->addMarSystem(mng.create("Spectrum", "spk"));
  spectralNet->addMarSystem(mng.create("PowerSpectrum", "pspk"));

  MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
  featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
  featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));

  spectralNet->addMarSystem(featureFanout);
  extractNet->addMarSystem(spectralNet);
  extractNet->addMarSystem(mng.create("Memory", "mem"));

  MarSystem* stats = mng.create("Fanout", "stats");
  stats->addMarSystem(mng.create("Mean", "mn1"));
  stats->addMarSystem(mng.create("StandardDeviation", "std1"));
  extractNet->addMarSystem(stats);

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updControl("mrs_natural/nTimes", 1200);
  acc->addMarSystem(extractNet);

  total_ = mng.create("Series", "total");
  total_->addMarSystem(acc);
  MarSystem* stats2 = mng.create("Fanout", "stats2");
  stats2->addMarSystem(mng.create("Mean", "mn2"));
  stats2->addMarSystem(mng.create("StandardDeviation", "std2"));

  total_->addMarSystem(stats2);
  total_->addMarSystem(mng.create("Annotator", "ann"));



  total_->linkControl("mrs_string/filename",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/filename");


  total_->linkControl("mrs_string/currentlyPlaying",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/currentlyPlaying");


  total_->linkControl("mrs_bool/shuffle",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/shuffle");

  total_->linkControl("mrs_natural/pos",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/pos");

  total_->linkControl("mrs_real/repetitions",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_real/repetitions");


  total_->linkControl("mrs_natural/cindex",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/cindex");

  total_->linkControl("mrs_natural/numFiles",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/numFiles");

  total_->linkControl("mrs_string/allfilenames",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/allfilenames");

  total_->linkControl("mrs_natural/numFiles",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/numFiles");


  total_->linkControl("mrs_bool/hasData",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/hasData");
  total_->linkControl("mrs_bool/advance",
                      "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/advance");

  total_->linkControl("mrs_bool/memReset",
                      "Accumulator/acc/Series/extractNet/Memory/mem/mrs_bool/reset");

  total_->linkControl("mrs_natural/label",
                      "Annotator/ann/mrs_natural/label");



  total_->updControl("mrs_natural/inSamples", 512);


  string predictFname = "test.mf";
  total_->updControl("mrs_string/filename", trainFname);
  total_->updControl("mrs_real/repetitions", 1.0);

  // EXTRACT FEATURES
  int index= 0;
  int numFiles = total_->getctrl("mrs_natural/numFiles")->to<mrs_natural>();

  int som_height = 12;
  int som_width = 12;


  realvec som_in;
  realvec som_res;
  realvec som_fmatrix;

  mrs_natural total_onObservations =
    total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>();

  som_in.create(total_->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                total_->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  som_res.create(total_onObservations,
                 total_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  som_fmatrix.create(total_onObservations,
                     numFiles);

  // calculate features
  cout << "Calculating features" << endl;
  for (index=0; index < numFiles; index++)
  {
    total_->updControl("mrs_natural/label", index);
    total_->updControl("mrs_bool/memReset", true);
    total_->updControl("mrs_natural/cindex", index);
    string current = total_->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>();
    cout << current  << " - ";

    cout << "Processed " << index << " files " << endl;

    total_->process(som_in,som_res);

    for (int o=0; o < total_onObservations; o++)
      som_fmatrix(o, index) = som_res(o, 0);
    total_->updControl("mrs_bool/advance", true);

  }

  ofstream oss;
  oss.open("som_fmatrix.txt");
  oss << som_fmatrix << endl;


  // Read the feature matrix from file som_fmatrix.txt
  realvec train_som_fmatrix;
  ifstream iss;
  iss.open("som_fmatrix.txt");
  iss >> train_som_fmatrix;

  MarSystem*  norm_;
  MarSystem*  som_;
  realvec norm_som_fmatrix;



  // Normalize the feature matrix so that all features are between 0 and 1
  norm_som_fmatrix.create(train_som_fmatrix.getRows(),
                          train_som_fmatrix.getCols());
  norm_ = mng.create("NormMaxMin", "norm");
  norm_->updControl("mrs_natural/inSamples", train_som_fmatrix.getCols());
  norm_->updControl("mrs_natural/inObservations",
                    total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>());
  norm_->updControl("mrs_string/mode", "train");
  norm_->process(train_som_fmatrix, norm_som_fmatrix);
  norm_->updControl("mrs_string/mode", "predict");
  norm_->process(train_som_fmatrix, norm_som_fmatrix);



  // Create netork for training the self-organizing map
  som_ = mng.create("SOM", "som");
  som_->updControl("mrs_natural/grid_width", som_width);
  som_->updControl("mrs_natural/grid_height", som_height);
  som_->updControl("mrs_natural/inSamples", norm_som_fmatrix.getCols());
  som_->updControl("mrs_natural/inObservations", norm_som_fmatrix.getRows());
  som_->updControl("mrs_string/mode", "train");



  realvec som_fmatrixres;
  som_fmatrixres.create(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                        som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  cout << "Starting training" << endl;

  for (int i=0; i < 2000; i ++)
  {
    cout << "Training iteration" << i << endl;
    norm_som_fmatrix.shuffle();
    som_->process(norm_som_fmatrix, som_fmatrixres);
  }

  cout << "Training done" << endl;

  // write the trained som network and the feature normalization networks
  ofstream oss1;
  oss1.open("som.mpl");
  oss1 << *som_ << endl;
  delete som_;

  ofstream noss;
  noss.open("norm.mpl");
  noss << *norm_ << endl;
  delete norm_;

  // read trained som network from file som.mpl and normalization network norm.mpl
  ifstream iss1;
  iss1.open("som.mpl");
  som_ = mng.getMarSystem(iss1);

  ifstream niss1;
  niss1.open("norm.mpl");
  norm_ = mng.getMarSystem(niss1);

  cout << "Starting prediction" << endl;
  som_->updControl("mrs_string/mode", "predict");

  Collection l1;
  l1.read(predictFname);
  cout << "Read collection" << endl;

  total_->updControl("mrs_natural/pos", 0);

  total_->updControl("mrs_string/filename", predictFname);

  som_->updControl("mrs_natural/inSamples", 1);

  realvec predict_res(som_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                      som_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  norm_->updControl("mrs_natural/inSamples", 1);


  realvec norm_som_res;

  som_in.create(total_->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                total_->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  som_res.create(total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                 total_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  norm_som_res.create(total_->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
                      total_->getctrl("mrs_natural/onSamples")->to<mrs_natural>());


  for (mrs_natural index = 0; index < (mrs_natural)l1.size(); index++)
  {
    total_->updControl("mrs_natural/label", (mrs_natural)index);
    total_->updControl("mrs_bool/memReset", true);
    total_->updControl("mrs_natural/cindex", (mrs_natural)index);
    string current = total_->getctrl("mrs_string/currentlyPlaying")->to<mrs_string>();

    total_->process(som_in, som_res);
    norm_->process(som_res, norm_som_res);
    som_->process(norm_som_res, predict_res);
    cout << "Predicting  " << current << endl;

    cout << predict_res(0) << endl;
    cout << predict_res(1) << endl;

    total_->updControl("mrs_bool/advance", true);
  }



  cout << "end_prediction" << endl;





}



void
toy_with_multichannel_merge(string sfName)
{
  (void) sfName;

  string in1AudioFileName = "in1.wav";
  string in2AudioFileName = "in2.wav";
  string in3AudioFileName = "in3.wav";
  string in4AudioFileName = "in4.wav";
  string in5AudioFileName = "in5.wav";
  string in6AudioFileName = "in6.wav";
  string outAudioFileName = "out.wav";

  MarSystemManager mng;

  //////////////////////////////////////////////////
  //
  // A network to contain everything
  //
  MarSystem* playbacknet = mng.create("Series", "playbacknet");

  //////////////////////////////////////////////////
  //
  // A Fanout that contains all our 6 sound sources
  //
  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(mng.create("SoundFileSource", "src1"));
  fanout->addMarSystem(mng.create("SoundFileSource", "src2"));
  fanout->addMarSystem(mng.create("SoundFileSource", "src3"));
  fanout->addMarSystem(mng.create("SoundFileSource", "src4"));
  fanout->addMarSystem(mng.create("SoundFileSource", "src5"));
  fanout->addMarSystem(mng.create("SoundFileSource", "src6"));

  //////////////////////////////////////////////////
  //
  // Set the filenames on all the SoundFileSources
  //
  fanout->updControl("SoundFileSource/src1/mrs_string/filename",in1AudioFileName);
  fanout->updControl("SoundFileSource/src2/mrs_string/filename",in2AudioFileName);
  fanout->updControl("SoundFileSource/src3/mrs_string/filename",in3AudioFileName);
  fanout->updControl("SoundFileSource/src4/mrs_string/filename",in4AudioFileName);
  fanout->updControl("SoundFileSource/src5/mrs_string/filename",in5AudioFileName);
  fanout->updControl("SoundFileSource/src6/mrs_string/filename",in6AudioFileName);

  //////////////////////////////////////////////////
  //
  // Add the fanout to the main network
  //
  playbacknet->addMarSystem(fanout);

  //////////////////////////////////////////////////
  //
  // The output file which is a SoundFileSink
  //
  playbacknet->addMarSystem(mng.create("SoundFileSink", "dest"));
  playbacknet->updControl("SoundFileSink/dest/mrs_string/filename",outAudioFileName);

  //////////////////////////////////////////////////
  //
  // Tick the network until the first sound file
  // doesn't have any more data.
  //
  while (playbacknet->getctrl("Fanout/fanout/SoundFileSource/src1/mrs_bool/hasData")->isTrue())	{
    playbacknet->tick();
  }

}

#include <marsyas/maroxml.h>
#include <marsyas/marosvg.h>
#include <marsyas/marohtml.h>
#include <marsyas/marojson.h>

void toy_with_marostring(std::string format)
{
  MarSystemManager mng;
  MarSystem* ser = mng.create("Series","ser");
  ser->addMarSystem(mng.create("SoundFileSource","src"));
  MarSystem* fan = mng.create("Fanout","fan");
  fan->addMarSystem(mng.create("Gain","g1"));
  fan->addMarSystem(mng.create("Gain","g2"));
  fan->addMarSystem(mng.create("Gain","g3"));
  ser->addMarSystem(fan);
  ser->addMarSystem(mng.create("Gain","g4"));
  ser->addMarSystem(mng.create("AudioSink","snk"));

  if(format=="html") {
    marohtml m;
    ser->toString(m);
    cout << m.str();
  }
  else if (format=="svg") {
    marosvg m;
    m.style("fanout","fill","green");
    ser->toString(m);
    cout << m.str();
  }
  else if (format=="xml") {
    maroxml m;
    ser->toString(m);
    cout << m.str();
  }
  else if (format=="json") {
    marojson m;
    ser->toString(m);
    cout << m.str();
  }
}


mrs_real find_max_rms(string inFileName)
{

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  MarSystem* sr = mng.create("SilenceRemove", "sr");
  // Threshold for silence removal
  sr->updControl("mrs_real/threshold",0.00001);

  sr->addMarSystem(mng.create("SoundFileSource", "src"));
  sr->updControl("SoundFileSource/src/mrs_string/filename",inFileName);

  net->addMarSystem(sr);

  MarSystem* rms = mng.create("Rms", "rms");
  net->addMarSystem(rms);

  net->addMarSystem(mng.create("Gain", "gain"));

  mrs_real max_rms = MINREAL;
  mrs_real r;
  while (sr->getctrl("SoundFileSource/src/mrs_bool/hasData")->isTrue())	{
    net->tick();
    r = rms->getctrl("mrs_realvec/processedData")->to<mrs_realvec>()(0);
    if (r > max_rms) {
      max_rms = r;
    }
  }

  return max_rms;

}

void
toy_with_volume_normalize(string inFileName, string outFileName)
{

  mrs_real max_rms = find_max_rms(inFileName);
  mrs_real gain = 0.8 / max_rms;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  MarSystem* sr = mng.create("SilenceRemove", "sr");
  // Threshold for silence removal
  sr->updControl("mrs_real/threshold",0.00001);

  sr->addMarSystem(mng.create("SoundFileSource", "src"));
  sr->updControl("SoundFileSource/src/mrs_string/filename",inFileName);

  net->addMarSystem(sr);

  net->addMarSystem(mng.create("Gain", "gain"));
  net->updControl("Gain/gain/mrs_real/gain",gain);

  net->addMarSystem(mng.create("SoundFileSink", "dest"));
  net->updControl("mrs_real/israte", 44100.0);
  net->updControl("SoundFileSink/dest/mrs_string/filename",outFileName);

  while (sr->getctrl("SoundFileSource/src/mrs_bool/hasData")->isTrue())	{
    net->tick();
  }

}

void toy_with_accent_filter_bank(string inFileName, string outFileName)
{
  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  net->updControl("mrs_natural/inSamples", 4096);

  net->updControl("mrs_real/israte", 44100.0);

  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->updControl("SoundFileSource/src/mrs_string/filename", inFileName);

  net->addMarSystem(mng.create("Pipe_Block", "pipe"));
  net->updControl("Pipe_Block/pipe/mrs_natural/factor", 12);

  net->addMarSystem(mng.create("SoundFileSink", "dest"));
  net->updControl("SoundFileSink/dest/mrs_string/filename",outFileName);

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->isTrue())	{
    net->tick();
  }

}

void toy_with_chroma(string inSoundFileName, string inTextFileName)
{
  MarSystemManager theManager;

  // ------------------------ DEFINE MARSYSTEM ------------------------
  MarSystem *theExtractorNet, *theNewSystem;
  theExtractorNet = theManager.create("Series", "SER1");

  // Add new MarSystems
  MarSystem* theDummy;
  theDummy = new Spectrum2ACMChroma("Anything");
  theManager.registerPrototype("Spectrum2ACMChroma",theDummy);

  // 1. Read sound file
  theNewSystem = theManager.create("SoundFileSource","Source");
  theExtractorNet->addMarSystem(theNewSystem);

  // 2. Convert stereo to mono
  theNewSystem = theManager.create("Stereo2Mono","ToMono");
  theExtractorNet->addMarSystem(theNewSystem);

  // (!!) Compensate for x0.5 in Stereo2Mono in case of mono file
  theNewSystem = theManager.create("Gain","Gain1");
  theExtractorNet->addMarSystem(theNewSystem);

  // 3. Downsample to ~8kHz
  theNewSystem = theManager.create("DownSampler","Resample");
  theExtractorNet->addMarSystem(theNewSystem);

  // 4. Store "windowsize" samples in buffer
  theNewSystem = theManager.create("ShiftInput","Buffer");
  theExtractorNet->addMarSystem(theNewSystem);

  // 5. Perform windowing on buffer values
  /* This function includes a 'x 2/Sum(w(n))' normalization
     with 2/Sum(w(n)) = 2 x 1/FS x FS/Sum(w(n)) with
     - 2 = energy in positive spectrum = energy in full spectrum
     - 1/FS = spectrum normalization (?)
     - FS/Sum(w(n)) = compensate for window shape */
  theNewSystem = theManager.create("Windowing","Windowing");
  theExtractorNet->addMarSystem(theNewSystem);

  // 6. Transform to frequency domain
  theNewSystem = theManager.create("Spectrum","CompSpectrum");
  theExtractorNet->addMarSystem(theNewSystem);

  // (!!) Compensate for normalization in fft
  theNewSystem = theManager.create("Gain","Gain2");
  theExtractorNet->addMarSystem(theNewSystem);

  // 7. Compute amplitude spectrum
  theNewSystem = theManager.create("PowerSpectrum","AmpSpectrum");
  theExtractorNet->addMarSystem(theNewSystem);

  // 8. Compute chroma profile
  theNewSystem = theManager.create("Spectrum2ACMChroma","Spectrum2Chroma");
  theExtractorNet->addMarSystem(theNewSystem);

  // 11. Text file output
  theNewSystem = theManager.create("RealvecSink","TextFileOutput");
  theExtractorNet->addMarSystem(theNewSystem);

  // ------------------------ SET PARAMETERS ------------------------
  mrs_real theHopSize = 0.02f;
  mrs_real theFrameSize = 0.08f;			// 0.150
  mrs_real theTargetSampleRate = 8000.f;
  mrs_natural theFFTSize = 8192;

  // First!! declare source, because (most) parameters rely on sample rate
  mrs_string theControlString = "SoundFileSource/Source/mrs_string/filename";
  theExtractorNet->updControl(theControlString,inSoundFileName);

  theControlString = "SoundFileSource/Source/mrs_real/osrate";
  MarControlPtr theControlPtr = theExtractorNet->getctrl(theControlString);
  mrs_real theInSampleRate = theControlPtr->to<mrs_real>();

  mrs_natural theHopNrOfSamples = (mrs_natural)floor(theHopSize*theInSampleRate+0.5);
  theControlString = "mrs_natural/inSamples";		// Why not "SoundFileSource/../inSamples" ?
  theExtractorNet->updControl(theControlString,theHopNrOfSamples);

  theControlString = "Gain/Gain1/mrs_real/gain";
  theExtractorNet->updControl(theControlString,2.);

  mrs_natural theFactor = (mrs_natural)floor(theInSampleRate/theTargetSampleRate);
  theControlString = "DownSampler/Resample/mrs_natural/factor";
  theExtractorNet->updControl(theControlString,theFactor);

  mrs_natural theFrameNrOfSamples = (mrs_natural)floor(theFrameSize*theInSampleRate+0.5);
  theControlString = "ShiftInput/Buffer/mrs_natural/winSize";
  theExtractorNet->updControl(theControlString,theFrameNrOfSamples);

  theControlString = "Windowing/Windowing/mrs_natural/zeroPadding";
  theExtractorNet->updControl(theControlString,theFFTSize-theFrameNrOfSamples);
  // Default: Hamming window

  theControlString = "Windowing/Windowing/mrs_bool/normalize";
  theExtractorNet->updControl(theControlString,true);

  theControlString = "Gain/Gain2/mrs_real/gain";
  theExtractorNet->updControl(theControlString,(mrs_real)theFFTSize);

  theControlString = "PowerSpectrum/AmpSpectrum/mrs_string/spectrumType";
  theExtractorNet->updControl(theControlString,"magnitude");

  theControlString = "RealvecSink/TextFileOutput/mrs_string/fileName";
  theExtractorNet->updControl(theControlString,inTextFileName);

  // ------------------------ COMPUTE CHROMA PROFILES ------------------------
  clock_t start = clock();
  theControlString = "SoundFileSource/Source/mrs_bool/hasData";
  while (theExtractorNet->getctrl(theControlString)->to<mrs_bool>())
    theExtractorNet->tick();

  clock_t finish = clock();
  cout << "Duration: " << (double)(finish-start)/CLOCKS_PER_SEC << endl;

  // For debugging
  //vector<string> theSupportedMarSystems = theManager.registeredPrototypes();
  //map<string,MarControlPtr> theMap = theExtractorNet->getControls();

  //theExtractorNet->tick();
  //theExtractorNet->tick();
  //theExtractorNet->tick();
  //theExtractorNet->tick();

  delete theExtractorNet;
}

void toy_with_orca_record(string outFileName)
{
  MarSystemManager mng;

  mrs_natural copt = 1;
  mrs_real sropt = 44100.0;
  int bufferSize = 512;
//  	int bufferSize = 614;
  mrs_real length = 5;

  // To test the Tascam FW-1804 for orcarecord
  //     copt = 8;
  //     sropt = 44100.0;
  //     int bufferSize = 614;

  MarSystem* recordNet = mng.create("Series", "recordNet");
  MarSystem* asrc = mng.create("AudioSource", "asrc");
  MarSystem* dest = mng.create("SoundFileSink", "dest");

  recordNet->addMarSystem(asrc);
  recordNet->addMarSystem(dest);

  recordNet->updControl("mrs_real/israte", sropt);

  asrc->setctrl("mrs_natural/nChannels", copt);
  asrc->setctrl("mrs_natural/inSamples", bufferSize);
  asrc->setctrl("mrs_natural/bufferSize", bufferSize);
  asrc->setctrl("mrs_real/israte", sropt);

  dest->updControl("mrs_natural/inObservations", copt);
  dest->updControl("mrs_natural/inSamples", bufferSize);
  dest->updControl("mrs_real/israte", sropt);

  // Ready to initialize audio device
  recordNet->updControl("AudioSource/asrc/mrs_bool/initAudio", true);
  recordNet->updControl("SoundFileSink/dest/mrs_string/filename", outFileName);

  mrs_real srate = recordNet->getctrl("mrs_real/israte")->to<mrs_real>();
  mrs_natural nChannels = recordNet->getctrl("AudioSource/asrc/mrs_natural/nChannels")->to<mrs_natural>();
  cout << "AudioSource srate =  " << srate << endl;
  cout << "AudioSource nChannels = " << nChannels << endl;
  mrs_natural inSamples = recordNet->getctrl("mrs_natural/inSamples")->to<mrs_natural>();


  mrs_natural iterations = (mrs_natural)((srate * length) / inSamples);

  cout << "Iterations = " << iterations << endl;


  for (mrs_natural t = 0; t < iterations; t++)
  {
    recordNet->tick();
  }

}

void toy_with_realvec_record(string outFileName)
{
  MarSystemManager mng;

  mrs_natural copt = 1;
  mrs_real sropt = 44100.0;
  int bufferSize = 8 * 512;
//  	int bufferSize = 614;
  mrs_real length = 60;

  MarSystem* recordNet = mng.create("Series", "recordNet");
//     MarSystem* asrc = mng.create("SoundFileSource", "asrc");
  MarSystem* asrc = mng.create("AudioSource", "asrc");
  MarSystem* dest = mng.create("RealvecSink", "dest");

  recordNet->addMarSystem(asrc);
  recordNet->addMarSystem(dest);

  recordNet->updControl("mrs_real/israte", sropt);

  recordNet->updControl("AudioSource/asrc/mrs_natural/nChannels", copt);
  recordNet->updControl("AudioSource/asrc/mrs_natural/bufferSize", bufferSize);
  recordNet->updControl("mrs_natural/inSamples", bufferSize);
  recordNet->updControl("mrs_real/israte", sropt);

// 	recordNet->updControl("SoundFileSource/asrc/mrs_string/filename", "small_click.wav");
//  	recordNet->updControl("SoundFileSource/asrc/mrs_string/filename", "click_track.wav");

  // Ready to initialize audio device
  recordNet->updControl("AudioSource/asrc/mrs_bool/initAudio", true);

  mrs_real srate = recordNet->getctrl("mrs_real/israte")->to<mrs_real>();
  mrs_natural nChannels = recordNet->getctrl("AudioSource/asrc/mrs_natural/nChannels")->to<mrs_natural>();
  cout << "AudioSource srate =  " << srate << endl;
  cout << "AudioSource nChannels = " << nChannels << endl;
  mrs_natural inSamples = recordNet->getctrl("mrs_natural/inSamples")->to<mrs_natural>();


  mrs_natural iterations = (mrs_natural)((srate * length) / inSamples);

  cout << "Iterations = " << iterations << endl;

  cout << "Reading data into the RealvecSink" << endl;
  for (mrs_natural t = 0; t < iterations; t++)
  {
    recordNet->tick();
  }

  // The data in the RealvecSink
// 	cout << "########## INPUT ##########" << endl;
// 	cout << dest->getctrl("mrs_realvec/data")->to<mrs_realvec>() << endl;

  //
  // Output the data to an audio file
  //
  cout << "Outputting data to SoundFileSink" << endl;

  MarSystem* playbackNet = mng.create("Series", "playbackNet");
  MarSystem* rsrc = mng.create("RealvecSource", "rsrc");
  MarSystem* sdest = mng.create("SoundFileSink", "dest");

  playbackNet->addMarSystem(rsrc);
  playbackNet->addMarSystem(sdest);

  playbackNet->updControl("mrs_natural/inObservations", copt);
  playbackNet->updControl("mrs_natural/inSamples", bufferSize);
  playbackNet->updControl("mrs_real/israte", sropt);

  playbackNet->updControl("RealvecSource/rsrc/mrs_realvec/data", dest->getctrl("mrs_realvec/data")->to<mrs_realvec>());
  playbackNet->updControl("SoundFileSink/dest/mrs_string/filename", outFileName);

// 	cout << "########## OUTPUT ##########" << endl;
//  	cout << rsrc->getctrl("mrs_realvec/data") << endl;

  for (mrs_natural t = 0; t < iterations; t++)
  {
// 		cout << "tick" << endl;
    playbackNet->tick();
  }

}

void toy_with_stereospectrum_bin_change(string inAudioFileName)
{
  MarSystem* net_;

  MarSystemManager mng;

  net_ = mng.create("Series", "net");
  net_->addMarSystem(mng.create("SoundFileSource", "src"));
  net_->addMarSystem(mng.create("AudioSink", "dest"));

  MarSystem* fanout = mng.create("Fanout", "fanout");

  MarSystem* powerspectrum_series = mng.create("Series", "powerspectrum_series");
  powerspectrum_series->addMarSystem(mng.create("Stereo2Mono", "stereo2mono"));
  powerspectrum_series->addMarSystem(mng.create("Windowing", "ham"));
  powerspectrum_series->addMarSystem(mng.create("Spectrum", "spk"));
  powerspectrum_series->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  powerspectrum_series->addMarSystem(mng.create("Gain", "gain"));

  MarSystem* stereobranches_series = mng.create("Series", "stereobranches_series");
  MarSystem* stereobranches_parallel = mng.create("Parallel", "stereobranches_parallel");
  MarSystem* left = mng.create("Series", "left");
  MarSystem* right = mng.create("Series", "right");

  left->addMarSystem(mng.create("Windowing", "hamleft"));
  left->addMarSystem(mng.create("Spectrum", "spkleft"));

  right->addMarSystem(mng.create("Windowing", "hamright"));
  right->addMarSystem(mng.create("Spectrum", "spkright"));

  stereobranches_parallel->addMarSystem(left);
  stereobranches_parallel->addMarSystem(right);
  stereobranches_series->addMarSystem(stereobranches_parallel);
  stereobranches_series->addMarSystem(mng.create("StereoSpectrum", "sspk"));

  stereobranches_series->addMarSystem(mng.create("Gain", "gain"));

  fanout->addMarSystem(powerspectrum_series);
  fanout->addMarSystem(stereobranches_series);

  net_->addMarSystem(fanout);
  net_->addMarSystem(mng.create("Gain", "gain"));

  net_->updControl("SoundFileSource/src/mrs_string/filename",inAudioFileName);
  net_->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  int i = 0;
  while (net_->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())  {
    net_->tick();

    cout << "i=" << i << endl;
    if (i == 100) {
      cout << "32" << endl;
      net_->updControl("mrs_natural/inSamples",32);
    }
    if (i == 120) {
      cout << "32768" << endl;
      net_->updControl("mrs_natural/inSamples",32768);
    }
    if (i == 140) {
      cout << "256" << endl;
      net_->updControl("mrs_natural/inSamples",256);
    }
    if (i == 160) {
      cout << "32" << endl;
      net_->updControl("mrs_natural/inSamples",256);
    }
    if (i == 180) {
      cout << "32768" << endl;
      net_->updControl("mrs_natural/inSamples",32768);
    }
    if (i == 240) {
      cout << "256" << endl;
      net_->updControl("mrs_natural/inSamples",256);
    }
    if (i == 260) {
      cout << "32" << endl;
      net_->updControl("mrs_natural/inSamples",256);
    }
    if (i == 280) {
      cout << "32768" << endl;
      net_->updControl("mrs_natural/inSamples",32768);
    }

    ++i;
  }

}

void toy_with_delay (string inAudioFileName, string outAudioFileName)
{
  mrs_real			fs;
  mrs_natural			sampleCount		= 0;

  MarSystemManager	mng;
  MarSystem			*net_;

  // create series
  net_ = mng.create("Series", "net");
  net_->addMarSystem(mng.create("SoundFileSource", "src"));
  net_->addMarSystem (mng.create("Delay", "del"));
  net_->addMarSystem (mng.create("MixToMono", "m2m"));
  net_->addMarSystem(mng.create("SoundFileSink", "dest"));

  // set parameters
  net_->updControl("SoundFileSource/src/mrs_string/filename",inAudioFileName);
  net_->updControl("SoundFileSink/dest/mrs_string/filename", outAudioFileName);

  net_->updControl("Delay/del/mrs_real/maxDelaySeconds",5.0);

  fs = 	net_->getctrl("Delay/del/mrs_real/israte")->to<mrs_real>();

  cout << "input file with vibrato is rendered to output file..." << endl;

  while (net_->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    mrs_realvec r,
                test,
                modFreqInHz;
    mrs_real	modAmpInSecs	= .1;
    test.stretch(2);
    modFreqInHz.stretch(2);
    modFreqInHz(0)	= 1;
    modFreqInHz(1)	= 4;
    test (0)	= fs*modAmpInSecs*.5*(1+sin(TWOPI*modFreqInHz(0)*sampleCount / fs));
    test (1)	= fs*modAmpInSecs*.5*(1+sin(TWOPI*modFreqInHz(1)*sampleCount / fs));

    net_->updControl("Delay/del/mrs_realvec/delaySamples", test);
    //net_->updControl("Delay/del/mrs_real/delaySeconds", modAmpInSecs*.5*(1+sin(TWOPI*modFreqInHz*sampleCount / fs)));
    net_->tick();

    r = net_->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

    sampleCount	+= net_->getctrl ("mrs_natural/onSamples")->to<mrs_natural>();
  }
}

void toy_with_peaker(string inAudioFileName)
{
  MarSystem* net_;

  MarSystemManager mng;

  net_ = mng.create("Series", "net");
  net_->addMarSystem(mng.create("SoundFileSource", "src"));
  net_->addMarSystem(mng.create("AudioSink", "dest"));
  net_->addMarSystem(mng.create("Peaker", "peaker"));
  net_->addMarSystem(mng.create("Gain", "gain"));

  net_->updControl("SoundFileSource/src/mrs_string/filename",inAudioFileName);
  net_->updControl("AudioSink/dest/mrs_bool/initAudio", true);

//     mrs_natural winsize = 8820;

  mrs_natural winsize = 512;
  net_->updControl("mrs_natural/inSamples",winsize);

//   net_->updControl("Peaker/peaker/mrs_real/peakStrength",100000.0);
  net_->updControl("Peaker/peaker/mrs_real/peakSpacing",10.0);

  realvec r;
  while (net_->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())  {
    net_->tick();

//  	  r = net_->getctrl("SoundFileSource/src/mrs_realvec/processedData")->to<mrs_realvec>();
// 	  cout << "****************************** SoundFileSource ******************************" << endl;
//  	  cout << r << endl;

//    	  r = net_->getctrl("Peaker/peaker/mrs_realvec/processedData")->to<mrs_realvec>();
//  	  cout << "****************************** Peaker ******************************" << endl;
//   	  cout << r << endl;


    r = net_->getctrl("Peaker/peaker/mrs_realvec/processedData")->to<mrs_realvec>();
    int peakfound = 0;
    for (int i = 0; i < winsize; ++i) {
      if (r(0,i) > 0.7) {
//  		  cout << "r(0," << i << ")=" << r(0,i) << " peak" << endl;
        peakfound = 1;
      }
    }
    if (peakfound) {
      cout << "peak" << endl;
    }
  }

}

void
toy_with_robot_peak_onset(string sfName, string portNum)
{
  int port;
  if (portNum == "MARSYAS_EMPTY") {
    port = 1;
  } else {
    port = atoi(portNum.c_str());
  }
  cout << "toying with robot_peak_onset" << endl;
  cout << endl;
  cout << "To use another midi port, enter it as the second argument:" << endl;
  cout << "  mudbox -t robot_peak_onset in.wav 2" << endl;
  cout << endl;
  MarSystemManager mng;

  // assemble the processing network
  MarSystem* onsetnet = mng.create("Series", "onsetnet");
  MarSystem* onsetaccum = mng.create("Accumulator", "onsetaccum");
  MarSystem* onsetseries= mng.create("Series","onsetseries");
  onsetseries->addMarSystem(mng.create("SoundFileSource", "src"));
  onsetseries->addMarSystem(mng.create("Stereo2Mono", "src"));
  MarSystem* onsetdetector = mng.create("FlowThru", "onsetdetector");
  onsetdetector->addMarSystem(mng.create("ShiftInput", "si")); //<---
  onsetdetector->addMarSystem(mng.create("Windowing", "win")); //<---
  onsetdetector->addMarSystem(mng.create("Spectrum","spk"));
  onsetdetector->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  onsetdetector->addMarSystem(mng.create("Flux", "flux"));
  //onsetdetector->addMarSystem(mng.create("Memory","mem"));
  onsetdetector->addMarSystem(mng.create("ShiftInput","sif"));
  onsetdetector->addMarSystem(mng.create("Filter","filt1"));
  onsetdetector->addMarSystem(mng.create("Reverse","rev1"));
  onsetdetector->addMarSystem(mng.create("Filter","filt2"));
  onsetdetector->addMarSystem(mng.create("Reverse","rev2"));
  onsetdetector->addMarSystem(mng.create("PeakerOnset","peaker"));
  onsetseries->addMarSystem(onsetdetector);
  onsetaccum->addMarSystem(onsetseries);
  onsetnet->addMarSystem(onsetaccum);
  MarSystem* onsetmix = mng.create("Fanout","onsetmix");
  onsetmix->addMarSystem(mng.create("Gain","gainaudio"));
  MarSystem* onsetsynth = mng.create("Series","onsetsynth");
  onsetsynth->addMarSystem(mng.create("NoiseSource","noisesrc"));
  onsetsynth->addMarSystem(mng.create("ADSR","env"));
  onsetsynth->addMarSystem(mng.create("Gain", "gainonsets"));
  onsetmix->addMarSystem(onsetsynth);
  onsetnet->addMarSystem(onsetmix);

  onsetnet->addMarSystem(mng.create("AudioSink", "dest"));
  // onsetnet->addMarSystem(mng.create("SoundFileSink", "fdest"));


  ///////////////////////////////////////////////////////////////////////////////////////
  //link controls
  ///////////////////////////////////////////////////////////////////////////////////////
  onsetnet->linkControl("mrs_bool/hasData",
                        "Accumulator/onsetaccum/Series/onsetseries/SoundFileSource/src/mrs_bool/hasData");
  //onsetnet->linkControl("ShiftOutput/so/mrs_natural/Interpolation","mrs_natural/inSamples");
  onsetnet->linkControl("Accumulator/onsetaccum/mrs_bool/flush",
                        "Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_bool/onsetDetected");
  //onsetnet->linkControl("Fanout/onsetmix/Series/onsetsynth/Gain/gainonsets/mrs_real/gain",
  //	"Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_real/confidence");

  //onsetnet->linkControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Memory/mem/mrs_bool/reset",
  //	"Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_bool/onsetDetected");

  //link FILTERS coeffs
  onsetnet->linkControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt2/mrs_realvec/ncoeffs",
                        "Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/ncoeffs");
  onsetnet->linkControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt2/mrs_realvec/dcoeffs",
                        "Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/dcoeffs");

  ///////////////////////////////////////////////////////////////////////////////////////
  // update controls
  ///////////////////////////////////////////////////////////////////////////////////////
  FileName outputFile(sfName);
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/SoundFileSource/src/mrs_string/filename", sfName);
// 	onsetnet->updControl("SoundFileSink/fdest/mrs_string/filename", outputFile.nameNoExt() + "_onsets.wav");
  mrs_real fs = onsetnet->getctrl("mrs_real/osrate")->to<mrs_real>();

  mrs_natural winSize = 2048;//2048;
  mrs_natural hopSize = 512;//411;
  mrs_natural lookAheadSamples = 6;
  mrs_real thres = 1.75;

  mrs_real textureWinMinLen = 0.050; //secs
  mrs_natural minTimes = (mrs_natural) (textureWinMinLen*fs/hopSize); //12;//onsetWinSize+1;//15;
  mrs_real textureWinMaxLen = 3.000; //secs
  mrs_natural maxTimes = (mrs_natural) (textureWinMaxLen*fs/hopSize);//1000; //whatever... just a big number for now...

  //best result till now are using dB power Spectrum!
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PowerSpectrum/pspk/mrs_string/spectrumType",
                       "wrongdBonsets");

  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Flux/flux/mrs_string/mode",
                       "DixonDAFX06");

  //configure zero-phase Butterworth filter of Flux time series (from J.P.Bello TASLP paper)
  // Coefficients taken from MATLAB butter(2, 0.28)
  realvec bcoeffs(1,3);
  bcoeffs(0) = 0.1174;
  bcoeffs(1) = 0.2347;
  bcoeffs(2) = 0.1174;
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/ncoeffs",
                       bcoeffs);
  realvec acoeffs(1,3);
  acoeffs(0) = 1.0;
  acoeffs(1) = -0.8252;
  acoeffs(2) = 0.2946;
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/Filter/filt1/mrs_realvec/dcoeffs",
                       acoeffs);

  onsetnet->updControl("mrs_natural/inSamples", hopSize);
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/ShiftInput/si/mrs_natural/winSize", winSize);

  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_natural/lookAheadSamples", lookAheadSamples);
  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/PeakerOnset/peaker/mrs_real/threshold", thres); //!!!

  onsetnet->updControl("Accumulator/onsetaccum/Series/onsetseries/FlowThru/onsetdetector/ShiftInput/sif/mrs_natural/winSize", 4*lookAheadSamples+1);

  mrs_natural winds = 1+lookAheadSamples+mrs_natural(ceil(mrs_real(winSize)/hopSize/2.0));
  onsetnet->updControl("Accumulator/onsetaccum/mrs_natural/timesToKeep", winds);
  onsetnet->updControl("Accumulator/onsetaccum/mrs_string/mode","explicitFlush");
  onsetnet->updControl("Accumulator/onsetaccum/mrs_natural/maxTimes", maxTimes);
  onsetnet->updControl("Accumulator/onsetaccum/mrs_natural/minTimes", minTimes);

  //set audio/onset resynth balance and ADSR params for onset sound
  onsetnet->updControl("Fanout/onsetmix/Gain/gainaudio/mrs_real/gain", 1.0);
  onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/Gain/gainonsets/mrs_real/gain", 0.8);
  onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/aTarget", 1.0);
  onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/aTime", winSize/80/fs); //!!!
  onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/susLevel", 0.0);
  onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/dTime", winSize/4/fs); //!!!

  onsetnet->updControl("AudioSink/dest/mrs_bool/initAudio", true);

  ///////////////////////////////////////////////////////////////////////////////////////
  // Process input file (till EOF)
  ///////////////////////////////////////////////////////////////////////////////////////
  mrs_natural timestamps_samples = 0;
  //mrs_real sampling_rate;
  //sampling_rate = onsetnet->getctrl("mrs_real/osrate")->to<mrs_real>();

  MarSystem* playback = mng.create("Series", "playback");
  playback->addMarSystem(mng.create("MidiOutput", "midiout"));

  playback->updControl("MidiOutput/midiout/mrs_natural/port", port);
  playback->updControl("MidiOutput/midiout/mrs_bool/initMidi", true);

  while(onsetnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/nton", 1.0); //note on
    onsetnet->tick();
    timestamps_samples += onsetnet->getctrl("mrs_natural/onSamples")->to<mrs_natural>();

#ifdef MARSYAS_MIDIIO
    playback->updControl("MidiOutput/midiout/mrs_natural/byte1", 0x99);
// 		playback->updControl("MidiOutput/midiout/mrs_natural/byte2", 60);
    playback->updControl("MidiOutput/midiout/mrs_natural/byte2", 82);
    playback->updControl("MidiOutput/midiout/mrs_natural/byte3", 127);
    playback->updControl("MidiOutput/midiout/mrs_bool/sendMessage", true);
#endif

    cout << "peak!!!" << endl;
    onsetnet->updControl("Fanout/onsetmix/Series/onsetsynth/ADSR/env/mrs_real/ntoff", 0.0); //note off
  }


}

void toy_with_midiout() {
  cout << "toy with midiout" << endl;

  MarSystemManager mng;

  MarSystem* playback = mng.create("Series", "playback");
  playback->addMarSystem(mng.create("MidiOutput", "midiout"));

  playback->updControl("MidiOutput/midiout/mrs_natural/port", 1);
  playback->updControl("MidiOutput/midiout/mrs_bool/initMidi", true);

  for (int i = 0; i < 100; ++i) {
    cout << "Sending note" << endl;
#ifdef MARSYAS_MIDIIO
    playback->updControl("MidiOutput/midiout/mrs_natural/byte1", 0x99);
    playback->updControl("MidiOutput/midiout/mrs_natural/byte2", 60);
    playback->updControl("MidiOutput/midiout/mrs_natural/byte3", 127);
    playback->updControl("MidiOutput/midiout/mrs_bool/sendMessage", true);
#endif

  }
}

void toy_with_beats(mrs_string score_function, mrs_string sfName, mrs_string progName)
{
  mrs_natural induction_time = 5; //Time (in seconds) of induction before tracking. Has to be > 60/MIN_BPM (5)
  mrs_natural bpm_hypotheses = 6; //Nr. of initial BPM hypotheses (must be <= than the nr. of agents) (6)
  mrs_natural phase_hypotheses = 20;//Nr. of phases per BPM hypothesis (20)
  mrs_natural min_bpm = 50; //minimum tempo considered, in BPMs (50)
  mrs_natural max_bpm = 250; //maximum tempo considered, in BPMs (250)
  mrs_natural nr_agents = 50; //Nr. of agents in the pool (50)
  mrs_real lft_outter_margin = 0.20; //The size of the outer half-window (in % of IBI) before the predicted beat time (0.30)
  mrs_real rgt_outter_margin = 0.40; //The size of the outer half-window (in % of IBI) after the predicted beat time (0.30)
  mrs_real inner_margin = 4.0; //Inner tolerance window margin size (in ticks) (4.0)
  mrs_real obsolete_factor = 1.5; //An agent is killed if, at any time, the difference between its score and the bestScore is below OBSOLETE_FACTOR * bestScore (1.5)
  mrs_real child_factor = 0.01; //(Inertia1) Each created agent imports its father score decremented by the current dScore divided by this factor (0.05)
  mrs_real best_factor = 1.01; //(Inertia2) Mutiple of the bestScore an agent's score must have for replacing the current best agent (1.15)
  mrs_natural eq_period = 0; //Period threshold which identifies two agents as predicting the same period (IBI, in ticks) (1)
  mrs_natural eq_phase = 0; //Period threshold which identifies two agents as predicting the same phase (beat time, in ticks) (2)

  mrs_natural winSize = 2048; //2048
  mrs_natural hopSize = 512; //512

  mrs_natural audio = 0;
  mrs_natural audio_file = 1;

  MarSystemManager mng;

  // assemble the processing network
  MarSystem* audioflow = mng.create("Series", "audioflow");
  audioflow->addMarSystem(mng.create("SoundFileSource", "src"));
  audioflow->addMarSystem(mng.create("Stereo2Mono", "src")); //replace by a "Monofier" MarSystem (to be created) [!]

  MarSystem* beattracker= mng.create("FlowThru","beattracker");
  //beattracker->addMarSystem(mng.create("Stereo2Mono", "src")); //replace by a "Monofier" MarSystem (to be created) [!]

  MarSystem* onsetdetectionfunction = mng.create("Series", "onsetdetectionfunction");
  onsetdetectionfunction->addMarSystem(mng.create("ShiftInput", "si"));
  onsetdetectionfunction->addMarSystem(mng.create("Windowing", "win"));
  onsetdetectionfunction->addMarSystem(mng.create("Spectrum","spk"));
  onsetdetectionfunction->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  onsetdetectionfunction->addMarSystem(mng.create("Flux", "flux"));

  //onsetdetectionfunction->addMarSystem(mng.create("SonicVisualiserSink", "sonicsink"));

  beattracker->addMarSystem(onsetdetectionfunction);
  beattracker->addMarSystem(mng.create("ShiftInput", "acc"));

  MarSystem* tempoinduction = mng.create("FlowThru", "tempoinduction");

  MarSystem* tempohypotheses = mng.create("Fanout", "tempohypotheses");

  MarSystem* tempo = mng.create("Series", "tempo");
  tempo->addMarSystem(mng.create("AutoCorrelation","acf"));
  tempo->addMarSystem(mng.create("Peaker", "pkr"));
  tempo->addMarSystem(mng.create("MaxArgMax", "mxr"));

  tempohypotheses->addMarSystem(tempo);

  MarSystem* phase = mng.create("Series", "phase");
  phase->addMarSystem(mng.create("PeakerOnset","pkronset"));
  phase->addMarSystem(mng.create("OnsetTimes","OnsetTimes"));

  tempohypotheses->addMarSystem(phase);

  tempoinduction->addMarSystem(tempohypotheses);
  tempoinduction->addMarSystem(mng.create("TempoHypotheses", "tempohyp"));

  beattracker->addMarSystem(tempoinduction);

  MarSystem* initialhypotheses = mng.create("FlowThru", "initialhypotheses");
  initialhypotheses->addMarSystem(mng.create("PhaseLock", "phaselock"));

  beattracker->addMarSystem(initialhypotheses);

  MarSystem* agentpool = mng.create("Fanout", "agentpool");
  for(int i = 0; i < nr_agents; ++i)
  {
    ostringstream oss;
    oss << "agent" << i;
    agentpool->addMarSystem(mng.create("BeatAgent", oss.str()));
  }

  beattracker->addMarSystem(agentpool);
  beattracker->addMarSystem(mng.create("BeatReferee", "br"));
  beattracker->addMarSystem(mng.create("BeatTimesSink", "sink"));

  audioflow->addMarSystem(beattracker);
  audioflow->addMarSystem(mng.create("Gain","gainaudio"));

  MarSystem* beatmix = mng.create("Fanout","beatmix");
  beatmix->addMarSystem(audioflow);
  MarSystem* beatsynth = mng.create("Series","beatsynth");
  beatsynth->addMarSystem(mng.create("NoiseSource","noisesrc"));
  beatsynth->addMarSystem(mng.create("ADSR","env"));
  beatsynth->addMarSystem(mng.create("Gain", "gainbeats"));
  beatmix->addMarSystem(beatsynth);

  MarSystem* IBTsystem = mng.create("Series", "IBTsystem");
  IBTsystem->addMarSystem(beatmix);
  IBTsystem->addMarSystem(mng.create("AudioSink", "output"));
  if(audio_file)
    IBTsystem->addMarSystem(mng.create("SoundFileSink", "fdest"));


  ///////////////////////////////////////////////////////////////////////////////////////
  //link controls
  ///////////////////////////////////////////////////////////////////////////////////////
  IBTsystem->linkControl("mrs_bool/hasData",
                         "Fanout/beatmix/Series/audioflow/SoundFileSource/src/mrs_bool/hasData");

  //Link LookAheadSamples used in PeakerOnset for compensation when retriving the actual initial OnsetTimes
  tempoinduction->linkControl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_natural/lookAheadSamples",
                              "Fanout/tempohypotheses/Series/phase/OnsetTimes/OnsetTimes/mrs_natural/lookAheadSamples");

  //Pass hypotheses matrix (from tempoinduction stage) to PhaseLock
  beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_realvec/beatHypotheses",
                           "FlowThru/tempoinduction/mrs_realvec/innerOut");

  //Pass initital hypotheses to BeatReferee
  beattracker->linkControl("BeatReferee/br/mrs_realvec/beatHypotheses",
                           "FlowThru/initialhypotheses/mrs_realvec/innerOut");

  //PhaseLock nr of BPM hypotheses = nr MaxArgMax from ACF
  beattracker->linkControl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nMaximums",
                           "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPeriods");
  //TempoHypotheses nr of BPMs = nr MaxArgMax from ACF
  beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPeriods",
                           "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPeriodHyps");
  //OnsetTimes nr of BPMs = nr MaxArgMax from ACF (this is to avoid FanOut crash!)
  beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPeriodHyps",
                           "FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/OnsetTimes/mrs_natural/nPeriods");

  //PhaseLock nr of Phases per BPM = nr of OnsetTimes considered
  beattracker->linkControl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/OnsetTimes/mrs_natural/n1stOnsets",
                           "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPhases");
  //TempoHypotheses nr of Beat hypotheses = nr of OnsetTimes considered
  beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPhases",
                           "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPhasesPerPeriod");
  //nr of MaxArgMax Phases per BPM = nr OnsetTimes considered (this is to avoid FanOut crash!)
  beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPhasesPerPeriod",
                           "FlowThru/tempoinduction/Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nPhases");

  //Pass enabled (muted) BeatAgents (from FanOut) to the BeatReferee
  beattracker->linkControl("Fanout/agentpool/mrs_realvec/muted", "BeatReferee/br/mrs_realvec/muted");
  //Pass tempohypotheses Fanout muted vector to the BeatReferee, for disabling induction after induction timming
  beattracker->linkControl("FlowThru/tempoinduction/Fanout/tempohypotheses/mrs_realvec/muted",
                           "BeatReferee/br/mrs_realvec/inductionEnabler");

  //Link agentControl matrix from the BeatReferee to each agent in the pool
  for(int i = 0; i < nr_agents; ++i)
  {
    ostringstream oss;
    oss << "agent" << i;
    beattracker->linkControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_realvec/agentControl",
                             "BeatReferee/br/mrs_realvec/agentControl");
  }

  //Defines tempo induction time after which the BeatAgents' hypotheses are populated:
  //TempoHypotheses indTime = induction time
  beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime",
                           "ShiftInput/acc/mrs_natural/winSize");
  //PhaseLock timming = induction time
  beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime",
                           "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime");
  //BeatReferee timming = induction time
  beattracker->linkControl("BeatReferee/br/mrs_natural/inductionTime",
                           "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime");

  //Link BPM conversion parameters to BeatReferee:
  beattracker->linkControl("BeatReferee/br/mrs_natural/hopSize", "mrs_natural/inSamples");

  //Link Output Sink parameters with the used ones:
  beattracker->linkControl("BeatTimesSink/sink/mrs_natural/hopSize", "BeatReferee/br/mrs_natural/hopSize");
  beattracker->linkControl("BeatTimesSink/sink/mrs_real/srcFs", "BeatReferee/br/mrs_real/srcFs");
  beattracker->linkControl("BeatTimesSink/sink/mrs_natural/winSize",
                           "Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize");
  beattracker->linkControl("BeatTimesSink/sink/mrs_natural/tickCount", "BeatReferee/br/mrs_natural/tickCount");

  //Link SonicVisualiserSink parameters with the used ones:
  /*
    beattracker->linkControl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_natural/hopSize",
    "BeatTimesSink/sink/mrs_natural/hopSize");
    beattracker->linkControl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_real/srcFs",
    "BeatTimesSink/sink/mrs_real/srcFs");
  */

  beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/hopSize",
                           "BeatTimesSink/sink/mrs_natural/hopSize");
  beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs",
                           "BeatTimesSink/sink/mrs_real/srcFs");

  //link beatdetected with noise ADSR -> for clicking when beat:
  IBTsystem->linkControl("Fanout/beatmix/Series/audioflow/FlowThru/beattracker/BeatReferee/br/mrs_real/beatDetected",
                         "Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/nton");


  ///////////////////////////////////////////////////////////////////////////////////////
  // update controls
  ///////////////////////////////////////////////////////////////////////////////////////
  //FileName outputFile(sfName);
  audioflow->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  //best result till now are using dB power Spectrum!
  beattracker->updControl("Series/onsetdetectionfunction/PowerSpectrum/pspk/mrs_string/spectrumType", "magnitude");

  beattracker->updControl("Series/onsetdetectionfunction/Flux/flux/mrs_string/mode", "DixonDAFX06");

  beattracker->updControl("mrs_natural/inSamples", hopSize);
  beattracker->updControl("Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize", winSize);

  mrs_real fsSrc = beattracker->getctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_real/israte")->to<mrs_real>();

  mrs_natural inductionTickCount = (mrs_natural) ceil((induction_time * fsSrc) / hopSize); //induction time (in nr. of ticks)
  mrs_natural inputSize = audioflow->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>(); //(in samples)

  //to avoid induction time greater than input file size
  //(in this case the induction time will equal the file size)
  if((inputSize / hopSize) < inductionTickCount)
    inductionTickCount = inputSize / hopSize;

  beattracker->updControl("ShiftInput/acc/mrs_natural/winSize", inductionTickCount);

  mrs_natural pkinS = tempoinduction->getctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural peakEnd = (mrs_natural)((60.0 * fsSrc)/(min_bpm * hopSize)); //50BPM (in frames)
  mrs_natural peakStart = (mrs_natural) ((60.0 * fsSrc)/(max_bpm * hopSize));  //250BPM (in frames)
  mrs_real peakSpacing = ((mrs_natural) ((60.0 * fsSrc)/(60 * hopSize)) //4BMP resolution
                          - ((60.0 * fsSrc)/(64 * hopSize))) / (pkinS * 1.0);

  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakStrength", 0.75);
  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakStart", peakStart);
  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakEnd", peakEnd);
  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakGain", 2.0);

  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nMaximums", bpm_hypotheses);

  mrs_natural lookAheadSamples = 9; //multiple of 3
  mrs_real thres = 1.75;

  tempoinduction->updControl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_natural/lookAheadSamples", lookAheadSamples);
  tempoinduction->updControl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_real/threshold", thres);

  tempoinduction->updControl("Fanout/tempohypotheses/Series/phase/OnsetTimes/OnsetTimes/mrs_natural/n1stOnsets", phase_hypotheses);

  //Pass chosen score_function to each BeatAgent in the pool:
  for(int i = 0; i < nr_agents; ++i)
  {
    ostringstream oss, oss2;
    oss << "agent" << i;
    oss2 << "Agent" << i;
    beattracker->updControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_string/scoreFunc", score_function);

    beattracker->updControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_real/lftOutterMargin", lft_outter_margin);
    beattracker->updControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_real/rgtOutterMargin", rgt_outter_margin);
    beattracker->updControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_real/innerMargin", inner_margin);

    //THIS IS TO REMOVE -> SEE INOBSNAMES IN BEATAGENT!!
    beattracker->updControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_string/identity", oss2.str());
  }

  beattracker->updControl("BeatReferee/br/mrs_real/srcFs", fsSrc);
  beattracker->updControl("BeatReferee/br/mrs_natural/minTempo", min_bpm);
  beattracker->updControl("BeatReferee/br/mrs_natural/maxTempo", max_bpm);
  beattracker->updControl("BeatReferee/br/mrs_real/obsoleteFactor", obsolete_factor);
  beattracker->updControl("BeatReferee/br/mrs_real/childFactor", child_factor);
  beattracker->updControl("BeatReferee/br/mrs_real/bestFactor", best_factor);
  beattracker->updControl("BeatReferee/br/mrs_natural/eqPeriod", eq_period);
  beattracker->updControl("BeatReferee/br/mrs_natural/eqPhase", eq_phase);

  FileName outputFile(sfName);

  FileName progFileName(progName);
  progName =  progFileName.path();

  ostringstream path;
  path << progName << "/" << outputFile.nameNoExt();

  beattracker->updControl("BeatTimesSink/sink/mrs_string/destFileName", path.str());
  //beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "medianTempo");
  //beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "beatTimes");
  beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "all");

  /*
    beattracker->updControl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_string/mode", "seconds");
    beattracker->updControl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_string/destFileName", + path.str() + "_onsetFunction.txt");
  */

  //set audio/onset resynth balance and ADSR params for onset sound
  IBTsystem->updControl("Fanout/beatmix/Series/audioflow/Gain/gainaudio/mrs_real/gain", 0.6);
  IBTsystem->updControl("Fanout/beatmix/Series/beatsynth/Gain/gainbeats/mrs_real/gain", 1.2);
  IBTsystem->updControl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/aTarget", 1.0);
  IBTsystem->updControl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/aTime", winSize/80/fsSrc);
  IBTsystem->updControl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/susLevel", 0.0);
  IBTsystem->updControl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/dTime", winSize/4/fsSrc);

  //for saving file with audio+clicks (on beats):
  if(audio_file)
    IBTsystem->updControl("SoundFileSink/fdest/mrs_string/filename", path.str() + "_beats.wav");

  //MATLAB Engine inits

  //MATLAB_EVAL("FluxTS = [];");
  //MATLAB_EVAL("FinalBeats=[];");
  /*
    MATLAB_EVAL("clear;");
    MATLAB_PUT(induction_time, "timmbing");
    MATLAB_PUT(fsSrc, "SrcFs");
    MATLAB_PUT(inductionTickCount, "inductionTickCount");
    MATLAB_PUT(winSize, "winSize");
    MATLAB_PUT(hopSize, "hopSize");
    MATLAB_EVAL("srcAudio = [];");
    MATLAB_EVAL("FluxTS = [];");
    MATLAB_EVAL("FinalTS = [];");
    MATLAB_EVAL("BeatAgentTS=[];");
    MATLAB_EVAL("BeatAgentsTS=[];");
    MATLAB_EVAL("bestAgentScore=[];");
    MATLAB_EVAL("Flux_FilterTS=[];");
  */

  ///////////////////////////////////////////////////////////////////////////////////////
  //process input file (till EOF)
  ///////////////////////////////////////////////////////////////////////////////////////
  mrs_natural frameCount = 0;
  inputSize = (inputSize / hopSize) + inductionTickCount; //inputSize in ticks

  //while(IBTsystem->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  while(frameCount <= inputSize)
  {
    IBTsystem->tick();

    frameCount++;
    //Just after induction:
    if(frameCount == inductionTickCount)
    {
      //Restart reading audio file
      audioflow->updControl("SoundFileSource/src/mrs_natural/pos", hopSize);
      //for playing audio (with clicks on beats):
      if(audio)
        IBTsystem->updControl("AudioSink/output/mrs_bool/initAudio", true);

      //cout << "Finnished Induction!" << endl;
    }
    //Display percentage of processing complete...
    //cout << (mrs_natural) frameCount*100/inputSize << "%" << endl;
  }
}

// Dick Lyon's Pole-Zero Filter Cascade
void
toy_with_aim_pzfc(string sfName)
{
  cout << "Toy_with: aim_pzfc" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("AimPZFC2", "aimpzfc"));

  net->updControl("SoundFileSource/src/mrs_string/filename", sfName);


  cout << *net << endl;
  net->updControl("mrs_natural/inSamples", 1024);


  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    net->tick();
    // cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  }
  delete net;
}

// Slaney's gammatone filterbank
void
toy_with_aim_gammatone(string sfName)
{
  cout << "Toy_with: aim_gammatone" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("AimGammatone", "aimgammatone"));

  net->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    net->tick();
    cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  }
  delete net;
}

// Halfwave rectification, compression and lowpass filtering
void
toy_with_aim_hcl(string sfName)
{
  cout << "Toy_with: aim_hcl" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("AimPZFC", "aimpzfc"));
  net->addMarSystem(mng.create("AimHCL", "aimhcl"));

  net->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    net->tick();
    cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  }
  delete net;
}

// Local maximum strobe criterion: decaying threshold with timeout
void
toy_with_aim_localmax(string sfName)
{
  cout << "Toy_with: aim_localmax" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->addMarSystem(mng.create("AimPZFC", "aimpfzc"));
  net->addMarSystem(mng.create("AimHCL", "aimhcl"));
  net->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));

  net->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    net->tick();
    // cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  }
  delete net;
}

// Stabilised auditory image
void
toy_with_aim_sai(string sfName)
{
  cout << "Toy_with: aim_sai" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  net->addMarSystem(mng.create("SoundFileSource", "src"));

  // net->addMarSystem(mng.create("AimGammatone", "aimgammatone"));
  net->addMarSystem(mng.create("AimPZFC", "aimpzfc"));
  net->addMarSystem(mng.create("AimHCL", "aimhcl"));
  net->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));
  net->addMarSystem(mng.create("AimSAI", "aimsai"));

  cout << "UPDATE" << endl;

  net->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  // cout << *net;

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    cout << "tik tok" << endl;
    net->tick();
    // cout << "AFTER" << endl;
    // cout << *net;

    cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  }
  delete net;
}

// Size-shape image (aka the 'sscAI')
void
toy_with_aim_ssi(string sfName)
{
  cout << "Toy_with: aim_ssi" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  net->addMarSystem(mng.create("SoundFileSource", "src"));

  net->addMarSystem(mng.create("AimGammatone", "aimgammatone"));

  // net->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));

  MarSystem* fanout = mng.create("Fanout", "fanout");
  // fanout->addMarSystem(mng.create("AimHCL", "aimhcl"));
  fanout->addMarSystem(mng.create("Gain", "gain"));
  fanout->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));

  net->addMarSystem(fanout);

  net->addMarSystem(mng.create("AimSAI", "aimsai"));
  net->addMarSystem(mng.create("AimSSI", "aimssi"));

  cout << "UPDATE" << endl;

  net->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  // cout << *net;

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    // cout << "tik tok" << endl;
    net->tick();
    // cout << "AFTER" << endl;
    // cout << *net;

    cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  }
  delete net;
}

//
// Playing with AIM-C and Accumulators
//
void
toy_with_aim(string sfName)
{
  // mrs_natural memSize = 40;
  // mrs_natural winSize = 512;
  // mrs_natural hopSize = 512;
  // mrs_natural accSize_ = 1298;
  // mrs_real samplingRate_ = 22050.0;
  // mrs_real start = 0.0;
  // mrs_real length = -1.0;

  cout << "Toy_with: aim" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  MarSystem* accum = mng.create("Accumulator", "accum");

  MarSystem* accumseries = mng.create("Series", "accumseries");
  accumseries->addMarSystem(mng.create("SoundFileSource", "src"));
  accumseries->addMarSystem(mng.create("AimPZFC", "aimpzfc"));

  // net->addMarSystem(mng.create("AimGammatone", "aimgammatone"));

  // net->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));

  MarSystem* fanout = mng.create("Fanout", "fanout");
  // fanout->addMarSystem(mng.create("AimHCL", "aimhcl"));
  fanout->addMarSystem(mng.create("Gain", "gain"));
  fanout->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));

  accumseries->addMarSystem(fanout);

  accumseries->addMarSystem(mng.create("AimSAI", "aimsai"));
  accumseries->addMarSystem(mng.create("AimSSI", "aimssi"));

  // sness - A little hack to take the PZFC output and figure out the
  // amount of signal in each channel
  accumseries->addMarSystem(mng.create("MaxMin", "maxmin"));
  accumseries->addMarSystem(mng.create("Square", "square"));
  accumseries->addMarSystem(mng.create("Mean", "mean"));

  accum->addMarSystem(accumseries);
  // accum->updControl("mrs_natural/nTimes", 10);
  accum->updControl("mrs_natural/nTimes", 129);
  // // accum->updControl("mrs_natural/nTimes", accSize_);

  net->addMarSystem(accum);
  //    net->addMarSystem(accumseries);

  MarSystem* statistics = mng.create("Fanout", "statistics2");
  statistics->addMarSystem(mng.create("Mean", "mn"));
  statistics->addMarSystem(mng.create("StandardDeviation", "std"));
  net->addMarSystem(statistics);

  accumseries->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  // while (accumseries->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  // {
  net->tick();
  // cout << *net << endl;

  cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  // }
  // delete net;
}

// Make boxes out of Stabilised auditory image
void
toy_with_aim_boxes(string sfName)
{
  cout << "Toy_with: aim_boxes" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  net->addMarSystem(mng.create("SoundFileSource", "src"));

  net->addMarSystem(mng.create("AimPZFC", "aimpzfc"));
  net->addMarSystem(mng.create("AimHCL", "aimhcl"));
  net->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));
  net->addMarSystem(mng.create("AimSAI", "aimsai"));
  net->addMarSystem(mng.create("AimBoxes", "aimboxes"));

  cout << "UPDATE" << endl;

  net->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  // cout << *net;

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    cout << "tik tok" << endl;
    net->tick();
    // cout << "AFTER" << endl;
    // cout << *net;

    cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  }
  delete net;
}

// Unfold boxes of Stabilised auditory image
//
// sness - I could have done this more simply, but the main use case
// for now is to unfold either a SAI or the output of the boxcutting
// algorithm from SAI.
void
toy_with_unfold(string sfName)
{
  cout << "Toy_with: unfold" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  net->addMarSystem(mng.create("SoundFileSource", "src"));

  net->addMarSystem(mng.create("AimPZFC", "aimpzfc"));
  net->addMarSystem(mng.create("AimHCL", "aimhcl"));
  net->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));
  net->addMarSystem(mng.create("AimSAI", "aimsai"));
  net->addMarSystem(mng.create("AimBoxes", "aimboxes"));
  net->addMarSystem(mng.create("Unfold", "unfold"));

  cout << "UPDATE" << endl;

  net->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  // cout << *net;

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    cout << "tik tok" << endl;
    net->tick();
    // cout << "AFTER" << endl;
    // cout << *net;

    cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  }
  delete net;
}


// Dump out a .wav file as text
void
toy_with_dumpwav(string sfName, string outName)
{
  cout << "Toy_with: dumpwav" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  net->addMarSystem(mng.create("SoundFileSource", "src"));

  net->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  realvec out;

  ofstream outputstream(outName.c_str());

  while (net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    net->tick();
    out = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    for (int i = 0; i < out.getCols(); i++) {
      outputstream << out(0,i) << endl;
    }
  }
  delete net;
}

// Dump out a .wav file as text
void
toy_with_sness_shredder(string sfName)
{
  cout << "Toy_with: sness_shredder" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  MarSystem* accum = mng.create("Accumulator", "accum");
  accum->addMarSystem(mng.create("SoundFileSource", "src"));
  // accum->updControl("mrs_natural/nTimes", 1293);
  accum->updControl("mrs_natural/nTimes", 1000);
  accum->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  net->addMarSystem(accum);

  MarSystem* shredder = mng.create("Shredder", "shredder");
  shredder->addMarSystem(mng.create("Gain", "gain"));
  shredder->updControl("mrs_natural/nTimes", 10);
  net->addMarSystem(shredder);

  net->addMarSystem(mng.create("Gain", "gain"));

  realvec out;

  while (accum->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    net->tick();
    cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  }
  delete net;
}

// Vector Quantize an input file
void
toy_with_aim_vq(string sfName)
{
  cout << "Toy_with: aim_vq" << endl;

  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");

  MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
  featureNetwork->addMarSystem(mng.create("SoundFileSource", "src"));
  featureNetwork->addMarSystem(mng.create("AimPZFC", "aimpzfc"));
  featureNetwork->addMarSystem(mng.create("AimHCL", "aimhcl"));
  featureNetwork->addMarSystem(mng.create("AimLocalMax", "aimlocalmax"));
  featureNetwork->addMarSystem(mng.create("AimSAI", "aimsai"));
  featureNetwork->addMarSystem(mng.create("AimBoxes", "aimboxes"));
  featureNetwork->addMarSystem(mng.create("AimVQ", "aimvq"));

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updControl("mrs_natural/nTimes", 649);
  acc->addMarSystem(featureNetwork);
  net->addMarSystem(acc);

  net->addMarSystem(mng.create("Sum", "sum"));
  net->updControl("Sum/sum/mrs_string/mode", "sum_observations");

  featureNetwork->updControl("SoundFileSource/src/mrs_string/filename", sfName);

  cout << "UPDATE" << endl;

  // cout << *net;

  while (featureNetwork->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    cout << "tik tok" << endl;
    net->tick();
    // cout << "AFTER" << endl;
    // cout << *net;

    cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
  }
  delete net;
}


void toy_with_NCut ()
{
  MarSystemManager mng;
  mrs_realvec simMat(1);
  mrs_natural t,k,i,j,
              numTests = 3;

  MarSystem* NCutNet = mng.create("Series","NCutNet");
  NCutNet->addMarSystem(mng.create("RealvecSource","SimMatrix"));
  NCutNet->addMarSystem(mng.create("NormCut","NCut"));


  for (t=0; t < numTests; t++)
  {
    mrs_natural dim			= 0,	// dim has to be integer-dividable by numClusters
                  numClusters = 0,
                  kStart		= 0;

    // generate input matrix
    dim			= 50;  // dim has to be integer-dividable by numClusters
    numClusters = 5;
    kStart		= 0;
    simMat.stretch(dim, dim);
    simMat.setval (0.);

    for (k = 0; k < numClusters; k++)
    {
      for (i = kStart; i < (kStart + dim/numClusters); i++)
        for (j = kStart; j < (kStart + dim/numClusters); j++)
          simMat(i,j)	= 1.;

      kStart	+= dim/numClusters;
    }

    switch (t)
    {
    case 0:
    default:
    {
      break;
    }
    case 1:
    {
      mrs_realvec scramble (dim,dim);
      mrs_realvec tmp (dim,dim);
      scramble.setval (0.);

      for (i = 0; i < dim; i++)
        scramble(dim-i-1,i)	= 1.;
      // scramble
      realvec::matrixMulti (simMat, scramble, tmp);
      //realvec::matrixMulti (scramble, tmp, simMat);
      simMat = tmp;

      break;
    }
    case 2:
    {
      dim			= 20;
      numClusters = 3;
      kStart		= 0;
      simMat.stretch(dim, dim);
      simMat.setval (0.);

      for (k = 0; k < numClusters; k++)
      {
        for (i = 0; i < dim; i++)
          for (j = 0; j < dim; j++)
            simMat(i,j)	= ((i+j)%2==0)? 1. : 0.;
      }
    }
    }

    ///////////////////////////////////////////////////////////////////////
    // run test
    cout << "Clustering Input:" << endl;
    cout << simMat << endl;

    // set parameters

    NCutNet->updControl("RealvecSource/SimMatrix/mrs_realvec/data", simMat);
    NCutNet->updControl("mrs_natural/inSamples", dim);
    NCutNet->updControl("NormCut/NCut/mrs_natural/numClusters", numClusters);

    // do the clustering
    NCutNet->tick ();

    // display the clustering results
    cout << "Clustering Output:" << endl;
    cout << NCutNet->getControl ("mrs_realvec/processedData")->to<mrs_realvec>() << endl << endl;

    cout << "press enter to continue..." << endl;
    getchar();

  }
}



int
main(int argc, const char **argv)
{
  string progName = argv[0];
  if (argc == 1)
    printUsage(progName);

  // handling of command-line options
  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  vector<string> soundfiles = cmd_options.getRemaining();

  string fname0 = EMPTYSTRING;
  string fname1 = EMPTYSTRING;
  string fname2 = EMPTYSTRING;
  string fname3 = EMPTYSTRING;
  string fname4 = EMPTYSTRING;
  string fname5 = EMPTYSTRING;

  if (soundfiles.size() > 0)
    fname0 = soundfiles[0];
  if (soundfiles.size() > 1)
    fname1 = soundfiles[1];
  if (soundfiles.size() > 2)
    fname2 = soundfiles[2];
  if (soundfiles.size() > 3)
    fname3 = soundfiles[3];
  if (soundfiles.size() > 4)
    fname4 = soundfiles[4];
  if (soundfiles.size() > 5)
    fname5 = soundfiles[5];

  cout << "Marsyas toy_with name = " << toy_withName << endl;
  cout << "fname0 = " << fname0 << endl;
  cout << "fname1 = " << fname1 << endl;


  if (toy_withName == "ADRess")
    toy_with_ADRess(fname0, fname1);
  else if (toy_withName == "LPC_LSP")
    toy_with_LPC_LSP(fname0);
  else if (toy_withName == "MATLABengine")
    toy_with_MATLABengine();
  else if (toy_withName == "MarControls")
    toy_with_MarControls(fname0);
  else if (toy_withName == "labelsfplay")
    toy_with_labelsfplay(fname0);
  else if (toy_withName == "SNR")
    toy_with_SNR(fname0, fname1, fname2, fname3, (fname4 == EMPTYSTRING)? 0 : atoi(fname4.c_str()));
  else if (toy_withName == "SOM")
    toy_with_SOM("music.mf");
  else if (toy_withName == "Windowing")
    toy_with_Windowing();
  else if (toy_withName == "audiodevices")
    toy_with_audiodevices();
  else if (toy_withName == "cascade")
    toy_with_cascade();
  else if (toy_withName == "collection")
    toy_with_CollectionFileSource(fname0);
  else if (toy_withName == "centroid")
    toy_with_centroid(fname0);
  else if (toy_withName == "confidence")
    toy_with_confidence(fname0);
  else if (toy_withName == "drumclassify")
    drumClassify(fname0);
  else if (toy_withName == "dtw")
    toy_with_dtw(fname0, fname1);
  else if (toy_withName == "duplex")
    toy_with_duplex();
  else if (toy_withName == "duplex2")
    toy_with_duplex2(fname0);
  else if (toy_withName == "fanoutswitch")
    toy_with_fanoutswitch();
  else if (toy_withName == "fft")
    toy_with_fft(fname0, atoi(fname1.c_str()));
  else if (toy_withName == "filter")
    toy_with_filter();
  else if (toy_withName == "getControls")
    toy_with_getControls(fname0);
  else if (toy_withName == "inSamples")
    toy_with_inSamples(fname0);
  else if (toy_withName == "knn")
    toy_with_knn();
  else if (toy_withName == "marsystemIO")
    toy_with_marsystemIO();
  else if (toy_withName == "multiple")
    toy_with_multiple(fname0, fname1);
  else if (toy_withName == "matlab")
    toy_with_matlab(fname0);
  else if (toy_withName == "margrid")
    toy_with_margrid(fname0);
  else if (toy_withName == "mixer")
    toy_with_mixer(fname0, fname1, fname2);
  else if (toy_withName == "mono2stereo")
    toy_with_mono2stereo(fname0);
  else if (toy_withName == "mp3convert")
    toy_with_mp3convert(fname0);
  else if (toy_withName == "normMaxMin")
    toy_with_normMaxMin();
  else if (toy_withName == "onsets")
    toy_with_onsets(fname0);
  else if (toy_withName == "panorama")
    toy_with_panorama(fname0);
  else if (toy_withName == "parallel")
    toy_with_parallel();
  else if (toy_withName == "phase")
    toy_with_phase(fname0, atoi(fname1.c_str()));
  else if (toy_withName == "pngwriter")
    toy_with_pngwriter(fname0);
  else if (toy_withName == "phisem")
    toy_phisem();
  else if (toy_withName == "pitch")
    toy_with_pitch(fname0);
  else if (toy_withName == "plucked")
    toy_with_plucked(fname0,fname1,fname2,fname3,fname4,fname5);
  else if (toy_withName == "pluckedLive")
    toy_with_pluckedLive(fname0,fname1,fname2,fname3,fname4);
  else if (toy_withName == "pluckedBug")
    toy_with_pluckedBug();
  else if (toy_withName == "power")
    toy_with_power(fname0);
  else if (toy_withName == "probe")
    toy_with_probe();
  else if (toy_withName == "radiodrum")
    toy_with_RadioDrumInput();
  else if (toy_withName == "rats")
    toy_with_rats(fname0);
  else if (toy_withName == "realvec")
    toy_with_realvec();
  else if (toy_withName == "realvecCtrl")
    toy_with_realvecCtrl(fname0);
  else if (toy_withName == "reverb")
    toy_with_reverb(fname0);
  else if (toy_withName == "rmsilence")
    toy_with_rmsilence(fname0);
  else if (toy_withName == "scheduler")
    toy_with_scheduler(fname0);
  else if (toy_withName == "shredder")
    toy_with_shredder(fname0);
  else if (toy_withName == "sfplay")
    toy_with_sfplay(fname0);
  else if (toy_withName == "sine")
    toy_with_sine();
  else if (toy_withName == "scales")
    toy_with_scales();
  else if (toy_withName == "spectralSNR")
    toy_with_spectralSNR(fname0, fname1);
  else if (toy_withName == "stereo2mono")
    toy_with_stereo2mono(fname0);
  else if (toy_withName == "stereoFeatures")
    toy_with_stereoFeatures(fname0, fname1);
  else if (toy_withName == "stereoFeaturesVisualization")
    toy_with_stereoFeaturesVisualization(fname0);
  else if (toy_withName == "stereoMFCC")
    toy_with_stereoMFCC(fname0, fname1);
  else if (toy_withName == "swipe")
    toy_with_swipe(fname0);
  else if (toy_withName == "stretchLinear")
    toy_with_stretchLinear(fname0);
  else if (toy_withName == "tempo")
    toy_with_tempo(fname0, 120, 1);
  else if (toy_withName == "train_predict")
    toy_with_train_predict(fname0, fname1);
  else if (toy_withName == "MidiFileSynthSource")
    toy_with_MidiFileSynthSource(fname0);
  else if (toy_withName == "updControl")
    toy_with_updControl(fname0);
  else if (toy_withName == "vibrato")
    toy_with_vibrato(fname0);
  else if (toy_withName == "vicon")
    toy_with_vicon(fname0);
  else if (toy_withName == "weka")
    toy_with_weka(fname0);
  else if (toy_withName == "windowedsource")
    toy_with_windowedsource(fname0);
  else if (toy_withName =="stereoFeaturesMFCC")
    toy_with_stereoFeaturesMFCC(fname0, fname1);
  else if (toy_withName == "marostring")
    toy_with_marostring(fname0);
  else if (toy_withName == "volume_normalize")
    toy_with_volume_normalize(fname0,fname1);
  else if (toy_withName == "accent_filter_bank")
    toy_with_accent_filter_bank(fname0,fname1);
  else if (toy_withName == "ExtractChroma")
    toy_with_chroma(fname0,fname1);
  else if (toy_withName == "orca_record")
    toy_with_orca_record(fname0);
  else if (toy_withName == "realvec_record")
    toy_with_realvec_record(fname0);
  else if (toy_withName == "stereospectrum_bin_change")
    toy_with_stereospectrum_bin_change(fname0);
  else if (toy_withName == "peaker")
    toy_with_peaker(fname0);
  else if (toy_withName == "robot_peak_onset")
    toy_with_robot_peak_onset(fname0,fname1);
  else if (toy_withName == "midiout")
    toy_with_midiout();
  else if (toy_withName == "beats")
    toy_with_beats(fname0, fname1, progName);
  else if (toy_withName == "auditorytbx")
    toy_with_auditorytbx(fname0);
  else if (toy_withName == "lyons_passive_ear")
    toy_with_lyons(fname0);
  else if (toy_withName == "delay")
    toy_with_delay(fname0,fname1);
  else if (toy_withName == "aim_pzfc")
    toy_with_aim_pzfc(fname0);
  else if (toy_withName == "aim_gammatone")
    toy_with_aim_gammatone(fname0);
  else if (toy_withName == "aim_hcl")
    toy_with_aim_hcl(fname0);
  else if (toy_withName == "aim_localmax")
    toy_with_aim_localmax(fname0);
  else if (toy_withName == "aim_sai")
    toy_with_aim_sai(fname0);
  else if (toy_withName == "aim_ssi")
    toy_with_aim_ssi(fname0);
  else if (toy_withName == "aim")
    toy_with_aim(fname0);
  else if (toy_withName == "aim_boxes")
    toy_with_aim_boxes(fname0);
  else if (toy_withName == "unfold")
    toy_with_unfold(fname0);
  else if (toy_withName == "dumpwav")
    toy_with_dumpwav(fname0, fname1);
  else if (toy_withName == "sness_shredder")
    toy_with_sness_shredder(fname0);
  else if (toy_withName == "aim_vq")
    toy_with_aim_vq(fname0);
  else if (toy_withName == "ncut")
    toy_with_NCut();
  else if (toy_withName == "peakmerge")
    toy_with_PeakView(fname0,fname1,fname2,fname3);
  else if (toy_withName == "peakeval")
    toy_with_PeakEval(fname0,fname1,fname2);
  else if (toy_withName == "orcasnip")
    toy_with_orcaSnip(fname0,fname1);
  else if (toy_withName == "realvecsource_realtime")
    toy_with_realvecsource_realtime();

  else
  {
    cout << "Unsupported toy_with " << endl;
    printHelp(progName);
  }

}
