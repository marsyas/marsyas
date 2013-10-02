#include <string>
#include <cstdio>
#include<iostream>
#include<iomanip>

#include <marsyas/CommandLineOptions.h>
#include <marsyas/Collection.h>
#include <marsyas/FileName.h>

//#include <marsyas/PeUtilities.h>
#include <marsyas/peakView.h>

using namespace std;
using namespace Marsyas;

// Global variables for command-line options
bool helpopt = 0;
bool usageopt =0;

string outFileName = "MARSYAS_EMPTY";

CommandLineOptions cmd_options;

void
printUsage(string progName)
{
  MRSDIAG("peakEval.cpp - printUsage");
  cerr << "Usage : " << progName << " [peakfile1] [peakfile2] -o [outFileName]" << endl;
  cerr << endl;
  cerr << "[peakfile1] and [peakfile2] can be collections (.mf) of .peak files. " << endl;
}

void
printHelp(string progName)
{
  MRSDIAG("peakEval.cpp - printHelp");
  cerr << "peakEval, MARSYAS" << endl;
  cerr << "report bugs to lmartins@inescporto.pt" << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Usage : " << progName << " [peakfile1] [peakfile2] -o [outFileName]" << endl;
  cerr << endl;
  cerr << "[peakfile1] and [peakfile2] can be collections (.mf) of .peak files. " << endl;
  cerr << "Options:" << endl;
  cerr << "-u --usage           : display short usage info" << endl;
  cerr << "-h --help            : display this information " << endl;
  cerr << "-o --outputFileName  : output filename for similarity results" << endl;

  exit(1);
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addStringOption("outputfilename", "o", "similarity.out");
}

void
loadOptions()
{
  helpopt = cmd_options.getBoolOption("help");
  usageopt = cmd_options.getBoolOption("usage");
  outFileName   = cmd_options.getStringOption("outputfilename");
}

mrs_real
calculateSimilarity(string peaks1fname, string peaks2fname)
{
  mrs_real fs1, fs2;
  //mrs_natural nbSines1, nbSines2, nbFrames1, nbFrames2, hopSize1, hopSize2;

  //load peak files into realvecs
  realvec peaks1, peaks2;
  peakView peaks1View(peaks1);
  peakView peaks2View(peaks2);

  //load data from .peak file into a realvec (using peakView wrapper class)
  peaks1View.peakRead(peaks1fname);// fs1, nbSines1, nbFrames1, hopSize1, false); //load peaks in table format
  peaks2View.peakRead(peaks2fname);// fs2, nbSines2, nbFrames2, hopSize2, false); //load peaks in table format

  //get frequency and amplitude parameters for the peaks in file1
  vector<realvec> peaks1Freqs;
  vector<realvec> peaks1Amps;
  peaks1View.getPeaksParam(peaks1Freqs, peakView::pkFrequency, 0, peaks1View.getNbFrames()-1);
  peaks1View.getPeaksParam(peaks1Amps, peakView::pkAmplitude, 0, peaks1View.getNbFrames()-1);
  //normalize frequency vectors by Nyquist frequency
  fs1 = peaks1View.getFs();
  for(mrs_natural v=0; v < peaks1Freqs.size(); ++v)
    peaks1Freqs[v]/= fs1/2;

  //get frequency and amplitude parameters for the peaks in file2
  vector<realvec> peaks2Freqs;
  vector<realvec> peaks2Amps;
  peaks2View.getPeaksParam(peaks2Freqs, peakView::pkFrequency, 0, peaks2View.getNbFrames()-1);
  peaks2View.getPeaksParam(peaks2Amps, peakView::pkAmplitude, 0, peaks2View.getNbFrames()-1);
  //normalize frequency vectors by Nyquist frequency
  fs2 = peaks2View.getFs();
  for(mrs_natural v=0; v < peaks2Freqs.size(); ++v)
    peaks2Freqs[v]/= fs2/2;

  //use HWPS correlation computation...
  mrs_natural histSize = 1024; //hardcoded!!!!! [!]
  mrs_natural nrFrames = min(peaks1Freqs.size(), peaks2Freqs.size());

  mrs_real sim = 0;
  mrs_real globalAmp=0;

  for(mrs_natural f = 0; f < nrFrames; ++f)
  {
    mrs_real amp = (peaks1Amps[f].sum()+peaks2Amps[f].sum())/2;
    globalAmp += amp;

    sim += amp * peakView::cosineDistance(peaks1Freqs[f], peaks1Amps[f],
                                          peaks2Freqs[f], peaks2Amps[f],
                                          peaks1Amps[f], peaks2Amps[f],
                                          histSize);

  }
  return sim/globalAmp;
}

void
readCollection(Collection& l, string name)
{
  MRSDIAG("peakEval.cpp - readCollection");
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

int
main(int argc, const char **argv)
{
  MRSDIAG("peakEval.cpp - main");

  initOptions();
  cmd_options.readOptions(argc, argv);
  loadOptions();

  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;

  string progName = argv[0];

  if (helpopt)
    printHelp(progName);

  if (usageopt)
    printUsage(progName);

  //load peak files and calculate similarity between them
  if (soundfiles.size() == 2)
  {
    FileName peaksfname1(soundfiles[0]);
    FileName peaksfname2(soundfiles[1]);

    if(peaksfname1.ext()== "peak" && peaksfname2.ext()=="peak")
    {
      //calculate similarity between peaks
      mrs_real similarity;
      similarity = calculateSimilarity(peaksfname1.fullname(), peaksfname2.fullname());
      cout << "Similarity = " << similarity << endl;
    }
    else if(peaksfname1.ext()== "mf" && peaksfname2.ext()=="mf")
    {
      //read collections
      Collection l1, l2;
      readCollection(l1, peaksfname1.fullname());
      readCollection(l2, peaksfname2.fullname());

      if(l1.size() != l2.size())
      {
        cout << "collections have not the same number of peak files! Exiting..." << endl;
        exit(1);
      }

      mrs_natural collSize = l1.size();
      realvec similarities(collSize);
      //iterate over collections and calculate similarities
      for(mrs_natural i=0; i<collSize; ++i)
      {
        mrs_real similarity;
        similarity = calculateSimilarity(l1.entry(i), l2.entry(i));
        similarities(i) = similarity;
        cout << "Similarity = " << similarity << endl;
      }

      //save results to a file
      similarities.write(outFileName);
    }
    else
    {
      cout << "input files must be in .peak or in .mf collection format! Exiting..." << endl;
      exit(1);
    }
  }
  else
  {
    cout << "No peak files to compare... exiting." << endl;
    exit(1);
  }


}


