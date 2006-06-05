


#include <stdio.h>

#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h" 
#include "Conversions.h"

#include <string> 
#include <iostream> 
using namespace std;

CommandLineOptions cmd_options;



int helpopt;
int usageopt;
int wopt = 2 * MRS_DEFAULT_SLICE_NSAMPLES;
int hopt = 2 * MRS_DEFAULT_SLICE_NSAMPLES;
real lpopt = 36.0;
real upopt = 128.0;
int plopt = 0;
float topt = 0.2f;







void 
printUsage(string progName)
{
  MRSDIAG("LPCExample.cpp - printUsage");
  cerr << "Usage : " << progName << "[-c collection] [-w windowSize] [-p hopSize] [-l lowerPitch] [-u upperPitch] [-t threshold] -p file1 file2 file3" << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format" << endl;
  exit(1);
}

void 
printHelp(string progName)
{
  MRSDIAG("LPCExample.cpp - printHelp");
  cerr << "LPCExample, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Extracts pitch from the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "[-c collection] [-w windowSize] [-p hopSize] [-l lowerPitch] [-u upperPitch] [-o order]file1 file2 file3" << endl;
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
  //cerr << "-o --order : order " << endl;
  exit(1);
}




void LPCTestSoundFile(string sfName,natural winSize, natural hoSize, 
		      real lowFrek, real highFrek)
{
 
  MarSystemManager mng;
  //LPC network
  natural lpcOrder = 20;
  // cout<<"hopeSize" <<hoSize <<endl;
  MarSystem* input = mng.create("Series", "input");
  input->addMarSystem(mng.create("SoundFileSource","src"));
  input->updctrl("SoundFileSource/src/string/filename", sfName);
  input->updctrl("SoundFileSource/src/natural/inSamples", hoSize);


  input->addMarSystem(mng.create("ShiftInput", "si"));

  input->updctrl("ShiftInput/si/natural/Decimation", hoSize);
  input->updctrl("ShiftInput/si/natural/WindowSize", winSize);

  input->addMarSystem(mng.create("LPC", "lpc"));
  input->updctrl("LPC/lpc/natural/order",lpcOrder);
  //need to this so array outbound error doesnt occur in LPC.cpp
  //max hopSize = inSamples-lpcOrder
  
  input->updctrl("LPC/lpc/natural/hopSize",hoSize);
  input->updctrl("LPC/lpc/real/minPitchRes",0.1);
  input->updctrl("LPC/lpc/real/lowFreq",(real)lowFrek);
  input->updctrl("LPC/lpc/real/highFreq",(real)highFrek);

  input ->addMarSystem(mng.create("LPCResyn", "lpcResyn"));
  input->updctrl("LPCResyn/lpcResyn/natural/order",lpcOrder);
  input->updctrl("LPCResyn/lpcResyn/natural/onSamples",input->getctrl("LPC/lpc/natural/hopSize").toNatural());
  
  

  input->addMarSystem(mng.create("ShiftOutput", "so"));
  
  input->updctrl("ShiftOutput/so/natural/WindowSize", winSize);      
  input->updctrl("ShiftOutput/so/natural/Decimation", hoSize);


  input->addMarSystem(mng.create("AudioSink","sink"));
  // input->updctrl("AudioSink/sink/natural/inSamples",input->getctrl("LPC/lpc/natural/hopSize").toNatural());
  input->updctrl("AudioSink/sink/natural/inSamples", hoSize);
  
  //input->updctrl("SoundFileSink/sink/string/filename", "LPCResynthed.wav");

  //cout << (*input) <<endl;
  while(input->getctrl("SoundFileSource/src/bool/notEmpty").toBool()){
   input->tick();
  }


}

void LPCTestLive(natural winSize, natural hoSize,real lowFrek, real highFrek)
{
 
  MarSystemManager mng;
  //LPC network
  natural lpcOrder=7;
  cout <<"winsize: "<<winSize<<endl;
  MarSystem* input = mng.create("Series", "input");
  input->addMarSystem(mng.create("AudioSource","src"));
  input->updctrl("AudioSource/src/natural/inSamples", winSize);  
  input->addMarSystem(mng.create("Gain","g"));
  input->updctrl("Gain/g/real/gain", 3.0);

  input->addMarSystem(mng.create("LPC", "lpc"));
  input->updctrl("LPC/lpc/natural/order",lpcOrder);
  //need to this so array outbound error doesnt occur in LPC.cpp
  //max hopSize = inSamples-lpcOrder
  input->updctrl("LPC/lpc/natural/hopSize",hoSize);
  //input->updctrl("LPC/lpc/natural/inSamples",winSize);
  
  input->updctrl("LPC/lpc/real/minPitchRes",0.008);
  input->updctrl("LPC/lpc/real/lowFreq",200.0);
  input->updctrl("LPC/lpc/real/highFreq",2000.0);

  input ->addMarSystem(mng.create("LPCResyn", "lpcResyn"));
  input->updctrl("LPCResyn/lpcResyn/natural/order",lpcOrder);
  input->updctrl("LPCResyn/lpcResyn/natural/onSamples",input->getctrl("LPC/lpc/natural/hopSize").toNatural());

  input->addMarSystem(mng.create("AudioSink","sink"));
  input->updctrl("AudioSink/sink/natural/inSamples",input->getctrl("LPC/lpc/natural/hopSize").toNatural());
  

  //cout << (*input) <<endl;
  while(1){
   input->tick();
  }


}




void 
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addNaturalOption("winSize", "w", MRS_DEFAULT_SLICE_NSAMPLES);
  cmd_options.addNaturalOption("hopSize", "p", MRS_DEFAULT_SLICE_NSAMPLES-20);
  cmd_options.addRealOption("lowerPitch", "l", 200.0);
  cmd_options.addRealOption("upperPitch", "u", 3000.0);
  cmd_options.addBoolOption("playback", "i", false);
  //cmd_options.addNaturalOption("order", "o", 7);
}


void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  wopt = cmd_options.getNaturalOption("winSize");
  hopt = cmd_options.getNaturalOption("hopSize");
  lpopt = cmd_options.getRealOption("lowerPitch");
  upopt = cmd_options.getRealOption("upperPitch");
  plopt = cmd_options.getBoolOption("playback");
  //ordopt  = cmd_options.getNaturalOption("order");
}


int
main(int argc, const char **argv)
{
  int i = 0;

  MRSDIAG("LPCExample.cpp - main");



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

  cout << "LPCExample windowSize = " << wopt << endl;
  cout << "LPCExample hopSize = " << hopt << endl;
  cout << "LPCExample lowerPitch = " << lpopt << endl;
  cout << "LPCExample upperPitch = " << upopt << endl;
  //cout << "LPCExample order  = " << ordopt << endl;
  cout << "LPCExample playback   = " << plopt << endl;

  //LPCTestLive(wopt, hopt, lpopt, upopt);
  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) 
    {
      string sfname = *sfi;
     
      LPCTestSoundFile(sfname,wopt, hopt, lpopt, upopt);
     
      i++;
    }
  exit(1);
  


}

      
