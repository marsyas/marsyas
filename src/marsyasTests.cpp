// Various tests of MarSystems. This is the place 
// to put code that is still at testing/debugging phase. 


#include <stdio.h>
#include "MarSystemManager.h" 
#include "Cascade.h" 
#include "Parallel.h" 
#include "CommandLineOptions.h"
#include "FileName.h"



#include <string> 
using namespace std;

#define EMPTYSTRING "MARSYAS_EMPTY"

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
  cerr << "fanoutswitch    : test disabling fanout branches " << endl;
  cerr << "filter          : test filter MarSystem " << endl;
  cerr << "fft             : test fft analysis/resynthesis " << endl;
  cerr << "knn             : test K-NearestNeighbor classifier " << endl;
  cerr << "parallel        : test Parallel composite " << endl;
  cerr << "rmsilence  	   : test removing silences " << endl;
  cerr << "scheduler       : test scheduler " << endl;
  cerr << "schedulerExpr   : test scheduler with expressions " << endl;
  cerr << "vicon           : test processing of vicon motion capture data" << endl;
  cerr << "mixer           : test fanout for mixing " << endl;
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
  series1->updctrl("natural/inSamples", 256);
  series1->updctrl("SoundFileSource/src/string/filename", ipName);
  series1->updctrl("SoundFileSink/dest/string/filename", opName);
  
  // post events to the scheduler using updctrl(..)
  series1->updctrl("Gain/gain/real/gain", 1.0);
  series1->updctrl("1s", Repeat("2s", 3), new EvValUpd(series1,"Gain/gain/real/gain", 0.0));
  series1->updctrl("2s", Repeat("2s", 3), new EvValUpd(series1,"Gain/gain/real/gain", 1.0));
  
  for (int i=0; i<10000; i++) {
    series1->tick();
  }
  
  // Composite deletes the added MarSystems
  // so you must not delete them
  delete series1;
  
  
}



void 
test_schedulerExpr()
{
  
  MarSystemManager mng;
  
  // Create a series Composite
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SineSource", "src"));
  series->addMarSystem(mng.create("AudioSink", "dest"));
  
  // only update controls from Composite level
  series->updctrl("natural/inSamples", 256);
  
  series->updctrl("0s",Repeat("0.15s"), new EvExpUpd(series,
 "SineSource/src/real/frequency", "(120+3000*(Math.rand() / Math.RAND_MAX))"));
  
  for (int i=0; i<10000; i++) 
    {
      series->tick();
    }
  
  // Composite deletes the added MarSystems
  // so you must not delete them
  delete series;
}





void 
test_fanoutswitch()
{
  cout << "Testing fanout switch" << endl;
  
  MarSystemManager mng;
  
  MarSystem* pnet = mng.create("Series", "src");
  MarSystem* src = mng.create("SoundFileSource", "src");
  src->updctrl("string/filename", "/home/gtzan/data/sound/music_speech/music/gravity.au");

  pnet->addMarSystem(src);
  pnet->addMarSystem(mng.create("PlotSink", "psink1"));  
  pnet->updctrl("PlotSink/psink1/string/outputFilename", "in");
  

  MarSystem* mix = mng.create("Fanout", "mix");
  MarSystem* g1 = mng.create("Gain", "g1");
  MarSystem* g2 = mng.create("Gain", "g2");
  MarSystem* g3 = mng.create("Gain", "g3");
  MarSystem* g4 = mng.create("Gain", "g4");
  
  
  g1->updctrl("real/gain", 1.5);
  g2->updctrl("real/gain", 2.5);
  g3->updctrl("real/gain", 3.0);
  g4->updctrl("real/gain", 4.0);
  
  mix->addMarSystem(g1);
  mix->addMarSystem(g2);
  mix->addMarSystem(g3);
  mix->addMarSystem(g4);
  
  pnet->addMarSystem(mix);
  pnet->addMarSystem(mng.create("PlotSink", "psink2"));
  
  // Disable subset of Fanout branches 
  pnet->updctrl("Fanout/mix/natural/disable", 2);
  pnet->updctrl("Fanout/mix/natural/disable", 0);
  
  // tick to check the result 
  // PlotSinks are used for output 
  pnet->tick();
}


void 
test_rmsilence(string sfName)
{
  cout << "Removing silences from: " << sfName << endl;
  MarSystemManager mng;

  MarSystem* rmnet = mng.create("Series", "rmnet");
  
  MarSystem* srm = mng.create("SilenceRemove", "srm");
  MarSystem* src = mng.create("SoundFileSource", "src");
  src->updctrl("string/filename", sfName);
  srm->addMarSystem(src);
  
  

  rmnet->addMarSystem(srm);
  rmnet->addMarSystem(mng.create("SoundFileSink", "dest"));

  FileName fname(sfName);  
  rmnet->updctrl("SoundFileSink/dest/string/filename", "srm" + fname.name() + ".wav");

  
  

  while (rmnet->getctrl("SilenceRemove/srm/SoundFileSource/src/bool/notEmpty").toBool())
    {
      rmnet->tick();
    }
  
  cout << "Finished removing silences. Output is " << "srm" + fname.name() + ".wav" << endl;
  
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

  
  pnet->updctrl("Fanout/mix/Series/branch0/SoundFileSource/src0/string/filename", sfName0);
  pnet->updctrl("Fanout/mix/Series/branch1/SoundFileSource/src1/string/filename", sfName1);
  pnet->updctrl("Fanout/mix/Series/branch0/Gain/gain0/real/gain", 0.5);
  pnet->updctrl("Fanout/mix/Series/branch1/Gain/gain1/real/gain", 0.5);

  while(1)
    {
      pnet->tick();
    }
  
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
  series->addMarSystem(mng.create("Gain", "g1"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));

  /* MarSystem* innerSeries = mng.create("Series", "inner");
  innerSeries->addMarSystem(mng.create("Spectrum", "inner_spk"));
  innerSeries->addMarSystem(mng.create("InvSpectrum", "inner_ispk"));  
  innerSeries->addMarSystem(mng.create("Gain", "g2"));  
  series->addMarSystem(innerSeries);
  */ 
  
  series->updctrl("SoundFileSource/src/string/filename", sfName);
  series->updctrl("SoundFileSink/dest/string/filename",  "sftransformOutput.au");

  real g;
  natural i =0;
  
  while (series->getctrl("SoundFileSource/src/bool/notEmpty").toBool())
    {
      g = series->getctrl("Gain/g1/real/gain").toReal();
      g+=0.01;
      series->updctrl("Gain/g1/real/gain",g);
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
  in.create(natural(10), natural(10));
  for (natural i = 0; i < in.getRows(); i++){
    for (natural j = 0; j < in.getCols(); j++){
      in(i,j) = i*j;
    }
  }
  
  realvec out;
  out.create(in.getRows(),in.getCols());
  
  Gain* g0 = new Gain("g0");
  g0->setctrl("natural/inObservations", natural(3));
  g0->setctrl("natural/inSamples", in.getCols());
  g0->setctrl("real/gain", 3.0f);
  
  Gain* g1 = new Gain("g1");
  g1->setctrl("natural/inObservations", natural(2));
  g1->setctrl("real/gain", 2.0f);
  
  Gain* g2 = new Gain("g2");
  g2->setctrl("natural/inObservations", natural(5));
  g2->setctrl("real/gain", 5.0f);
  
  parallel->addMarSystem(g0);
  parallel->addMarSystem(g1);
  parallel->addMarSystem(g2);
  
  parallel->process(in, out);
  
  cout << out << endl;
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
  f0->setctrl("realvec/ncoeffs", a);
  f0->setctrl("realvec/dcoeffs", b);
  f0->setctrl("natural/inSamples", natural(5));
  f0->setctrl("natural/inObservations", natural(1));
  f0->setctrl("real/israte", 44100.0f);
  
  Filter* f1 = new Filter("f1");
  a(0) = 1.0f;
  a(1) = 1.0f;
  a(2) = 0.0f;
  b(0) = 1.0f;
  b(1) = 0.0f;
  b(2) = 0.0f;
  f1->setctrl("realvec/ncoeffs", a);
  f1->setctrl("realvec/dcoeffs", b);
  
  cascade->addMarSystem(f0);
  cascade->addMarSystem(f1);
  
  realvec in, out;
  in.create(natural(1),natural(5));
  in(0,0) = 1.0f;
  out.create(natural(2),natural(5));
  
  cascade->process(in, out);
  
  cout << out << endl;
  
}



void 
test_knn()
{
  MarSystemManager mng;
  MarSystem *knn = mng.create("KNNClassifier", "knn");

 
  // ---- TEST TRAIN ---------------------
  
  knn->updctrl("string/mode", "train");
  
  natural inS = 9;
  natural inO = 3;
  
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
  
  knn->updctrl("natural/inSamples", inS);
  knn->updctrl("natural/inObservations", inO);
  
  realvec output(knn->getctrl("natural/onObservations").toNatural(), knn->getctrl("natural/onSamples").toNatural());
 

  cout << "INPUT: " << input << endl;
  
  knn->process(input, output);
  cout << "TEST: " << output << endl;


  // IMPORTANT updcontrol done and then process to indicate to KNN to finish  
  knn->updctrl("bool/done", true);
  knn->tick();
  
  // --------------- TEST PREDICT -----------------
  knn->updctrl("string/mode", "predict");
  knn->updctrl("natural/k", 3);
  knn->updctrl("natural/nLabels", 2);
  inS = 1;
  inO = 3;

  realvec input2(inO, inS); 
  
  input2(0,0) = 3.0;
  input2(1,0) = 3.2;
  input2(2,0) = 1.0;

  knn->updctrl("natural/inSamples", inS);
  knn->updctrl("natural/inObservations", inO);
 
  realvec output2(knn->getctrl("natural/onObservations").toNatural(), knn->getctrl("natural/onSamples").toNatural());

  cout << "Predict" << endl; 
  knn->process(input2, output2);

  cout << "PREDICT: " << output2 << endl;
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
  
  f->setctrl("realvec/ncoeffs", a);
  f->setctrl("realvec/dcoeffs", b);
  f->setctrl("natural/inSamples", natural(5));
  f->setctrl("natural/inObservations", natural(2));
  f->setctrl("real/israte", 44100.0f);
  f->update();
  
  realvec in, out;
  in.create(natural(2),natural(5));
  in(0,0) = 1.0f;
  in(1,0) = 1.0f;
  out.create(natural(2),natural(5));
  
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

  f->setctrl("realvec/ncoeffs", bl);
  f->setctrl("realvec/dcoeffs", al);
  f->setctrl("natural/inSamples", natural(20));
  f->setctrl("natural/inObservations", natural(2));
  f->setctrl("real/israte", 44100.0f);
  f->update();
  
  realvec in, out;
  in.create(natural(2),natural(20));
  in(0,0) = 1.0f;
  in(1,0) = 1.0f;
  out.create(natural(2),natural(20));

  f->process(in, out);
  cout << out << endl;
  
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
  
  viconNet->updctrl("ViconFileSource/vsrc/natural/inSamples", 1);
  viconNet->updctrl("ViconFileSource/vsrc/string/filename", vfName);
  viconNet->updctrl("ViconFileSource/vsrc/real/israte", 120.0);
   

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

  realvec in(viconNet->getctrl("natural/inObservations").toNatural(), 
	     viconNet->getctrl("natural/inSamples").toNatural());
  realvec out(viconNet->getctrl("natural/onObservations").toNatural(), 
	      viconNet->getctrl("natural/onSamples").toNatural());
  
  playbacknet->updctrl("natural/inSamples", 184);


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
  

  
 
  while (viconNet->getctrl("ViconFileSource/vsrc/bool/notEmpty").toBool()) 
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
     

      // playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc1/real/frequency", fabs(out(13,0)));
      // playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc1/real/frequency", fabs(fabs(out(1.0))));

      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc2/real/frequency", fabs(out(2,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc3/real/frequency", fabs(out(3,0)));
      
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc4/real/frequency", fabs(out(7,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc5/real/frequency", fabs(out(8,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc6/real/frequency", fabs(out(9,0)));

      
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc7/real/frequency", fabs(out(10,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc8/real/frequency", fabs(out(11,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc9/real/frequency", fabs(out(12,0)));


      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc10/real/frequency", fabs(out(16,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc11/real/frequency", fabs(out(17,0)));
      playbacknet->updctrl("Fanout/sinebank/SineSource/ssrc12/real/frequency", fabs(out(18,0)));
      

      // cout << out(13,0) << endl;

      // cout << "AfterTouch       -1.0 1 " << fabs((out(1,0) / 18.0)) << endl;
      // usleep(50000);
      
      // cout << "AfterTouch       -1.0 1 " << fabs((out(1,0) / 18.0)) << endl;
      // cout << "PitchChange       0.0 1 " << fabs((out(1,0) / 18.0)) << endl;
      
      playbacknet->tick();
    }
  
      
  // cout << viconNet->getctrl("ViconFileSource/vsrc/string/markers") << endl;
  // cout << "Sample Rate: " << viconNet->getctrl("ViconFileSource/vsrc/real/israte") << endl;

 


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
  
  

  // cout << "Vicon File is: " << vfName << endl;
  

  cout << "Marsyas test name = " << testName << endl;
  
  if (testName == "vicon") 
    test_vicon(fname0);   
  else if (testName == "filter") 
    test_filter();
  else if (testName == "knn")
    test_knn();
  else if (testName == "cascade") 
    test_cascade();
  else if (testName == "parallel") 
    test_parallel();
  else if (testName == "fft") 
    test_fft(fname0);
  else if (testName == "rmsilence") 
    test_rmsilence(fname0);
  else if (testName == "fanoutswitch")
    test_fanoutswitch();
  else if (testName == "scheduler") 
    test_scheduler(fname0);
  else if (testName == "schedulerExpr") 
    test_schedulerExpr();
  else if (testName == "mixer")
    test_mixer(fname0, fname1);
  else 
    {
      cout << "Unsupported test " << endl;
      printHelp(progName);
    }
  
}
