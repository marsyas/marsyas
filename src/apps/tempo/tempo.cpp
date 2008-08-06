
#include <cstdio>
#include <cstdlib>
#include <algorithm>

#include "Collection.h"
#include "MarSystemManager.h" 
#include "CommandLineOptions.h"
#include "Esitar.h"
#include "mididevices.h"
#include <string> 
#include <fstream>
#include <iostream>
#include <iomanip> 


using namespace std;
using namespace Marsyas;


string fileName;
string pluginName;
string methodopt;
CommandLineOptions cmd_options;


int helpopt;
int usageopt;
long offset = 0;
long duration = 1000 * 44100;
long band;
mrs_natural bandopt = 0;

float start = 0.0f;
float length = 1000.0f;
float gain = 1.0f;
float repetitions = 1;



void 
printUsage(string progName)
{
  MRSDIAG("waveletplay.cpp - printUsage");
  cerr << "Usage : " << progName << " [-m method] [-g gain] [-o offset(samples)] [-d duration(samples)] [-s start(seconds)] [-l length(seconds)] [-f outputfile] [-p pluginName] [-r repetitions] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format or collections " << endl;
  exit(1);
}

void 
printHelp(string progName)
{
  MRSDIAG("waveletplay.cpp - printHelp");
  cerr << "waveletplay, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Prints information about the sound files provided as arguments " << endl;
  cerr << endl;
  cerr << "Usage : " << progName << "file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information " << endl;
  cerr << "-f --file       : output to file " << endl;
  cerr << "-m --method     : tempo induction method " << endl;
  cerr << "-g --gain       : linear volume gain " << endl;
  cerr << "-o --offset     : playback start offset in samples " << endl;
  cerr << "-d --duration   : playback duration in samples     " << endl;
  cerr << "-s --start      : playback start offest in seconds " << endl;
  cerr << "-l --length     : playback length in seconds " << endl;
  cerr << "-p --plugin     : output plugin name " << endl;
  cerr << "-r --repetitions: number of repetitions " << endl;


  cerr << "Available methods: " << endl;
  cerr << "MEDIAN_SUMBANDS" << endl;
  cerr << "MEDIAN_MULTIBANDS" << endl;
  cerr << "HISTO_SUMBANDS" << endl;
  
  exit(1);
}


// Play soundfile given by sfName, msys contains the playback 
// network of MarSystem objects 
void tempo_medianMultiBands(string sfName, string resName)
{
  MarSystemManager mng;


  // prepare network 
  MarSystem *total = mng.create("Series", "src");
  total->addMarSystem(mng.create("SoundFileSource", "src"));
  total->addMarSystem(mng.create("ShiftInput", "si"));

  total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
  total->addMarSystem(mng.create("WaveletBands", "wvbnds"));


  total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
  total->addMarSystem(mng.create("OnePole", "lpf"));
  total->addMarSystem(mng.create("Norm", "norm"));

  total->addMarSystem(mng.create("Sum", "sum"));

  total->addMarSystem(mng.create("DownSampler", "ds"));
  total->addMarSystem(mng.create("AutoCorrelation", "acr"));
  total->addMarSystem(mng.create("Peaker", "pkr"));
  total->addMarSystem(mng.create("MaxArgMax", "mxr"));
  total->addMarSystem(mng.create("PeakPeriods2BPM", "p2bpm"));



  


  
  
  // update the controls 

  mrs_real srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();

  // input filename with hopSize/winSize 
  mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
  mrs_natural hopSize = winSize / 16;
  total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  total->updctrl("mrs_natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);      
  total->updctrl("ShiftInput/si/mrs_natural/winSize", winSize);


  // wavelt filterbank envelope extraction controls 
  total->updctrl("WaveletPyramid/wvpt/mrs_bool/forward", true);
  total->updctrl("OnePole/lpf/mrs_real/alpha", 0.99f);
  mrs_natural factor = 32;
  total->updctrl("DownSampler/ds/mrs_natural/factor", factor);  
  
  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM 

  mrs_natural pkinS = total->getctrl("Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_real peakSpacing = ((mrs_natural)(srate * 60.0 / (factor *60.0)) - 
		      (mrs_natural)(srate * 60.0 / (factor*64.0))) / pkinS;
  mrs_natural peakStart = (mrs_natural)(srate * 60.0 / (factor * 180.0));
  mrs_natural peakEnd   = (mrs_natural)(srate * 60.0 / (factor * 50.0));
  total->updctrl("Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
  total->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.75);
  total->updctrl("Peaker/pkr/mrs_natural/peakStart", peakStart);
  total->updctrl("Peaker/pkr/mrs_natural/peakEnd", peakEnd);
  total->updctrl("Peaker/pkr/mrs_real/peakGain", 2.0);


  total->updctrl("PeakPeriods2BPM/p2bpm/mrs_natural/factor", factor);
  
  
  // prepare vectors for processing 
  realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
	       total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec estimate(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
		   total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  mrs_natural bin;
  mrs_natural onSamples, nChannels;
  int numPlayed =0;
  mrs_natural wc=0;
  mrs_natural samplesPlayed = 0;
  mrs_natural repeatId = 1;
  // vector of bpm estimate used to calculate median 
  vector<int> bpms;
  
  onSamples = total->getctrl("ShiftInput/si/mrs_natural/onSamples")->to<mrs_natural>();
  nChannels = total->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();

  // playback offset & duration
  offset = (mrs_natural) (start * srate * nChannels);
  duration = (mrs_natural) (length * srate * nChannels);
  
  while (repetitions * duration > samplesPlayed)
    {
      total->process(iwin, estimate);

      
      
      // convert highest peak to BPMs and add to vector 
      for (int b=0; b < 5; b++)
	{
	  // pitch = srate * 60.0 / (estimate(b,1) * factor);
	  // bin = (mrs_natural) (pitch);
	  bin = (mrs_natural)(estimate(b,1));
	  cout << "max bpm(" << b << ") = " << bin << endl;
	  bpms.push_back(bin);
	}
      numPlayed++;
      if (samplesPlayed > repeatId * duration)
	{
	  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);   
	  repeatId++;
	}
      wc ++;
      samplesPlayed += onSamples;
      // no duration specified so use all of source input 
      if (!(total->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>()) && (repeatId == 1))
	{ 
	  duration = samplesPlayed-onSamples;
	}
    }
  
  // sort bpm estimates for median filtering 
  sort(bpms.begin(), bpms.end());
  cout << "FINAL = " << bpms[bpms.size()/2] << endl; 

  // Output to file 
  ofstream oss(resName.c_str());
  oss << bpms[bpms.size()/2] << endl;
  cerr << "Played " << wc << " slices of " << onSamples << " samples" 
       << endl;
  cout << "Processed " << sfName << endl;
  cout << "Wrote " << resName << endl;
  delete total;
}





void 
tempo_new(string sfName, string resName)
{
  
  MarSystemManager mng;

  mrs_real srate = 0.0;

  
  // prepare network 
  MarSystem *total = mng.create("Series", "src");



  
  total->addMarSystem(mng.create("SoundFileSource", "src"));
  total->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  total->addMarSystem(mng.create("ShiftInput", "si"));



  
  
  total->addMarSystem(mng.create("DownSampler", "initds"));
  total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
  // implicit fanout 
  total->addMarSystem(mng.create("WaveletBands", "wvbnds"));
  total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
  total->addMarSystem(mng.create("OnePole", "lpf"));
  total->addMarSystem(mng.create("Norm", "norm"));

  // implicit fanin 
  total->addMarSystem(mng.create("Sum", "sum"));


  total->addMarSystem(mng.create("DownSampler", "ds"));
  total->addMarSystem(mng.create("AutoCorrelation", "acr"));
  // total->addMarSystem(mng.create("PlotSink", "psink1"));
  total->addMarSystem(mng.create("Peaker", "pkr"));
  // total->addMarSystem(mng.create("PlotSink", "psink2"));
  total->addMarSystem(mng.create("MaxArgMax", "mxr"));
  total->addMarSystem(mng.create("PeakPeriods2BPM", "p2bpm"));
  total->addMarSystem(mng.create("Histogram", "histo"));
  
  // total->addMarSystem(mng.create("Peaker", "pkr1"));  
  // total->addMarSystem(mng.create("PlotSink", "psink3"));
  // total->addMarSystem(mng.create("Reassign", "reassign"));
  // total->addMarSystem(mng.create("PlotSink", "psink4"));  
  total->addMarSystem(mng.create("HarmonicEnhancer", "harm"));
  // total->addMarSystem(mng.create("HarmonicEnhancer", "harm"));
  // total->addMarSystem(mng.create("PlotSink", "psink4"));
  // total->addMarSystem(mng.create("MaxArgMax", "mxr1"));



  mrs_natural ifactor = 8;;
  total->updctrl("DownSampler/initds/mrs_natural/factor", ifactor);  
  
  total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);


  srate = total->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  // srate = total->getctrl("DownSampler/initds/mrs_real/osrate")->to<mrs_real>();
  // cout << "srate = " << srate << endl;



  
  // update the controls 
  // input filename with hopSize/winSize 
  mrs_natural winSize = (mrs_natural)((srate / 22050.0) * 2 * 65536);
  mrs_natural hopSize = winSize / 16;

  // cout << "winSize = " << winSize << endl;
  // cout << "hopSize = " << hopSize << endl;
  

  offset = (mrs_natural) (start * srate); 
duration = (mrs_natural) (length * srate);
  
  // total->updctrl("PlotSink/psink1/mrs_string/filename", "acr");
  // total->updctrl("PlotSink/psink2/mrs_string/filename", "peaks");
  
  // total->updctrl("PlotSink/psink3/mrs_string/filename", "histo");
  // total->updctrl("PlotSink/psink4/mrs_string/filename", "rhisto");   
  total->updctrl("mrs_natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);      

  total->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 5);
  // total->updctrl("MaxArgMax/mxr1/mrs_natural/nMaximums", 2);  

  
  total->updctrl("ShiftInput/si/mrs_natural/winSize", winSize);



  // wavelt filterbank envelope extraction controls 
  total->updctrl("WaveletPyramid/wvpt/mrs_bool/forward", true);
  total->updctrl("OnePole/lpf/mrs_real/alpha", 0.99f);
  mrs_natural factor = 32;
  total->updctrl("DownSampler/ds/mrs_natural/factor", factor);  

  srate = total->getctrl("DownSampler/initds/mrs_real/osrate")->to<mrs_real>();  




  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM 
  mrs_natural pkinS = total->getctrl("Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_real peakSpacing = ((mrs_natural)(srate * 60.0 / (factor *60.0)) - 
		      (mrs_natural)(srate * 60.0 / (factor*62.0))) / (pkinS * 1.0);

  
  mrs_natural peakStart = (mrs_natural)(srate * 60.0 / (factor * 230.0));
  mrs_natural peakEnd   = (mrs_natural)(srate * 60.0 / (factor * 30.0));


  total->updctrl("Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
  total->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.5);
  total->updctrl("Peaker/pkr/mrs_natural/peakStart", peakStart);
  total->updctrl("Peaker/pkr/mrs_natural/peakEnd", peakEnd);
  total->updctrl("Peaker/pkr/mrs_real/peakGain", 2.0);


  /* total->updctrl("Peaker/pkr1/mrs_real/peakSpacing", 0.1);
  total->updctrl("Peaker/pkr1/mrs_real/peakStrength", 1.2);
  total->updctrl("Peaker/pkr1/mrs_natural/peakStart", 20);
  total->updctrl("Peaker/pkr1/mrs_natural/peakEnd", 180);
  */ 
  


  
  total->updctrl("Histogram/histo/mrs_natural/startBin", 0);
  total->updctrl("Histogram/histo/mrs_natural/endBin", 230);
  
  // prepare vectors for processing 
  realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
	       total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec estimate(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
		   total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  
  // mrs_natural bin;
  mrs_natural onSamples;
  
  int numPlayed =0;
  mrs_natural wc=0;
  mrs_natural samplesPlayed = 0;
  mrs_natural repeatId = 1;

  onSamples = total->getctrl("ShiftInput/si/mrs_natural/onSamples")->to<mrs_natural>();
  


  

   


  while (total->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>())
    {
      total->process(iwin, estimate);

      numPlayed++;
      if (samplesPlayed > repeatId * duration)
	  {
		  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);   
		  repeatId++;
	  }
      wc ++;
      samplesPlayed += onSamples;
    }



  // phase calculation 

  MarSystem *total1 = mng.create("Series", "total1");
  total1->addMarSystem(mng.create("SoundFileSource", "src1"));
  total1->addMarSystem(mng.create("FullWaveRectifier", "fwr1"));
  
  // implicit fanin 
  total1->addMarSystem(mng.create("Sum", "sum1"));
  total1->addMarSystem(mng.create("DownSampler", "ds1"));
  total1->updctrl("SoundFileSource/src1/mrs_string/filename", sfName);


  srate = total1->getctrl("SoundFileSource/src1/mrs_real/osrate")->to<mrs_real>();
  

  
  // update the controls 
  // input filename with hopSize/winSize 
  winSize = (mrs_natural)(srate / 22050.0) * 8 * 65536;
  
  total1->updctrl("mrs_natural/inSamples", winSize);
  total1->updctrl("SoundFileSource/src1/mrs_natural/pos", 0);      

  // wavelt filterbank envelope extraction controls 
  // total1->updctrl("OnePole/lpf1/mrs_real/alpha", 0.99f);
  factor = 4;
  total1->updctrl("DownSampler/ds1/mrs_natural/factor", factor);  



  

  
  

  realvec iwin1(total1->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
	       total1->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec estimate1(total1->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
		    total1->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  
  total1->process(iwin1, estimate1);
  

  
  mrs_real s1 = estimate(0);
  mrs_real s2 = estimate(2);
  mrs_real t1 = estimate(1);
  mrs_real t2 = estimate(3);  

  mrs_natural p1 = (mrs_natural)((int)((srate * 60.0) / (factor * t1)+0.5));
  mrs_natural p2 = (mrs_natural)((int)((srate * 60.0) / (factor * t2)+0.5));
  

  mrs_real mx = 0.0;
  mrs_natural imx = 0;
  mrs_real sum = 0.0;


  
  
  for (mrs_natural i = 0; i < p1; i++)
    {
      sum = 0.0;
      sum += estimate1(0,i);
      
      sum += estimate1(0,i+p1);
      sum += estimate1(0,i+p1-1);
      sum += estimate1(0,i+p1+1);

      sum += estimate1(0,i+2*p1);
      sum += estimate1(0,i+2*p1-1);
      sum += estimate1(0,i+2*p1+1);


      sum += estimate1(0,i+3*p1);
      sum += estimate1(0,i+3*p1-1);
      sum += estimate1(0,i+3*p1+1);
      
      if (sum > mx) 
	{
	  mx = sum;
	  imx = i;
	}
    }

  mrs_real ph1 = (imx * factor * 1.0) / srate;
  
  for (mrs_natural i = 0; i < p2; i++)
    {
      sum = 0.0;
      sum += estimate1(0,i);

      sum += estimate1(0,i+p2);
      sum += estimate1(0,i+p2-1);
      sum += estimate1(0,i+p2+1);
      
      sum += estimate1(0,i+2*p2);
      sum += estimate1(0,i+2*p2-1);
      sum += estimate1(0,i+2*p2+1);
      
      sum += estimate1(0,i+3*p2);
      sum += estimate1(0,i+3*p2-1);
      sum += estimate1(0,i+3*p2+1);
      
      
      if (sum > mx) 
	{
	  mx = sum;
	  imx = i;
	}
    }
  
  mrs_real ph2 = (imx * factor * 1.0) / srate;
  
  

  

  mrs_real st = s1 / (s1 + s2);

  
  
  ofstream os(resName.c_str());

  
  

  os << fixed << setprecision(1) 
     << t1 << "\t" 
     << t2 << "\t" 
     << setprecision(2)  
     << st << "\t"
     << setprecision(3)  
     << ph1 << "\t" 
     << ph2 << "\t"
     << endl;
  
  // cout << "Estimated tempo = " << t1 << endl;

  cout << sfName << " " << t1 << " " << s1 << endl;
  
  delete total;
  delete total1;
}



void 
tempo_histoSumBands(string sfName, string resName)
{
  MarSystemManager mng;
  mrs_natural nChannels;
  mrs_real srate = 0.0;
  
  // prepare network 
  MarSystem *total = mng.create("Series", "src");
  total->addMarSystem(mng.create("SoundFileSource", "src"));
  total->addMarSystem(mng.create("ShiftInput", "si"));
  total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
  
  // implicit fanout 
  total->addMarSystem(mng.create("WaveletBands", "wvbnds"));
  

  total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
  total->addMarSystem(mng.create("OnePole", "lpf"));
  total->addMarSystem(mng.create("Norm", "norm"));

  // implicit fanin 
  total->addMarSystem(mng.create("Sum", "sum"));


  total->addMarSystem(mng.create("DownSampler", "ds"));
  total->addMarSystem(mng.create("AutoCorrelation", "acr"));
  total->addMarSystem(mng.create("Peaker", "pkr"));
  total->addMarSystem(mng.create("MaxArgMax", "mxr"));
  total->addMarSystem(mng.create("PeakPeriods2BPM", "p2bpm"));
  
  total->addMarSystem(mng.create("Histogram", "histo"));
  total->addMarSystem(mng.create("MaxArgMax", "mxr"));
  
  // update the controls 
  // input filename with hopSize/winSize 
  mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
  mrs_natural hopSize = winSize / 8;
  total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);

  nChannels = total->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();
  srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();

  offset = (mrs_natural) (start * srate * nChannels);
  duration = (mrs_natural) (length * srate * nChannels);
  

  total->updctrl("mrs_natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);      
  total->updctrl("ShiftInput/si/mrs_natural/winSize", winSize);


  // wavelt filterbank envelope extraction controls 
  total->updctrl("WaveletPyramid/wvpt/mrs_bool/forward", true);
  total->updctrl("OnePole/lpf/mrs_real/alpha", 0.99f);
  mrs_natural factor = 32;
  total->updctrl("DownSampler/ds/mrs_natural/factor", factor);  
  
  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM 
  mrs_natural pkinS = total->getctrl("Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_real peakSpacing = ((mrs_natural)(srate * 60.0 / (factor *60.0)) - 
		      (mrs_natural)(srate * 60.0 / (factor*64.0))) / pkinS;
  mrs_natural peakStart = (mrs_natural)(srate * 60.0 / (factor * 180.0));
  mrs_natural peakEnd   = (mrs_natural)(srate * 60.0 / (factor * 50.0));
  total->updctrl("Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
  total->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.75);
  total->updctrl("Peaker/pkr/mrs_natural/peakStart", peakStart);
  total->updctrl("Peaker/pkr/mrs_natural/peakEnd", peakEnd);
  total->updctrl("Peaker/pkr/mrs_real/peakGain", 2.0);

  total->updctrl("PeakPeriods2BPM/p2bpm/mrs_natural/factor", factor);

  total->updctrl("Histogram/histo/mrs_natural/startBin", 0);
  total->updctrl("Histogram/histo/mrs_natural/endBin", 180);
  
  // prepare vectors for processing 
  realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
	       total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec estimate(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
		   total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());
  
  mrs_natural bin;
  mrs_natural onSamples;
  
  int numPlayed =0;
  mrs_natural wc=0;
  mrs_natural samplesPlayed = 0;
  mrs_natural repeatId = 1;

  // vector of bpm estimate used to calculate median 
  vector<int> bpms;
  onSamples = total->getctrl("ShiftInput/si/mrs_natural/onSamples")->to<mrs_natural>();

  while (repetitions * duration > samplesPlayed)
    {
      total->process(iwin, estimate);

      bin = (mrs_natural) estimate(1);
      cout << "max bpm = " << bin << endl;
      bpms.push_back(bin);
      
      numPlayed++;
      if (samplesPlayed > repeatId * duration)
	{
	  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);   
	  repeatId++;
	}
      wc ++;
      samplesPlayed += onSamples;
      // no duration specified so use all of source input 
      if (!(total->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>()) && (repeatId == 1))
	{
	  duration = samplesPlayed-onSamples;
	}
    } 

  cout << "FINAL = " << bpms[bpms.size()-1] << endl;
  
  // Output to file 
  ofstream oss(resName.c_str());
  oss << bpms[bpms.size()- 1] << endl;
  cerr << "Played " << wc << " slices of " << onSamples << " samples" 
       << endl;
  cout << "Processed " << sfName << endl;
  cout << "Wrote " << resName << endl;
  delete total;
}


void 
tempo_medianSumBands(string sfName, string resName)
{
  MarSystemManager mng;
  mrs_natural nChannels;
  mrs_real srate;
  
  // prepare network 
  MarSystem *total = mng.create("Series", "src");
  total->addMarSystem(mng.create("SoundFileSource", "src"));
  total->addMarSystem(mng.create("ShiftInput", "si"));
  total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
  total->addMarSystem(mng.create("WaveletBands", "wvbnds"));

  // envelope extraction 
  total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
  total->addMarSystem(mng.create("OnePole", "lpf"));
  total->addMarSystem(mng.create("Norm", "norm"));
  total->addMarSystem(mng.create("Sum", "sum"));
  total->addMarSystem(mng.create("DownSampler", "ds"));
  total->addMarSystem(mng.create("AutoCorrelation", "acr"));
  total->addMarSystem(mng.create("Peaker", "pkr"));
  total->addMarSystem(mng.create("MaxArgMax", "mxr"));
  total->addMarSystem(mng.create("PeakPeriods2BPM", "p2bpm"));
  
  // update the controls 
  // input filename with hopSize/winSize 

  total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);

  nChannels = total->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();
  srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();

  mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
  mrs_natural hopSize = winSize / 8;

  offset = (mrs_natural) (start * srate * nChannels);
  duration = (mrs_natural) (length * srate * nChannels);

  total->updctrl("mrs_natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);      
  total->updctrl("ShiftInput/si/mrs_natural/winSize", winSize);


  // wavelt filterbank envelope extraction controls 
  total->updctrl("WaveletPyramid/wvpt/mrs_bool/forward", true);
  total->updctrl("OnePole/lpf/mrs_real/alpha", 0.99f);
  mrs_natural factor = 32;
  total->updctrl("DownSampler/ds/mrs_natural/factor", factor);  
  
  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM 
  mrs_natural pkinS = total->getctrl("Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_real peakSpacing = ((mrs_natural)(srate * 60.0 / (factor *60.0)) - 
		      (mrs_natural)(srate * 60.0 / (factor*64.0))) / pkinS;
  mrs_natural peakStart = (mrs_natural)(srate * 60.0 / (factor * 180.0));
  mrs_natural peakEnd   = (mrs_natural)(srate * 60.0 / (factor * 50.0));
  total->updctrl("Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
  total->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.75);
  total->updctrl("Peaker/pkr/mrs_natural/peakStart", peakStart);
  total->updctrl("Peaker/pkr/mrs_natural/peakEnd", peakEnd);
  total->updctrl("Peaker/pkr/mrs_real/peakGain", 2.0);

  total->updctrl("PeakPeriods2BPM/p2bpm/mrs_natural/factor", factor);

  
  // prepare vectors for processing 
  realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
	       total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec estimate(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
		   total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  mrs_natural bin;
  mrs_natural onSamples;
  
  int numPlayed =0;
  mrs_natural wc=0;
  mrs_natural samplesPlayed = 0;
  mrs_natural repeatId = 1;
  // vector of bpm estimate used to calculate median 
  vector<int> bpms;
  onSamples = total->getctrl("ShiftInput/si/mrs_natural/onSamples")->to<mrs_natural>();

  while (repetitions * duration > samplesPlayed)
    {
      total->process(iwin, estimate);
      bin = (mrs_natural) estimate(1);
      
      cout << "max bpm = " << bin << endl;
      bpms.push_back(bin);
      
      numPlayed++;
      if (samplesPlayed > repeatId * duration)
	{
	  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);   
	  repeatId++;
	}
      wc ++;
      samplesPlayed += onSamples;
      // no duration specified so use all of source input 
      if (!(total->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>()) && (repeatId == 1))
	{
	  duration = samplesPlayed-onSamples;
	}
    }
  
  // sort bpm estimates for median filtering 
  sort(bpms.begin(), bpms.end());
  cout << "FINAL = " << bpms[bpms.size()/2] << endl; 

  // Output to file 
  ofstream oss(resName.c_str());
  oss << bpms[bpms.size()/2] << endl;
  cerr << "Played " << wc << " slices of " << onSamples << " samples" 
       << endl;
  cout << "Processed " << sfName << endl;
  cout << "Wrote " << resName << endl;
  delete total;
}



void 
tempo_bcWavelet(string sfName, string resName)
{
  MarSystemManager mng;
  mrs_natural nChannels;
  mrs_real srate = 0.0;
  
  // prepare network 
  MarSystem *total = mng.create("Series", "src");
  total->addMarSystem(mng.create("SoundFileSource", "src"));
  // wavelet filterbank 
  total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
  total->addMarSystem(mng.create("WaveletBands", "wvbnds"));
  // for each channel of filterbank extract envelope 
  total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
  total->addMarSystem(mng.create("OnePole", "lpf"));
  total->addMarSystem(mng.create("Norm", "norm"));
  total->addMarSystem(mng.create("FullWaveRectifier", "fwr1"));
  total->addMarSystem(mng.create("ClipAudioRange", "clp"));


  cout << "NETWORK PREPARED" << endl;
  
  
  // prepare filename for reading 
  total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  srate = total->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
  mrs_natural hopSize = winSize;
  nChannels = total->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();
  srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();
  offset = (mrs_natural) (start * srate * nChannels);
  duration = (mrs_natural) (length * srate * nChannels);
  total->updctrl("mrs_natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);      
  
  // wavelt filterbank envelope extraction controls 
  total->updctrl("WaveletPyramid/wvpt/mrs_bool/forward", true);
  total->updctrl("OnePole/lpf/mrs_real/alpha", 0.99f);

  // prepare vectors for processing 
  realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
	       total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec lowwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
		 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec hiwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
		total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec plowwin(1,
		  total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  realvec phiwin(1,
		 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  realvec bands(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
		total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  
  mrs_natural samplesPlayed = 0;

  
  // MarSystem* dest = mng.create("AudioSink", "dest");
  MarSystem* lowdest = mng.create("SoundFileSink", "lowdest");
  MarSystem* hidest  = mng.create("SoundFileSink", "hidest");
  MarSystem* plowdest = mng.create("SoundFileSink", "plowdest");
  MarSystem* phidest = mng.create("SoundFileSink", "phidest");
  

  mrs_natural onSamples = total->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural inSamples = total->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  // mrs_natural onObs = total->getctrl("mrs_natural/onObservations")->to<mrs_natural>();
  // mrs_natural inObs = total->getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  

  // Peak pickers for high and low band
  MarSystem* lowpkr = mng.create("PeakerAdaptive", "lowpkr");
  lowpkr->updctrl("mrs_natural/inSamples", inSamples);
  lowpkr->updctrl("mrs_real/peakSpacing", 0.1);
  lowpkr->updctrl("mrs_real/peakStrength", 0.5);
  lowpkr->updctrl("mrs_natural/peakStart", 0);
  lowpkr->updctrl("mrs_natural/peakEnd", inSamples);
  lowpkr->updctrl("mrs_real/peakGain", 1.0);


  MarSystem* hipkr = mng.create("PeakerAdaptive", "hipkr");
  hipkr->updctrl("mrs_natural/inSamples", inSamples);
  hipkr->updctrl("mrs_real/peakSpacing", 0.05);
  hipkr->updctrl("mrs_real/peakStrength", 0.6);
  hipkr->updctrl("mrs_natural/peakStart", 0);
  hipkr->updctrl("mrs_natural/peakEnd", inSamples);
  hipkr->updctrl("mrs_real/peakGain", 1.0);
  
  
  
  
  lowdest->updctrl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
  hidest->updctrl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
  plowdest->updctrl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
  phidest->updctrl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));

  lowdest->updctrl("mrs_real/israte", srate);
  lowdest->updctrl("mrs_string/filename", "lowband.wav");

  hidest->updctrl("mrs_real/israte", srate);
  hidest->updctrl("mrs_string/filename", "hiband.wav");

  plowdest->updctrl("mrs_real/israte", srate);
  plowdest->updctrl("mrs_string/filename", "plowband.wav");

  phidest->updctrl("mrs_real/israte", srate);
  phidest->updctrl("mrs_string/filename", "phiband.wav");
  
  cout << "BOOM-CHICK PROCESSING" << endl;
  cout << "sfName = " << sfName << endl;
  
  vector<mrs_natural> lowtimes;
  vector<mrs_natural> hitimes;
  
  while (total->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>())
    {
      total->process(iwin, bands);
      for (mrs_natural t=0; t < onSamples; t++)
	lowwin(0,t) = bands(1, t);
      
      for (mrs_natural t=0; t < onSamples; t++)
	hiwin(0,t) = bands(3, t);
       
      
      lowpkr->process(lowwin, plowwin);
      hipkr->process(hiwin, phiwin);
      
      lowdest->process(lowwin, lowwin);
      hidest->process(hiwin, hiwin);

      plowdest->process(plowwin, plowwin);
      phidest->process(phiwin, phiwin);


      for (mrs_natural t=0; t < onSamples; t++) 
	if (plowwin(0,t) > 0.0) 
	  lowtimes.push_back(samplesPlayed+t);

      for (mrs_natural t=0; t < onSamples; t++) 
	if (phiwin(0,t) > 0.0) 
	  hitimes.push_back(samplesPlayed+t);
      
      samplesPlayed += onSamples;
    } 

  cout << "Done with first loop" << endl;
  



  vector<mrs_natural>::iterator vi;
  
  MarSystem* playback = mng.create("Series", "playback");
  MarSystem* mix = mng.create("Fanout", "mix");
  mix->addMarSystem(mng.create("SoundFileSource", "orsrc"));
  mix->updctrl("SoundFileSource/orsrc/mrs_bool/mute", true);
  mix->addMarSystem(mng.create("SoundFileSource", "bdsrc"));
  mix->addMarSystem(mng.create("SoundFileSource", "sdsrc"));

  playback->addMarSystem(mix);
  playback->addMarSystem(mng.create("Sum", "sum"));
  playback->addMarSystem(mng.create("SoundFileSink", "adest"));
  cout << "SOUNDFILESINK srate = " << srate << endl;




  playback->updctrl("Fanout/mix/SoundFileSource/orsrc/mrs_string/filename", sfName);  

  
  string sdname;
  string bdname;

  if (srate == 22050.0) 
    {
      sdname = "./sd22k.wav";
      bdname = ".,/bd22k.wav";
    }
  else 				// assume everything is either 22k or 44.1k 
    {
      sdname = "./sd.wav";
      bdname = "./bd.wav";
    }

  
  samplesPlayed = 0;
  onSamples = playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural lowtindex = 0;
  mrs_natural hitindex = 0;


  playback->updctrl("SoundFileSink/adest/mrs_real/israte", srate);
  playback->updctrl("SoundFileSink/adest/mrs_string/filename", "boomchick.wav");

  
  cout << "******PLAYBACK******" << endl;
  
  while(playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_bool/notEmpty")->to<mrs_bool>()) 
    {
      if (lowtimes[lowtindex] < samplesPlayed) 
	{
	  lowtindex++;

	  if (lowtindex > 1) 
	    cout << "IOI = " << lowtimes[lowtindex] - lowtimes[lowtindex-1] << endl;
	  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_string/filename", bdname);
	  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_natural/pos", 0);
	}

      if (hitimes[hitindex] < samplesPlayed) 
	{
	  hitindex++;
	  playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/mrs_string/filename", sdname);
	  playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/mrs_natural/pos", 0);
	}
      playback->tick();
      samplesPlayed += onSamples;
    }



  cout << "FINISHED PROCESSING " << endl;
  cout << "audacity " << sfName << " band.wav peak.wav" << endl;
  delete hidest;
  delete lowdest;
  delete total;
}


void 
tempo_bcFilter(string sfName, string resName)
{

  cout << "BOOMCICK_Filter PROCESSING" << endl;

  MarSystemManager mng;
  mrs_natural nChannels;
  mrs_real srate = 0.0;
  
  // prepare network 
  MarSystem *total = mng.create("Series", "src");
  total->addMarSystem(mng.create("SoundFileSource", "src"));
  total->addMarSystem(mng.create("Sum", "sum"));
  total->addMarSystem(mng.create("Gain", "tgain"));
  // total->addMarSystem(mng.create("AudioSink", "dest"));
  total->addMarSystem(mng.create("MidiOutput", "devibot"));

  Esitar* esitar = new Esitar("esitar");
  total->addMarSystem(esitar);


  // high and low bandpass filters 
  MarSystem *filters = mng.create("Fanout", "filters");
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

  MarSystem *lfilter = mng.create("Series", "lfilter");
  lfilter->addMarSystem(mng.create("Filter", "llfilter"));
  lfilter->updctrl("Filter/llfilter/mrs_realvec/ncoeffs", bl);
  lfilter->updctrl("Filter/llfilter/mrs_realvec/dcoeffs", al); 
  filters->addMarSystem(lfilter);

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

  MarSystem *hfilter = mng.create("Series", "hfilter");
  hfilter->addMarSystem(mng.create("Filter", "hhfilter"));
  hfilter->addMarSystem(mng.create("Gain", "gain"));
  hfilter->updctrl("Filter/hhfilter/mrs_realvec/ncoeffs", bh);
  hfilter->updctrl("Filter/hhfilter/mrs_realvec/dcoeffs", ah);  
  filters->addMarSystem(hfilter);
  
  total->addMarSystem(filters);
  
  // prepare filename for reading 
  total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  // total->updctrl("AudioSink/dest/mrs_bool/initAudio", true);

  srate = total->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
  mrs_natural ch = total->getctrl("SoundFileSource/src/mrs_natural/onObservations")->to<mrs_natural>();
  mrs_real tg = 1.0 / ch;
  total->updctrl("Gain/tgain/mrs_real/gain", tg);
  mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 2048;

  mrs_natural hopSize = winSize;
  nChannels = total->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();
  srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();
  offset = (mrs_natural) (start * srate * nChannels);
  duration = (mrs_natural) (length * srate * nChannels);
  total->updctrl("mrs_natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);      
  
  // prepare vectors for processing 
  realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
	       total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec lowwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
		 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec hiwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(), 
		total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec plowwin(1,
		  total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  
  realvec phiwin(1,
		 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  
  realvec bands(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(), 
		total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  
  mrs_natural samplesPlayed = 0;
  
  // MarSystem* dest = mng.create("AudioSink", "dest");
  MarSystem* lowdest = mng.create("SoundFileSink", "lowdest");
  MarSystem* hidest  = mng.create("SoundFileSink", "hidest");
  MarSystem* plowdest = mng.create("SoundFileSink", "plowdest");
  MarSystem* phidest = mng.create("SoundFileSink", "phidest");
  

  mrs_natural onSamples = total->getctrl("mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural inSamples = total->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  
  // Peak pickers for high and low band
  MarSystem* lowpkr = mng.create("PeakerAdaptive", "lowpkr");
  lowpkr->updctrl("mrs_natural/inSamples", inSamples);
  lowpkr->updctrl("mrs_real/peakSpacing", 0.3);
  lowpkr->updctrl("mrs_real/peakStrength", 0.7);
  lowpkr->updctrl("mrs_natural/peakStart", 0);
  lowpkr->updctrl("mrs_natural/peakEnd", inSamples);
  lowpkr->updctrl("mrs_real/peakGain", 1.0);
  lowpkr->updctrl("mrs_natural/peakStrengthReset", 4);
  lowpkr->updctrl("mrs_real/peakDecay", 0.9);  

  MarSystem* hipkr = mng.create("PeakerAdaptive", "hipkr");
  hipkr->updctrl("mrs_natural/inSamples", inSamples);
  hipkr->updctrl("mrs_real/peakSpacing", 0.3);
  hipkr->updctrl("mrs_real/peakStrength", 0.7);
  hipkr->updctrl("mrs_natural/peakStart", 0);
  hipkr->updctrl("mrs_natural/peakEnd", inSamples);
  hipkr->updctrl("mrs_real/peakGain", 1.0);
  hipkr->updctrl("mrs_natural/peakStrengthReset", 4);
  hipkr->updctrl("mrs_real/peakDecay", 0.9);
  
  lowdest->updctrl("mrs_natural/inSamples", 
		   total->getctrl("mrs_natural/onSamples"));
  hidest->updctrl("mrs_natural/inSamples", 
		  total->getctrl("mrs_natural/onSamples"));
  plowdest->updctrl("mrs_natural/inSamples", 
		    total->getctrl("mrs_natural/onSamples"));
  phidest->updctrl("mrs_natural/inSamples", 
		   total->getctrl("mrs_natural/onSamples"));

  lowdest->updctrl("mrs_real/israte", srate);
  lowdest->updctrl("mrs_string/filename", "lowband.wav");

  hidest->updctrl("mrs_real/israte", srate);
  hidest->updctrl("mrs_string/filename", "hiband.wav");

  plowdest->updctrl("mrs_real/israte", srate);
  plowdest->updctrl("mrs_string/filename", "plowband.wav");

  phidest->updctrl("mrs_real/israte", srate);
  phidest->updctrl("mrs_string/filename", "phiband.wav");
  
  cout << "BOOM-CHICK PROCESSING" << endl;
  vector<mrs_natural> lowtimes;
  vector<mrs_natural> hitimes;

  // Initialize vectors for file writing    
  //int r;
  //int len;
  //len = 5500;
  //realvec thumb(len);

  while (total->getctrl("SoundFileSource/src/mrs_bool/notEmpty")->to<mrs_bool>())
    {
      total->process(iwin, bands);
      
      for (mrs_natural t=0; t < onSamples; t++)
	lowwin(0,t) = bands(0, t);
      
      for (mrs_natural t=0; t < onSamples; t++)
	hiwin(0,t) = bands(1, t);

      //  for (mrs_natural t=0; t < onSamples; t++)
      //	{
      //	  r = esitar->thumb;
      //	  thumb(samplesPlayed+t) = r;
      //	}
      lowpkr->process(lowwin, plowwin);
      hipkr->process(hiwin, phiwin);
      
      lowdest->process(lowwin, lowwin);
      hidest->process(hiwin, hiwin);

      plowdest->process(plowwin, plowwin);
      phidest->process(phiwin, phiwin);

      for (mrs_natural t=0; t < onSamples; t++) 
	{
	  if (plowwin(0,t) > 0.0) 
	    {
	      lowtimes.push_back(samplesPlayed+t);
	#ifdef MARSYAS_MIDIIO
	      total->updctrl("MidiOutput/devibot/mrs_natural/byte2", DEVIBOT_NA);
	      total->updctrl("MidiOutput/devibot/mrs_natural/byte3", 50);
	      total->updctrl("MidiOutput/devibot/mrs_natural/byte1", 144);
	      total->updctrl("MidiOutput/devibot/mrs_bool/sendMessage", true);
	#endif //MARSYAS_MIDIIO
	    }
	}
      for (mrs_natural t=0; t < onSamples; t++) 
	{
	  if (phiwin(0,t) > 0.0)
	    {
	      hitimes.push_back(samplesPlayed+t);
#ifdef MARSYAS_MIDIO
	      total->updctrl("MidiOutput/devibot/mrs_natural/byte2", DEVIBOT_GE);
	      total->updctrl("MidiOutput/devibot/mrs_natural/byte3", 50);
	      total->updctrl("MidiOutput/devibot/mrs_natural/byte1", 144);
	      total->updctrl("MidiOutput/devibot/mrs_bool/sendMessage", true);
#endif
	    }
	}
      samplesPlayed += onSamples;

     

    } 
  
  // Write Thumb data
  //  thumb.write("boomchickthumb.plot");

  // Write IOI files 
  //  lowtimes.write("lowIOI.txt");
  // hitimes.write("hiIOI.txt");
  
  vector<mrs_natural>::iterator vi;

  //  return;  

  MarSystem* playback = mng.create("Series", "playback");
  MarSystem* mix = mng.create("Fanout", "mix");
  mix->addMarSystem(mng.create("SoundFileSource", "orsrc"));
  mix->updctrl("SoundFileSource/orsrc/mrs_bool/mute", true);
  mix->addMarSystem(mng.create("SoundFileSource", "bdsrc"));
  mix->addMarSystem(mng.create("SoundFileSource", "sdsrc"));

  playback->addMarSystem(mix);
  playback->addMarSystem(mng.create("Sum", "sum"));
  playback->addMarSystem(mng.create("SoundFileSink", "adest"));
  playback->addMarSystem(mng.create("AudioSink", "dest"));
  playback->addMarSystem(mng.create("MidiOutput", "devibot"));

  cout << "SOUNDFILESINK srate = " << srate << endl;

  playback->updctrl("Fanout/mix/SoundFileSource/orsrc/mrs_string/filename", sfName);  
  
  string sdname;
  string bdname;

  if (srate == 22050.0) 
    {
      sdname = "./sd22k.wav";
      bdname = "./bd22k.wav";
    }
  else 				// assume everything is either 22k or 44.1k 
    {
      sdname = "./sd.wav";
      bdname = "./bd.wav";
    }

  
  samplesPlayed = 0;
  onSamples = playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural lowtindex = 0;
  mrs_natural hitindex = 0;


  playback->updctrl("SoundFileSink/adest/mrs_real/israte", srate);
  playback->updctrl("SoundFileSink/adest/mrs_string/filename", "boomchick.wav");
  
  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_string/filename", bdname);
  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_natural/pos", 0);


  playback->updctrl("AudioSink/dest/mrs_bool/initAudio", true);
  while(playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_bool/notEmpty")->to<mrs_bool>()) 
    {
       if (lowtimes[lowtindex] < samplesPlayed) 
	{
	  lowtindex++;
	  
	  if (lowtindex > 1) 
	    
	    cout << "IOI = " << lowtimes[lowtindex] - lowtimes[lowtindex-1] << endl;
	  // Robot Control
#ifdef MARSYAS_MIDIIO
	  playback->updctrl("MidiOutput/devibot/mrs_natural/byte2", DEVIBOT_GE);
	  playback->updctrl("MidiOutput/devibot/mrs_natural/byte3", 50);
	  playback->updctrl("MidiOutput/devibot/mrs_natural/byte1", 144);
	  playback->updctrl("MidiOutput/devibot/mrs_bool/sendMessage", true);
#endif 
	  
	  // Bass Drum Play back
	  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_string/filename", bdname);
	  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/mrs_natural/pos", 0);
	}

      if (hitimes[hitindex] < samplesPlayed) 
	{
	  hitindex++;
	  
	  // Robot Control
#ifdef MARSYAS_MIDIO
	  playback->updctrl("MidiOutput/devibot/mrs_natural/byte2", DEVIBOT_NA);
	  playback->updctrl("MidiOutput/devibot/mrs_natural/byte3", 50);
	  playback->updctrl("MidiOutput/devibot/mrs_natural/byte1", 144);
	  playback->updctrl("MidiOutput/devibot/mrs_bool/sendMessage", true);
#endif

	  // Snare Drum PlayBack
	  playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/mrs_string/filename", sdname);
	  playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/mrs_natural/pos", 0);
	}
      playback->tick();
      samplesPlayed += onSamples;
    }

  cout << "FINISHED PROCESSING " << endl;
  cout << "audacity " << sfName << " lowband.wav plowband.wav hiband.wav phiband.wav" << endl;
  delete hidest;
  delete lowdest;
  delete total;
}




// Play a collection l of soundfiles
void tempo(string inFname, string outFname, string method)
{
  
  MRSDIAG("tempo.cpp - tempo");

  

    

  string resName;
  string sfName;
  

  // For each file in collection estimate tempo
  sfName = inFname;
  resName = outFname;
  
  /* resName = sfName.substr(0,sfName.rfind(".", sfName.length()));
  resName += "Marsyas";
  resName += ".txt";
  */ 
  
  if (method == "MEDIAN_SUMBANDS") 
    {
      cout << "TEMPO INDUCTION USING MEDIAN_SUMBANDS method" << endl;
      tempo_medianSumBands(sfName,resName);
    }
  else if (method == "MEDIAN_MULTIBANDS")
    {
      cout << "TEMPO INDUCTION USING MEDIAN_MULTIBANDS method" << endl;
      tempo_medianMultiBands(sfName,resName);
    }
  else if (method == "HISTO_SUMBANDS")
    {
      cout << "TEMPO INDUCTION USING HISTO_SUMBANDS method" << endl;
      tempo_histoSumBands(sfName, resName);
    }
  else if (method == "NEW") 
    {
      tempo_new(sfName, resName);
    }
  else if (method == "BOOMCHICK_WAVELET") 
    {
      cout << "BOOM-CHICK Wavelet RHYTHM EXTRACTION method " << endl;
      tempo_bcWavelet(sfName, resName);
    }
  else if (method == "BOOMCHICK_FILTER") 
    {
      cout << "BOOM-CHICK Filter RHYTHM EXTRACTION method " << endl;
      tempo_bcFilter(sfName, resName);
    }
  
  else 
    cout << "Unsupported tempo induction method " << endl;
  

}



void
readCollection(Collection& l, string name)
{
  MRSDIAG("sfplay.cpp - readCollection");
  ifstream from1(name.c_str());
  mrs_natural attempts  =0;


  MRSDIAG("Trying current working directory: " + name);
  if (from1.good() == false)
    {
      attempts++;
    }
  else
    {
      from1 >> l;
      l.setName(name.substr(0, name.rfind(".", name.length())));
    }

  
  if (attempts == 1) 
    {
      string warn;
      warn += "Problem reading collection ";
      warn += name; 
      warn += " - tried both default mf directory and current working directory";
      MRSWARN(warn);
      exit(1);

    }
} 

void 
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addBoolOption("verbose", "v", false);
  cmd_options.addRealOption("start", "s", 0.0f);
  cmd_options.addStringOption("filename", "f", EMPTYSTRING);
  cmd_options.addRealOption("length", "l", 1000.0f);
  cmd_options.addRealOption("gain", "g", 1.0);
  cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
  cmd_options.addRealOption("repetitions", "r", 1.0);
  cmd_options.addStringOption("method", "m", EMPTYSTRING);
  cmd_options.addRealOption("band", "b", 0.0);
  
}

void 
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  start = cmd_options.getRealOption("start");
  length = cmd_options.getRealOption("length");
  repetitions = cmd_options.getRealOption("repetitions");
  gain = cmd_options.getRealOption("gain");
  pluginName = cmd_options.getStringOption("plu1gin");
  fileName   = cmd_options.getStringOption("filename");
  methodopt = cmd_options.getStringOption("method");
  bandopt = (mrs_natural)cmd_options.getRealOption("band");
}





int
main(int argc, const char **argv)
{
  
  MRSDIAG("tempo.cpp - main");

  string progName = argv[0];  
  if (argc == 1)
    {
      printUsage(progName);
      exit(1);
    }
  

    


  

  initOptions();
  cmd_options.readOptions(argc,argv);
  loadOptions();
  

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  
  if (helpopt) 
    printHelp(progName);
  
  if (usageopt)
    printUsage(progName);

   
  string method;
  
  if (methodopt == EMPTYSTRING) 
    method = "NEW";
  else 
    method = methodopt;


  // collection code for batch processing 
  /* Collection l;

  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
    {
      string sfname = *sfi;
      
      readCollection(l,sfname);
    }
  */ 
  
  // for (int i=0; i < l.size(); i++)
  // {
  // tempo(l.entry(i), "default.txt", method);
  // }
  
  
  
  tempo(soundfiles[0], soundfiles[1], method);
  exit(0);
}






