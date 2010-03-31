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


#include <cstdio>
#include <cstdlib>
#include <algorithm>


#include "FileName.h"
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "Esitar.h"
#include "mididevices.h"
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

#ifdef MARSYAS_PNG
#include "pngwriter.h" 
#endif 



#define BPM_HYPOTHESES 6 //Nr. of initial BPM hypotheses (must be <= than the nr. of agents) (6)
#define PHASE_HYPOTHESES 20//Nr. of phases per BPM hypothesis (20)
#define MIN_BPM 50 //minimum tempo considered, in BPMs (50)
#define MAX_BPM 180 //maximum tempo considered, in BPMs (250)
#define NR_AGENTS 30 //Nr. of agents in the pool (30)
#define LFT_OUTTER_MARGIN 0.20 //The size of the outer half-window (in % of the IBI) before the predicted beat time (0.20)
#define RGT_OUTTER_MARGIN 0.30 //The size of the outer half-window (in % of the IBI) after the predicted beat time (0.30)
#define INNER_MARGIN 3.0 //Inner tolerance window margin size (= half inner window size -> in ticks) (3.0)
#define OBSOLETE_FACTOR 1.5 //An agent is killed if, at any time (after the initial 5secs), the difference between its score and the bestScore is below OBSOLETE_FACTOR * bestScore (1.5)
#define CHILDREN_SCORE_FACTOR 0.01 //(Inertia1) Each created agent imports its father score decremented by the current father's score multiplied by this factor (0.01)
#define BEST_FACTOR 1.1 //(Inertia2) Mutiple of the bestScore an agent's score must have for replacing the current best agent (1.1)
#define CORRECTION_FACTOR 0.25 //correction factor for compensating each agents' own {phase, period} hypothesis errors (0.25)
#define EQ_PERIOD 0 //Period threshold which identifies two agents as predicting the same period (IBI, in ticks) (1)
#define EQ_PHASE 0 //Phase threshold which identifies two agents as predicting the same phase (phase, in ticks) (2)
#define CHILD1_FACTOR 2 //correction factor (error proportion-[0.0-1.0]) for compensating its father's {phase, period} hypothesis - used by child1 (2.0 - only full phase adjustment; -1 - no child considered) (2.0)
#define CHILD2_FACTOR 0.5 //correction factor (error proportion-[0.0-1.0]) for compensating its father's {phase, period} hypothesis - used by child2 (2.0 - only full phase adjustment; -1 - no child considered) (0.5)
#define CHILD3_FACTOR 1 //correction factor (error proportion-[0.0-1.0]) for compensating its father's {phase, period} hypothesis - used by child3 (2.0 - only full phase adjustment; -1 - no child considered) (1.0)

#define WINSIZE 1024 //(2048?)
#define HOPSIZE 512 //(512)





using namespace std;
using namespace Marsyas;


mrs_string output;
mrs_bool audiofileopt;
mrs_real induction_time;
mrs_real metrical_change_time;
mrs_string score_function;
mrs_bool audioopt;
mrs_bool backtraceopt;

string fileName;
string pluginName = EMPTYSTRING;
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


int correct_predictions; 
int correct_harmonic_predictions; 
int correct_mirex_predictions;

int total_instances;
int total_errors;
float total_differences;


void
printUsage(string progName)
{
	MRSDIAG("tempo.cpp - printUsage");
	cerr << "Usage : " << progName << " [-m method] [-g gain] [-o offset(samples)] [-d duration(samples)] [-s start(seconds)] [-l length(seconds)] [-f outputfile] [-p pluginName] [-r repetitions] file1 file2 file3" << endl;
	cerr << endl;
	cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format or collections " << endl;
	exit(1);
}

void
printHelp(string progName)
{
	MRSDIAG("tempo.cpp - printHelp");
	cerr << "tempo, MARSYAS, Copyright George Tzanetakis " << endl;
	cerr << "--------------------------------------------" << endl;
	cerr << "Prints information about the sound files provided as arguments " << endl;
	cerr << endl;
	cerr << "Usage : " << progName << "file1 file2 file3" << endl;
	cerr << endl;
	cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
	cerr << "Help Options:" << endl;
	cerr << "-u --usage      : display short usage info" << endl;
	cerr << "-h --help       : display this information " << endl;
	cerr << "-v --verbose    : verbose output " << endl;
	cerr << "-f --filename   : output to file " << endl;
	cerr << "-m --method     : tempo induction method " << endl;
	cerr << "-b --band       : set band" << endl;
	cerr << "-g --gain       : linear volume gain " << endl;
	cerr << "-o --offset     : playback start offset in samples " << endl;
	cerr << "-d --duration   : playback duration in samples     " << endl;
	cerr << "-s --start      : playback start offset in seconds " << endl;
	cerr << "-l --length     : playback length in seconds " << endl;
	cerr << "-p --plugin     : output plugin name " << endl;
	cerr << "-r --repetitions: number of repetitions " << endl;
	cerr << "Available methods: " << endl;
	cerr << "MEDIAN_SUMBANDS" << endl;
	cerr << "MEDIAN_MULTIBANDS" << endl;
	cerr << "HISTO_SUMBANDS" << endl;
	cerr << "HISTO_SUMBANDSQ" << endl;
	
	cerr << "BOOMCHICK_WAVELET" << endl;
	cerr << "BOOMCHICK_FILTER" << endl;

	exit(1);
}


// Play soundfile given by sfName, msys contains the playback
// network of MarSystem objects
void tempo_medianMultiBands(string sfName, string label, string resName)
{
	MarSystemManager mng;


	// prepare network
	MarSystem *total = mng.create("Series", "src");
	total->addMarSystem(mng.create("Stereo2Mono", "s2m"));
	total->addMarSystem(mng.create("SoundFileSource", "src"));
	total->addMarSystem(mng.create("ShiftInput", "si"));

	total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
	total->addMarSystem(mng.create("WaveletBands", "wvbnds"));


	total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
	total->addMarSystem(mng.create("OnePole", "lpf"));
	total->addMarSystem(mng.create("Norm", "norm"));
	
	// total->addMarSystem(mng.create("Sum", "sum"));

	total->addMarSystem(mng.create("DownSampler", "ds"));
	total->addMarSystem(mng.create("AutoCorrelation", "acr"));
	total->addMarSystem(mng.create("Peaker", "pkr"));
	total->addMarSystem(mng.create("MaxArgMax", "mxr"));
	total->addMarSystem(mng.create("PeakPeriods2BPM", "p2bpm"));

  


	total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	// update the controls
	mrs_real srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();

	// input filename with hopSize/winSize
	mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
	mrs_natural hopSize = winSize / 16;

	total->updctrl("mrs_natural/inSamples", hopSize);
	total->updctrl("SoundFileSource/src/mrs_natural/pos", offset);
	total->updctrl("ShiftInput/si/mrs_natural/winSize", winSize);


	// wavelet filterbank envelope extraction controls
	total->updctrl("WaveletPyramid/wvpt/mrs_bool/forward", true);
	total->updctrl("OnePole/lpf/mrs_real/alpha", 0.99f);
	mrs_natural factor = 32;
	total->updctrl("DownSampler/ds/mrs_natural/factor", factor);

	// Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM

	mrs_natural pkinS = total->getctrl("Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
	mrs_real peakSpacing = ((mrs_natural)(srate * 60.0 / (factor *60.0)) -
							(mrs_natural)(srate * 60.0 / (factor*80.0))) / pkinS;
	mrs_natural peakStart = (mrs_natural)(srate * 60.0 / (factor * 180.0));
	mrs_natural peakEnd   = (mrs_natural)(srate * 60.0 / (factor * 40.0));
	total->updctrl("Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
	total->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.75);
	total->updctrl("Peaker/pkr/mrs_natural/peakStart", peakStart);
	total->updctrl("Peaker/pkr/mrs_natural/peakEnd", peakEnd);
	

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
	nChannels = total->getctrl("SoundFileSource/src/mrs_natural/onObservations")->to<mrs_natural>();

	total->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 4);
	total->updctrl("MaxArgMax/mxr/mrs_natural/interpolation", 1);
	
	
	total->updctrl("Peaker/pkr/mrs_natural/interpolation", 1);



	// playback offset & duration
	offset = (mrs_natural) (start * srate * nChannels);
	duration = (mrs_natural) (length * srate * nChannels);


	while (total->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
	{
		total->process(iwin, estimate);
		
		// convert highest peak to BPMs and add to vector
		for (int b=0; b < 4; b++)
		{
			// pitch = srate * 60.0 / (estimate(b,1) * factor);
			// bin = (mrs_natural) (pitch);
			bin = (mrs_natural)(estimate(b,1));
			// cout << "max bpm(" << b << ") = " << bin << endl;
			bpms.push_back(bin);
		}
		numPlayed++;
		wc ++;
		samplesPlayed += onSamples;
	}
		      
	// sort bpm estimates for median filtering
	sort(bpms.begin(), bpms.end());
	
	istringstream iss(label);
	float ground_truth_tempo; 
	iss >> ground_truth_tempo;
	float predicted_tempo;
	predicted_tempo = bpms[bpms.size()/2];

	float diff1 = fabs(predicted_tempo - ground_truth_tempo);
	float diff2 = fabs(predicted_tempo - 2 * ground_truth_tempo);
	float diff3 = fabs(2 * predicted_tempo - ground_truth_tempo);
	float diff4 = fabs(3 * predicted_tempo - ground_truth_tempo);
	float diff5 = fabs(3 * predicted_tempo - ground_truth_tempo);
	

	cout << sfName << "\t" << predicted_tempo << ":" << ground_truth_tempo <<  "---" << diff1 << ":" << diff2 << ":" << diff3 << ":" << diff4 << ":" << diff5 << endl;
	if (diff1 <= 1.0)
		correct_predictions++;
	if ((diff1 <= 1.0)||(diff2 <= 1.0)||(diff3 <= 1.0)||(diff4 <= 1.0)||(diff5 <= 1.0))
		correct_harmonic_predictions++;
	else 
    {
		if ((diff1 < diff2)&&(diff1 < diff3))
			total_differences += diff1;
		if ((diff2 < diff3)&&(diff1 < diff1))
			total_differences += diff2;
		if ((diff3 < diff2)&&(diff3 < diff1))
			total_differences += diff3;
		total_errors++;
    }
  
	total_instances++;


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
	{
		// Extra gain added for compensating the cleanup of the Norm Marsystem,
		// which used a 0.05 internal gain for some unknown reason.
		// \todo is this weird gain factor actually required?
		total->addMarSystem(mng.create("Gain", "normGain"));
		total->updctrl("Gain/normGain/mrs_real/gain", 0.05);
	}
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


	mrs_natural ifactor = 8;
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



	// wavelet filterbank envelope extraction controls
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








	while (total->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
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

	cout << "Estimated tempo = " << t1 << endl;

	cout << sfName << " " << t1 << " " << s1 << endl;

	delete total;
	delete total1;
}

void 
tempo_fluxBands(string sfName, string label, string resName) 
{
	cout << "Flux bands" << endl;
	
	MarSystemManager mng;
	
	MarSystem *big = mng.create("Series/big");
	
	MarSystem *accum = mng.create("Accumulator/accum");
	
	MarSystem *total = mng.create("Series/total");
	total->addMarSystem(mng.create("SoundFileSource", "src"));
	total->addMarSystem(mng.create("Stereo2Mono", "s2m"));
	total->addMarSystem(mng.create("ShiftInput", "si"));	
	total->addMarSystem(mng.create("Windowing", "windowing"));
	total->addMarSystem(mng.create("Spectrum", "spk"));
	total->addMarSystem(mng.create("PowerSpectrum", "pspk"));
	total->addMarSystem(mng.create("Flux", "flux"));
	accum->addMarSystem(total);



	big->addMarSystem(accum);
	big->addMarSystem(mng.create("AutoCorrelation", "acr"));
	big->addMarSystem(mng.create("BeatHistogram", "bhisto"));
	
	big->addMarSystem(mng.create("Peaker", "pkr"));
	big->addMarSystem(mng.create("MaxArgMax", "mxr"));


	big->updctrl("Peaker/pkr/mrs_natural/peakNeighbors", 10);
	big->updctrl("Peaker/pkr/mrs_real/peakSpacing", 0.1);
	big->updctrl("Peaker/pkr/mrs_natural/peakStart", 50);
	big->updctrl("Peaker/pkr/mrs_natural/peakEnd", 180);
	big->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.65);
	big->updctrl("Peaker/pkr/mrs_bool/peakHarmonics", true);


	accum->updctrl("Series/total/PowerSpectrum/pspk/mrs_string/spectrumType", "magnitude");
	accum->updctrl("Series/total/Flux/flux/mrs_string/mode", "DixonDAFX06");
	


	cout << "I am here" << endl;
	
	mrs_real srate = big->getctrl("Accumulator/accum/Series/total/SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
	cout << "I am here 2" << endl;

	big->updctrl("Accumulator/accum/mrs_natural/nTimes", 128);
	
	big->updctrl("BeatHistogram/bhisto/mrs_natural/startBin", 0);
	big->updctrl("BeatHistogram/bhisto/mrs_natural/endBin", 200);

	
	mrs_natural winSize = 256;
	mrs_natural hopSize = 128;

	big->updctrl("Accumulator/accum/Series/total/mrs_natural/inSamples", hopSize);
	big->updctrl("Accumulator/accum/Series/total/ShiftInput/si/mrs_natural/winSize", winSize);

	big->updctrl("Accumulator/accum/Series/total/SoundFileSource/src/mrs_string/filename", sfName);

	
	cout << "I am here 2" << endl;
	

	
	vector<mrs_real> bpms;
	mrs_real bin;

	ofstream ofs;
	ofs.open("accum.mpl");
	ofs << *big << endl;
	ofs.close();


	
	while (big->getctrl("Accumulator/accum/Series/total/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
	{
		big->tick();
		mrs_realvec estimate = big->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
		bin = estimate(1);
		bpms.push_back(bin);
	}
	


	
	istringstream iss(label);
	float ground_truth_tempo; 
	iss >> ground_truth_tempo;
	float predicted_tempo;
	predicted_tempo = bpms[bpms.size()-1];
	float diff1 = fabs(predicted_tempo - ground_truth_tempo);
	float diff2 = fabs(predicted_tempo - 2 * ground_truth_tempo);
	float diff3 = fabs(2 * predicted_tempo - ground_truth_tempo);
	float diff4 = fabs(3 * predicted_tempo - ground_truth_tempo);
	float diff5 = fabs(3 * predicted_tempo - ground_truth_tempo);
	

	cout << sfName << "\t" << predicted_tempo << ":" << ground_truth_tempo <<  "---" << diff1 << ":" << diff2 << ":" << diff3 << ":" << diff4 << ":" << diff5 << endl;
	if (diff1 <= 1.0)
		correct_predictions++;
	
	if (diff1 <= 0.04 * ground_truth_tempo)
		correct_mirex_predictions++;
	
	if ((diff1 <= 1.0)||(diff2 <= 1.0)||(diff3 <= 1.0)||(diff4 <= 1.0)||(diff5 <= 1.0))
		correct_harmonic_predictions++;
	else 
    {
		if ((diff1 < diff2)&&(diff1 < diff3))
			total_differences += diff1;
		if ((diff2 < diff3)&&(diff1 < diff1))
			total_differences += diff2;
		if ((diff3 < diff2)&&(diff3 < diff1))
			total_differences += diff3;
		total_errors++;
    }
  
	total_instances++;

	
	cout << "Correct Predictions = " << correct_predictions << "/" << total_instances << endl;
	cout << "Correct MIREX Predictions = " << correct_mirex_predictions << "/" << total_instances << endl;
	cout << "Correct Harmonic Predictions = " << correct_harmonic_predictions << "/" << total_instances << endl;
	cout << "Average error difference = " << total_differences << "/" << total_errors << "=" << total_differences / total_errors << endl;
	delete big;


}




void
tempo_histoSumBands(string sfName, string label, string resName)
{
	MarSystemManager mng;
	mrs_real srate = 0.0;


	// prepare network
	MarSystem *total = mng.create("Series", "src");
	total->addMarSystem(mng.create("SoundFileSource", "src"));
	total->addMarSystem(mng.create("Stereo2Mono", "s2m"));
	total->addMarSystem(mng.create("ShiftInput", "si"));
	total->addMarSystem(mng.create("AudioSink", "dest"));
	total->addMarSystem(mng.create("DownSampler", "dsr1"));
	total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));

	// implicit fanout
	total->addMarSystem(mng.create("WaveletBands", "wvbnds"));

	
	total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
	total->addMarSystem(mng.create("OnePole", "lpf"));
	total->addMarSystem(mng.create("Reverse", "reverse"));
	total->addMarSystem(mng.create("OnePole", "lpf1"));
	total->addMarSystem(mng.create("Norm", "norm"));
	
	
	// implicit fanin
	total->addMarSystem(mng.create("Sum", "sum"));
	total->addMarSystem(mng.create("DownSampler", "ds"));
	total->addMarSystem(mng.create("Delta", "delta"));
	total->addMarSystem(mng.create("AutoCorrelation", "acr"));
	total->addMarSystem(mng.create("BeatHistogram", "histo"));


	MarSystem* hfanout = mng.create("Fanout", "hfanout");
	hfanout->addMarSystem(mng.create("Gain", "id1"));
	hfanout->addMarSystem(mng.create("TimeStretch", "tsc1"));
	// hfanout->addMarSystem(mng.create("TimeStretch", "tsc2"));
	
	total->addMarSystem(hfanout);
	total->addMarSystem(mng.create("Sum", "hsum"));
	total->addMarSystem(mng.create("Peaker", "pkr"));
	total->addMarSystem(mng.create("MaxArgMax", "mxr"));
	
	// update the controls
	// input filename with hopSize/winSize
	total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();

	mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
	mrs_natural hopSize = winSize / 8;

	total->updctrl("mrs_natural/inSamples", hopSize);
	total->updctrl("ShiftInput/si/mrs_natural/winSize", winSize);
	total->updctrl("DownSampler/dsr1/mrs_natural/factor", 8);
	
	// wavelt filterbank envelope extraction controls
	total->updctrl("WaveletPyramid/wvpt/mrs_bool/forward", true);
	total->updctrl("OnePole/lpf/mrs_real/alpha", 0.99f);
	total->updctrl("OnePole/lpf1/mrs_real/alpha", 0.99f);
	mrs_natural factor = 2;
	total->updctrl("DownSampler/ds/mrs_natural/factor", factor);
	factor = 16;
	
	total->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 2);
	
	total->updctrl("Peaker/pkr/mrs_natural/peakNeighbors", 10);
	total->updctrl("Peaker/pkr/mrs_real/peakSpacing", 0.1);
	total->updctrl("Peaker/pkr/mrs_natural/peakStart", 50);
	total->updctrl("Peaker/pkr/mrs_natural/peakEnd", 180);
	
	total->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.65);
	total->updctrl("Peaker/pkr/mrs_bool/peakHarmonics", true);
	
	total->updctrl("BeatHistogram/histo/mrs_natural/startBin", 0);
	total->updctrl("BeatHistogram/histo/mrs_natural/endBin", 200);
	total->updctrl("Fanout/hfanout/TimeStretch/tsc1/mrs_real/factor", 0.5);
	// total->updctrl("Fanout/hfanout/TimeStretch/tsc2/mrs_real/factor", 0.33);

	

	total->linkctrl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
	total->linkctrl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
	total->linkctrl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

	// prepare vectors for processing
	realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
				 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
	realvec estimate(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
					 total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());




	mrs_real bin;
	mrs_natural onSamples;

	int numPlayed =0;
	mrs_natural wc=0;
	mrs_natural samplesPlayed = 0;
	mrs_natural repeatId = 1;

	// vector of bpm estimate used to calculate median
	vector<mrs_real> bpms;
	onSamples = total->getctrl("ShiftInput/si/mrs_natural/onSamples")->to<mrs_natural>();

	if (pluginName != EMPTYSTRING)
	{
		ofstream ofs;
		ofs.open(pluginName.c_str());
		ofs << *total << endl;
		ofs.close();
		pluginName = EMPTYSTRING;
	}


	while (total->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
	// for (int i=0; i< 400; i++)
    {
		total->process(iwin, estimate);
		bin = estimate(1);
		bpms.push_back(bin);
    }

	
	

  
	istringstream iss(label);
	float ground_truth_tempo; 
	iss >> ground_truth_tempo;
	float predicted_tempo;
	predicted_tempo = bpms[bpms.size()-1];
	float diff1 = fabs(predicted_tempo - ground_truth_tempo);
	float diff2 = fabs(predicted_tempo - 2 * ground_truth_tempo);
	float diff3 = fabs(2 * predicted_tempo - ground_truth_tempo);
	float diff4 = fabs(3 * predicted_tempo - ground_truth_tempo);
	float diff5 = fabs(3 * predicted_tempo - ground_truth_tempo);
	

	cout << sfName << "\t" << predicted_tempo << ":" << ground_truth_tempo <<  "---" << diff1 << ":" << diff2 << ":" << diff3 << ":" << diff4 << ":" << diff5 << endl;
	if (diff1 <= 1.0)
		correct_predictions++;
	
	if (diff1 <= 0.04 * ground_truth_tempo)
		correct_mirex_predictions++;
	
	if ((diff1 <= 1.0)||(diff2 <= 1.0)||(diff3 <= 1.0)||(diff4 <= 1.0)||(diff5 <= 1.0))
		correct_harmonic_predictions++;
	else 
    {
		if ((diff1 < diff2)&&(diff1 < diff3))
			total_differences += diff1;
		if ((diff2 < diff3)&&(diff1 < diff1))
			total_differences += diff2;
		if ((diff3 < diff2)&&(diff3 < diff1))
			total_differences += diff3;
		total_errors++;
    }
  
	total_instances++;

	
	cout << "Correct Predictions = " << correct_predictions << "/" << total_instances << endl;
	cout << "Correct MIREX Predictions = " << correct_mirex_predictions << "/" << total_instances << endl;
	cout << "Correct Harmonic Predictions = " << correct_harmonic_predictions << "/" << total_instances << endl;
	cout << "Average error difference = " << total_differences << "/" << total_errors << "=" << total_differences / total_errors << endl;
	delete total;
}



void
tempo_histoSumBandsQ(string sfName, string label, string resName)
{
	MarSystemManager mng;
	mrs_real srate = 0.0;


	// prepare network
	MarSystem *total = mng.create("Series", "src");
	total->addMarSystem(mng.create("SoundFileSource", "src"));
	total->addMarSystem(mng.create("Stereo2Mono", "s2m"));
	total->addMarSystem(mng.create("ShiftInput", "si"));
	total->addMarSystem(mng.create("AudioSink", "dest"));
	total->addMarSystem(mng.create("DownSampler", "dsr1"));
	total->addMarSystem(mng.create("ConstQFiltering", "cqf"));
	
	// total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
	// total->addMarSystem(mng.create("WaveletBands", "wvbnds"));

	
	total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
	total->addMarSystem(mng.create("OnePole", "lpf"));
	total->addMarSystem(mng.create("Reverse", "reverse"));
	total->addMarSystem(mng.create("OnePole", "lpf1"));
	total->addMarSystem(mng.create("Norm", "norm"));
	
	
	// implicit fanin
	total->addMarSystem(mng.create("Sum", "sum"));
	total->addMarSystem(mng.create("DownSampler", "ds"));
	total->addMarSystem(mng.create("Delta", "delta"));
	total->addMarSystem(mng.create("AutoCorrelation", "acr"));
	total->addMarSystem(mng.create("BeatHistogram", "histo"));
	total->addMarSystem(mng.create("Peaker", "pkr"));
	total->addMarSystem(mng.create("MaxArgMax", "mxr"));

	// update the controls
	// input filename with hopSize/winSize
	total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();

	mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
	mrs_natural hopSize = winSize / 8;

	total->updctrl("mrs_natural/inSamples", hopSize);
	total->updctrl("ShiftInput/si/mrs_natural/winSize", winSize);

	total->updctrl("DownSampler/dsr1/mrs_natural/factor", 8);

	total->updctrl("ConstQFiltering/cqf/mrs_natural/channels", 5);
	total->updctrl("ConstQFiltering/cqf/mrs_real/qValue", 4.0);
	total->updctrl("ConstQFiltering/cqf/mrs_real/lowFreq", 41.2);
	total->updctrl("ConstQFiltering/cqf/mrs_real/highFreq", 1318.5);
	total->updctrl("ConstQFiltering/cqf/mrs_natural/width", winSize/ 8);
	
	
	// wavelt filterbank envelope extraction controls
	// total->updctrl("WaveletPyramid/wvpt/mrs_bool/forward", true);
	total->updctrl("OnePole/lpf/mrs_real/alpha", 0.99f);
	total->updctrl("OnePole/lpf1/mrs_real/alpha", 0.99f);
	mrs_natural factor = 2;
	total->updctrl("DownSampler/ds/mrs_natural/factor", factor);
	factor = 16;
	
	total->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 2);
	
	total->updctrl("Peaker/pkr/mrs_natural/peakNeighbors", 10);
	total->updctrl("Peaker/pkr/mrs_real/peakSpacing", 0.1);
	total->updctrl("Peaker/pkr/mrs_natural/peakStart", 50);
	total->updctrl("Peaker/pkr/mrs_natural/peakEnd", 180);
	
	total->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.65);
	total->updctrl("Peaker/pkr/mrs_bool/peakHarmonics", true);
	
	total->updctrl("BeatHistogram/histo/mrs_natural/startBin", 0);
	total->updctrl("BeatHistogram/histo/mrs_natural/endBin", 200);

	total->linkctrl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
	total->linkctrl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
	total->linkctrl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

	// prepare vectors for processing
	realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
				 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
	realvec estimate(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
					 total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());




	mrs_real bin;
	mrs_natural onSamples;

	int numPlayed =0;
	mrs_natural wc=0;
	mrs_natural samplesPlayed = 0;
	mrs_natural repeatId = 1;

	// vector of bpm estimate used to calculate median
	vector<mrs_real> bpms;
	onSamples = total->getctrl("ShiftInput/si/mrs_natural/onSamples")->to<mrs_natural>();

	if (pluginName != EMPTYSTRING)
	{
		ofstream ofs;
		ofs.open(pluginName.c_str());
		ofs << *total << endl;
		ofs.close();
		pluginName = EMPTYSTRING;
	}


	while (total->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
	// for (int i=0; i< 400; i++)
    {
		total->process(iwin, estimate);
		bin = estimate(1);
		bpms.push_back(bin);
    }
  
	istringstream iss(label);
	float ground_truth_tempo; 
	iss >> ground_truth_tempo;
	float predicted_tempo;
	predicted_tempo = bpms[bpms.size()-1];
	float diff1 = fabs(predicted_tempo - ground_truth_tempo);
	float diff2 = fabs(predicted_tempo - 2 * ground_truth_tempo);
	float diff3 = fabs(2 * predicted_tempo - ground_truth_tempo);
	float diff4 = fabs(3 * predicted_tempo - ground_truth_tempo);
	float diff5 = fabs(3 * predicted_tempo - ground_truth_tempo);
	

	cout << sfName << "\t" << predicted_tempo << ":" << ground_truth_tempo <<  "---" << diff1 << ":" << diff2 << ":" << diff3 << ":" << diff4 << ":" << diff5 << endl;
	if (diff1 <= 1.0)
		correct_predictions++;
	if ((diff1 <= 1.0)||(diff2 <= 1.0)||(diff3 <= 1.0)||(diff4 <= 1.0)||(diff5 <= 1.0))
		correct_harmonic_predictions++;
	else 
    {
		if ((diff1 < diff2)&&(diff1 < diff3))
			total_differences += diff1;
		if ((diff2 < diff3)&&(diff1 < diff1))
			total_differences += diff2;
		if ((diff3 < diff2)&&(diff3 < diff1))
			total_differences += diff3;
		total_errors++;
    }
  
	total_instances++;

	
	cout << "Correct Predictions = " << correct_predictions << "/" << total_instances << endl;
	cout << "Correct Harmonic Predictions = " << correct_harmonic_predictions << "/" << total_instances << endl;
	cout << "Average error difference = " << total_differences << "/" << total_errors << "=" << total_differences / total_errors << endl;
	delete total;
}




void
tempo_medianSumBands(string sfName, string label, string resName)
{
	MarSystemManager mng;
	mrs_natural nChannels;
	mrs_real srate;

	// prepare network
	MarSystem *total = mng.create("Series", "src");
	total->addMarSystem(mng.create("SoundFileSource", "src"));
	total->addMarSystem(mng.create("Stereo2Mono", "s2m"));
	total->addMarSystem(mng.create("ShiftInput", "si"));
	total->addMarSystem(mng.create("DownSampler", "dsr1"));
	total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));
	total->addMarSystem(mng.create("WaveletBands", "wvbnds"));

	// envelope extraction
	total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
	total->addMarSystem(mng.create("OnePole", "lpf"));
	total->addMarSystem(mng.create("Reverse", "reverse"));
	total->addMarSystem(mng.create("OnePole", "lpf1"));
	total->addMarSystem(mng.create("Norm", "norm"));
	

	total->addMarSystem(mng.create("Sum", "sum"));
	total->addMarSystem(mng.create("DownSampler", "ds"));
	total->addMarSystem(mng.create("Delta", "delta"));
	
	total->addMarSystem(mng.create("AutoCorrelation", "acr"));
	total->addMarSystem(mng.create("BeatHistogram", "histo"));	
	total->addMarSystem(mng.create("Peaker", "pkr"));
	total->addMarSystem(mng.create("MaxArgMax", "mxr"));
	
	// update the controls
	// input filename with hopSize/winSize

	total->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	srate = total->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
	
	mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
	mrs_natural hopSize = winSize / 8;

	total->updctrl("mrs_natural/inSamples", hopSize);
	total->updctrl("ShiftInput/si/mrs_natural/winSize", winSize);
	total->updctrl("DownSampler/dsr1/mrs_natural/factor", 8);
	
	
	// wavelt filterbank envelope extraction controls
	total->updctrl("WaveletPyramid/wvpt/mrs_bool/forward", true);
	total->updctrl("OnePole/lpf/mrs_real/alpha", 0.99f);
	total->updctrl("OnePole/lpf1/mrs_real/alpha", 0.99f);
	mrs_natural factor = 2;
	total->updctrl("DownSampler/ds/mrs_natural/factor", factor);
	factor = 16;
	
	
	total->updctrl("MaxArgMax/mxr/mrs_natural/nMaximums", 2);

	total->updctrl("Peaker/pkr/mrs_natural/peakNeighbors", 10);
	total->updctrl("Peaker/pkr/mrs_real/peakSpacing", 0.1);
	total->updctrl("Peaker/pkr/mrs_natural/peakStart", 50);
	total->updctrl("Peaker/pkr/mrs_natural/peakEnd", 180);

	total->updctrl("Peaker/pkr/mrs_real/peakStrength", 0.65);
	total->updctrl("Peaker/pkr/mrs_bool/peakHarmonics", true);

	total->updctrl("BeatHistogram/histo/mrs_natural/startBin", 0);
	total->updctrl("BeatHistogram/histo/mrs_natural/endBin", 200);

	
	total->linkctrl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
	total->linkctrl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
	total->linkctrl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

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

  
	int k=0;
   

	ofstream ofs;
	ofs.open("tempo.mpl");
	ofs << *total << endl;
	ofs.close();

	while (total->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
    {
      
		total->process(iwin, estimate);
		bin = (mrs_natural) estimate(1);
		
		// cout << "max bpm = " << bin << endl;
		bpms.push_back(bin);

		numPlayed++;
		wc ++;
		samplesPlayed += onSamples;
		k++;
    }
  
	// sort bpm estimates for median filtering
	sort(bpms.begin(), bpms.end());



	istringstream iss(label);
	float ground_truth_tempo; 
	iss >> ground_truth_tempo;
	float predicted_tempo;
	predicted_tempo = bpms[bpms.size()/2];
	float diff1 = fabs(predicted_tempo - ground_truth_tempo);
	float diff2 = fabs(predicted_tempo - 2 * ground_truth_tempo);
	float diff3 = fabs(2 * predicted_tempo - ground_truth_tempo);
	float diff4 = fabs(3 * predicted_tempo - ground_truth_tempo);
	float diff5 = fabs(3 * predicted_tempo - ground_truth_tempo);
	

	cout << sfName << "\t" << predicted_tempo << ":" << ground_truth_tempo <<  "---" << diff1 << ":" << diff2 << ":" << diff3 << ":" << diff4 << ":" << diff5 << endl;
	if (diff1 <= 1.0)
		correct_predictions++;
	if ((diff1 <= 1.0)||(diff2 <= 1.0)||(diff3 <= 1.0)||(diff4 <= 1.0)||(diff5 <= 1.0))
		correct_harmonic_predictions++;
	else 
    {
		if ((diff1 < diff2)&&(diff1 < diff3))
			total_differences += diff1;
		if ((diff2 < diff3)&&(diff1 < diff1))
			total_differences += diff2;
		if ((diff3 < diff2)&&(diff3 < diff1))
			total_differences += diff3;
		total_errors++;
    }
	total_instances++;

	cout << "Correct Predictions = " << correct_predictions << "/" << total_instances << endl;
	cout << "Correct Harmonic Predictions = " << correct_harmonic_predictions << "/" << total_instances << endl;
	cout << "Average error difference = " << total_differences << "/" << total_errors << "=" << total_differences / total_errors << endl;

  


	// Output to file
  
	/* 
	   ofstream oss(resName.c_str());
	   oss << bpms[bpms.size()/2] << endl;
	   cerr << "Played " << wc << " slices of " << onSamples << " samples"
       << endl;
	   cout << "Processed " << sfName << endl;
	   cout << "Wrote " << resName << endl;
	   delete total;
	*/ 

  

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
	{
		// Extra gain added for compensating the cleanup of the Norm Marsystem,
		// which used a 0.05 internal gain for some unknown reason.
		// \todo is this weird gain factor actually required?
		total->addMarSystem(mng.create("Gain", "normGain"));
		total->updctrl("Gain/normGain/mrs_real/gain", 0.05);
	}

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

	while (total->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
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

	while(playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_bool/hasData")->to<mrs_bool>())
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

	while (total->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
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
	while(playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_bool/hasData")->to<mrs_bool>())
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



float 
refine(string sfName, float predicted_tempo)
{
	return predicted_tempo;
	
	cout << "Refining tempo for " << sfName << endl;

	predicted_tempo ++;
	
	cout << "Initial tempo = " << predicted_tempo << endl;
	
	MarSystemManager mng;
	
	MarSystem *net = mng.create("Series/net");
	net->addMarSystem(mng.create("SoundFileSource/src"));
	net->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	mrs_real srate = net->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();
	mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
	
	net->updctrl("mrs_natural/inSamples", winSize);

	ofstream ofs;
	ofs.open("net.mpl");
	ofs << *net << endl;
	
	ofs.close();
	
	
	net->tick();
	const mrs_realvec& data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

	
	float max = 0.0;
	int max_i;
	
	for (int i=0; i < winSize/4; i++) 
	{
		if (fabs(data(0,i)) >= max)
		{
			max_i = i;
			max = fabs(data(0,i));
		}
		
	}
	int start;
	int end;
	
	start = (int)((srate * 60.0) / (predicted_tempo + 2.0));
	end = (int)((srate * 60.0) / (predicted_tempo - 2.0));
	
	
	max = 0.0;
	int max_i2;
	
	
	for (int i = max_i + start; i <= max_i + end; i++)
	{
		if (data(0,i) >= max)
		{
			max = data(0,i);
			max_i2 = i;
		}
	}

	cout << "--------" << endl;
	cout << max_i << endl;
	cout << max_i + start << endl;
	cout << max_i + end << endl;
	cout << max_i2 << endl;
	

	net->tick();
	const mrs_realvec& data2 = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

	
	max = 0.0;
	
	for (int i=0; i < winSize/4; i++) 
	{
		if (fabs(data2(0,i)) >= max)
		{
			max_i = i;
			max = fabs(data2(0,i));
		}
		
	}

	
	start = (int)((srate * 60.0) / (predicted_tempo + 2.0));
	end = (int)((srate * 60.0) / (predicted_tempo - 2.0));
	
	
	max = 0.0;
	max_i2;
	
	
	for (int i = max_i + start; i <= max_i + end; i++)
	{
		if (data2(0,i) >= max)
		{
			max = data2(0,i);
			max_i2 = i;
		}
	}
	cout << "--------" << endl;
	
	cout << max_i << endl;
	cout << max_i + start << endl;
	cout << max_i + end << endl;
	cout << max_i2 << endl;
	
	



	return srate * 60.0 / (max_i2 - max_i);
	
}



void
tempo_ibt(string sfName, string label, string outputTxt)
{
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

	//if(sonicOut)
	//	onsetdetectionfunction->addMarSystem(mng.create("SonicVisualiserSink", "sonicsink"));
							
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
	for(int i = 0; i < NR_AGENTS; i++)
	{
		ostringstream oss;
		oss << "agent" << i;
		agentpool->addMarSystem(mng.create("BeatAgent", oss.str()));
	}

	beattracker->addMarSystem(agentpool);
	beattracker->addMarSystem(mng.create("BeatReferee", "br"));
	if(output != "none") 
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
	if(audiofileopt)
		IBTsystem->addMarSystem(mng.create("SoundFileSink", "fdest"));


	///////////////////////////////////////////////////////////////////////////////////////
	//link controls
	///////////////////////////////////////////////////////////////////////////////////////
	IBTsystem->linkctrl("mrs_bool/hasData", 
						"Fanout/beatmix/Series/audioflow/SoundFileSource/src/mrs_bool/hasData");

	//Link LookAheadSamples used in PeakerOnset for compensation when retriving the actual initial OnsetTimes
	tempoinduction->linkctrl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_natural/lookAheadSamples", 
							 "Fanout/tempohypotheses/Series/phase/OnsetTimes/OnsetTimes/mrs_natural/lookAheadSamples");

	//Pass hypotheses matrix (from tempoinduction stage) to PhaseLock
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_realvec/beatHypotheses", 
						  "FlowThru/tempoinduction/mrs_realvec/innerOut");
	
	//Pass initital hypotheses to BeatReferee
	beattracker->linkctrl("BeatReferee/br/mrs_realvec/beatHypotheses", 
						  "FlowThru/initialhypotheses/mrs_realvec/innerOut");

	//PhaseLock nr of BPM hypotheses = nr MaxArgMax from ACF
	beattracker->linkctrl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nMaximums", 
						  "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPeriods");
	//TempoHypotheses nr of BPMs = nr MaxArgMax from ACF
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPeriods",  
						  "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPeriodHyps");
	//OnsetTimes nr of BPMs = nr MaxArgMax from ACF (this is to avoid FanOut crash!)
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPeriodHyps", 
						  "FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/OnsetTimes/mrs_natural/nPeriods");
	
	//PhaseLock nr of Phases per BPM = nr of OnsetTimes considered
	beattracker->linkctrl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/OnsetTimes/mrs_natural/n1stOnsets", 
						  "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPhases");
	//TempoHypotheses nr of Beat hypotheses = nr of OnsetTimes considered
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPhases", 
						  "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPhasesPerPeriod");
	//nr of MaxArgMax Phases per BPM = nr OnsetTimes considered (this is to avoid FanOut crash!)
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPhasesPerPeriod", 
						  "FlowThru/tempoinduction/Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/fanoutLength");

	//Pass enabled (muted) BeatAgents (from FanOut) to the BeatReferee
	beattracker->linkctrl("Fanout/agentpool/mrs_realvec/muted", "BeatReferee/br/mrs_realvec/mutedAgents");
	//Pass tempohypotheses Fanout muted vector to the BeatReferee, for disabling induction after induction timming
	beattracker->linkctrl("FlowThru/tempoinduction/Fanout/tempohypotheses/mrs_realvec/muted", 
						  "BeatReferee/br/mrs_realvec/inductionEnabler");

	//Link agentControl matrix from the BeatReferee to each agent in the pool
	for(int i = 0; i < NR_AGENTS; i++)
	{
		ostringstream oss;
		oss << "agent" << i;
		beattracker->linkctrl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_realvec/agentControl", 
							  "BeatReferee/br/mrs_realvec/agentControl");
	}

	//Defines tempo induction time after which the BeatAgents' hypotheses are populated:
	//PhaseLock timming = induction time
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime", 
						  "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime");	
	//BeatReferee timming = induction time
	beattracker->linkctrl("BeatReferee/br/mrs_natural/inductionTime", 
						  "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime");

	//Link score function from BeatAgent to PhaseLock
	beattracker->linkctrl("Fanout/agentpool/BeatAgent/agent0/mrs_string/scoreFunc", 
						  "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/scoreFunc");

	//Link BPM conversion parameters to BeatReferee:
	beattracker->linkctrl("BeatReferee/br/mrs_natural/hopSize", "mrs_natural/inSamples");

	//Link BPM conversion parameters to TempoHypotheses
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/hopSize", 
						  "BeatReferee/br/mrs_natural/hopSize");
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs", 
						  "BeatReferee/br/mrs_real/srcFs");

	//Link TickCounter from BeatRefree -> for updating IBT's timer
	beattracker->linkctrl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/OnsetTimes/mrs_natural/tickCount",
						  "BeatReferee/br/mrs_natural/tickCount");
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/tickCount",	
						  "FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/OnsetTimes/mrs_natural/tickCount");
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/tickCount", 
						  "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/tickCount");	
	
	//Link Beat Output Sink parameters with the used ones:
	if(output !=  "none") 
	{
		beattracker->linkctrl("BeatTimesSink/sink/mrs_natural/hopSize", 
							  "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/hopSize");
		beattracker->linkctrl("BeatTimesSink/sink/mrs_real/srcFs", 
							  "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs");
		beattracker->linkctrl("BeatTimesSink/sink/mrs_natural/winSize", 
							  "Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize");
		beattracker->linkctrl("BeatTimesSink/sink/mrs_natural/tickCount", 
							  "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/tickCount");
	}
	
	/*
	//Link SonicVisualiserSink parameters with the used ones:
	if(sonicOut)
	{
	beattracker->linkctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_natural/hopSize", 
	"BeatReferee/br/mrs_natural/hopSize");
	beattracker->linkctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_real/srcFs", 
	"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs");
	}
	*/

	//link beatdetected with noise ADSR -> for clicking when beat:
	IBTsystem->linkctrl("Fanout/beatmix/Series/audioflow/FlowThru/beattracker/BeatReferee/br/mrs_real/beatDetected", 
						"Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/nton");

	

	///////////////////////////////////////////////////////////////////////////////////////
	// update controls
	///////////////////////////////////////////////////////////////////////////////////////
	audioflow->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
	mrs_natural inputSize = audioflow->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>(); //sound input file size (in samples)
	
	//best result till now are using dB power Spectrum!
	beattracker->updctrl("Series/onsetdetectionfunction/PowerSpectrum/pspk/mrs_string/spectrumType", "magnitude");

	beattracker->updctrl("Series/onsetdetectionfunction/Flux/flux/mrs_string/mode", "DixonDAFX06");

	beattracker->updctrl("mrs_natural/inSamples", HOPSIZE);
	beattracker->updctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize", WINSIZE);

	mrs_real fsSrc = beattracker->getctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_real/israte")->to<mrs_real>();

	//induction time (in nr. of ticks) -> -1 because starting index on accumulator is 0 and it finnishes at accSize-1
	//So IBT's tick time notion starts also on 0 and finnishes on sound_file_size(in_frames)-1.
	mrs_natural inductionTickCount = ((mrs_natural) ceil((induction_time * fsSrc) / HOPSIZE)) -1;
	//to avoid induction time greater than input file size
	//(in this case the induction time will equal the file size)
	if((inputSize / HOPSIZE) < inductionTickCount)
		inductionTickCount = (inputSize / HOPSIZE) -1;

	beattracker->updctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime", inductionTickCount);

	mrs_natural metricalChangeTime = inputSize / HOPSIZE; //if metricalChangeTime = -1 it shall equalize the full input file size (in ticks)
	if(metrical_change_time != -1.0)
		metricalChangeTime = ((mrs_natural) (metrical_change_time * fsSrc) / HOPSIZE) + 1; //allowed metrical change time (in nr. of ticks)
	if(metricalChangeTime < 0.0) //if negative value -> default (=5.0secs)
		metricalChangeTime = (5 * fsSrc) / HOPSIZE;
	
	//Size of accumulator equals inductionTime + 1 -> [0, inductionTime]
	beattracker->updctrl("ShiftInput/acc/mrs_natural/winSize", inductionTickCount+1);

	mrs_natural pkinS = tempoinduction->getctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
	mrs_natural peakEnd = (mrs_natural)((60.0 * fsSrc)/(MIN_BPM * HOPSIZE)); //MinBPM (in frames)
	mrs_natural peakStart = (mrs_natural) ((60.0 * fsSrc)/(MAX_BPM * HOPSIZE));  //MaxBPM (in frames)
	mrs_real peakSpacing = ((mrs_natural) (fsSrc/HOPSIZE) * (1.0-(60.0/64.0))) / (pkinS * 1.0); //spacing between peaks
	//mrs_real peakSpacing = ((mrs_natural) (peakEnd-peakStart) / (2*BPM_HYPOTHESES)) / (pkinS * 1.0);  //nrPeaks <= 2*nrBPMs

	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakStrength", 0.9); //0.75
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakStart", peakStart);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakEnd", peakEnd);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakGain", 2.0);
	

	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nMaximums", BPM_HYPOTHESES);

	mrs_natural lookAheadSamples = 18; //multiple of 3
	mrs_real thres = 1.75; //1.75

	tempoinduction->updctrl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_natural/lookAheadSamples", lookAheadSamples);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_real/threshold", thres);
	
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/phase/OnsetTimes/OnsetTimes/mrs_natural/n1stOnsets", PHASE_HYPOTHESES);
	
	//Pass chosen score_function to each BeatAgent in the pool:
	for(int i = 0; i < NR_AGENTS; i++)
	{
		ostringstream oss, oss2;
		oss << "agent" << i;
		oss2 << "Agent" << i;
		beattracker->updctrl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_string/scoreFunc", score_function);

		beattracker->updctrl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_real/lftOutterMargin", LFT_OUTTER_MARGIN);
		beattracker->updctrl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_real/rgtOutterMargin", RGT_OUTTER_MARGIN);
		beattracker->updctrl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_real/innerMargin", INNER_MARGIN);
		
		//THIS IS TO REMOVE -> SEE INOBSNAMES IN BEATAGENT!!
		beattracker->updctrl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_string/identity", oss2.str());
	}

	beattracker->updctrl("BeatReferee/br/mrs_real/srcFs", fsSrc);
	beattracker->updctrl("BeatReferee/br/mrs_natural/minTempo", MIN_BPM);
	beattracker->updctrl("BeatReferee/br/mrs_natural/maxTempo", MAX_BPM);
	beattracker->updctrl("BeatReferee/br/mrs_real/obsoleteFactor", OBSOLETE_FACTOR);
	beattracker->updctrl("BeatReferee/br/mrs_real/childrenScoreFactor", CHILDREN_SCORE_FACTOR);
	beattracker->updctrl("BeatReferee/br/mrs_real/bestFactor", BEST_FACTOR);
	beattracker->updctrl("BeatReferee/br/mrs_real/corFactor", CORRECTION_FACTOR);
	beattracker->updctrl("BeatReferee/br/mrs_real/child1Factor", (mrs_real) CHILD1_FACTOR);
	beattracker->updctrl("BeatReferee/br/mrs_real/child2Factor", (mrs_real) CHILD2_FACTOR);
	beattracker->updctrl("BeatReferee/br/mrs_real/child3Factor", (mrs_real) CHILD3_FACTOR);
	beattracker->updctrl("BeatReferee/br/mrs_natural/eqPeriod", EQ_PERIOD);
	beattracker->updctrl("BeatReferee/br/mrs_natural/eqPhase", EQ_PHASE);
	beattracker->updctrl("BeatReferee/br/mrs_natural/metricalChangeTime", metricalChangeTime);
	beattracker->updctrl("BeatReferee/br/mrs_bool/backtrace", backtraceopt);

	ostringstream path;
	FileName outputFile(sfName);
	//if no outputTxt dir defined -> exec dir is assumed:
	if(outputTxt == "")
		path << outputFile.nameNoExt();
	else 
	{
		path.str("");
		mrs_natural loc;
		loc = outputTxt.rfind(".txt", outputTxt.length()-1);
		
		if(loc == -1) //if only output dir defined -> append filename:
			path << outputTxt << outputFile.nameNoExt();
		else
		{
			outputTxt = outputTxt.substr(0, loc);
			path << outputTxt;
		}
	}

	
	/*
	//SonicVisualiser Controls:
	if(sonicOut)
	{
	beattracker->updctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_string/mode", "frames");
	beattracker->updctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_string/destFileName", path.str() + "_onsetFunction.txt");
	if(backtraceopt)
	beattracker->updctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_natural/offset", inductionTickCount);
	else
	beattracker->updctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_natural/offset", 0);
	}
	*/

	//set audio/onset resynth balance and ADSR params for onset sound
	IBTsystem->updctrl("Fanout/beatmix/Series/audioflow/Gain/gainaudio/mrs_real/gain", 0.6);
	IBTsystem->updctrl("Fanout/beatmix/Series/beatsynth/Gain/gainbeats/mrs_real/gain", 1.2);
	IBTsystem->updctrl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/aTarget", 1.0);
 	IBTsystem->updctrl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/aTime", WINSIZE/80/fsSrc);
 	IBTsystem->updctrl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/susLevel", 0.0);
 	IBTsystem->updctrl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/dTime", WINSIZE/4/fsSrc);

	//for saving file with audio+clicks (on beats):
	if(audiofileopt)
	{
		IBTsystem->updctrl("SoundFileSink/fdest/mrs_string/filename", path.str() + "_beats.wav");
		cout << "Audiofile w/ beats being saved: " << path.str() << "_beats.wav" << endl;
	}

	//ostringstream onsetFunction [-> for inputing sonicvisualiser spectral flux];
	//onsetFunction << "C:\\Users\\João Lobato\\Desktop\\onsetFunctions\\" << outputFile.nameNoExt() << "_vamp_vamp-aubio_aubioonset_detectionfunction.csv";

	//MATLAB Engine inits
	//used for autocorrelation.m
	//mrs_natural winSize = WINSIZE;
	//mrs_natural hopSize = HOPSIZE;
	//MATLAB_EVAL("clear;");
	//MATLAB_EVAL("FluxTS = [];");
	//MATLAB_EVAL("srcAudio = [];");
	//MATLAB_EVAL("FinalBeats=[];");
	/*
	  MATLAB_PUT(induction_time, "timmbing");
	  MATLAB_PUT(fsSrc, "SrcFs");
	  MATLAB_PUT(inductionTickCount, "inductionTickCount");
	  MATLAB_PUT(winSize, "winSize");
	  MATLAB_PUT(hopSize, "hopSize");
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
	inputSize = (inputSize / HOPSIZE); //inputSize in ticks

	//suming inductionTickCount because with backtrace it will analyse two times the inductionWindow
	if(backtraceopt)
		inputSize += inductionTickCount; 

	//while(IBTsystem->getctrl("mrs_bool/hasData")->to<mrs_bool>())


	ofstream ofs;
	ofs.open("tempo.mpl");
	ofs << *IBTsystem << endl;
	ofs.close();

	while(frameCount <= inputSize)
	{	
		//cout << "FrameCount: " << frameCount << endl;
		IBTsystem->tick();

		if(frameCount == 1)
		{
			if(audioopt && !backtraceopt)
			{
				IBTsystem->updctrl("AudioSink/output/mrs_bool/initAudio", true);
				cout << "Playing Audio........" << endl;
			}
		}

		//Just after induction:
		if(frameCount == inductionTickCount)
		{	
			//for playing audio (with clicks on beats):
		  
			if(backtraceopt)
			{
				//Restart reading audio file
				audioflow->updctrl("SoundFileSource/src/mrs_natural/pos", 0);
				cout << "Backtraced!" << endl;
				if(audioopt)
					IBTsystem->updctrl("AudioSink/output/mrs_bool/initAudio", true);
			}
			if(audioopt)
				cout << "Playing audio with beat taps........" << endl;
			
		}
		//Display percentage of processing complete...
		//printf("  %d % \r", (mrs_natural) frameCount*100/inputSize);
		//cout << (mrs_natural) frameCount*100/inputSize << "%" << endl;
		
		frameCount++;
	}


	istringstream iss(label);
	float ground_truth_tempo; 
	iss >> ground_truth_tempo;
	float predicted_tempo;
	predicted_tempo = beattracker->getctrl("BeatTimesSink/sink/mrs_real/tempo")->to<mrs_real>();


	float diff1 = fabs(predicted_tempo - ground_truth_tempo);
	float diff2 = fabs(predicted_tempo - 2 * ground_truth_tempo);
	float diff3 = fabs(2 * predicted_tempo - ground_truth_tempo);
	

	cout << sfName << "\t" << predicted_tempo << ":" << ground_truth_tempo <<  "---" << diff1 << ":" << diff2 << ":" << diff3 << endl;
	predicted_tempo = refine(sfName, predicted_tempo);

	
	diff1 = fabs(predicted_tempo - ground_truth_tempo);
	diff2 = fabs(predicted_tempo - 2 * ground_truth_tempo);
	diff3 = fabs(2 * predicted_tempo - ground_truth_tempo);

	

	cout << sfName << "\t" << predicted_tempo << ":" << ground_truth_tempo <<  "---" << diff1 << ":" << diff2 << ":" << diff3 << endl;
	

	
	if (diff1 <= 1.0)
		correct_predictions++;
	if ((diff1 <= 1.0)||(diff2 <= 1.0)||(diff3 <= 1.0))
		correct_harmonic_predictions++;
	else 
	{
	    if ((diff1 < diff2)&&(diff1 < diff3))
			total_differences += diff1;
	    if ((diff2 < diff3)&&(diff1 < diff1))
			total_differences += diff2;
	    if ((diff3 < diff2)&&(diff3 < diff1))
			total_differences += diff3;
	    total_errors++;
	}

	total_instances++;
	
	// cout << "Finish!" << endl;
  
  
	delete IBTsystem;
	
  
}









// Play a collection l of soundfiles
void tempo(string inFname, string outFname, string label, string method)
{
	MRSDIAG("tempo.cpp - tempo");

	// For each file in collection estimate tempo
	string sfName = inFname;
	string resName = outFname;


	/* resName = sfName.substr(0,sfName.rfind(".", sfName.length()));
	   resName += "Marsyas";
	   resName += ".txt";
	*/


	if (method == "MEDIAN_SUMBANDS")
    {
		tempo_medianSumBands(sfName, label, resName);
    }
	else if (method == "MEDIAN_MULTIBANDS")
    {
		tempo_medianMultiBands(sfName,label, resName);
    }
	else if (method == "FLUX_BANDS")
	{
		tempo_fluxBands(sfName, label, resName);
	}
	else if (method == "HISTO_SUMBANDS")
    {
		tempo_histoSumBands(sfName, label, resName);
    }
	else if (method == "HISTO_SUMBANDSQ")
	{
		tempo_histoSumBandsQ(sfName,label, resName);
	}
	
	else if (method == "IBT")
    {
		// cout << "TEMPO INDUCTION USING THE INESC BEAT TRACKER" << endl;
		induction_time = 5.0;
		metrical_change_time = 5.0;
		score_function = "regular"; 
		output = "beats+tempo"; 
		tempo_ibt(sfName, label, resName);
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
	start = (float)cmd_options.getRealOption("start");
	length = (float)cmd_options.getRealOption("length");
	repetitions = (float)cmd_options.getRealOption("repetitions");
	gain = (float)cmd_options.getRealOption("gain");
	pluginName = cmd_options.getStringOption("plugin");
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

	if (helpopt)
		printHelp(progName);

	if (usageopt)
		printUsage(progName);

	string method;
	if (methodopt == EMPTYSTRING)
		method = "NEW";
	else
		method = methodopt;

	vector<string> soundfiles = cmd_options.getRemaining();

	FileName inputfile(soundfiles[0]);
  
	bool haveCollections = true;
	if (inputfile.ext() == "mf")
		haveCollections = true; 
	else 
		haveCollections = false; 
  
	// collection code for batch processing
	if (haveCollections)
	{
	  
		Collection l;
		vector<string>::iterator sfi;
		for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
		{
			string sfname = *sfi;
			readCollection(l, sfname);
		}

		correct_predictions = 0;
		correct_harmonic_predictions = 0;
		correct_mirex_predictions = 0;
		
		total_instances = 0;
		total_differences = 0.0;
		total_errors = 0;

		for (int i=0; i < l.size(); i++)
		{
			tempo(l.entry(i), "default.txt", l.labelEntry(i), method);
		}


		cout << "Correct Predictions = " << correct_predictions << "/" << total_instances << endl;
		cout << "Correct MIREX Predictions = " << correct_mirex_predictions << "/" << total_instances << endl;
		cout << "Correct Harmonic Predictions = " << correct_harmonic_predictions << "/" << total_instances << endl;
		cout << "Average error difference = " << total_differences << "/" << total_errors << "=" << total_differences / total_errors << endl;

	}
	else
	{
		for (vector<string>::iterator sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
		{
			string sfname = *sfi;
			cout << "Processing - " << sfname << endl;
	  
			tempo(*sfi, "default.txt", "0.0" , method);
		}
	}

	// This will crash unless the user knows to specify an outfile.
	//tempo(soundfiles[0], soundfiles[1], method);

	exit(0);
}






