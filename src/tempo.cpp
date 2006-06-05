
#include <stdio.h>

#include "Collection.h"
#include "MarSystemManager.h" 
#include "CommandLineOptions.h"
#include <string> 
using namespace std;



#define EMPTYSTRING "MARSYAS_EMPTY"
string fileName;
string pluginName;
string methodopt;
CommandLineOptions cmd_options;


int helpopt;
int usageopt;
long offset = 0;
long duration = 1000 * 44100;
long band;
natural bandopt = 0;

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

  real srate = total->getctrl("SoundFileSource/src/real/israte").toReal();

  // input filename with hopSize/winSize 
  natural winSize = (natural)(srate / 22050.0) * 65536;
  natural hopSize = winSize / 16;
  total->updctrl("SoundFileSource/src/string/filename", sfName);
  total->updctrl("SoundFileSource/src/natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/natural/pos", offset);      
  total->updctrl("ShiftInput/si/natural/WindowSize", winSize);
  total->updctrl("ShiftInput/si/natural/Decimation", hopSize);

  // wavelt filterbank envelope extraction controls 
  total->updctrl("WaveletPyramid/wvpt/bool/forward", (MarControlValue)true);
  total->updctrl("OnePole/lpf/real/alpha", 0.99f);
  natural factor = 32;
  total->updctrl("DownSampler/ds/natural/factor", factor);  
  
  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM 

  natural pkinS = total->getctrl("Peaker/pkr/natural/onSamples").toNatural();
  real peakSpacing = ((natural)(srate * 60.0 / (factor *60.0)) - 
		      (natural)(srate * 60.0 / (factor*64.0))) / pkinS;
  natural peakStart = (natural)(srate * 60.0 / (factor * 180.0));
  natural peakEnd   = (natural)(srate * 60.0 / (factor * 50.0));
  total->updctrl("Peaker/pkr/real/peakSpacing", peakSpacing);
  total->updctrl("Peaker/pkr/real/peakStrength", 0.75);
  total->updctrl("Peaker/pkr/natural/peakStart", peakStart);
  total->updctrl("Peaker/pkr/natural/peakEnd", peakEnd);
  total->updctrl("Peaker/pkr/real/peakGain", 2.0);


  total->updctrl("PeakPeriods2BPM/p2bpm/natural/factor", factor);
  
  
  // prepare vectors for processing 
  realvec iwin(total->getctrl("natural/inObservations").toNatural(), 
	       total->getctrl("natural/inSamples").toNatural());
  realvec estimate(total->getctrl("natural/onObservations").toNatural(), 
		   total->getctrl("natural/onSamples").toNatural());
  natural bin;
  natural onSamples, nChannels;
  int numPlayed =0;
  natural wc=0;
  natural samplesPlayed = 0;
  natural repeatId = 1;
  // vector of bpm estimate used to calculate median 
  vector<int> bpms;
  
  onSamples = total->getctrl("ShiftInput/si/natural/onSamples").toNatural();
  nChannels = total->getctrl("SoundFileSource/src/natural/nChannels").toNatural();

  // playback offset & duration
  offset = (natural) (start * srate * nChannels);
  duration = (natural) (length * srate * nChannels);
  
  while (repetitions * duration > samplesPlayed)
    {
      total->process(iwin, estimate);

      
      
      // convert highest peak to BPMs and add to vector 
      for (int b=0; b < 5; b++)
	{
	  // pitch = srate * 60.0 / (estimate(b,1) * factor);
	  // bin = (natural) (pitch);
	  bin = (natural)(estimate(b,1));
	  cout << "max bpm(" << b << ") = " << bin << endl;
	  bpms.push_back(bin);
	}
      numPlayed++;
      if (samplesPlayed > repeatId * duration)
	{
	  total->updctrl("SoundFileSource/src/natural/pos", offset);   
	  repeatId++;
	}
      wc ++;
      samplesPlayed += onSamples;
      // no duration specified so use all of source input 
      if (!(total->getctrl("SoundFileSource/src/bool/notEmpty").toBool()) && (repeatId == 1))
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

  natural nChannels;
  real srate = 0.0;

  
  // prepare network 
  MarSystem *total = mng.create("Series", "src");

  
  total->addMarSystem(mng.create("SoundFileSource", "src"));
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




  natural ifactor = 8;;
  total->updctrl("DownSampler/initds/natural/factor", ifactor);  
  
  total->updctrl("SoundFileSource/src/string/filename", sfName);
  nChannels = total->getctrl("SoundFileSource/src/natural/nChannels").toNatural();
  srate = total->getctrl("SoundFileSource/src/real/osrate").toReal();
  // srate = total->getctrl("DownSampler/initds/real/osrate").toReal();
  // cout << "srate = " << srate << endl;
  

  
  // update the controls 
  // input filename with hopSize/winSize 
  natural winSize = (natural)((srate / 22050.0) * 2 * 65536);
  natural hopSize = winSize / 16;

  // cout << "winSize = " << winSize << endl;
  // cout << "hopSize = " << hopSize << endl;
  

  offset = (natural) (start * srate * nChannels);
  duration = (natural) (length * srate * nChannels);
  
  // total->updctrl("PlotSink/psink1/string/outputFilename", "acr");
  // total->updctrl("PlotSink/psink2/string/outputFilename", "peaks");
  
  // total->updctrl("PlotSink/psink3/string/outputFilename", "histo");
  // total->updctrl("PlotSink/psink4/string/outputFilename", "rhisto");   
  total->updctrl("SoundFileSource/src/natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/natural/pos", offset);      

  total->updctrl("MaxArgMax/mxr/natural/nMaximums", 5);
  // total->updctrl("MaxArgMax/mxr1/natural/nMaximums", 2);  

  
  total->updctrl("ShiftInput/si/natural/WindowSize", winSize);
  total->updctrl("ShiftInput/si/natural/Decimation", hopSize);


  // wavelt filterbank envelope extraction controls 
  total->updctrl("WaveletPyramid/wvpt/bool/forward", (MarControlValue)true);
  total->updctrl("OnePole/lpf/real/alpha", 0.99f);
  natural factor = 32;
  total->updctrl("DownSampler/ds/natural/factor", factor);  

  srate = total->getctrl("DownSampler/initds/real/osrate").toReal();  

  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM 
  natural pkinS = total->getctrl("Peaker/pkr/natural/onSamples").toNatural();
  real peakSpacing = ((natural)(srate * 60.0 / (factor *60.0)) - 
		      (natural)(srate * 60.0 / (factor*62.0))) / (pkinS * 1.0);

  
  natural peakStart = (natural)(srate * 60.0 / (factor * 230.0));
  natural peakEnd   = (natural)(srate * 60.0 / (factor * 30.0));


  total->updctrl("Peaker/pkr/real/peakSpacing", peakSpacing);
  total->updctrl("Peaker/pkr/real/peakStrength", 0.5);
  total->updctrl("Peaker/pkr/natural/peakStart", peakStart);
  total->updctrl("Peaker/pkr/natural/peakEnd", peakEnd);
  total->updctrl("Peaker/pkr/real/peakGain", 2.0);


  /* total->updctrl("Peaker/pkr1/real/peakSpacing", 0.1);
  total->updctrl("Peaker/pkr1/real/peakStrength", 1.2);
  total->updctrl("Peaker/pkr1/natural/peakStart", 20);
  total->updctrl("Peaker/pkr1/natural/peakEnd", 180);
  */ 
  


  
  total->updctrl("Histogram/histo/natural/startBin", 0);
  total->updctrl("Histogram/histo/natural/endBin", 230);
  
  // prepare vectors for processing 
  realvec iwin(total->getctrl("natural/inObservations").toNatural(), 
	       total->getctrl("natural/inSamples").toNatural());
  realvec estimate(total->getctrl("natural/onObservations").toNatural(), 
		   total->getctrl("natural/onSamples").toNatural());
  
  // natural bin;
  natural onSamples;
  
  int numPlayed =0;
  natural wc=0;
  natural samplesPlayed = 0;
  natural repeatId = 1;

  onSamples = total->getctrl("ShiftInput/si/natural/onSamples").toNatural();



  while (total->getctrl("SoundFileSource/src/bool/notEmpty").toBool())
    {
      total->process(iwin, estimate);

      numPlayed++;
      if (samplesPlayed > repeatId * duration)
	{
	  total->updctrl("SoundFileSource/src/natural/pos", offset);   
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
  total1->updctrl("SoundFileSource/src1/string/filename", sfName);

  nChannels = total1->getctrl("SoundFileSource/src1/natural/nChannels").toNatural();
  srate = total1->getctrl("SoundFileSource/src1/real/osrate").toReal();
  

  
  // update the controls 
  // input filename with hopSize/winSize 
  winSize = (natural)(srate / 22050.0) * 8 * 65536;
  
  total1->updctrl("SoundFileSource/src1/natural/inSamples", winSize);
  total1->updctrl("SoundFileSource/src1/natural/pos", 0);      

  // wavelt filterbank envelope extraction controls 
  // total1->updctrl("OnePole/lpf1/real/alpha", 0.99f);
  factor = 4;
  total1->updctrl("DownSampler/ds1/natural/factor", factor);  



  

  
  

  realvec iwin1(total1->getctrl("natural/inObservations").toNatural(), 
	       total1->getctrl("natural/inSamples").toNatural());
  realvec estimate1(total1->getctrl("natural/onObservations").toNatural(), 
		    total1->getctrl("natural/onSamples").toNatural());

  
  total1->process(iwin1, estimate1);
  

  
  real s1 = estimate(0);
  real s2 = estimate(2);
  real t1 = estimate(1);
  real t2 = estimate(3);  

  natural p1 = (natural)(round ((srate * 60.0) / (factor * t1)));
  natural p2 = (natural)(round ((srate * 60.0) / (factor * t2)));
  

  real mx = 0.0;
  natural imx = 0;
  real sum = 0.0;


  
  
  for (natural i = 0; i < p1; i++)
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

  real ph1 = (imx * factor * 1.0) / srate;
  
  for (natural i = 0; i < p2; i++)
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
  
  real ph2 = (imx * factor * 1.0) / srate;
  
  

  

  real st = s1 / (s1 + s2);

  
  
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
  natural nChannels;
  real srate = 0.0;
  
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
  natural winSize = (natural)(srate / 22050.0) * 65536;
  natural hopSize = winSize / 8;
  total->updctrl("SoundFileSource/src/string/filename", sfName);

  nChannels = total->getctrl("SoundFileSource/src/natural/nChannels").toNatural();
  srate = total->getctrl("SoundFileSource/src/real/israte").toReal();

  offset = (natural) (start * srate * nChannels);
  duration = (natural) (length * srate * nChannels);
  

  total->updctrl("SoundFileSource/src/natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/natural/pos", offset);      
  total->updctrl("ShiftInput/si/natural/WindowSize", winSize);
  total->updctrl("ShiftInput/si/natural/Decimation", hopSize);

  // wavelt filterbank envelope extraction controls 
  total->updctrl("WaveletPyramid/wvpt/bool/forward", (MarControlValue)true);
  total->updctrl("OnePole/lpf/real/alpha", 0.99f);
  natural factor = 32;
  total->updctrl("DownSampler/ds/natural/factor", factor);  
  
  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM 
  natural pkinS = total->getctrl("Peaker/pkr/natural/onSamples").toNatural();
  real peakSpacing = ((natural)(srate * 60.0 / (factor *60.0)) - 
		      (natural)(srate * 60.0 / (factor*64.0))) / pkinS;
  natural peakStart = (natural)(srate * 60.0 / (factor * 180.0));
  natural peakEnd   = (natural)(srate * 60.0 / (factor * 50.0));
  total->updctrl("Peaker/pkr/real/peakSpacing", peakSpacing);
  total->updctrl("Peaker/pkr/real/peakStrength", 0.75);
  total->updctrl("Peaker/pkr/natural/peakStart", peakStart);
  total->updctrl("Peaker/pkr/natural/peakEnd", peakEnd);
  total->updctrl("Peaker/pkr/real/peakGain", 2.0);

  total->updctrl("PeakPeriods2BPM/p2bpm/natural/factor", factor);

  total->updctrl("Histogram/histo/natural/startBin", 0);
  total->updctrl("Histogram/histo/natural/endBin", 180);
  
  // prepare vectors for processing 
  realvec iwin(total->getctrl("natural/inObservations").toNatural(), 
	       total->getctrl("natural/inSamples").toNatural());
  realvec estimate(total->getctrl("natural/onObservations").toNatural(), 
		   total->getctrl("natural/onSamples").toNatural());
  
  natural bin;
  natural onSamples;
  
  int numPlayed =0;
  natural wc=0;
  natural samplesPlayed = 0;
  natural repeatId = 1;

  // vector of bpm estimate used to calculate median 
  vector<int> bpms;
  onSamples = total->getctrl("ShiftInput/si/natural/onSamples").toNatural();

  while (repetitions * duration > samplesPlayed)
    {
      total->process(iwin, estimate);

      bin = (natural) estimate(1);
      cout << "max bpm = " << bin << endl;
      bpms.push_back(bin);
      
      numPlayed++;
      if (samplesPlayed > repeatId * duration)
	{
	  total->updctrl("SoundFileSource/src/natural/pos", offset);   
	  repeatId++;
	}
      wc ++;
      samplesPlayed += onSamples;
      // no duration specified so use all of source input 
      if (!(total->getctrl("SoundFileSource/src/bool/notEmpty").toBool()) && (repeatId == 1))
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
  natural nChannels;
  real srate;
  
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

  total->updctrl("SoundFileSource/src/string/filename", sfName);

  nChannels = total->getctrl("SoundFileSource/src/natural/nChannels").toNatural();
  srate = total->getctrl("SoundFileSource/src/real/israte").toReal();

  natural winSize = (natural)(srate / 22050.0) * 65536;
  natural hopSize = winSize / 8;

  offset = (natural) (start * srate * nChannels);
  duration = (natural) (length * srate * nChannels);

  total->updctrl("SoundFileSource/src/natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/natural/pos", offset);      
  total->updctrl("ShiftInput/si/natural/WindowSize", winSize);
  total->updctrl("ShiftInput/si/natural/Decimation", hopSize);

  // wavelt filterbank envelope extraction controls 
  total->updctrl("WaveletPyramid/wvpt/bool/forward", (MarControlValue)true);
  total->updctrl("OnePole/lpf/real/alpha", 0.99f);
  natural factor = 32;
  total->updctrl("DownSampler/ds/natural/factor", factor);  
  
  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM 
  natural pkinS = total->getctrl("Peaker/pkr/natural/onSamples").toNatural();
  real peakSpacing = ((natural)(srate * 60.0 / (factor *60.0)) - 
		      (natural)(srate * 60.0 / (factor*64.0))) / pkinS;
  natural peakStart = (natural)(srate * 60.0 / (factor * 180.0));
  natural peakEnd   = (natural)(srate * 60.0 / (factor * 50.0));
  total->updctrl("Peaker/pkr/real/peakSpacing", peakSpacing);
  total->updctrl("Peaker/pkr/real/peakStrength", 0.75);
  total->updctrl("Peaker/pkr/natural/peakStart", peakStart);
  total->updctrl("Peaker/pkr/natural/peakEnd", peakEnd);
  total->updctrl("Peaker/pkr/real/peakGain", 2.0);

  total->updctrl("PeakPeriods2BPM/p2bpm/natural/factor", factor);

  
  // prepare vectors for processing 
  realvec iwin(total->getctrl("natural/inObservations").toNatural(), 
	       total->getctrl("natural/inSamples").toNatural());
  realvec estimate(total->getctrl("natural/onObservations").toNatural(), 
		   total->getctrl("natural/onSamples").toNatural());

  natural bin;
  natural onSamples;
  
  int numPlayed =0;
  natural wc=0;
  natural samplesPlayed = 0;
  natural repeatId = 1;
  // vector of bpm estimate used to calculate median 
  vector<int> bpms;
  onSamples = total->getctrl("ShiftInput/si/natural/onSamples").toNatural();

  while (repetitions * duration > samplesPlayed)
    {
      total->process(iwin, estimate);
      bin = (natural) estimate(1);
      
      cout << "max bpm = " << bin << endl;
      bpms.push_back(bin);
      
      numPlayed++;
      if (samplesPlayed > repeatId * duration)
	{
	  total->updctrl("SoundFileSource/src/natural/pos", offset);   
	  repeatId++;
	}
      wc ++;
      samplesPlayed += onSamples;
      // no duration specified so use all of source input 
      if (!(total->getctrl("SoundFileSource/src/bool/notEmpty").toBool()) && (repeatId == 1))
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
  natural nChannels;
  real srate = 0.0;
  
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
  total->updctrl("SoundFileSource/src/string/filename", sfName);
  srate = total->getctrl("SoundFileSource/src/real/osrate").toReal();
  natural winSize = (natural)(srate / 22050.0) * 65536;
  natural hopSize = winSize;
  nChannels = total->getctrl("SoundFileSource/src/natural/nChannels").toNatural();
  srate = total->getctrl("SoundFileSource/src/real/israte").toReal();
  offset = (natural) (start * srate * nChannels);
  duration = (natural) (length * srate * nChannels);
  total->updctrl("SoundFileSource/src/natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/natural/pos", offset);      
  
  // wavelt filterbank envelope extraction controls 
  total->updctrl("WaveletPyramid/wvpt/bool/forward", (MarControlValue)true);
  total->updctrl("OnePole/lpf/real/alpha", 0.99f);

  // prepare vectors for processing 
  realvec iwin(total->getctrl("natural/inObservations").toNatural(), 
	       total->getctrl("natural/inSamples").toNatural());
  realvec lowwin(total->getctrl("natural/inObservations").toNatural(), 
		 total->getctrl("natural/inSamples").toNatural());
  realvec hiwin(total->getctrl("natural/inObservations").toNatural(), 
		total->getctrl("natural/inSamples").toNatural());
  realvec plowwin(1,
		  total->getctrl("natural/inSamples").toNatural());

  realvec phiwin(1,
		 total->getctrl("natural/inSamples").toNatural());

  realvec bands(total->getctrl("natural/onObservations").toNatural(), 
		total->getctrl("natural/onSamples").toNatural());

  
  natural samplesPlayed = 0;

  
  // MarSystem* dest = mng.create("AudioSink", "dest");
  MarSystem* lowdest = mng.create("SoundFileSink", "lowdest");
  MarSystem* hidest  = mng.create("SoundFileSink", "hidest");
  MarSystem* plowdest = mng.create("SoundFileSink", "plowdest");
  MarSystem* phidest = mng.create("SoundFileSink", "phidest");
  

  natural onSamples = total->getctrl("natural/onSamples").toNatural();
  natural inSamples = total->getctrl("natural/inSamples").toNatural();
  // natural onObs = total->getctrl("natural/onObservations").toNatural();
  // natural inObs = total->getctrl("natural/inObservations").toNatural();
  

  // Peak pickers for high and low band
  MarSystem* lowpkr = mng.create("Peaker1", "lowpkr");
  lowpkr->updctrl("natural/inSamples", inSamples);
  lowpkr->updctrl("real/peakSpacing", 0.1);
  lowpkr->updctrl("real/peakStrength", 0.5);
  lowpkr->updctrl("natural/peakStart", 0);
  lowpkr->updctrl("natural/peakEnd", inSamples);
  lowpkr->updctrl("real/peakGain", 1.0);


  MarSystem* hipkr = mng.create("Peaker1", "hipkr");
  hipkr->updctrl("natural/inSamples", inSamples);
  hipkr->updctrl("real/peakSpacing", 0.05);
  hipkr->updctrl("real/peakStrength", 0.6);
  hipkr->updctrl("natural/peakStart", 0);
  hipkr->updctrl("natural/peakEnd", inSamples);
  hipkr->updctrl("real/peakGain", 1.0);
  
  
  
  
  lowdest->updctrl("natural/inSamples", total->getctrl("natural/onSamples"));
  hidest->updctrl("natural/inSamples", total->getctrl("natural/onSamples"));
  plowdest->updctrl("natural/inSamples", total->getctrl("natural/onSamples"));
  phidest->updctrl("natural/inSamples", total->getctrl("natural/onSamples"));

  lowdest->updctrl("real/israte", srate);
  lowdest->updctrl("string/filename", "lowband.wav");

  hidest->updctrl("real/israte", srate);
  hidest->updctrl("string/filename", "hiband.wav");

  plowdest->updctrl("real/israte", srate);
  plowdest->updctrl("string/filename", "plowband.wav");

  phidest->updctrl("real/israte", srate);
  phidest->updctrl("string/filename", "phiband.wav");
  
  cout << "BOOM-CHICK PROCESSING" << endl;
  cout << "sfName = " << sfName << endl;
  
  vector<natural> lowtimes;
  vector<natural> hitimes;
  
  while (total->getctrl("SoundFileSource/src/bool/notEmpty").toBool())
    {
      total->process(iwin, bands);
      for (natural t=0; t < onSamples; t++)
	lowwin(0,t) = bands(1, t);
      
      for (natural t=0; t < onSamples; t++)
	hiwin(0,t) = bands(3, t);
       
      
      lowpkr->process(lowwin, plowwin);
      hipkr->process(hiwin, phiwin);
      
      lowdest->process(lowwin, lowwin);
      hidest->process(hiwin, hiwin);

      plowdest->process(plowwin, plowwin);
      phidest->process(phiwin, phiwin);


      for (natural t=0; t < onSamples; t++) 
	if (plowwin(0,t) > 0.0) 
	  lowtimes.push_back(samplesPlayed+t);

      for (natural t=0; t < onSamples; t++) 
	if (phiwin(0,t) > 0.0) 
	  hitimes.push_back(samplesPlayed+t);
      
      samplesPlayed += onSamples;
    } 

  cout << "Done with first loop" << endl;
  



  vector<natural>::iterator vi;
  
  MarSystem* playback = mng.create("Series", "playback");
  MarSystem* mix = mng.create("Fanout", "mix");
  mix->addMarSystem(mng.create("SoundFileSource", "orsrc"));
  mix->updctrl("SoundFileSource/orsrc/bool/mute", true);
  mix->addMarSystem(mng.create("SoundFileSource", "bdsrc"));
  mix->addMarSystem(mng.create("SoundFileSource", "sdsrc"));

  playback->addMarSystem(mix);
  playback->addMarSystem(mng.create("Sum", "sum"));
  playback->addMarSystem(mng.create("SoundFileSink", "adest"));
  cout << "SOUNDFILESINK srate = " << srate << endl;




  playback->updctrl("Fanout/mix/SoundFileSource/orsrc/string/filename", sfName);  

  
  string sdname;
  string bdname;

  if (srate == 22050.0) 
    {
      sdname = "../rawwaves/sd22k.wav";
      bdname = "../rawwaves/bd22k.wav";
    }
  else 				// assume everything is either 22k or 44.1k 
    {
      sdname = "../rawwaves/sd.wav";
      bdname = "../rawwaves/bd.wav";
    }

  
  samplesPlayed = 0;
  onSamples = playback->getctrl("Fanout/mix/SoundFileSource/orsrc/natural/onSamples").toNatural();
  natural lowtindex = 0;
  natural hitindex = 0;


  playback->updctrl("SoundFileSink/adest/real/israte", srate);
  playback->updctrl("SoundFileSink/adest/string/filename", "boomchick.wav");

  
  cout << "******PLAYBACK******" << endl;
  
  while(playback->getctrl("Fanout/mix/SoundFileSource/orsrc/bool/notEmpty").toBool()) 
    {
      if (lowtimes[lowtindex] < samplesPlayed) 
	{
	  lowtindex++;

	  if (lowtindex > 1) 
	    cout << "IOI = " << lowtimes[lowtindex] - lowtimes[lowtindex-1] << endl;
	  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/string/filename", bdname);
	  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/natural/pos", 0);
	}

      if (hitimes[hitindex] < samplesPlayed) 
	{
	  hitindex++;
	  playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/string/filename", sdname);
	  playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/natural/pos", 0);
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
  natural nChannels;
  real srate = 0.0;
  
  // prepare network 
  MarSystem *total = mng.create("Series", "src");
  total->addMarSystem(mng.create("SoundFileSource", "src"));
  // wavelet filterbank 

  MarSystem *filters = mng.create("Fanout", "filters");


    


  realvec al(5),bl(5);

  /*  al(0) = 1.0;
  al(1) = -3.9680;
  al(2) = 5.9062;
  al(3) = -3.9084;
  al(4) = 0.9702;

  bl(0) = 0.0001125;
  bl(1) = 0.0;
  bl(2) = -0.0002250;
  bl(3) = 0.0;
  bl(4) = 0.0001125;
  */

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

  
  // filters->addMarSystem(mng.create("Filter", "lfilter"));

  MarSystem *lfilter = mng.create("Series", "lfilter");

  lfilter->addMarSystem(mng.create("Spectrum", "spk"));
  lfilter->updctrl("Spectrum/spk/real/cutoff", 0.012721);
  lfilter->updctrl("Spectrum/spk/real/lowcutoff", 0.00136);
  lfilter->addMarSystem(mng.create("InvSpectrum", "ispk"));  
  
  filters->addMarSystem(lfilter);
  filters->addMarSystem(mng.create("Filter", "hfilter"));
  

  //filters->updctrl("Filter/lfilter/realvec/ncoeffs", bl);
  //filters->updctrl("Filter/lfilter/realvec/dcoeffs", al);

  filters->updctrl("Filter/hfilter/realvec/ncoeffs", bh);
  filters->updctrl("Filter/hfilter/realvec/dcoeffs", ah);




  total->addMarSystem(filters);
  

  // total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
  // total->addMarSystem(mng.create("WaveletBands", "wvbnds"));
  // for each channel of filterbank extract envelope 
  // total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
  // total->addMarSystem(mng.create("OnePole", "lpf"));
  // total->addMarSystem(mng.create("Norm", "norm"));
  // total->addMarSystem(mng.create("FullWaveRectifier", "fwr1"));
  // total->addMarSystem(mng.create("ClipAudioRange", "clp"));
  
  // prepare filename for reading 
  total->updctrl("SoundFileSource/src/string/filename", sfName);
  srate = total->getctrl("SoundFileSource/src/real/osrate").toReal();
  natural winSize = (natural)(srate / 22050.0) * 65536;
  natural hopSize = winSize;
  nChannels = total->getctrl("SoundFileSource/src/natural/nChannels").toNatural();
  srate = total->getctrl("SoundFileSource/src/real/israte").toReal();
  offset = (natural) (start * srate * nChannels);
  duration = (natural) (length * srate * nChannels);
  total->updctrl("SoundFileSource/src/natural/inSamples", hopSize);
  total->updctrl("SoundFileSource/src/natural/pos", offset);      
  
  // wavelt filterbank envelope extraction controls 
  // total->updctrl("WaveletPyramid/wvpt/bool/forward", (MarControlValue)true);
  // total->updctrl("OnePole/lpf/real/alpha", 0.99f);

  // prepare vectors for processing 
  realvec iwin(total->getctrl("natural/inObservations").toNatural(), 
	       total->getctrl("natural/inSamples").toNatural());
  realvec lowwin(total->getctrl("natural/inObservations").toNatural(), 
		 total->getctrl("natural/inSamples").toNatural());
  realvec hiwin(total->getctrl("natural/inObservations").toNatural(), 
		total->getctrl("natural/inSamples").toNatural());
  realvec plowwin(1,
		  total->getctrl("natural/inSamples").toNatural());

  realvec phiwin(1,
		 total->getctrl("natural/inSamples").toNatural());

  realvec bands(total->getctrl("natural/onObservations").toNatural(), 
		total->getctrl("natural/onSamples").toNatural());

  
  natural samplesPlayed = 0;

  
  // MarSystem* dest = mng.create("AudioSink", "dest");
  MarSystem* lowdest = mng.create("SoundFileSink", "lowdest");
  MarSystem* hidest  = mng.create("SoundFileSink", "hidest");
  MarSystem* plowdest = mng.create("SoundFileSink", "plowdest");
  MarSystem* phidest = mng.create("SoundFileSink", "phidest");
  

  natural onSamples = total->getctrl("natural/onSamples").toNatural();
  natural inSamples = total->getctrl("natural/inSamples").toNatural();
  /* natural onObs = total->getctrl("natural/onObservations").toNatural();
  natural inObs = total->getctrl("natural/inObservations").toNatural();
  */ 
  

  // Peak pickers for high and low band
  MarSystem* lowpkr = mng.create("Peaker1", "lowpkr");
  lowpkr->updctrl("natural/inSamples", inSamples);
  lowpkr->updctrl("real/peakSpacing", 0.1);
  lowpkr->updctrl("real/peakStrength", 0.5);
  lowpkr->updctrl("natural/peakStart", 0);
  lowpkr->updctrl("natural/peakEnd", inSamples);
  lowpkr->updctrl("real/peakGain", 1.0);


  MarSystem* hipkr = mng.create("Peaker1", "hipkr");
  hipkr->updctrl("natural/inSamples", inSamples);
  hipkr->updctrl("real/peakSpacing", 0.1);
  hipkr->updctrl("real/peakStrength", 0.5);
  hipkr->updctrl("natural/peakStart", 0);
  hipkr->updctrl("natural/peakEnd", inSamples);
  hipkr->updctrl("real/peakGain", 1.0);
  
  
  
  
  lowdest->updctrl("natural/inSamples", total->getctrl("natural/onSamples"));
  hidest->updctrl("natural/inSamples", total->getctrl("natural/onSamples"));
  plowdest->updctrl("natural/inSamples", total->getctrl("natural/onSamples"));
  phidest->updctrl("natural/inSamples", total->getctrl("natural/onSamples"));

  lowdest->updctrl("real/israte", srate);
  lowdest->updctrl("string/filename", "lowband.wav");

  hidest->updctrl("real/israte", srate);
  hidest->updctrl("string/filename", "hiband.wav");

  plowdest->updctrl("real/israte", srate);
  plowdest->updctrl("string/filename", "plowband.wav");

  phidest->updctrl("real/israte", srate);
  phidest->updctrl("string/filename", "phiband.wav");
  
  cout << "BOOM-CHICK PROCESSING" << endl;


  vector<natural> lowtimes;
  vector<natural> hitimes;
  
  while (total->getctrl("SoundFileSource/src/bool/notEmpty").toBool())
    {
      total->process(iwin, bands);
      
      for (natural t=0; t < onSamples; t++)
	lowwin(0,t) = bands(0, t);
      
      for (natural t=0; t < onSamples; t++)
	hiwin(0,t) = bands(1, t);
      

      lowpkr->process(lowwin, plowwin);
      hipkr->process(hiwin, phiwin);
      
      lowdest->process(lowwin, lowwin);
      hidest->process(hiwin, hiwin);

      plowdest->process(plowwin, plowwin);
      phidest->process(phiwin, phiwin);


      for (natural t=0; t < onSamples; t++) 
	if (plowwin(0,t) > 0.0) 
	  lowtimes.push_back(samplesPlayed+t);

      for (natural t=0; t < onSamples; t++) 
	if (phiwin(0,t) > 0.0) 
	  hitimes.push_back(samplesPlayed+t);

      samplesPlayed += onSamples;
    } 

  vector<natural>::iterator vi;
  
  MarSystem* playback = mng.create("Series", "playback");
  MarSystem* mix = mng.create("Fanout", "mix");
  mix->addMarSystem(mng.create("SoundFileSource", "orsrc"));
  mix->updctrl("SoundFileSource/orsrc/bool/mute", true);
  mix->addMarSystem(mng.create("SoundFileSource", "bdsrc"));
  mix->addMarSystem(mng.create("SoundFileSource", "sdsrc"));

  playback->addMarSystem(mix);
  playback->addMarSystem(mng.create("Sum", "sum"));
  playback->addMarSystem(mng.create("SoundFileSink", "adest"));
  cout << "SOUNDFILESINK srate = " << srate << endl;




  playback->updctrl("Fanout/mix/SoundFileSource/orsrc/string/filename", sfName);  

  
  string sdname;
  string bdname;

  if (srate == 22050.0) 
    {
      sdname = "../rawwaves/sd22k.wav";
      bdname = "../rawwaves/bd22k.wav";
    }
  else 				// assume everything is either 22k or 44.1k 
    {
      sdname = "../rawwaves/sd.wav";
      bdname = "../rawwaves/bd.wav";
    }

  
  samplesPlayed = 0;
  onSamples = playback->getctrl("Fanout/mix/SoundFileSource/orsrc/natural/onSamples").toNatural();
  natural lowtindex = 0;
  natural hitindex = 0;


  playback->updctrl("SoundFileSink/adest/real/israte", srate);
  playback->updctrl("SoundFileSink/adest/string/filename", "boomchick.wav");

  
  while(playback->getctrl("Fanout/mix/SoundFileSource/orsrc/bool/notEmpty").toBool()) 
    {
      if (lowtimes[lowtindex] < samplesPlayed) 
	{
	  lowtindex++;

	  if (lowtindex > 1) 
	    cout << "IOI = " << lowtimes[lowtindex] - lowtimes[lowtindex-1] << endl;
	  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/string/filename", bdname);
	  playback->updctrl("Fanout/mix/SoundFileSource/bdsrc/natural/pos", 0);
	}

      if (hitimes[hitindex] < samplesPlayed) 
	{
	  hitindex++;
	  playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/string/filename", sdname);
	  playback->updctrl("Fanout/mix/SoundFileSource/sdsrc/natural/pos", 0);
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
  natural attempts  =0;


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
  bandopt = (natural)cmd_options.getRealOption("band");
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






