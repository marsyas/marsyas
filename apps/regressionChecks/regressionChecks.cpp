#include "common-reg.h"

/*
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
*/

#define EMPTYSTRING "MARSYAS_EMPTY"

using namespace std;
using namespace Marsyas;

/*
CommandLineOptions cmd_options;
string testName;
*/
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
  cerr << "audiodevices    : test audio devices " << endl;
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
  
/*
  if (testName == "audiodevices")
    test_audiodevices();
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
*/
  
}
