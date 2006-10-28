


#include <cstdio>

#include "Collection.h"
#include "MarSystemManager.h"

#include "CommandLineOptions.h" 
#include "Conversions.h"

#include <string>

using namespace std;
using namespace Marsyas;

CommandLineOptions cmd_options;



int helpopt;
int usageopt;
int wopt = 2 * MRS_DEFAULT_SLICE_NSAMPLES;
int hopt = 2 * MRS_DEFAULT_SLICE_NSAMPLES;
int lpopt = 36;
int upopt = 128;
int plopt = 0;
float topt = 0.2f;







void 
printUsage(string progName)
{
  MRSDIAG("pitchextract.cpp - printUsage");
  cerr << "Usage : " << progName << "[-c collection] [-w windowSize] [-s hopSize] [-l lowerPitch] [-u upperPitch] [-t threshold] -p file1 file2 file3" << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format" << endl;
  exit(1);
}

void 
printHelp(string progName)
{
  MRSDIAG("pitchextract.cpp - printHelp");
  cerr << "pitchextract, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Extracts pitch from the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "[-c collection] [-w windowSize] [-p hopSize] [-l lowerPitch] [-u upperPitch] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-c --collection : Marsyas collection of sound files " << endl;
  cerr << "-w --windowSize : windowSize " << endl;
  cerr << "-p --hopSize    : hopSize " << endl;
  cerr << "-l --lowerPitch : lowerPitch " << endl;
  cerr << "-u --upperPitch : upperPitch " << endl;
  exit(1);
}



void testLPC()
{

  MarSystemManager mng;
  
  // LPC network 
  MarSystem* input = mng.create("Series", "input");
  input->addMarSystem(mng.create("AudioSource","src1"));


  
  MarSystem* featExtract = mng.create("Fanout", "featExtract");
  featExtract->addMarSystem(mng.create("LPC", "lpc"));

  input->addMarSystem(featExtract);
  
  
  cout << (*input) << endl;
  
  int counter = 0;
  
  // Main processing loop 
  while (1)
    {
      
      input->tick();
      
    }
}



void pitchextract1(string sfName, mrs_natural winSize, mrs_natural hopSize, 
		  mrs_natural lowPitch, mrs_natural highPitch, mrs_real threshold)
{
  MRSDIAG("pitchextract.cpp - pitchextract");
  
  MarSystemManager mng;
  
  // Build the pitch extractor network 
  MarSystem* pitchExtractor = mng.create("Series", "pitchExtractor");
  // pitchExtractor->addMarSystem(mng.create("AudioSource", "src"));

  pitchExtractor->addMarSystem(mng.create("SoundFileSource", "src"));
  pitchExtractor->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  


  pitchExtractor->addMarSystem(mng.create("AutoCorrelation", "acr"));
  pitchExtractor->updctrl("AutoCorrelation/acr/mrs_real/magcompress", 0.67);
  pitchExtractor->addMarSystem(mng.create("HalfWaveRectifier", "hwr"));
  
  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(mng.create("Gain", "id1"));
  fanout->addMarSystem(mng.create("TimeStretch", "tsc"));
  
  pitchExtractor->addMarSystem(fanout);
  
  MarSystem* fanin = mng.create("Fanin", "fanin");
  fanin->addMarSystem(mng.create("Gain", "id2"));
  fanin->addMarSystem(mng.create("Negative", "nid"));
  
  pitchExtractor->addMarSystem(fanin);
  pitchExtractor->addMarSystem(mng.create("HalfWaveRectifier", "hwr"));
  pitchExtractor->addMarSystem(mng.create("Peaker", "pkr"));
  pitchExtractor->addMarSystem(mng.create("MaxArgMax", "mxr"));

  // update controls 
  pitchExtractor->updctrl("mrs_natural/inSamples", 512);
  pitchExtractor->updctrl("Fanout/fanout/TimeStretch/tsc/mrs_real/factor", 0.5);  
   // Convert pitch bounds to samples 
  cout << "lowPitch = " << lowPitch << endl;
  cout << "highPitch = " << highPitch << endl;
  
   mrs_real lowFreq = pitch2hertz(lowPitch);
   mrs_real highFreq = pitch2hertz(highPitch);
   mrs_natural lowSamples = 
     // hertz2samples(highFreq, pitchExtractor->getctrl("AudioSource/src/mrs_real/osrate")->toReal());
     hertz2samples(highFreq, pitchExtractor->getctrl("SoundFileSource/src/mrs_real/osrate").toReal());
   mrs_natural highSamples = 
     //     hertz2samples(lowFreq, pitchExtractor->getctrl("AudioSource/src/mrs_real/osrate")->toReal());
     hertz2samples(lowFreq, pitchExtractor->getctrl("SoundFileSource/src/mrs_real/osrate").toReal());
   pitchExtractor->updctrl("Peaker/pkr/mrs_real/peakSpacing", 0.00);
   pitchExtractor->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.4);
   pitchExtractor->updctrl("Peaker/pkr/mrs_natural/peakStart", lowSamples);
   pitchExtractor->updctrl("Peaker/pkr/mrs_natural/peakEnd", highSamples);
   pitchExtractor->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 1);
   
   
   
   cout << (*pitchExtractor) << endl;
   
   realvec pitchres(pitchExtractor->getctrl("mrs_natural/onObservations").toNatural(), pitchExtractor->getctrl("mrs_natural/onSamples").toNatural());
   
  
  realvec win(pitchExtractor->getctrl("mrs_natural/inObservations").toNatural(), 
	      pitchExtractor->getctrl("mrs_natural/inSamples").toNatural());
  
  
  
  mrs_real pitch;
  mrs_natural t;
  mrs_natural counter = 0;      
  mrs_natural pos = 0;
  
  /// playback network 
  MarSystem* playback = mng.create("Series", "playback");
  playback->addMarSystem(mng.create("SineSource", "ss"));
  playback->addMarSystem(mng.create("AudioSink", "dest"));
  playback->updctrl("mrs_natural/inSamples", 512);
  
  
  
  counter = 0;
  
  // Main processing loop 
  while (1)
    {
      playback->tick();
      pitchExtractor->process(win, pitchres);
      
      // pitch = samples2hertz((mrs_natural)pitchres(1), pitchExtractor->getctrl("AudioSource/src/mrs_real/osrate")->toReal());

      pitch = samples2hertz((mrs_natural)pitchres(1), pitchExtractor->getctrl("SoundFileSource/src/mrs_real/osrate").toReal());
      
      
      // cout << pitch << "---" << pitchres(0) << endl;
      // cout << "midi" << "---" << hertz2pitch(pitch) << endl; 
      if (pitchres(0) > 0.05) 
	playback->updctrl("SineSource/ss/mrs_real/frequency", pitch);
      pitch = hertz2pitch(pitch);
      
      
      
      
      
      counter++;
    }
  
  
  
  
}



void pitchextract(string sfName, mrs_natural winSize, mrs_natural hopSize, 
		  mrs_natural lowPitch, mrs_natural highPitch, 
		  bool playback, mrs_real threshold)
{
	/* 
  MRSDIAG("pitchextract.cpp - pitchextract");
  
  SoundFileSource* src = new SoundFileSource("src");
  src->updctrl("mrs_string/filename", sfName);  

  if (src == NULL) 
    {
      string errmsg = "Skipping file: " + sfName + " (problem with reading)";
      MRSWARN(errmsg);
      return;
    }
  else 
    {
      src->updctrl("mrs_natural/inSamples", winSize);
      
      // Print information about SoundFileSource
      cout << endl << (*src) << endl;       
      
      
      // Convert pitch bounds to samples 
      mrs_real lowFreq =  pitch2hertz(lowPitch);
      mrs_real highFreq = pitch2hertz(highPitch);

      mrs_natural lowSamples = hertz2samples(highFreq, src->getctrl("mrs_real/osrate")->toReal());
      mrs_natural highSamples = hertz2samples(lowFreq, src->getctrl("mrs_real/osrate")->toReal());

      realvec win(src->getctrl("mrs_natural/onObservations")->toNatural(), 
		  src->getctrl("mrs_natural/onSamples")->toNatural());
	
      
	  
      // Build the pitch extractor network 
      AutoCorrelation acr("acr");
      acr.updctrl("mrs_real/magcompress", 0.67);
      
      HalfWaveRectifier* hwr = new HalfWaveRectifier("hwr");
      
      Gain* id1 = new Gain("id1");
      id1->updctrl("mrs_real/gain", 1.0);

      TimeStretch* tsc = new TimeStretch("tsc");
      tsc->updctrl("mrs_real/factor", 0.5);

      Gain* id2 = new Gain("id2");
      id2->updctrl("mrs_real/gain", 1.0);

      Gain* id3 = new Gain("id3");
      id3->updctrl("mrs_real/gain", 1.0);

      Negative* neg = new Negative("id");
      
      Fanout* fanout = new Fanout("fanout");
      fanout->addMarSystem(id1);
      fanout->addMarSystem(tsc);

      
      Fanin* fanin = new Fanin("fanin");
      fanin->addMarSystem(id2);
      fanin->addMarSystem(neg);


      Peaker* pkr = new Peaker("pkr");
      pkr->updctrl("mrs_real/peakSpacing", 0.1);
      pkr->updctrl("mrs_real/peakStrength", 0.5);
      pkr->updctrl("mrs_natural/peakStart", lowSamples);
      pkr->updctrl("mrs_natural/peakEnd", highSamples);

      
      MaxArgMax* mxr = new MaxArgMax("mxr");
      mxr->updctrl("mrs_natural/nMaximums", 1);
      
      
      Series* pitchExtractor = new Series("pitchExtractor");
      pitchExtractor->addMarSystem(&acr);
      pitchExtractor->addMarSystem(hwr); 
      pitchExtractor->addMarSystem(fanout);
      pitchExtractor->addMarSystem(fanin);
      pitchExtractor->addMarSystem(hwr);
      pitchExtractor->addMarSystem(pkr);
      pitchExtractor->addMarSystem(mxr);
     
      
      pitchExtractor->updctrl("mrs_natural/inObservations", src->getctrl("mrs_natural/onObservations")->toNatural());
      pitchExtractor->updctrl("mrs_natural/inSamples", src->getctrl("mrs_natural/inSamples")->toNatural());
      pitchExtractor->updctrl("mrs_real/israte", src->getctrl("mrs_real/israte")->toReal());

      cout << (*pitchExtractor) << endl;

      
      realvec pitchres(pitchExtractor->getctrl("mrs_natural/onObservations")->toNatural(), pitchExtractor->getctrl("mrs_natural/onSamples")->toNatural());
      


      mrs_real pitch;
      mrs_natural t;
      mrs_natural counter = 0;      
      mrs_natural pos = 0;
      
      // mrs_natural resSize = src->getHops(hopSize, winSize);
      // mrs_natural resSize = 5000;
      mrs_real size = src->getctrl("mrs_natural/size")->toNatural();
      mrs_natural resSize = (mrs_natural) ((mrs_real)size / hopSize) + 1;
      cout << "size = " << size << endl;
      cout << "hopSize = " << hopSize << endl;
      cout << "resSize = " << resSize << endl;
      
      

      counter = 0;
      
      // Main processing loop 
      while (1)
	{
	  pos += hopSize;
	  src->updctrl("mrs_natural/pos", pos);
	  src->process(win,win);
	  pitchExtractor->process(win, pitchres);
	  pitch = samples2hertz((mrs_natural)pitchres(1), src->getctrl("mrs_real/osrate")->toReal());
	  pitch = hertz2pitch(pitch);
	  counter++;
	}
      
      
      
    }
 */  
}

  




void 
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addNaturalOption("winSize", "w", 2 * MRS_DEFAULT_SLICE_NSAMPLES);
  cmd_options.addNaturalOption("hopSize", "h", 2 * MRS_DEFAULT_SLICE_NSAMPLES);
  cmd_options.addNaturalOption("lowerPitch", "l", 36);
  cmd_options.addNaturalOption("upperPitch", "u", 79);
  cmd_options.addBoolOption("playback", "p", false);
  cmd_options.addRealOption("threshold", "t", 0.2);
}


void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  wopt = cmd_options.getNaturalOption("winSize");
  hopt = cmd_options.getNaturalOption("hopSize");
  lpopt = cmd_options.getNaturalOption("lowerPitch");
  upopt = cmd_options.getNaturalOption("upperPitch");
  plopt = cmd_options.getBoolOption("playback");
  topt  = cmd_options.getRealOption("threshold");
}


int
main(int argc, const char **argv)
{
  int i = 0;

  MRSDIAG("pitchextract.cpp - main");



  string progName = argv[0];  
  progName = progName.erase(0,3);


  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  
  
  if (helpopt) 
    printHelp(progName);
  
  if (usageopt)
    printUsage(progName);

  cout << "PitchExtract windowSize = " << wopt << endl;
  cout << "PitchExtract hopSize = " << hopt << endl;
  cout << "PitchExtract lowerPitch = " << lpopt << endl;
  cout << "PitchExtract upperPitch = " << upopt << endl;
  cout << "PitchExtract threshold  = " << topt << endl;
  cout << "PitchExtract playback   = " << plopt << endl;


  testLPC();
  exit(1);
  


  // pitchextract1("patata", wopt, hopt, lpopt, upopt, topt);

  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
    {
      string sfname = *sfi;
      // pitchextract(sfname, wopt, hopt, lpopt, upopt, plopt, topt);
      pitchextract1(sfname, wopt, hopt, lpopt, upopt, topt);
      i++;
    }
  exit(1);
  


}

	
