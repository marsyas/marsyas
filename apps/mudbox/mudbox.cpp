// mudbox is a container executable for various simple 
// functions/applications that typically test drive 
// a single MarSystem or type of processing. It can 
// either be viewed as repository of simple (but sometimes 
// broken) examples or as an incubator for more complicated 
// applications that deserve a separate executable. This 
// is the best place to experiment with Marsyas without 
// changing adding your own application and having to change 
// the build process.  

#include <cstdio>
#include <string>

#include "common.h"
#include "MarSystemManager.h"
#include "Cascade.h" 
#include "Parallel.h" 
#include "CommandLineOptions.h"
#include "FileName.h"
#include "Filter.h"
#include "Gain.h"
#include "RtAudio.h"
#include "RtMidi.h"
#include "MarSystemTemplateBasic.h"
#include "MarSystemTemplateAdvanced.h"
#include "EvValUpd.h"
#include "Collection.h"
#include "NumericLib.h"

using namespace std;
using namespace Marsyas;

CommandLineOptions cmd_options;
string testName;
int helpopt;
int usageopt;
int verboseopt;

void 
printUsage(string progName)
{
  MRSDIAG("marsyasTests.cpp - printUsage");
  cerr << "Usage : " << progName << " -t testName file1 file2 ... fileN" << endl;
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
  cerr << "Usage : " << progName << "-t testName file1 file2 file3" << endl;
  cerr << endl;
  cerr << "Supported tests:" << endl;
  cerr << "cascade         : test Cascade composite " << endl;
  cerr << "collection      : test Collection " << endl;
  cerr << "drumclassify    : test drumclassify (mplfile argument)" << endl;
  cerr << "fanoutswitch    : test disabling fanout branches " << endl;
  cerr << "filter          : test filter MarSystem " << endl;
  cerr << "fft             : test fft analysis/resynthesis " << endl;
  cerr << "knn             : test K-NearestNeighbor classifier " << endl;
  cerr << "marsystemIO     : test marsystem IO " << endl;
  cerr << "mixer           : test fanout for mixing " << endl;
  cerr << "mp3convert      : test convertion of a collection of .mp3 files to .wav files" << endl;
  cerr << "normMaxMin      : test of normalize marsSystem " << endl;
  cerr << "panorama     : test Panorama amplitude panning " << endl;
  cerr << "parallel        : test Parallel composite " << endl;
  cerr << "probe           : test Probe functionality " << endl;
  cerr << "realvec         : test realvec functions " << endl;
  cerr << "rmsilence  	   : test removing silences " << endl;
  cerr << "scheduler       : test scheduler " << endl;
  cerr << "schedulerExpr   : test scheduler with expressions " << endl;
  cerr << "SOM		         : test support vector machine " << endl;
  cerr << "spectralSNR     : test spectral SNR " << endl;
  cerr << "stereoFeatures  : test stereo features " << endl;
  cerr << "stereoMFCC      : test stereo MFCC " << endl;
  cerr << "stereoFeaturesMFCC : test stereo features and MFCCs" << endl;
  cerr << "stereo2mono     : test stereo to mono conversion " << endl;
  cerr << "tempo	         : test tempo estimation " << endl;
  cerr << "vibrato       : test vibrato using time-varying delay line" << endl;
  cerr << "vicon           : test processing of vicon motion capture data" << endl;
  cerr << "Windowing       : test different window functions of Windowing marsystem" << endl;
  cerr << "weka            : test weka source and sink functionality" << endl;
  cerr << "updctrl         : test updating control with pointers " << endl;
  cerr << "duplex          : test duplex audio input/output" << endl;
  cerr << "simpleSFPlay    : plays a sound file" << endl;
  cerr << "getControls     : test getControls functionality " << endl;
  cerr << "mono2stereo     : test mono2stereo MarSystem " << endl;
  exit(1);
}

void 
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addStringOption("testName", "t", EMPTYSTRING);
}

void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  verboseopt = cmd_options.getBoolOption("verbose");
  testName = cmd_options.getStringOption("testName");
}

void 
test_scheduler(string sfName)
{
  cout << "Testing scheduler" << endl;
  
  string ipName=sfName;
  string opName="scheduled.wav";
  
  cout << "Input: " << ipName << "\nOutput: " << opName << endl;
  
  MarSystemManager mng;
  
  // Create a series Composite 
  //    type, name
  MarSystem* series1 = mng.create("Series", "series1");
  series1->addMarSystem(mng.create("SoundFileSource", "src"));
  series1->addMarSystem(mng.create("Gain", "gain"));
  series1->addMarSystem(mng.create("SoundFileSink", "dest"));
  
  // only update controls from Composite level 
  series1->updctrl("mrs_natural/inSamples", 256);
  series1->updctrl("SoundFileSource/src/mrs_string/filename", ipName);
  series1->updctrl("SoundFileSink/dest/mrs_string/filename", opName);
  
  // post events to the scheduler using updctrl(..)
  series1->updctrl("Gain/gain/mrs_real/gain", 1.0);
  series1->updctrl("1s", Repeat("2s", 3), new EvValUpd(series1,"Gain/gain/mrs_real/gain", 0.0));
  series1->updctrl("2s", Repeat("2s", 3), new EvValUpd(series1,"Gain/gain/mrs_real/gain", 1.0));
  
  for (int i=0; i<10000; i++) {
    series1->tick();
  }
  
  // Composite deletes the added MarSystems
  // so you must not delete them
  delete series1;
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

    TimeLoop->updctrl("PeakerAdaptive/peak/mrs_natural/peakEnd", 512);
    TimeLoop->updctrl("PeakerAdaptive/peak/mrs_real/peakSpacing", 0.5);
    TimeLoop->updctrl("PeakerAdaptive/peak/mrs_real/peakStrength", 0.7);
    TimeLoop->updctrl("PeakerAdaptive/peak/mrs_natural/peakStart", 0);
    TimeLoop->updctrl("PeakerAdaptive/peak/mrs_natural/peakStrengthReset", 2);
    TimeLoop->updctrl("PeakerAdaptive/peak/mrs_real/peakDecay", 0.9);
    TimeLoop->updctrl("PeakerAdaptive/peak/mrs_real/peakGain", 0.5);

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
    extractNet->updctrl("GaussianClassifier/classifier/mrs_string/mode","predict");
    extractNet->updctrl("GaussianClassifier/classifier/mrs_natural/nLabels",2);

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

    for ( unsigned int i = 0; i < words.size(); i++)
    {

        if ( words[i] == "means" )
            for ( int p = 0; p < numberOfCoefficients; p++)
            {
                // get he current word from the file
                word = words[i++];
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
                word = words[i++];
                // cast the string to a float
                float readval;
                istringstream mystream(word);
                mystream >> readval;
                means.setval( i,i,readval);
            }
    }

    cout << means << endl;
    cout << vars << endl;

    extractNet->updctrl("GaussianClassifier/classifier/realvec/means", means);
    extractNet->updctrl("GaussianClassifier/classifier/realvec/covars", vars);
    extractNet->updctrl("mrs_natural/inSamples",512);
    extractNet->updctrl("mrs_natural/onSamples",512);
    extractNet->updctrl("mrs_real/israte", 44100.0);
    extractNet->updctrl("mrs_real/osrate", 44100.0);

    realvec in1;
    realvec out1;
    realvec out2;

    in1.create(TimeLoop->getctrl("mrs_natural/inObservations")->toNatural(), 
            TimeLoop->getctrl("mrs_natural/inSamples")->toNatural());

    out1.create(TimeLoop->getctrl("mrs_natural/onObservations")->toNatural(), 
            TimeLoop->getctrl("mrs_natural/onSamples")->toNatural());

    out2.create(extractNet->getctrl("mrs_natural/onObservations")->toNatural(),
            extractNet->getctrl("mrs_natural/onSamples")->toNatural());

    RtMidiOut *midiout = new RtMidiOut();
    midiout->openPort(0);
    vector<unsigned char> message;

    message.push_back(144);
    message.push_back(60);
    message.push_back(0);

    // PeakerAdaptive looks for hits and then if it finds one reports a non-zero value
    // When a non-zero value is found extractNet is ticked
    while ( TimeLoop->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool() )
    {
        TimeLoop->process(in1,out1);
        for (int i = 0; i < windowsize;i++)
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
test_simpleSFPlay(string sfName)
{
  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");

  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));

  playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  playbacknet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
  playbacknet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  
  mrs_bool isEmpty;
  //cout << *playbacknet << endl;
  while (isEmpty = playbacknet->getctrl("mrs_bool/notEmpty")->toBool()) 
    {
      //cout << "pos " << playbacknet->getctrl("mrs_natural/pos")->to<mrs_natural>() << endl;
      
      playbacknet->tick();
      
      //test if setting "mrs_natural/pos" to 0 for rewinding is working
      //if(playbacknet->getctrl("mrs_natural/pos")->to<mrs_natural>() > 100000)
      //	playbacknet->updctrl("mrs_natural/pos", 0);
    }
  cout << "tick " << isEmpty << endl;
  delete playbacknet;
}

void
test_getControls(string sfName)
{
  MarSystemManager mng;
  
  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));


  MarSystem* newseries = mng.create("Series", "newseries");
  newseries->addMarSystem(mng.create("Gain", "g1"));
  newseries->addMarSystem(mng.create("Gain", "g2"));

  playbacknet->addMarSystem(newseries);
  

  playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  playbacknet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");

  
  cout << *playbacknet << endl;

  std::map<std::string, MarControlPtr> mycontrols = playbacknet->getControls();
  std::map<std::string, MarControlPtr>::iterator myc;
  
  for (myc = mycontrols.begin(); myc != mycontrols.end(); ++myc)
    cout << myc->first << endl;
  
  
  delete playbacknet;
}

void
test_mono2stereo(string sfName)
{
  cout << "Mono2Stereo test" << endl;
  
  MarSystemManager mng;

  MarSystem* playbacknet = mng.create("Series", "playbacknet");

  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("Mono2Stereo", "m2s"));
  playbacknet->addMarSystem(mng.create("SoundFileSink", "dest"));

  string outName = "m2s.wav";
  
  playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updctrl("SoundFileSink/dest/mrs_string/filename", outName);

  
  while (playbacknet->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>()) 
    {
      playbacknet->tick();
    }
  
  delete playbacknet;
}

void 
test_fanoutswitch()
{
  cout << "Testing fanout switch" << endl;
  
  MarSystemManager mng;
  
  MarSystem* pnet = mng.create("Series", "src");
  MarSystem* src = mng.create("SoundFileSource", "src");
  src->updctrl("mrs_string/filename", "/home/gtzan/data/sound/music_speech/music/gravity.au");

  pnet->addMarSystem(src);
  pnet->addMarSystem(mng.create("PlotSink", "psink1"));  
  pnet->updctrl("PlotSink/psink1/mrs_string/filename", "in");
 
  MarSystem* mix = mng.create("Fanout", "mix");
  MarSystem* g1 = mng.create("Gain", "g1");
  MarSystem* g2 = mng.create("Gain", "g2");
  MarSystem* g3 = mng.create("Gain", "g3");
  MarSystem* g4 = mng.create("Gain", "g4");

  g1->updctrl("mrs_real/gain", 1.5);
  g2->updctrl("mrs_real/gain", 2.5);
  g3->updctrl("mrs_real/gain", 3.0);
  g4->updctrl("mrs_real/gain", 4.0);
  
  mix->addMarSystem(g1);
  mix->addMarSystem(g2);
  mix->addMarSystem(g3);
  mix->addMarSystem(g4);
  
  pnet->addMarSystem(mix);
  pnet->addMarSystem(mng.create("PlotSink", "psink2"));
  
  // Disable subset of Fanout branches 
  pnet->updctrl("Fanout/mix/mrs_natural/disable", 2);
  pnet->updctrl("Fanout/mix/mrs_natural/disable", 0);
  
  // tick to check the result 
  // PlotSinks are used for output 
  pnet->tick();

	delete pnet;
}

void 
test_rmsilence(string sfName)
{
  cout << "Removing silences from: " << sfName << endl;
  MarSystemManager mng;

  MarSystem* rmnet = mng.create("Series", "rmnet");
  
  MarSystem* srm = mng.create("SilenceRemove", "srm");
  MarSystem* src = mng.create("SoundFileSource", "src");
  src->updctrl("mrs_string/filename", sfName);
  srm->addMarSystem(src);

  
  rmnet->addMarSystem(srm);
  rmnet->addMarSystem(mng.create("SoundFileSink", "dest"));
  
  FileName fname(sfName);  
  rmnet->updctrl("SoundFileSink/dest/mrs_string/filename", "srm.wav");
  
  cout << *rmnet << endl;
  while (rmnet->getctrl("SilenceRemove/srm/SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>())
    {
      rmnet->tick();
    }
  
  cout << "Finished removing silences. Output is " << "srm.wav" << endl;

  delete rmnet;
}

void
test_marsystemIO()
{
  cout << "Testing IO of MarSystems" << endl;
  MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");
  
  MarSystem* src = mng.create("SoundFileSource", "src");
  MarSystem* dest = mng.create("AudioSink", "dest");

  
  pnet->addMarSystem(src);
  pnet->addMarSystem(dest);
  
  pnet->updctrl("mrs_natural/inSamples", 1024);
  
  
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
test_mixer(string sfName0, string sfName1)
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
  pnet->addMarSystem(mng.create("AudioSink", "dest"));

  pnet->updctrl("Fanout/mix/Series/branch0/SoundFileSource/src0/mrs_string/filename", sfName0);
  pnet->updctrl("Fanout/mix/Series/branch1/SoundFileSource/src1/mrs_string/filename", sfName1);
  pnet->updctrl("Fanout/mix/Series/branch0/Gain/gain0/mrs_real/gain", 0.5);
  pnet->updctrl("Fanout/mix/Series/branch1/Gain/gain1/mrs_real/gain", 0.5);

  while(1)
    {
      pnet->tick();
    }

	delete pnet;
}

void 
test_fft(string sfName) 
{
  cout << "test_fft: sfName = " << sfName << endl;

  MarSystemManager mng;
  
  MarSystem* series = mng.create("Series","network");
  series->addMarSystem(mng.create("SoundFileSource","src"));
  series->addMarSystem(mng.create("Spectrum", "spk"));
  series->addMarSystem(mng.create("InvSpectrum", "ispk"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));

  series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  series->updctrl("SoundFileSink/dest/mrs_string/filename",  "sftransformOutput.au");

  mrs_natural i =0;
  
  while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
    {
      series->tick();
      i++; 
    }

  cout << (*series) << endl;
  
	delete series;
}

void 
test_parallel()
{
  Parallel *parallel = new Parallel("parallel");
  
  realvec in;
  in.create(mrs_natural(10), mrs_natural(10));
  for (mrs_natural i = 0; i < in.getRows(); i++){
    for (mrs_natural j = 0; j < in.getCols(); j++){
      in(i,j) = i*j;
    }
  }
  
  realvec out;
  out.create(in.getRows(),in.getCols());
  
  Gain* g0 = new Gain("g0");
  g0->setctrl("mrs_natural/inObservations", mrs_natural(3));
  g0->setctrl("mrs_natural/inSamples", in.getCols());
  g0->setctrl("mrs_real/gain", 3.0f);
  
  Gain* g1 = new Gain("g1");
  g1->setctrl("mrs_natural/inObservations", mrs_natural(2));
  g1->setctrl("mrs_real/gain", 2.0f);
  
  Gain* g2 = new Gain("g2");
  g2->setctrl("mrs_natural/inObservations", mrs_natural(5));
  g2->setctrl("mrs_real/gain", 5.0f);
  
  parallel->addMarSystem(g0);
  parallel->addMarSystem(g1);
  parallel->addMarSystem(g2);
  
  parallel->process(in, out);
  
  cout << out << endl;

	delete parallel;
}

void 
test_probe()
{
  cout << "Testing probe functionality" << endl;
  
  // create the Marsyas 
  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");
  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("Gain", "gain"));
  pnet->addMarSystem(mng.create("Gain", "gain2"));
 
  cout << "BEFORE PROBE " << endl;  
  cout << "pnet = " << *pnet << endl;
  pnet->updctrl("mrs_bool/probe", true);
  cout << "AFTER PROBE" << endl;

  cout << "AFTER ONE TICK" << endl;
  pnet->tick();  
  cout << "pnet = " << *pnet << endl;
}

void 
test_cascade()
{
  Cascade *cascade = new Cascade("cascade");
  
  realvec a(3),b(3);
  Filter* f0 = new Filter("f0");
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
  
  Filter* f1 = new Filter("f1");
  a(0) = 1.0f;
  a(1) = 1.0f;
  a(2) = 0.0f;
  b(0) = 1.0f;
  b(1) = 0.0f;
  b(2) = 0.0f;
  f1->setctrl("mrs_realvec/ncoeffs", a);
  f1->setctrl("mrs_realvec/dcoeffs", b);
  
  cascade->addMarSystem(f0);
  cascade->addMarSystem(f1);
  
  realvec in, out;
  in.create(mrs_natural(1),mrs_natural(5));
  in(0,0) = 1.0f;
  out.create(mrs_natural(2),mrs_natural(5));
  
  cascade->process(in, out);
  
  cout << out << endl;

	delete cascade;
}

void 
test_collection(string sfName) 
{
  
  MarSystemManager mng;
  
  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));
  
  playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  
  playbacknet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
  playbacknet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  
  mrs_bool isEmpty;
  //cout << *playbacknet << endl;
  int cindex = 0;
  while (isEmpty = playbacknet->getctrl("mrs_bool/notEmpty")->toBool()) 
    {
      cout << "tick " << isEmpty << endl;
      
      // for (int i=0; i<100; i++)
	playbacknet->tick();
      
	// playbacknet->updctrl("SoundFileSource/src/mrs_natural/cindex", cindex);
      cout << playbacknet->getctrl("SoundFileSource/src/mrs_string/currentlyPlaying")->to<mrs_string>() << endl;
      
      // cindex++;
      // cout << "cindex = " << cindex << endl;
      

      //test if setting "mrs_natural/pos" to 0 for rewinding is working
      //if(playbacknet->getctrl("mrs_natural/pos")->to<mrs_natural>() > 100000)
      //	playbacknet->updctrl("mrs_natural/pos", 0);
    }
  cout << "tick " << isEmpty << endl;
  delete playbacknet;
}

void 
test_knn()
{
  MarSystemManager mng;
  MarSystem *knn = mng.create("KNNClassifier", "knn");

  // ---- TEST TRAIN ---------------------
  
  knn->updctrl("mrs_string/mode", "train");
  
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
  
  knn->updctrl("mrs_natural/inSamples", inS);
  knn->updctrl("mrs_natural/inObservations", inO);
  
  realvec output(knn->getctrl("mrs_natural/onObservations")->toNatural(), knn->getctrl("mrs_natural/onSamples")->toNatural());
 
  cout << "INPUT: " << input << endl;
  
  knn->process(input, output);
  cout << "TEST: " << output << endl;

  // IMPORTANT updcontrol done and then process to indicate to KNN to finish  
  knn->updctrl("mrs_bool/done", true);
  knn->tick();
  
  // --------------- TEST PREDICT -----------------
  knn->updctrl("mrs_string/mode", "predict");
  knn->updctrl("mrs_natural/k", 3);
  knn->updctrl("mrs_natural/nLabels", 2);
  inS = 1;
  inO = 3;

  realvec input2(inO, inS); 
  
  input2(0,0) = 3.0;
  input2(1,0) = 3.2;
  input2(2,0) = 1.0;

  knn->updctrl("mrs_natural/inSamples", inS);
  knn->updctrl("mrs_natural/inObservations", inO);
 
  realvec output2(knn->getctrl("mrs_natural/onObservations")->toNatural(), knn->getctrl("mrs_natural/onSamples")->toNatural());

  cout << "Predict" << endl; 
  knn->process(input2, output2);

  cout << "PREDICT: " << output2 << endl;

	delete knn;
}

// test filter 
void 
test_filter() 
{
  // Test 1 

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

  // Test 2 

  Filter* f = new Filter("f");

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
test_panorama(string sfName)
{
  cout << "Testing panorama amplitude panning" << endl;
  MarSystemManager mng;
  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("Panorama", "pan"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));
  playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  
  playbacknet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
  playbacknet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  mrs_bool isEmpty;
  mrs_natural t = 0;	
  mrs_real angle = -PI/4.0;
  playbacknet->updctrl("Panorama/pan/mrs_real/angle", angle);
  while (isEmpty = playbacknet->getctrl("mrs_bool/notEmpty")->toBool()) 
    {
      playbacknet->tick();
      t++;
      if (t % 10 == 0)
	{
	  angle += (PI/32);
	  playbacknet->updctrl("Panorama/pan/mrs_real/angle", angle);
	}
    }
}


// moved into regressionChecks on July 14.  -gp
void 
test_vibrato(string sfName)
{
  cout << "Testing vibrato" << endl;

  MarSystemManager mng;
  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("Vibrato", "vib"));
  playbacknet->addMarSystem(mng.create("Vibrato2", "vib2"));
  playbacknet->addMarSystem(mng.create("AudioSink", "dest"));
  
  playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  playbacknet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  
  playbacknet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
  playbacknet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  
  playbacknet->updctrl("Vibrato/vib/mrs_real/mod_freq", 10.0);

  
  mrs_bool isEmpty;
  while (isEmpty = playbacknet->getctrl("mrs_bool/notEmpty")->toBool()) 
    {
      playbacknet->tick();
    }
}


// test input,processing and sonification 
// of Vicon (motion capture system) 

void 
test_vicon(string vfName)
{
  if (vfName != EMPTYSTRING) 
    cout << "Testing Vicon file: " << vfName << endl;
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
  
  viconNet->updctrl("mrs_natural/inSamples", 1);
  viconNet->updctrl("ViconFileSource/vsrc/mrs_string/filename", vfName);
  viconNet->updctrl("mrs_real/israte", 120.0);
 
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

  realvec in(viconNet->getctrl("mrs_natural/inObservations")->toNatural(), 
	     viconNet->getctrl("mrs_natural/inSamples")->toNatural());
  realvec out(viconNet->getctrl("mrs_natural/onObservations")->toNatural(), 
	      viconNet->getctrl("mrs_natural/onSamples")->toNatural());
  
  playbacknet->updctrl("mrs_natural/inSamples", 184);

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

  while (viconNet->getctrl("ViconFileSource/vsrc/mrs_bool/notEmpty")->toBool()) 
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
  
      // playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc1/mrs_real/frequency", fabs(out(13,0)));
      // playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc1/mrs_real/frequency", fabs(fabs(out(1.0))));

      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc2/mrs_real/frequency", fabs(out(2,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc3/mrs_real/frequency", fabs(out(3,0)));
      
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc4/mrs_real/frequency", fabs(out(7,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc5/mrs_real/frequency", fabs(out(8,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc6/mrs_real/frequency", fabs(out(9,0)));

			playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc7/mrs_real/frequency", fabs(out(10,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc8/mrs_real/frequency", fabs(out(11,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc9/mrs_real/frequency", fabs(out(12,0)));

      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc10/mrs_real/frequency", fabs(out(16,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc11/mrs_real/frequency", fabs(out(17,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc12/mrs_real/frequency", fabs(out(18,0)));
      
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
test_MATLABengine()
{
  //In order to test the MATLABengine class
  // the following define must be set:
  //	  MARSYAS_MATLAB
  //
  // To build this test with MATLAB engine support, please consult the following site 
  // for detailed info:
  //
  // http://www.mathworks.com/access/helpdesk/help/techdoc/matlab_external/f39903.html
  //
  // <lmartins@inescporto.pt> - 17.06.2006

#ifdef MARSYAS_MATLAB
	
  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TEST evaluate MATLAB command" << endl;
  cout << "******************************************************" << endl;
  cout << endl << "Run MATLAB benchmark utility..." << endl;
  MATLABengine::getMatlabEng()->evalString("bench;");
  cout << endl << "Press any key to continue..." << endl;
  cout << endl << "Run other MATLAB stuff..." << endl;
  MATLABengine::getMatlabEng()->evalString("a = magic(10);");
  MATLABengine::getMatlabEng()->evalString("figure(3)");
  MATLABengine::getMatlabEng()->evalString("imagesc(a);");
  MATLABengine::getMatlabEng()->evalString("clear a;");
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TEST mrs_natural" << endl;
  cout << "******************************************************" << endl;
  mrs_natural Marsyas_natural = 123456789;
  cout << "Send a mrs_natural to MATLAB: " << Marsyas_natural << endl;
  MATLABengine::getMatlabEng()->putVariable(Marsyas_natural,"Marsyas_natural");
  cout << endl << "Variable sent. Check MATLAB variable 'Marsyas_natural' and compare values..." << endl;
  getchar();
  Marsyas_natural = 0;
  if(MATLABengine::getMatlabEng()->getVariable("Marsyas_natural", Marsyas_natural) == 0)
    cout << "Get it from MATLAB back to Marsyas: " << Marsyas_natural << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();
	
  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TEST mrs_real" << endl;
  cout << "******************************************************" << endl;
  mrs_real Marsyas_real = 3.123456789;
  cout << "Send a mrs_real to MATLAB: " << Marsyas_real << endl;
  MATLABengine::getMatlabEng()->putVariable(Marsyas_real,"Marsyas_real");
  cout << endl << "Variable sent: check MATLAB variable 'Marsyas_real' and compare values..." << endl;
  getchar();
  Marsyas_real = 0.0;
  if(MATLABengine::getMatlabEng()->getVariable("Marsyas_real", Marsyas_real)== 0)
    cout << "Get it from MATLAB back to Marsyas: " << Marsyas_real << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();
	
  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TEST mrs_complex" << endl;
  cout << "******************************************************" << endl;
  mrs_complex Marsyas_complex = mrs_complex(1.123456789, 2.123456789);
  cout << "Send a mrs_complex to MATLAB: " << Marsyas_complex.real() << " + j" << Marsyas_complex.imag() << endl;
  MATLABengine::getMatlabEng()->putVariable(Marsyas_complex,"Marsyas_complex");
  cout << endl << "Variable sent: check MATLAB variable 'Marsyas_complex' and compare values..." << endl;
  getchar();
  Marsyas_complex = mrs_complex(0.0, 0.0);
  if(MATLABengine::getMatlabEng()->getVariable("Marsyas_complex", Marsyas_complex) == 0)
    cout << "Get it from MATLAB back to Marsyas: " << Marsyas_complex.real() << " + j" << Marsyas_complex.imag() << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();
	
  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TEST realvec (row vector)" << endl;
  cout << "******************************************************" << endl;
  realvec marRow_realvec1D(4);//Marsyas row vector
  marRow_realvec1D(0) = 1.123456789;
  marRow_realvec1D(1) = 2.123456789;
  marRow_realvec1D(2) = 3.123456789;
  marRow_realvec1D(3) = 4.123456789;
  cout << "Send a realvec to MATLAB: " << endl;
  cout << endl << marRow_realvec1D  << endl;
  MATLABengine::getMatlabEng()->putVariable(marRow_realvec1D,"marRow_realvec1D");
  cout << endl << "Variable sent: check MATLAB variable 'marRow_realvec1D' and compare values..." << endl;
  getchar();
  marRow_realvec1D.setval(0.0);
  if(MATLABengine::getMatlabEng()->getVariable("marRow_realvec1D", marRow_realvec1D) == 0)
    cout << "Get it from MATLAB back to Marsyas: " << endl << endl << marRow_realvec1D << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TEST realvec (col vector)" << endl;
  cout << "******************************************************" << endl;
  realvec marCol_realvec1D(4,1);//Marsyas col vector
  marCol_realvec1D(0) = 1.123456789;
  marCol_realvec1D(1) = 2.123456789;
  marCol_realvec1D(2) = 3.123456789;
  marCol_realvec1D(3) = 4.123456789;
  cout << "Send a realvec to MATLAB: " << endl;
  cout << endl << marCol_realvec1D  << endl;
  MATLABengine::getMatlabEng()->putVariable(marCol_realvec1D,"marCol_realvec1D");
  cout << endl << "Variable sent: check MATLAB variable 'marCol_realvec1D' and compare values..." << endl;
  getchar();
  marCol_realvec1D.setval(0.0);
  if(MATLABengine::getMatlabEng()->getVariable("marCol_realvec1D", marCol_realvec1D) == 0)
    cout << "Get it from MATLAB back to Marsyas: " << endl << endl << marCol_realvec1D << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TEST realvec (2D array)" << endl;
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
  MATLABengine::getMatlabEng()->putVariable(marsyas_realvec2D,"marsyas_realvec2D");
  cout << endl << "Variable sent: check MATLAB variable 'marsyas_realvec2D' and compare values..." << endl;
  getchar();
  marsyas_realvec2D.setval(0.0);
  if(MATLABengine::getMatlabEng()->getVariable("marsyas_realvec2D", marsyas_realvec2D)==0)
    cout << "Get it from MATLAB back to Marsyas: " << endl << endl << marsyas_realvec2D << endl;
  else
    cout << "Error getting value back from MATLAB!" << endl;
  getchar();

  //-------------------------------------------------------------------------------------------
  cout << "******************************************************" << endl;
  cout << "  TEST std::vector<mrs_natural>" << endl;
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

  MATLABengine::getMatlabEng()->putVariable(vector_natural,"vector_natural");
  cout << endl << "Variable sent: check MATLAB variable 'vector_natural' and compare values..." << endl;
  getchar();
  vector_natural.clear();
  if(MATLABengine::getMatlabEng()->getVariable("vector_natural", vector_natural)==0)
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
  cout << "  TEST std::vector<mrs_real>" << endl;
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
	
  MATLABengine::getMatlabEng()->putVariable(vector_real,"vector_real");
  cout << endl << "Variable sent: check MATLAB variable 'vector_real' and compare values..." << endl;
  getchar();
  vector_real.clear();
  if(MATLABengine::getMatlabEng()->getVariable("vector_real", vector_real)==0)
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
  cout << "  TEST std::vector<mrs_complex>" << endl;
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

  MATLABengine::getMatlabEng()->putVariable(vector_complex,"vector_complex");
  cout << endl << "Variable sent: check MATLAB variable 'vector_complex' and compare values..." << endl;
  getchar();
  vector_complex.clear();
  if(MATLABengine::getMatlabEng()->getVariable("vector_complex", vector_complex)==0)
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
  cout << endl << "MATLAB Engine not configured! Not possible to run test..." << endl;
  cout << "To build this test with MATLAB engine support, check:" << endl << endl;
  cout << "http://www.mathworks.com/access/helpdesk/help/techdoc/matlab_external/f39903.html" << endl;
  getchar();

#endif
}

void
test_LPC_LSP(string sfName)
{
  // In order to test the LPC and LSP routines using the MATLABengine class
  // for numeric validation of the routines and graphical plots of the results,
  // the following defines must be set:
  //
  //	  MARSYAS_MATLAB
  //    _MATLAB_LPC_ (in LPC.cpp) 
  //    _MATLAB_LSP_ (in LSP.cpp)
  //
  // Additionally, inside MATLAB, the /marsyasMATLAB directory should be in the path
  // so the LPC_test.m and LSP_test.m mfiles (included in /marsyasMATLAB) in can be  
  // called directly from the C++ code for testing and plotting purposes.
  //
  // <lmartins@inescporto.pt> - 17.06.2006

  cout << "TEST: LPC and LSP calculation and validation using MATLAB (engine)" << endl;
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
  input->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  input->updctrl("mrs_natural/inSamples", hopSize);

  input->addMarSystem(mng.create("ShiftInput", "si"));
  input->updctrl("ShiftInput/si/mrs_natural/Decimation", hopSize);
  input->updctrl("ShiftInput/si/mrs_natural/WindowSize", hopSize);

  input->addMarSystem(mng.create("LPC", "LPC"));
  input->updctrl("LPC/LPC/mrs_natural/order",lpcOrder);
  input->updctrl("LPC/LPC/mrs_real/lambda",0.0);
  input->updctrl("LPC/LPC/mrs_real/gamma",1.0);

  input->addMarSystem(mng.create("LSP", "lsp"));
  input->updctrl("LSP/lsp/mrs_natural/order",lpcOrder);
  input->updctrl("LSP/lsp/mrs_real/gamma",1.0);

  int i = 0;
  while(input->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
    {
      input->tick();
      cout << "Processed frame " << i << endl;
      i++;
    }

  cout << endl << "LPC and LSP processing finished!";

	delete input;
	*/

	MarSystemManager mng;

	MarSystem* input = mng.create("Series", "input");

	input->addMarSystem(mng.create("SoundFileSource","src"));
	input->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	input->updctrl("mrs_natural/inSamples", hopSize);

	//input->addMarSystem(mng.create("ShiftInput", "si"));
	//input->updctrl("ShiftInput/si/mrs_natural/Decimation", hopSize);
	//input->updctrl("ShiftInput/si/mrs_natural/WindowSize", hopSize);

	MarSystem* lspS = mng.create("Series","lspS");
	lspS->addMarSystem(mng.create("LPC", "LPC"));
	lspS->addMarSystem(mng.create("LSP", "lsp"));
	input->addMarSystem(lspS);
// 	input->updctrl("Series/lspS/LPC/LPC/mrs_natural/order",lpcOrder);
// 	input->updctrl("Series/lspS/LPC/LPC/mrs_real/lambda",0.0);
// 	input->updctrl("Series/lspS/LPC/LPC/mrs_real/gamma",1.0);
// 	input->updctrl("Series/lspS/LSP/lsp/mrs_natural/order",lpcOrder);
// 	input->updctrl("Series/lspS/LSP/lsp/mrs_real/gamma",1.0);

	int i = 0;
	while(input->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
	{
		input->tick();
		cout << "Processed frame " << i << endl;
		i++;
	}

	cout << endl << "LPC and LSP processing finished!";

	delete input;
}

void
test_realvec()
{
  //Test new operator= implementation:
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

  cout << "When tests stops - press a key to continue" << endl;
 
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
  MATLAB_GET	("invMatrix", matrixA);
  cout << matrixA << endl;
  getchar();
  cout << "Compare results: difference should be a zero (or infinitesimal) valued matrix: " << endl << endl;
  cout << matrixA - invMatrix << endl;
  cout << "Maximum absolute error = " << (matrixA - invMatrix).maxval() << endl;
  getchar();

  //test DivergenceShape metrics
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
test_MarControls(string sfName)
{
  cout << "TEST: new MarControl API" << endl;
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
	pnet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);

	//the new way to use controls (using a "smart" pointer MarControlPtr object)
	//in fact only makes sense to use this new form of controls' access if the
	//control is to be accessed intensively (e.g. in a for loop), as is this case.
	//(this control pointer can be somehow seen as an "efficient" link!)
	MarControlPtr ctrl_notEmpty = pnet->getctrl("SoundFileSource/src/mrs_bool/notEmpty");
	MarControlPtr ctrl_repeats = pnet->getctrl("MarSystemTemplateBasic/basic/mrs_natural/repeats");

	//Custom Controls
	MarControlPtr ctrl_hdr = pnet->getctrl("MarSystemTemplateAdvanced/advanced/mrs_myheader/hdrname");
	MyHeader hdr;
	hdr.someString = "myHeader.txt";
	hdr.someFlag = true;
	hdr.someValue = 666;
	hdr.someVec = realvec(2);
	ctrl_hdr->setValue(hdr);

	pnet->updctrl("MarSystemTemplateBasic/basic/mrs_natural/repeats", 2);

	mrs_natural tmp;
	while(ctrl_notEmpty->isTrue())
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

	cout << "Finished MarControls test!";

	delete pnet;
}

double 
randD(double max)
{
  return max  *  (double)rand() / ((double)(RAND_MAX)+(double)(1.0)) ; 
}  






void 
test_stereoFeaturesMFCC(string fname0, string fname1)
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
  total->updctrl("Accumulator/acc/mrs_natural/nTimes", 1000);
  total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));

  
  total->updctrl("WekaSink/wsink/mrs_natural/nLabels", 3);
  total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1); 
  total->updctrl("WekaSink/wsink/mrs_string/labelNames", "garage,grunge,jazz,");
  total->updctrl("WekaSink/wsink/mrs_string/filename", "stereoFeaturesMFCC.arff"); 
  
  playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", fname0);
  playbacknet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");


  total->updctrl("mrs_natural/inSamples", 1024);

  mrs_bool isEmpty;

  // cout << *total << endl;

  Collection l;
  l.read(fname0);
 
  int i,t;
  
  total->updctrl("Annotator/ann/mrs_natural/label", 0); 
  for (i=0; i < l.size(); i++) 
    {
      total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
      /* if (i==0) 
	total->updctrl("Accumulator/acc/Series/playbacknet/AudioSink/dest/mrs_bool/initAudio", true);
      */ 
      cout << "Processing " << l.entry(i) << endl;
      total->tick();
      cout << "i = " << i << endl;
      
    }
  
  Collection m;
  m.read(fname1);
  
  total->updctrl("Annotator/ann/mrs_natural/label", 1); 


  for (i=0; i < m.size(); i++)
    {
      total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
      cout << "Processing " << m.entry(i) << endl;
      total->tick();
      cout << "i=" << i << endl;
    }


  Collection n;
  n.read("j.mf");
  
  total->updctrl("Annotator/ann/mrs_natural/label", 2); 
  

  for (i=0; i < n.size(); i++)
    {
      total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", n.entry(i));
      cout << "Processing " << n.entry(i) << endl;
      total->tick();
      cout << "i=" << i << endl;
    }
  



 
}








void 
test_stereoMFCC(string fname0, string fname1)
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
  total->updctrl("Accumulator/acc/mrs_natural/nTimes", 1000);
  total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));

  
  total->updctrl("WekaSink/wsink/mrs_natural/nLabels", 3);
  total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1); 
  total->updctrl("WekaSink/wsink/mrs_string/labelNames", "garage,grunge,jazz,");
  total->updctrl("WekaSink/wsink/mrs_string/filename", "stereoMFCC.arff"); 
  
  playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", fname0);
  playbacknet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");


  total->updctrl("mrs_natural/inSamples", 1024);

  mrs_bool isEmpty;

  // cout << *total << endl;

  Collection l;
  l.read(fname0);
 
  int i,t;
  
  total->updctrl("Annotator/ann/mrs_natural/label", 0); 
  for (i=0; i < l.size(); i++) 
    {
      total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
      /* if (i==0) 
	total->updctrl("Accumulator/acc/Series/playbacknet/AudioSink/dest/mrs_bool/initAudio", true);
      */ 
      cout << "Processing " << l.entry(i) << endl;
      total->tick();
      cout << "i = " << i << endl;
      
    }
  
  Collection m;
  m.read(fname1);
  
  total->updctrl("Annotator/ann/mrs_natural/label", 1); 


  for (i=0; i < m.size(); i++)
    {
      total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
      cout << "Processing " << m.entry(i) << endl;
      total->tick();
      cout << "i=" << i << endl;
    }
 
  Collection n;
  n.read("j.mf");
  
  total->updctrl("Annotator/ann/mrs_natural/label", 2); 


  for (i=0; i < n.size(); i++)
    {
      total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", n.entry(i));
      cout << "Processing " << n.entry(i) << endl;
      total->tick();
      cout << "i=" << i << endl;
    }
  




}


void 
test_mp3convert(string fname0)
{
  MarSystemManager mng;
  MarSystem* convertNet = mng.create("Series", "convertNet");

  convertNet->addMarSystem(mng.create("SoundFileSource", "src"));
  convertNet->addMarSystem(mng.create("SoundFileSink", "dest"));
  

  Collection l;
  l.read(fname0);

  for (int i=0; i < l.size(); i++)
    {
      convertNet->updctrl("SoundFileSource/src/mrs_string/filename", l.entry(i));
      ostringstream oss;
      oss << "ojazz" << i << ".wav";
      cout << "Converting " << l.entry(i) << " to " << oss.str() << endl;
      convertNet->updctrl("SoundFileSink/dest/mrs_string/filename", oss.str());
      
      while(convertNet->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>() == true)
	{
	  convertNet->tick();
	}

    }






}




void
test_stereoFeaturesVisualization(string fname0)
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
  

  playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", fname0);
  playbacknet->updctrl("mrs_natural/inSamples", 1024);
  playbacknet->updctrl("mrs_bool/initAudio", true);

  acc->addMarSystem(playbacknet);
  acc->updctrl("mrs_natural/nTimes", 2500);
  
  total->addMarSystem(acc);
  total->addMarSystem(mng.create("PlotSink", "psink"));

  total->tick();
  
  
}


void 
test_stereoFeatures(string fname0, string fname1)
{
  
  cout << "TESTING STEREO FEATURES" << endl;

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
  total->updctrl("Accumulator/acc/mrs_natural/nTimes", 1000);
  total->addMarSystem(statistics2);

  total->addMarSystem(mng.create("Annotator", "ann"));
  total->addMarSystem(mng.create("WekaSink", "wsink"));

  
  total->updctrl("WekaSink/wsink/mrs_natural/nLabels", 4);
  total->updctrl("WekaSink/wsink/mrs_natural/downsample", 1); 
  total->updctrl("WekaSink/wsink/mrs_string/labelNames", "garage,grunge,jazz,ojazz");
  total->updctrl("WekaSink/wsink/mrs_string/filename", "stereoFeatures.arff"); 
  
  playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", fname0);
  playbacknet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");


  total->updctrl("mrs_natural/inSamples", 1024);

  mrs_bool isEmpty;

  // cout << *total << endl;

  Collection l;
  l.read(fname0);
 
  int i,t;
  
  total->updctrl("Annotator/ann/mrs_natural/label", 0); 
  for (i=0; i < l.size(); i++) 
    {
      total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", l.entry(i));
      /* if (i==0) 
	total->updctrl("Accumulator/acc/Series/playbacknet/AudioSink/dest/mrs_bool/initAudio", true);
      */ 
      cout << "Processing " << l.entry(i) << endl;
      total->tick();
      cout << "i = " << i << endl;
      
    }
  
  Collection n;
  n.read(fname1);
  
  total->updctrl("Annotator/ann/mrs_natural/label", 1); 


  for (i=0; i < n.size(); i++)
    {
      total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", n.entry(i));
      cout << "Processing " << n.entry(i) << endl;
      total->tick();
      cout << "i=" << i << endl;
    }
  

  Collection m;
  m.read("j.mf");
  
  total->updctrl("Annotator/ann/mrs_natural/label", 2); 


  for (i=0; i < m.size(); i++)
    {
      total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", m.entry(i));
      cout << "Processing " << m.entry(i) << endl;
      total->tick();
      cout << "i=" << i << endl;
    }



  Collection w;
  w.read("oj.mf");
  
  total->updctrl("Annotator/ann/mrs_natural/label", 3); 


  for (i=0; i < w.size(); i++)
    {
      total->updctrl("Accumulator/acc/Series/playbacknet/SoundFileSource/src/mrs_string/filename", w.entry(i));
      cout << "Processing " << w.entry(i) << endl;
      total->tick();
      cout << "i=" << i << endl;
    }
  
     
      


}





void 
test_stereo2mono(string fname)
{
  MarSystemManager mng;
  
  MarSystem* playbacknet = mng.create("Series", "playbacknet");
  playbacknet->addMarSystem(mng.create("SoundFileSource", "src"));
  playbacknet->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  playbacknet->addMarSystem(mng.create("SoundFileSink", "dest"));
  
  playbacknet->updctrl("SoundFileSource/src/mrs_string/filename", fname);
  playbacknet->updctrl("SoundFileSink/dest/mrs_string/filename", "monoFromStereo.wav");
  playbacknet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");

  mrs_bool isEmpty;
  
  while (isEmpty = playbacknet->getctrl("mrs_bool/notEmpty")->toBool()) 
    {
      playbacknet->tick();
    }

}



void test_spectralSNR(string fname0, string fname1) 
{
  cout << "Testing spectral SNR" << endl;
  cout << "Original  signal: " << fname0 << endl;  
  cout << "Extracted signal: " << fname1 << endl;  
  
  
  
  
  MarSystemManager mng;


  MarSystem* total = mng.create("Series", "total");
  
  

  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updctrl("mrs_natural/nTimes", 100);
  
  

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

  
  net->updctrl("Series/branch1/SoundFileSource/src1/mrs_string/filename", 
	       fname0);
  net->updctrl("Series/branch2/SoundFileSource/src2/mrs_string/filename", 
	       fname1);

  net->updctrl("Series/branch1/PowerSpectrum/pspk1/mrs_string/spectrumType", "magnitude");
  net->updctrl("Series/branch2/PowerSpectrum/pspk2/mrs_string/spectrumType", "magnitude");
  
  /* net->updctrl("Series/branch1/PlotSink/psink1/mrs_string/filename", "p1p");
  net->updctrl("Series/branch2/PlotSink/psink2/mrs_string/filename", "p2p");

  */ 

  
  snet->addMarSystem(net);
  snet->addMarSystem(mng.create("SpectralSNR", "ssnr"));


  acc->addMarSystem(snet);
  total->addMarSystem(acc);
  // total->addMarSystem(mng.create("Mean", "mean"));
  total->addMarSystem(mng.create("PlotSink", "psink"));

	total->updctrl("mrs_natural/inSamples",	2048);
  
  
  mrs_bool isEmpty;


  total->tick();
  

  
}



void test_SOM(string collectionName) 
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
  acc->updctrl("mrs_natural/nTimes", 1200);
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
  total->linkctrl("mrs_string/filename",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/filename");  

  total->linkctrl("mrs_natural/pos",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/pos");  


  total->linkctrl("mrs_string/allfilenames",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_string/allfilenames");  

  total->linkctrl("mrs_natural/numFiles",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_natural/numFiles");  

  total->linkctrl("mrs_bool/notEmpty",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/notEmpty");  
  total->linkctrl("mrs_bool/advance",
		  "Accumulator/acc/Series/extractNet/SoundFileSource/src/mrs_bool/advance");  

  total->linkctrl("mrs_bool/memReset",
		  "Accumulator/acc/Series/extractNet/Memory/mem/mrs_bool/reset");  

  total->linkctrl("mrs_natural/label",
		  "Annotator/ann/mrs_natural/label");

  total->updctrl("mrs_natural/inSamples", 512);
  
  total->updctrl("mrs_string/filename", collectionName);

  mrs_natural l=0;

  mrs_natural trainSize = 20000;
  mrs_natural grid_width = 10;
  mrs_natural grid_height = 10;
  mrs_natural iterations = 20;
  
  MarSystem* som = mng.create("SOM", "som");
  som->updctrl("mrs_natural/inObservations", 2);
  som->updctrl("mrs_natural/inSamples", 1);
  som->updctrl("mrs_natural/grid_height", grid_height);
  som->updctrl("mrs_natural/grid_width", grid_width);
  som->updctrl("mrs_string/mode", "train");

  realvec train_data(trainSize);
  
  realvec input;
  input.create(2);
  realvec output;
  output.create(3);
  

  // init train data 

  cout << "Initializing training data" << endl;
  
  for (mrs_natural i=0; i < trainSize; i++) 
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
      
      for (mrs_natural i=0; i < trainSize; i++) 
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
  
  som->updctrl("mrs_bool/done", true);
  // predict 
  som->updctrl("mrs_string/mode", "predict");
  
  for (mrs_natural i=0; i < 100; i++) 
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

void test_Windowing()
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

		//series->updctrl("Windowing/win/mrs_natural/size", 800);
		//series->updctrl("Windowing/win/mrs_natural/zeroPadding", 200);
		//series->updctrl("Windowing/win/mrs_bool/zeroPhasing", true);

		in.create(series->getctrl("mrs_natural/inSamples")->toNatural());
		out.create(series->getctrl("mrs_natural/onSamples")->toNatural());
		in.setval(1.0);

    for (unsigned int i = 0 ; i < winname.size(); i++)
    {
        series->updctrl("Windowing/win/mrs_string/type", winname[i]);                    
        series->updctrl("PlotSink/plot/mrs_string/filename", "marsyas" + winname[i]); 
				
				MRSMSG(winname[i]);
				MRSMSG("size: " << series->getctrl("Windowing/win/mrs_natural/size")->toNatural());
				MRSMSG("zero-padding: " << series->getctrl("Windowing/win/mrs_natural/zeroPadding")->toNatural());

        series->process(in,out); 
    }
}


void 
test_weka(string fname) 
{
  MarSystemManager mng;
  
  MarSystem* net;
  net = mng.create("Series", "net");
  net->addMarSystem(mng.create("WekaSource", "wsrc"));

  
  net->updctrl("WekaSource/wsrc/mrs_string/filename", fname);
  net->updctrl("WekaSource/wsrc/mrs_string/obsToExtract", "1,2,3");
  net->updctrl("mrs_natural/inSamples", 1);
  
  cout << "ready to process" << endl;
  
  net->tick();
  net->tick();
  net->tick();
  
  cout << net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>() << endl;
  
  
}




void 
test_updctrl(string fname) 
{
  MarSystemManager mng;  
  
  MarSystem* pnet_;
  
  pnet_ = mng.create("Series", "pnet_");
  pnet_->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet_->addMarSystem(mng.create("Gain", "gain"));
  pnet_->addMarSystem(mng.create("AudioSink", "dest"));
  
  MarControlPtr filePtr = pnet_->getctrl("SoundFileSource/src/mrs_string/filename");
  
  cout << filePtr->hasState() << endl;
  
  pnet_->updctrl(filePtr, fname);
  // pnet_->updctrl("SoundFileSource/src/mrs_string/filename", fname);
    
  cout << *pnet_ << endl;  
}

void 
test_duplex()
{
  cout << "Testing duplex audio input and output" << endl;
  MarSystemManager mng;  
  
  MarSystem* dnet;
  dnet = mng.create("Series", "dnet");

  dnet->addMarSystem(mng.create("AudioSource", "src"));
  dnet->addMarSystem(mng.create("Gain", "gain"));
  dnet->addMarSystem(mng.create("AudioSink", "dest"));

  dnet->updctrl("AudioSource/src/mrs_natural/nChannels", 2);
  dnet->updctrl("mrs_natural/inSamples", 1024);
  dnet->updctrl("mrs_real/israte", 44100.0);
  
  dnet->updctrl("AudioSource/src/mrs_bool/initAudio", true);
  dnet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  cout << *dnet << endl;
  
  while(1) 
    {
      dnet->tick();
    } 
}

// Pluck(0,100,1.0,0.5,"TestPluckedRich0_100hz.wav");
//Pluck Karplus Strong Model Kastro.cpp output to wavfile
void 
Pluck(mrs_real pos, mrs_real fre, mrs_real loz, mrs_real stret, string name)
{
  MarSystemManager mng;
  MarSystem* series = mng.create("Series", "series");
  
  series->addMarSystem(mng.create("Plucked", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));
  series->update();
 
	series->updctrl("Gain/gain/mrs_real/gain", 1.0);
  series->updctrl("SoundFileSink/dest/mrs_natural/nChannels", 
  series->getctrl("Plucked/src/mrs_natural/nChannels"));
  series->updctrl("mrs_real/israte", 
  series->getctrl("Plucked/src/mrs_real/osrate"));
  series->updctrl("SoundFileSink/dest/mrs_string/filename",name);

  series->updctrl("Plucked/src/mrs_real/frequency",fre);
	series->updctrl("Plucked/src/mrs_real/pluckpos",pos);
 // series->updctrl("Plucked/src/mrs_real/loss",loz);
 //series->updctrl("Plucked/src/mrs_real/stretch",stret);
  
  series->updctrl("mrs_natural/inSamples", 512);
  series->update();

  cout << (*series) << endl;

	realvec in(series->getctrl("mrs_natural/inObservations")->toNatural(), 
	     series->getctrl("mrs_natural/inSamples")->toNatural());
  realvec out(series->getctrl("mrs_natural/onObservations")->toNatural(), 
	      series->getctrl("mrs_natural/onSamples")->toNatural());

  mrs_natural t=0;
  
	for (t = 0; t < 400; t++)
	{
		series->process(in,out);
		t++;
	}	      
 
  //while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
  //{
  //  series->tick();
  //}
}

// PluckLive(0,100,1.0,0.5);
//Pluck Karplus Strong Model Plucked.cpp outputs to DAC
void PluckLive(mrs_real pos, mrs_real fre, mrs_real loz, mrs_real stret)
{
 

  MarSystemManager mng;
  
  MarSystem* series = mng.create("Series", "series");
  
  
  series->addMarSystem(mng.create("Plucked", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("AudioSink", "dest"));
 

  series->update();
  
  

  
  series->updctrl("Gain/gain/mrs_real/gain", 1.0);

  series->updctrl("AudioSink/dest/mrs_natural/nChannels", 
  series->getctrl("Plucked/src/mrs_natural/nChannels"));
series->updctrl("AudioSink/dest/mrs_real/israte", 
series->getctrl("Plucked/src/mrs_real/osrate"));



  series->updctrl("Plucked/src/mrs_real/frequency",fre);
 series->updctrl("Plucked/src/mrs_real/pluckpos",pos);
 //series->updctrl("Plucked/src/mrs_real/loss",loz);
 //series->updctrl("Plucked/src/mrs_real/stretch",stret);
  
  series->updctrl("mrs_natural/inSamples", 512);
  series->update();
  


  
  cout << (*series) << endl;


 realvec in(series->getctrl("mrs_natural/inObservations")->toNatural(), 
	     series->getctrl("mrs_natural/inSamples")->toNatural());
  realvec out(series->getctrl("mrs_natural/onObservations")->toNatural(), 
	      series->getctrl("mrs_natural/onSamples")->toNatural());

  mrs_natural t=0;
  
 
	 for (t = 0; t < 400; t++)
    {
	 series->process(in,out);
	 t++;
		
	}	      

 


  
  //while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
  //{
  //  series->tick();
  //}
  
  
}


void test_normMaxMin()
{
    MarSystemManager mng;
    // MarSystem * nrm = mng.create("NormMaxMin", "nrm");
    MarSystem* nrm = mng.create("Normalize", "nrm");

    realvec in;
    realvec out;



    in.create((mrs_natural)4,(mrs_natural)3);
    out.create((mrs_natural)4,(mrs_natural)3);

    nrm->updctrl("mrs_natural/inSamples", (mrs_natural)3);
    nrm->updctrl("mrs_natural/inObservations", (mrs_natural)4);



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
tempotest_sfplay(string sfName)
{
    cout << "Playing " << sfName << endl; 

    MarSystemManager mng;

    // Create a series Composite 
    MarSystem* series = mng.create("Series", "series");
    series->addMarSystem(mng.create("SoundFileSource", "src"));
    series->addMarSystem(mng.create("AudioSink", "dest"));

    // only update controls from Composite level 
    series->updctrl("mrs_natural/inSamples", 128);
    series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);

    while (series->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
        series->tick();

    delete series;
}

 

void 
test_tempo(string fname, mrs_natural tempo, mrs_natural rank)
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


    for (mrs_natural i=0; i < count; i++)
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








    for (mrs_natural ti = 0; ti < 11; ti++)
        for (mrs_natural si = 0; si < 11; si++)
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

        tempotest_sfplay(retrievedFiles[0]);
    }

    return;
}





void
test_pitch(string sfName) 
{

  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
	pnet->addMarSystem(mng.create("ShiftInput", "sfi"));
  pnet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->addMarSystem(mng.create("PitchPraat", "pitch")); 
  // pnet->addMarSystem(mng.create("PitchSACF", "pitch")); 
  pnet->addMarSystem(mng.create("RealvecSink", "rvSink")); 

  mrs_real lowPitch = 36;
  mrs_real highPitch = 79;
  mrs_real lowFreq = pitch2hertz(lowPitch);
  mrs_real highFreq = pitch2hertz(highPitch);

  mrs_natural lowSamples = 
     hertz2samples(highFreq, pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
  mrs_natural highSamples = 
     hertz2samples(lowFreq, pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
 
  pnet->updctrl("PitchPraat/pitch/mrs_natural/lowSamples", lowSamples);
  pnet->updctrl("PitchPraat/pitch/mrs_natural/highSamples", highSamples);
  
  //  The window should be just long
  //  enough to contain three periods (for pitch detection) 
  //  of MinimumPitch. E.g. if MinimumPitch is 75 Hz, the window length
  //  is 40 ms and padded with zeros to reach a power of two.
  mrs_real windowSize = 3/lowPitch*pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal();
  pnet->updctrl("mrs_natural/inSamples", 512);
	// pnet->updctrl("ShiftInput/sfi/mrs_natural/Decimation", 256);
	pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize", powerOfTwo(windowSize));
	//pnet->updctrl("ShiftInput/sfi/mrs_natural/WindowSize", 1024);

  while (pnet->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->toBool())
   pnet->tick();

	realvec data = pnet->getctrl("RealvecSink/rvSink/mrs_realvec/data")->toVec();
   for (mrs_natural i=1; i<data.getSize();i+=2)
	   data(i) = samples2hertz(data(i), pnet->getctrl("SoundFileSource/src/mrs_real/osrate")->toReal());
   
   pnet->updctrl("RealvecSink/rvSink/mrs_bool/done", true); 
	
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
test_confidence(string sfName) 
{
 MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("Confidence", "confidence"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));

  pnet->linkControl("SoundFileSource/src/mrs_string/currentlyPlaying", "Confidence/confidence/mrs_string/fileName");

  pnet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  
  pnet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
  pnet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  
  mrs_bool isEmpty;
  //cout << *pnet << endl;
  while (isEmpty = pnet->getctrl("mrs_bool/notEmpty")->toBool()) 
    {
      //cout << "pos " << pnet->getctrl("mrs_natural/pos")->to<mrs_natural>() << endl;
      cout << pnet->getctrl("SoundFileSource/src/mrs_string/currentlyPlaying")->toString() << endl;
      cout << pnet->getctrl("Confidence/confidence/mrs_string/fileName")->toString() << endl;
      
      pnet->tick();
      
      //test if setting "mrs_natural/pos" to 0 for rewinding is working
      //if(pnet->getctrl("mrs_natural/pos")->to<mrs_natural>() > 100000)
      //	pnet->updctrl("mrs_natural/pos", 0);
    }
  cout << "tick " << isEmpty << endl;
  delete pnet;
}

void
test_realvecCtrl(string sfName) 
{
 MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));


  pnet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  
  pnet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
  pnet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  
  while ( pnet->getctrl("mrs_bool/notEmpty")->toBool()) 
    {
      
      pnet->tick();
      
    }
   delete pnet;
}

void
test_power(string sfName) 
{
	MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");

  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->addMarSystem(mng.create("ShiftInput", "si"));
  pnet->addMarSystem(mng.create("Power", "pw"));
  pnet->addMarSystem(mng.create("RealvecSink", "rvSink")); 

  pnet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  pnet->updctrl("ShiftInput/si/mrs_natural/WindowSize", 2048);

  pnet->linkControl("mrs_bool/notEmpty", "SoundFileSource/src/mrs_bool/notEmpty");
  pnet->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
  
//  cout << *pnet;
  while ( pnet->getctrl("mrs_bool/notEmpty")->toBool()) 
    {
      cout << pnet->getctrl("mrs_natural/pos")->toNatural();
      pnet->tick();
      
    }
  realvec data = pnet->getctrl("RealvecSink/rvSink/mrs_realvec/data")->toVec();

  // dB conversion
  for (mrs_natural i=0 ; i<data.getSize() ; i++)
	  data(i) = 20*log10(data(i));

  cout << data;
 
  delete pnet;
}

void
test_shredder(string sfName) 
{
 MarSystemManager mng;

  MarSystem* pnet = mng.create("Series", "pnet");
  MarSystem* acc = mng.create("Accumulator", "acc");
  MarSystem* shred = mng.create("Shredder", "shred");

  acc->addMarSystem(mng.create("SoundFileSource", "src"));
	pnet->addMarSystem(acc); 
  shred->addMarSystem(mng.create("AudioSink", "dest"));
	pnet->addMarSystem(shred);

  pnet->updctrl("Accumulator/acc/SoundFileSource/src/mrs_string/filename", sfName);
  pnet->updctrl("Shredder/shred/AudioSink/dest/mrs_bool/initAudio", true);
  
  pnet->linkControl("mrs_bool/notEmpty", "Accumulator/acc/SoundFileSource/src/mrs_bool/notEmpty");
  pnet->linkControl("mrs_natural/pos", "Accumulator/acc/SoundFileSource/src/mrs_natural/pos");
  
	pnet->updctrl("Accumulator/acc/mrs_natural/nTimes", 10);
	pnet->updctrl("Shredder/shred/mrs_natural/nTimes", 10);


  while (pnet->getctrl("mrs_bool/notEmpty")->toBool()) 
    {      
      pnet->tick(); 
    }
  delete pnet;
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
  
  if (soundfiles.size() > 0)
    fname0 = soundfiles[0];
  if (soundfiles.size() > 1)  
    fname1 = soundfiles[1];
 
  cout << "Marsyas test name = " << testName << endl;
  cout << "fname0 = " << fname0 << endl;
  cout << "fname1 = " << fname1 << endl;
  

  if (testName == "audiodevices")
    {}
  else if (testName == "cascade") 
    test_cascade();
  else if (testName == "collection")
    test_collection(fname0);
  else if (testName == "fanoutswitch")
    test_fanoutswitch();
  else if (testName == "filter") 
    test_filter();
  else if (testName == "fft") 
    test_fft(fname0);
  else if (testName == "knn")
    test_knn();
  else if (testName == "marsystemIO")
    test_marsystemIO();
  else if (testName == "mixer")
    test_mixer(fname0, fname1);
  else if (testName == "mp3convert")
    test_mp3convert(fname0);
  else if (testName == "normMaxMin") 
    test_normMaxMin();
  else if (testName == "parallel") 
    test_parallel();
  else if (testName == "probe")
    test_probe();
  else if (testName == "vicon") 
    test_vicon(fname0);   
  else if (testName == "vibrato")
    test_vibrato(fname0);
  else if (testName == "panorama")
    test_panorama(fname0);
  else if (testName == "realvec")
    test_realvec();
  else if (testName == "rmsilence") 
    test_rmsilence(fname0);
  else if (testName == "scheduler") 
    test_scheduler(fname0);
  else if (testName == "stereoFeatures")
    test_stereoFeatures(fname0, fname1);
  else if (testName == "stereoFeaturesVisualization")
    test_stereoFeaturesVisualization(fname0);
  else if (testName == "stereoMFCC") 
    test_stereoMFCC(fname0, fname1);
  else if (testName =="stereoFeaturesMFCC") 
    test_stereoFeaturesMFCC(fname0, fname1);
  else if (testName == "stereo2mono")
    test_stereo2mono(fname0);
  else if (testName == "spectralSNR")
    test_spectralSNR(fname0, fname1);
  else if (testName == "SOM") 
    test_SOM("music.mf");
  else if (testName == "tempo") 
    test_tempo(fname0, 120, 1);
  else if (testName == "MATLABengine")
    test_MATLABengine();
  else if (testName == "LPC_LSP")
    test_LPC_LSP(fname0);
  else if (testName == "MarControls")
    test_MarControls(fname0);
  else if (testName == "Windowing")
    test_Windowing();
  else if (testName == "updctrl") 
    test_updctrl(fname0);
  else if (testName == "weka")
    test_weka(fname0);
  else if (testName == "duplex") 
    test_duplex();
  else if (testName == "simpleSFPlay") 
    test_simpleSFPlay(fname0);
  else if (testName == "getControls") 
    test_getControls(fname0);
  else if (testName == "mono2stereo")
    test_mono2stereo(fname0);
 else if (testName == "pitch")
   test_pitch(fname0);
  else if (testName == "confidence")
    test_confidence(fname0);
  else if (testName == "shredder")
    test_shredder(fname0);
  else if (testName == "realvecCtrl")
    test_realvecCtrl(fname0);
   else if (testName == "power")
    test_power(fname0);
  else if (testName == "drumclassify")
    drumClassify(fname0);
else 
    {
      cout << "Unsupported test " << endl;
      printHelp(progName);
    }
  
}
