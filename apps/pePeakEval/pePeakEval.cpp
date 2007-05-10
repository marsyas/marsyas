#include <string>
#include <cstdio>
#include<iostream>
#include<iomanip>

#include "CommandLineOptions.h"
#include "Collection.h"
#include "FileName.h"
#include "PeUtilities.h"

using namespace std;
using namespace Marsyas;

// Global variables for command-line options 
bool helpopt = 0;
bool usageopt =0;

string outFileName = EMPTYSTRING;

CommandLineOptions cmd_options;

void 
printUsage(string progName)
{
	MRSDIAG("pePeakEval.cpp - printUsage");
	cerr << "Usage : " << progName << " [peakfile1] [peakfile2] -o [outFileName]" << endl;
	cerr << endl;
	cerr << "[peakfile1] and [peakfile2] can be collections (.mf) of .peak files. " << endl;
}

void 
printHelp(string progName)
{
	MRSDIAG("pePeakEval.cpp - printHelp");
	cerr << "pePeakEval, MARSYAS" << endl;
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
calculateSimilarity(string peaksfname1, string peaksfname2)
{
	//load peak files into realvecs
	realvec peaks1, peaks2;
	mrs_real fs1, fs2;
	mrs_natural nbSines1, nbSines2, nbFrames1, nbFrames2, hopSize1, hopSize2;
	peakLoad(peaks1, peaksfname1, fs1, nbSines1, nbFrames1, hopSize1, false); //load peaks in table format
	peakLoad(peaks2, peaksfname2, fs2, nbSines2, nbFrames2, hopSize2, false); //load peaks in table format

	//get frequency and amplitude parameters for the peaks in file1
	vector<realvec> peaks1Freq, peaks1Amp;
	extractParameter(peaks1, peaks1Freq, pkFrequency, nbSines1);
	extractParameter(peaks1, peaks1Amp, pkAmplitude, nbSines1);
	//normalize frequency vectors by Nyquist frequency
	for(mrs_natural v=0; v < peaks1Freq.size(); ++v)
		peaks1Freq[v]/= fs1/2;

	//get frequency and amplitude parameters for the peaks in file2
	vector<realvec> peaks2Freq, peaks2Amp;
	extractParameter(peaks2, peaks2Freq, pkFrequency, nbSines2);
	extractParameter(peaks2, peaks2Amp, pkAmplitude, nbSines2);
	//normalize frequency vectors by Nyquist frequency
	for(mrs_natural v=0; v < peaks2Freq.size(); ++v)
		peaks2Freq[v]/= fs2/2;

	//use HWPS correlation computation...
	mrs_natural histSize = 1024; //hardcoded!!!!! [!]
	realvec x1(histSize);
	realvec x2(histSize);
	realvec x3(histSize);
	realvec x4(histSize);

	mrs_natural nrFrames = min(peaks1Freq.size(), peaks2Freq.size());

	mrs_real sim = 0;
	for(mrs_natural f = 0; f < nrFrames; ++f)
		sim += cosinePeakSets(peaks1Freq[f], peaks1Amp[f], peaks2Freq[f], peaks2Amp[f], peaks1Amp[f], peaks2Amp[f],
									x1, x2, x3, x4, histSize);

	return sim/nrFrames;
}

void
readCollection(Collection& l, string name)
{
	MRSDIAG("pePeakEval.cpp - readCollection");
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
	MRSDIAG("pePeakEval.cpp - main");

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


