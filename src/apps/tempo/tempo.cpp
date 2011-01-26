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


#include "common.h" 
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
#include <string.h> // This file also uses C-style string functions like strcmp().
#include <fstream>
#include <iostream>
#include <iomanip>

#ifdef MARSYAS_PNG
#include "pngwriter.h"
#endif


#ifdef MARSYAS_WIN32
#pragma warning(disable: 4251)
#endif





//#define SCORE_FUNCTION "regular" //the score function (heuristics) which conducts the beat tracking ("regular")
//#define INDUCTION_TIME 5.0 //Time (in seconds) of the beat induction step, before tracking. Has to be > 60/MIN_BPM (5.0)
//#define METRICAL_CHANGE_TIME 0.0 //Initial time (in seconds) allowed for eventual metrical changes within tracking (if 0.0 no metrical changes allowed at all; if -1.0 metrical changes would be allowed along all musical piece) (5.0)
#define BPM_HYPOTHESES 6 //Nr. of initial BPM hypotheses (must be <= than the nr. of agents) (6)
#define PHASE_HYPOTHESES 30//Nr. of phases per BPM hypothesis (30)
#define MIN_BPM 50 //minimum tempo considered, in BPMs (50)
#define MAX_BPM 250 //maximum tempo considered, in BPMs (250)
#define NR_AGENTS 30 //Nr. of agents in the pool (30)
#define LFT_OUTTER_MARGIN 0.20 //The size of the outer half-window (in % of the IBI) before the predicted beat time (0.20)
#define RGT_OUTTER_MARGIN 0.40 //The size of the outer half-window (in % of the IBI) after the predicted beat time (0.30)
#define INNER_MARGIN 4.0 //Inner tolerance window margin size (= half inner window size -> in ticks) (3.0)
#define OBSOLETE_FACTOR 0.8 //An agent is killed if, at any time (after the initial Xsecs-defined in BeatReferee), the difference between its score and the bestScore is below OBSOLETE_FACTOR * bestScore (0.8)
#define LOST_FACTOR 8 //An agent is killed if it become lost, i.e. if it found LOST_FACTOR consecutive beat predictions outside its inner tolerance window (8) 
#define CHILDREN_SCORE_FACTOR 0.8 //(Inertia1) Each created agent imports its father score multiplied (or divided if negative) by this factor (0.8)
#define BEST_FACTOR 1.0 //(Inertia2) Mutiple of the bestScore an agent's score must have for replacing the current best agent (1.0)
#define CORRECTION_FACTOR 0.25 //correction factor for compensating each agents' own {phase, period} hypothesis errors (0.25)
#define EQ_PERIOD 1 //Period threshold which identifies two agents as predicting the same period (IBI, in ticks) (1)
#define EQ_PHASE 2 //Phase threshold which identifies two agents as predicting the same phase (phase, in ticks) (2)
#define CHILD1_FACTOR 1.0 //correction factor (error proportion-[0.0-1.0]) for compensating its father's {phase, period} hypothesis - used by child1 (2.0 - only full phase adjustment; -1 - no child considered) (1.0)
#define CHILD2_FACTOR 2.0 //correction factor (error proportion-[0.0-1.0]) for compensating its father's {phase, period} hypothesis - used by child2 (2.0 - only full phase adjustment; -1 - no child considered) (2.0)
#define CHILD3_FACTOR 0.5 //correction factor (error proportion-[0.0-1.0]) for compensating its father's {phase, period} hypothesis - used by child3 (2.0 - only full phase adjustment; -1 - no child considered) (0.5)

#define WINSIZE 1024 //(2048?)
#define HOPSIZE 512 //(512)





using namespace std;
using namespace Marsyas;

vector<string> wrong_filenames_;
vector<float> wrong_filenames_tempos_;
vector<float> ground_truth_tempos_;


mrs_string output;
mrs_bool audiofileopt;
mrs_real induction_time;
mrs_real metrical_change_time;
mrs_string score_function;
mrs_bool audioopt;
mrs_bool backtraceopt;
mrs_real phase_;
mrs_realvec errors_;
string fileName;
string pluginName = EMPTYSTRING;
string methodopt;

CommandLineOptions cmd_options;

bool beatsopt_;

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
int correct_harmonic_mirex_predictions;
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
	cerr << "-b --beats      : output beat locations " << endl;
	
	cerr << "Available methods: " << endl;
	cerr << "MEDIAN_SUMBANDS" << endl;
	cerr << "MEDIAN_MULTIBANDS" << endl;
	cerr << "HISTO_SUMBANDS" << endl;
	cerr << "HISTO_SUMBANDSQ" << endl;
	
	cerr << "BOOMCHICK_WAVELET" << endl;
	cerr << "BOOMCHICK_FILTER" << endl;

	exit(1);
}

void 
evaluate_estimated_tempo(string sfName, mrs_realvec tempos, float ground_truth_tempo)
{
  
  mrs_real predicted_tempo = tempos(0);
  
	float diff1 = fabs(predicted_tempo - ground_truth_tempo);
	float diff2 = fabs(predicted_tempo - 2 * ground_truth_tempo);
	float diff3 = fabs(2 * predicted_tempo - ground_truth_tempo);
	float diff4 = fabs(3 * predicted_tempo - ground_truth_tempo);
	float diff5 = fabs(predicted_tempo - 3 * ground_truth_tempo);

	cout << sfName << "\t" << predicted_tempo << ":" << ground_truth_tempo <<  "---" << diff1 << ":" << diff2 << ":" << diff3 << ":" << diff4 << ":" << diff5 << endl;
	if (diff1 < 0.5)
		correct_predictions++;
	
	if (diff1 <= 0.04 * ground_truth_tempo)
		correct_mirex_predictions++;


	if (diff2 <= 0.04 * ground_truth_tempo)
	  errors_(0) = errors_(0) + 1;
	if (diff3 <= 0.04 * ground_truth_tempo)
	{
	  errors_(1) = errors_(1) + 1;
	  cout << "GT (for doublings) " << ground_truth_tempo << endl;
	  
	}
	
	if (diff4 <= 0.04 * ground_truth_tempo)
	  errors_(2) = errors_(2) + 1;
	if (diff5 <= 0.04 * ground_truth_tempo)
	  errors_(3) = errors_(3) + 1;
	
	  

	if ((diff1 <= 0.04 * ground_truth_tempo)||(diff2 <= 0.04 * ground_truth_tempo)||(diff3 <= 0.04 * ground_truth_tempo)||(diff4 <= 0.04 * ground_truth_tempo)||(diff5 <= 0.04 * ground_truth_tempo))
	{
		correct_harmonic_mirex_predictions++;
	}
	else 
	{
	}
	

	
	if ((diff1 < 0.5)||(diff2 < 0.5)||(diff3 < 0.5)||(diff4 < 0.5)||(diff5 < 0.5))
	{
		
		correct_harmonic_predictions++;

		if ((diff1 < diff2)&&(diff1 < diff3))
	      total_differences += diff1;
	    if ((diff2 < diff3)&&(diff1 < diff1))
	      total_differences += diff2;
	    if ((diff3 < diff2)&&(diff3 < diff1))
	      total_differences += diff3;
	    total_errors++;
	}
	
	else 
	  {
		  /* 
	    cout << "WRONG TEMPO ESTIMATION IN " << sfName << endl;
		  */ 

	  }



	wrong_filenames_.push_back(sfName);
	wrong_filenames_tempos_.push_back(predicted_tempo);
	ground_truth_tempos_.push_back(ground_truth_tempo);
	


	
	
	total_instances++;
	
	cout << "Correct Predictions = " << correct_predictions << "/" << total_instances << " - " << correct_predictions * 1.0 / total_instances * 100.0 << endl;
	cout << "Correct MIREX Predictions = " << correct_mirex_predictions << "/" << total_instances << " - " << correct_mirex_predictions * 1.0 / total_instances * 100.0 << endl;
	cout << "Correct Harmonic Predictions = " << correct_harmonic_predictions << "/" << total_instances << " - " << correct_harmonic_predictions * 1.0  / total_instances * 100.0  << endl;
	cout << "Correct Harmonic MIREX predictions = " << correct_harmonic_mirex_predictions << "/" << total_instances << " - " << correct_harmonic_mirex_predictions * 1.0  / total_instances * 100.0  << endl;
	cout << "Average error difference = " << total_differences << "/" << total_errors << "=" << total_differences / total_errors << endl;
	

	cout << "# Half   predicted tempos = " << errors_(0) * 1.0 / total_instances * 100.0 << " - " << errors_(0) << endl;;
	cout << "# Double predicted tempos = " << errors_(1) * 1.0 / total_instances * 100.0 << " - " << errors_(1) << endl;;
	cout << "# triple predicted tempos = " << errors_(2) * 1.0 / total_instances * 100.0 << endl;;
	cout << "# third predicted tempos = " << errors_(3) * 1.0 / total_instances * 100.0 << endl;;


}



// Play soundfile given by sfName, msys contains the playback
// network of MarSystem objects
void tempo_medianMultiBands(string sfName, float ground_truth_tempo, string resName, bool haveCollections)
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
	
  
	

  total->updControl("SoundFileSource/src/mrs_string/filename", sfName);
  // update the controls
  mrs_real srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();

  // input filename with hopSize/winSize
  mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
  mrs_natural hopSize = winSize / 16;

  total->updControl("mrs_natural/inSamples", hopSize);
  total->updControl("SoundFileSource/src/mrs_natural/pos", offset);
  total->updControl("ShiftInput/si/mrs_natural/winSize", winSize);


  // wavelet filterbank envelope extraction controls
  total->updControl("WaveletPyramid/wvpt/mrs_bool/forward", true);
  total->updControl("OnePole/lpf/mrs_real/alpha", 0.99f);
  mrs_natural factor = 32;
  total->updControl("DownSampler/ds/mrs_natural/factor", factor);

  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM

  mrs_natural pkinS = total->getctrl("Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_real peakSpacing = ((mrs_natural)(srate * 60.0 / (factor *60.0)) -
						  (mrs_natural)(srate * 60.0 / (factor*80.0))) / pkinS;
  mrs_natural peakStart = (mrs_natural)(srate * 60.0 / (factor * 180.0));
  mrs_natural peakEnd   = (mrs_natural)(srate * 60.0 / (factor * 40.0));
  total->updControl("Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
  total->updControl("Peaker/pkr/mrs_real/peakStrength", 0.75);
  total->updControl("Peaker/pkr/mrs_natural/peakStart", peakStart);
  total->updControl("Peaker/pkr/mrs_natural/peakEnd", peakEnd);
	

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
  //mrs_natural repeatId = 1;
  // vector of bpm estimate used to calculate median
  vector<int> bpms;

  onSamples = total->getctrl("ShiftInput/si/mrs_natural/onSamples")->to<mrs_natural>();
  nChannels = total->getctrl("SoundFileSource/src/mrs_natural/onObservations")->to<mrs_natural>();

  total->updControl("MaxArgMax/mxr/mrs_natural/nMaximums", 4);
  total->updControl("MaxArgMax/mxr/mrs_natural/interpolation", 1);
	
	
  total->updControl("Peaker/pkr/mrs_natural/interpolation", 1);



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
	
	
  float predicted_tempo;
  predicted_tempo = bpms[bpms.size()/2];
  evaluate_estimated_tempo(sfName, predicted_tempo, ground_truth_tempo);
	
  delete total;
}





void
tempo_wavelets(string sfName, string resName, bool haveCollections)
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
	total->updControl("Gain/normGain/mrs_real/gain", 0.05);
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
  total->addMarSystem(mng.create("BeatHistogramFromPeaks", "histo"));

  // total->addMarSystem(mng.create("Peaker", "pkr1"));
  // total->addMarSystem(mng.create("PlotSink", "psink3"));
  // total->addMarSystem(mng.create("Reassign", "reassign"));
  // total->addMarSystem(mng.create("PlotSink", "psink4"));
  total->addMarSystem(mng.create("HarmonicEnhancer", "harm"));
  // total->addMarSystem(mng.create("HarmonicEnhancer", "harm"));
  // total->addMarSystem(mng.create("PlotSink", "psink4"));
  // total->addMarSystem(mng.create("MaxArgMax", "mxr1"));


  mrs_natural ifactor = 8;
  total->updControl("DownSampler/initds/mrs_natural/factor", ifactor);
  total->updControl("SoundFileSource/src/mrs_string/filename", sfName);
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

  // total->updControl("PlotSink/psink1/mrs_string/filename", "acr");
  // total->updControl("PlotSink/psink2/mrs_string/filename", "peaks");

  // total->updControl("PlotSink/psink3/mrs_string/filename", "histo");
  // total->updControl("PlotSink/psink4/mrs_string/filename", "rhisto");
  total->updControl("mrs_natural/inSamples", hopSize);
  total->updControl("SoundFileSource/src/mrs_natural/pos", offset);

  total->updControl("MaxArgMax/mxr/mrs_natural/nMaximums", 5);
  // total->updControl("MaxArgMax/mxr1/mrs_natural/nMaximums", 2);


  total->updControl("ShiftInput/si/mrs_natural/winSize", winSize);



  // wavelet filterbank envelope extraction controls
  total->updControl("WaveletPyramid/wvpt/mrs_bool/forward", true);
  total->updControl("OnePole/lpf/mrs_real/alpha", 0.99f);
  mrs_natural factor = 32;
  total->updControl("DownSampler/ds/mrs_natural/factor", factor);

  srate = total->getctrl("DownSampler/initds/mrs_real/osrate")->to<mrs_real>();




  // Peak picker 4BPMs at 60BPM resolution from 50 BPM to 250 BPM
  mrs_natural pkinS = total->getctrl("Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_real peakSpacing = ((mrs_natural)(srate * 60.0 / (factor *60.0)) -
						  (mrs_natural)(srate * 60.0 / (factor*62.0))) / (pkinS * 1.0);


  mrs_natural peakStart = (mrs_natural)(srate * 60.0 / (factor * 230.0));
  mrs_natural peakEnd   = (mrs_natural)(srate * 60.0 / (factor * 30.0));


  total->updControl("Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
  total->updControl("Peaker/pkr/mrs_real/peakStrength", 0.5);
  total->updControl("Peaker/pkr/mrs_natural/peakStart", peakStart);
  total->updControl("Peaker/pkr/mrs_natural/peakEnd", peakEnd);
  total->updControl("Peaker/pkr/mrs_real/peakGain", 2.0);


  /* total->updControl("Peaker/pkr1/mrs_real/peakSpacing", 0.1);
	 total->updControl("Peaker/pkr1/mrs_real/peakStrength", 1.2);
	 total->updControl("Peaker/pkr1/mrs_natural/peakStart", 60);
	 total->updControl("Peaker/pkr1/mrs_natural/peakEnd", 180);
  */




  total->updControl("BeatHistogramFromPeaks/histo/mrs_natural/startBin", 0);
  total->updControl("BeatHistogramFromPeaks/histo/mrs_natural/endBin", 230);

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
	  total->updControl("SoundFileSource/src/mrs_natural/pos", offset);
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
  total1->updControl("SoundFileSource/src1/mrs_string/filename", sfName);


  srate = total1->getctrl("SoundFileSource/src1/mrs_real/osrate")->to<mrs_real>();



  // update the controls
  // input filename with hopSize/winSize
  winSize = (mrs_natural)(srate / 22050.0) * 8 * 65536;

  total1->updControl("mrs_natural/inSamples", winSize);
  total1->updControl("SoundFileSource/src1/mrs_natural/pos", 0);

  // wavelt filterbank envelope extraction controls
  // total1->updControl("OnePole/lpf1/mrs_real/alpha", 0.99f);
  factor = 4;
  total1->updControl("DownSampler/ds1/mrs_natural/factor", factor);








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




  for (mrs_natural i = 0; i < p1; ++i)
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

  for (mrs_natural i = 0; i < p2; ++i)
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
tempo_aim(string sfName, float ground_truth_tempo, string resName, bool haveCollections) 
{
  cout << "Tempo-aim" << endl;

 
  MarSystemManager mng;

  MarSystem* net = mng.create("Series/net");
  
  net->addMarSystem(mng.create("SoundFileSource/src"));
  net->addMarSystem(mng.create("AimPZFC2/aimpzfc"));
  net->addMarSystem(mng.create("AimHCL2/aimhcl2"));
  net->addMarSystem(mng.create("Sum/sum"));
  

  net->addMarSystem(mng.create("AutoCorrelation/acr"));
  net->addMarSystem(mng.create("BeatHistogram/histo"));

  MarSystem* hfanout = mng.create("Fanout", "hfanout");
  hfanout->addMarSystem(mng.create("Gain", "id1"));
  hfanout->addMarSystem(mng.create("TimeStretch", "tsc1"));
  net->addMarSystem(hfanout);
  net->addMarSystem(mng.create("Sum", "hsum"));


  net->addMarSystem(mng.create("Peaker/pkr"));
  net->addMarSystem(mng.create("MaxArgMax/mxr"));
  
  net->updControl("SoundFileSource/src/mrs_string/filename",
		  sfName);
  net->updControl("mrs_natural/inSamples", 16 * 4096);
 
  net->updControl("Fanout/hfanout/TimeStretch/tsc1/mrs_real/factor", 0.5);
  
  net->updControl("Sum/sum/mrs_string/mode","sum_samples");
  net->updControl("AutoCorrelation/acr/mrs_real/magcompress", 0.5);
  net->updControl("BeatHistogram/histo/mrs_natural/startBin", 0);
  net->updControl("BeatHistogram/histo/mrs_natural/endBin", 200);
  net->updControl("BeatHistogram/histo/mrs_bool/tempoWeighting", true);
  
  net->updControl("Peaker/pkr/mrs_natural/peakStart", 50);
  net->updControl("Peaker/pkr/mrs_natural/peakEnd", 160);

  
  
  ofstream ofs2;
  ofs2.open("tempo.mpl");
  ofs2 << *net << endl;
  ofs2.close();
  
  
  for (int i=0; i < 4; i++)
    {
      net->tick();    
    }   

  
  mrs_realvec amp_tempo  = net->getControl("mrs_realvec/processedData")->to<mrs_realvec>();
  cout << "Tempo = " << amp_tempo(1) << endl;
  mrs_real bpm_estimate = amp_tempo(1);





  mrs_realvec tempos(1);
  tempos(0) = bpm_estimate;

  if (haveCollections)
    {
      evaluate_estimated_tempo(sfName, tempos, ground_truth_tempo);
    }
  
  ofstream ofs;
  ofs.open(fileName.c_str());
  cout << bpm_estimate << endl;
  ofs << bpm_estimate << endl;
  
  ofs.close();
  
  delete net;

}


void 
tempo_flux(string sfName, float ground_truth_tempo, string resName, bool haveCollections) 
{
  MarSystemManager mng;
	
  MarSystem *beatTracker = mng.create("Series/beatTracker");
	
	
  MarSystem *onset_strength = mng.create("Series/onset_strength");
  MarSystem *accum = mng.create("Accumulator/accum");
  MarSystem *fluxnet = mng.create("Series/fluxnet");
  fluxnet->addMarSystem(mng.create("SoundFileSource", "src"));
  fluxnet->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  fluxnet->addMarSystem(mng.create("ShiftInput", "si"));	
  fluxnet->addMarSystem(mng.create("Windowing", "windowing1"));
  fluxnet->addMarSystem(mng.create("Spectrum", "spk"));
  // fluxnet->addMarSystem(mng.create("SpectralTransformations", "spktr"));
  fluxnet->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  


  // fluxnet->addMarSystem(mng.create("Spectrum2Chroma", "spk"));
  fluxnet->addMarSystem(mng.create("Flux", "flux"));
  accum->addMarSystem(fluxnet);
	
  onset_strength->addMarSystem(accum);
  onset_strength->addMarSystem(mng.create("ShiftInput/si2"));
  beatTracker->addMarSystem(onset_strength);
	
  MarSystem *tempoInduction = mng.create("FlowThru/tempoInduction");
  tempoInduction->addMarSystem(mng.create("Filter", "filt1"));
  tempoInduction->addMarSystem(mng.create("Reverse", "reverse1"));
  tempoInduction->addMarSystem(mng.create("Filter", "filt2"));
  tempoInduction->addMarSystem(mng.create("Reverse", "reverse2"));
  // tempoInduction->addMarSystem(mng.create("Windowing", "windowing2"));

  /* tempoInduction->addMarSystem(mng.create("Spectrum", "spk"));
  tempoInduction->addMarSystem(mng.create("SpectralTransformations", "spktr2"));
  tempoInduction->addMarSystem(mng.create("InvSpectrum", "ispk"));
  tempoInduction->addMarSystem(mng.create("HalfWaveRectifier", "hwr"));
  */ 

  tempoInduction->addMarSystem(mng.create("AutoCorrelation", "acr"));
  tempoInduction->addMarSystem(mng.create("BeatHistogram", "histo"));
	
  MarSystem* hfanout = mng.create("Fanout", "hfanout");
  hfanout->addMarSystem(mng.create("Gain", "id1"));
  hfanout->addMarSystem(mng.create("TimeStretch", "tsc1"));
  // hfanout->addMarSystem(mng.create("TimeStretch", "tsc2"));
  tempoInduction->addMarSystem(hfanout);
  tempoInduction->addMarSystem(mng.create("Sum", "hsum"));
  tempoInduction->addMarSystem(mng.create("Peaker", "pkr1"));
  tempoInduction->addMarSystem(mng.create("MaxArgMax", "mxr1"));				
  tempoInduction->addMarSystem(mng.create("Gain", "gain"));
  
  beatTracker->addMarSystem(tempoInduction);
  beatTracker->addMarSystem(mng.create("ShiftInput/si3"));
  beatTracker->addMarSystem(mng.create("BeatPhase/beatphase"));
  beatTracker->addMarSystem(mng.create("Gain/id"));
  mrs_natural winSize = 256;
  mrs_natural hopSize = 128;
  mrs_natural  bwinSize = 2048;
  mrs_natural bp_winSize = 8192;
  
  mrs_natural bhopSize = 128;
  mrs_natural nCandidates = 10;    // number of tempo candidates 
  
  
  onset_strength->updControl("Accumulator/accum/mrs_natural/nTimes", bhopSize);	  
  onset_strength->updControl("ShiftInput/si2/mrs_natural/winSize",bwinSize);
  beatTracker->updControl("ShiftInput/si3/mrs_natural/winSize", bp_winSize);
  
	
  realvec bcoeffs(1,3);
  bcoeffs(0) = 0.0564;
  bcoeffs(1) = 0.1129;
  bcoeffs(2) = 0.0564;
  tempoInduction->updControl("Filter/filt1/mrs_realvec/ncoeffs", bcoeffs);
	
  tempoInduction->updControl("Filter/filt2/mrs_realvec/ncoeffs", bcoeffs);
  realvec acoeffs(1,3);
  acoeffs(0) = 1.0000;
  acoeffs(1) = -1.2247;
  acoeffs(2) = 0.4504;
  tempoInduction->updControl("Filter/filt1/mrs_realvec/dcoeffs", acoeffs);
  tempoInduction->updControl("Filter/filt2/mrs_realvec/dcoeffs", acoeffs);

  fluxnet->updControl("SpectralTransformations/spktr/mrs_string/mode", "compress_magnitude");
  
  tempoInduction->updControl("SpectralTransformations/spktr2/mrs_string/mode", "three_peaks");
  
  // tempoInduction->updControl("Windowing/windowing2/mrs_string/type", "Hanning");
  
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakNeighbors", 40);
  tempoInduction->updControl("Peaker/pkr1/mrs_real/peakSpacing", 0.1);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakStart", 200);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakEnd", 640);
  // tempoInduction->updControl("Peaker/pkr1/mrs_bool/peakHarmonics", true);

  tempoInduction->updControl("MaxArgMax/mxr1/mrs_natural/interpolation", 0);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/interpolation", 0);
  beatTracker->updControl("FlowThru/tempoInduction/MaxArgMax/mxr1/mrs_natural/nMaximums", nCandidates);
	
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/PowerSpectrum/pspk/mrs_string/spectrumType", "magnitude");
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/Flux/flux/mrs_string/mode", "DixonDAFX06");

  tempoInduction->updControl("BeatHistogram/histo/mrs_natural/startBin", 0);
  tempoInduction->updControl("BeatHistogram/histo/mrs_natural/endBin", 800);
  tempoInduction->updControl("BeatHistogram/histo/mrs_real/factor", 16.0);
  tempoInduction->updControl("BeatHistogram/histo/mrs_bool/tempoWeighting", true);
  

  tempoInduction->updControl("Fanout/hfanout/TimeStretch/tsc1/mrs_real/factor", 0.5);
  tempoInduction->updControl("Fanout/hfanout/Gain/id1/mrs_real/gain", 1.0);
  tempoInduction->updControl("AutoCorrelation/acr/mrs_real/magcompress", 0.5); 
  tempoInduction->updControl("AutoCorrelation/acr/mrs_bool/setr0to0", true);
  tempoInduction->updControl("AutoCorrelation/acr/mrs_bool/setr0to1", true);
  // tempoInduction->updControl("AutoCorrelation/acr/mrs_natural/normalize", 1);
  
	
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/ShiftInput/si/mrs_natural/winSize", winSize);
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_string/filename", sfName);
  beatTracker->updControl("mrs_natural/inSamples", hopSize);
	

	
  vector<mrs_real> bpms;
  vector<mrs_real> secondary_bpms;
  vector<mrs_real> bpms_amps;
  vector<mrs_real> secondary_bpms_amps;
	
  mrs_real bin;



  beatTracker->updControl("BeatPhase/beatphase/mrs_natural/bhopSize", bhopSize);
  
  beatTracker->updControl("BeatPhase/beatphase/mrs_natural/bwinSize", bwinSize);
  beatTracker->updControl("BeatPhase/beatphase/mrs_natural/nCandidates", nCandidates);
  beatTracker->linkControl("BeatPhase/beatphase/mrs_realvec/tempo_candidates", 
						   "FlowThru/tempoInduction/MaxArgMax/mxr1/mrs_realvec/processedData");
  

  if (pluginName != EMPTYSTRING)
  {
	ofstream ofs;
	ofs.open(pluginName.c_str());
	ofs << *beatTracker << endl;
	ofs.close();
	pluginName = EMPTYSTRING;
  }
  

  int extra_ticks = bwinSize/bhopSize;
  mrs_realvec tempos(10);
  mrs_realvec tempo_scores(10);
  tempo_scores.setval(0.0);
  
  
  int ticks = 0;
  while (1) 
  {
    if (ticks == extra_ticks)
      tempoInduction->updControl("BeatHistogram/histo/mrs_bool/reset", true);
    
    
	beatTracker->tick();
    
	ticks++;
	mrs_realvec estimate = beatTracker->getctrl("FlowThru/tempoInduction/MaxArgMax/mxr1/mrs_realvec/processedData")->to<mrs_realvec>();

	mrs_realvec bhisto = beatTracker->getctrl("FlowThru/tempoInduction/BeatHistogram/histo/mrs_realvec/processedData")->to<mrs_realvec>();
		
		
	bin = estimate(1) * 0.25;
		
	
	
	for (int k=0; k < 10; k++)
	{
	  tempos(k) = estimate(2*k+1) * 0.25;
	  tempo_scores(k) = estimate(2*k);
	}
	
	
	//beatTracker->updControl("BeatPhase/beatphase/mrs_realvec/tempos", estimate);
	// beatTracker->updControl("BeatPhase/beatphase/mrs_realvec/tempo_scores", tempo_scores);
	
	mrs_real phase_tempo = beatTracker->getControl("BeatPhase/beatphase/mrs_real/phase_tempo")->to<mrs_real>();
	bpms.push_back(phase_tempo);
	bpms_amps.push_back(bhisto(bin * 4));
	
	if (!beatTracker->getctrl("Series/onset_strength/Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
	{
	  // extra_ticks --;
	  break;
	  
	}

	// if (extra_ticks == 0)
	// break;

	
  }


  

  sort(bpms.begin(), bpms.end());


  mrs_realvec phase_tempos(nCandidates);
  mrs_realvec phase_tempo_scores(nCandidates);
  
  phase_tempos = beatTracker->getControl("BeatPhase/beatphase/mrs_realvec/tempos")->to<mrs_realvec>();
  phase_tempo_scores= beatTracker->getControl("BeatPhase/beatphase/mrs_realvec/tempo_scores")->to<mrs_realvec>();
  

  
  mrs_real max_score = 0.0;
  int max_i=0;
  
  for (int i= 0; i < phase_tempos.getSize(); i++) 
  {
	if (phase_tempo_scores(i) > max_score) 
	{
	  max_score = phase_tempo_scores(i);
	  max_i = i;
	}
  }



  
  extra_ticks = bwinSize/bhopSize;
  extra_ticks = 0.0;
  
  mrs_real bpm_estimate = bpms[bpms.size()-1-extra_ticks];

  bpm_estimate = phase_tempos(0);
  
  // 	mrs_real secondary_bpm_estimate;
  // secondary_bpm_estimate = secondary_bpms[bpms.size()-1-extra_ticks];
  
  mrs_real bpm_amp = bpms_amps[bpms.size()-1-extra_ticks];
  // mrs_real secondary_bpm_amp = secondary_bpms_amps[bpms.size()-1-extra_ticks];


  // if ((bpm_estimate - bpms[bpms.size()/2]) < 0)
  //   tempos(0) = bpm_estimate;
  // else 
  //   tempos(0) = bpms[bpms.size()/2];
    


  mrs_real median = bpms[bpms.size()/2];
  

  // tempos(0) = tempos(max_i);
  // tempos(0) = bpm_estimate;
  tempos(0) = median;
  
  

  

  cout << "tempos(0) = " << tempos(0) << endl;
  cout << tempos << endl;
  

  if (haveCollections)
    {
      evaluate_estimated_tempo(sfName, tempos, ground_truth_tempo);
      // evaluate_estimated_tempo(sfName,secondary_bpm_estimate, ground_truth_tempo);
    }
  
	bpm_estimate *= 2.0;
	// secondary_bpm_estimate *= 2.0;
	bpm_estimate = (mrs_natural)bpm_estimate;
	// secondary_bpm_estimate = (mrs_natural)secondary_bpm_estimate;
	bpm_estimate /= 2.0;
	// secondary_bpm_estimate /= 2.0;	 
	
	// mrs_real strength = bpm_amp + secondary_bpm_amp;
	

	ofstream ofs;
	ofs.open(fileName.c_str());

	// if (bpm_estimate < secondary_bpm_estimate)
	// {
	// 	ofs << bpm_estimate << "\t" << secondary_bpm_estimate << "\t" << bpm_amp / strength << endl;
	// 	cout << bpm_estimate << "\t" << secondary_bpm_estimate << "\t" << bpm_amp / strength << endl;
	// }
	
	// else 
	// {
	// 	ofs << secondary_bpm_estimate << "\t" << bpm_estimate << "\t" << secondary_bpm_amp / strength << endl;
	// 	cout << bpm_estimate << "\t" << secondary_bpm_estimate << "\t" << bpm_amp / strength << endl;
	// }



	cout << bpm_estimate << endl;
	ofs << bpm_estimate << endl;
	


	
	ofs.close();

	delete beatTracker;
}



void 
tempo_aim_flux(string sfName, float ground_truth_tempo, string resName, bool haveCollections) 
{
  
  MarSystemManager mng;
	
  MarSystem *beatTracker = mng.create("Series/beatTracker");
	
	
  MarSystem *onset_strength = mng.create("Series/onset_strength");
  MarSystem *accum = mng.create("Accumulator/accum");
  MarSystem *fluxnet = mng.create("Series/fluxnet");
  fluxnet->addMarSystem(mng.create("SoundFileSource", "src"));
  fluxnet->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  fluxnet->addMarSystem(mng.create("ShiftInput", "si"));	
  fluxnet->addMarSystem(mng.create("AimPZFC2/aimpzfc"));
  // fluxnet->addMarSystem(mng.create("AimGammatone/aimgm"));
  fluxnet->addMarSystem(mng.create("AimHCL2/aimhcl2"));
  fluxnet->addMarSystem(mng.create("Sum/aimsum"));

  
  
  fluxnet->updControl("Sum/aimsum/mrs_string/mode", "sum_observations");
  
  
  // fluxnet->addMarSystem(mng.create("Windowing", "windowing1"));
  // fluxnet->addMarSystem(mng.create("Spectrum", "spk"));
  // fluxnet->addMarSystem(mng.create("SpectralTransformations", "spktr"));
  // fluxnet->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  // fluxnet->addMarSystem(mng.create("Spectrum2Chroma", "spk"));
  
  fluxnet->addMarSystem(mng.create("Flux", "flux"));
  accum->addMarSystem(fluxnet);
	
  onset_strength->addMarSystem(accum);
  onset_strength->addMarSystem(mng.create("ShiftInput/si2"));
  beatTracker->addMarSystem(onset_strength);
	
  MarSystem *tempoInduction = mng.create("FlowThru/tempoInduction");
  tempoInduction->addMarSystem(mng.create("Filter", "filt1"));
  tempoInduction->addMarSystem(mng.create("Reverse", "reverse1"));
  tempoInduction->addMarSystem(mng.create("Filter", "filt2"));
  tempoInduction->addMarSystem(mng.create("Reverse", "reverse2"));
  // tempoInduction->addMarSystem(mng.create("Windowing", "windowing2"));

  /* tempoInduction->addMarSystem(mng.create("Spectrum", "spk"));
  tempoInduction->addMarSystem(mng.create("SpectralTransformations", "spktr2"));
  tempoInduction->addMarSystem(mng.create("InvSpectrum", "ispk"));
  tempoInduction->addMarSystem(mng.create("HalfWaveRectifier", "hwr"));
  */ 

  tempoInduction->addMarSystem(mng.create("AutoCorrelation", "acr"));
  tempoInduction->addMarSystem(mng.create("BeatHistogram", "histo"));
	
  MarSystem* hfanout = mng.create("Fanout", "hfanout");
  hfanout->addMarSystem(mng.create("Gain", "id1"));
  hfanout->addMarSystem(mng.create("TimeStretch", "tsc1"));
  // hfanout->addMarSystem(mng.create("TimeStretch", "tsc2"));
  tempoInduction->addMarSystem(hfanout);
  tempoInduction->addMarSystem(mng.create("Sum", "hsum"));
  tempoInduction->addMarSystem(mng.create("Peaker", "pkr1"));
  tempoInduction->addMarSystem(mng.create("MaxArgMax", "mxr1"));				
  tempoInduction->updControl("MaxArgMax/mxr1/mrs_natural/nMaximums", 10);	
  beatTracker->addMarSystem(tempoInduction);
  beatTracker->addMarSystem(mng.create("ShiftInput/si3"));
  beatTracker->addMarSystem(mng.create("BeatPhase/beatphase"));
  beatTracker->addMarSystem(mng.create("Gain/id"));
  mrs_natural winSize = 256;
  mrs_natural hopSize = 128;
  mrs_natural  bwinSize = 2048;
  mrs_natural bhopSize = 128;
  mrs_natural bp_winSize = 4096;
  
  
  onset_strength->updControl("Accumulator/accum/mrs_natural/nTimes", bhopSize);	  
  onset_strength->updControl("ShiftInput/si2/mrs_natural/winSize",bwinSize);
  beatTracker->updControl("ShiftInput/si3/mrs_natural/winSize", bp_winSize);
  
	
  realvec bcoeffs(1,3);
  bcoeffs(0) = 0.0564;
  bcoeffs(1) = 0.1129;
  bcoeffs(2) = 0.0564;
  tempoInduction->updControl("Filter/filt1/mrs_realvec/ncoeffs", bcoeffs);
	
  tempoInduction->updControl("Filter/filt2/mrs_realvec/ncoeffs", bcoeffs);
  realvec acoeffs(1,3);
  acoeffs(0) = 1.0000;
  acoeffs(1) = -1.2247;
  acoeffs(2) = 0.4504;
  tempoInduction->updControl("Filter/filt1/mrs_realvec/dcoeffs", acoeffs);
  tempoInduction->updControl("Filter/filt2/mrs_realvec/dcoeffs", acoeffs);

  fluxnet->updControl("SpectralTransformations/spktr/mrs_string/mode", "compress_magnitude");
  
  tempoInduction->updControl("SpectralTransformations/spktr2/mrs_string/mode", "three_peaks");
  
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakNeighbors", 40);
  tempoInduction->updControl("Peaker/pkr1/mrs_real/peakSpacing", 0.1);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakStart", 200);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakEnd", 640);
  // tempoInduction->updControl("Peaker/pkr1/mrs_bool/peakHarmonics", true);

  tempoInduction->updControl("MaxArgMax/mxr1/mrs_natural/interpolation", 0);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/interpolation", 0);
  tempoInduction->updControl("MaxArgMax/mxr1/mrs_natural/nMaximums", 10);
	
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/PowerSpectrum/pspk/mrs_string/spectrumType", "magnitude");
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/Flux/flux/mrs_string/mode", "DixonDAFX06");

  tempoInduction->updControl("BeatHistogram/histo/mrs_natural/startBin", 0);
  tempoInduction->updControl("BeatHistogram/histo/mrs_natural/endBin", 800);
  tempoInduction->updControl("BeatHistogram/histo/mrs_real/factor", 16.0);
  tempoInduction->updControl("BeatHistogram/histo/mrs_bool/tempoWeighting", true);
  

  tempoInduction->updControl("Fanout/hfanout/TimeStretch/tsc1/mrs_real/factor", 0.5);
  tempoInduction->updControl("Fanout/hfanout/Gain/id1/mrs_real/gain", 1.0);
  tempoInduction->updControl("AutoCorrelation/acr/mrs_real/magcompress", 0.5); 
  tempoInduction->updControl("AutoCorrelation/acr/mrs_bool/setr0to0", true);
  
  
	
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/ShiftInput/si/mrs_natural/winSize", winSize);
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_string/filename", sfName);
  beatTracker->updControl("mrs_natural/inSamples", hopSize);
	

	
  vector<mrs_real> bpms;
  vector<mrs_real> secondary_bpms;
  vector<mrs_real> bpms_amps;
  vector<mrs_real> secondary_bpms_amps;
	
  mrs_real bin;


  if (pluginName != EMPTYSTRING)
  {
	ofstream ofs;
	ofs.open(pluginName.c_str());
	ofs << *beatTracker << endl;
	ofs.close();
	pluginName = EMPTYSTRING;
  }

  beatTracker->updControl("BeatPhase/beatphase/mrs_natural/bhopSize", bhopSize);
  beatTracker->updControl("BeatPhase/beatphase/mrs_natural/bwinSize", bwinSize);

  int extra_ticks = bwinSize/bhopSize;
  mrs_realvec tempos(10);
  mrs_realvec tempo_scores(10);
  tempo_scores.setval(0.0);


  int ticks = 0;
  while (1) 
  {
    if (ticks == extra_ticks)
      tempoInduction->updControl("BeatHistogram/histo/mrs_bool/reset", true);
	
	beatTracker->tick();
	ticks++;
	
	mrs_realvec estimate = beatTracker->getctrl("FlowThru/tempoInduction/MaxArgMax/mxr1/mrs_realvec/processedData")->to<mrs_realvec>();

	mrs_realvec bhisto = beatTracker->getctrl("FlowThru/tempoInduction/BeatHistogram/histo/mrs_realvec/processedData")->to<mrs_realvec>();

	bin = estimate(1) * 0.25;

	for (int k=0; k < 10; k++)
	{
	  tempos(k) = estimate(2*k+1) * 0.25;
	  tempo_scores(k) = estimate(2*k);
	}

	beatTracker->updControl("BeatPhase/beatphase/mrs_realvec/tempos", tempos);
	beatTracker->updControl("BeatPhase/beatphase/mrs_realvec/tempo_scores", tempo_scores);

	bpms.push_back(bin);
	bpms_amps.push_back(bhisto(bin * 4));

	if (!beatTracker->getctrl("Series/onset_strength/Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
	{
	  break;

	}
  }

  


  
		  




  extra_ticks = bwinSize/bhopSize;
  extra_ticks = 0;
  
  mrs_real bpm_estimate = bpms[bpms.size()-1-extra_ticks];

  

	// 	mrs_real secondary_bpm_estimate;
	// secondary_bpm_estimate = secondary_bpms[bpms.size()-1-extra_ticks];

   mrs_real bpm_amp = bpms_amps[bpms.size()-1-extra_ticks];
	// mrs_real secondary_bpm_amp = secondary_bpms_amps[bpms.size()-1-extra_ticks];

  // tempos(0) = tempos(max_i);
   tempos(0) = bpm_estimate;
	
	
	if (haveCollections)
	{
	  evaluate_estimated_tempo(sfName, tempos, ground_truth_tempo);
		// evaluate_estimated_tempo(sfName,secondary_bpm_estimate, ground_truth_tempo);
	}
	
	bpm_estimate *= 2.0;
	// secondary_bpm_estimate *= 2.0;
	bpm_estimate = (mrs_natural)bpm_estimate;
	// secondary_bpm_estimate = (mrs_natural)secondary_bpm_estimate;
	bpm_estimate /= 2.0;
	// secondary_bpm_estimate /= 2.0;	 
	
	// mrs_real strength = bpm_amp + secondary_bpm_amp;
	

	ofstream ofs;
	ofs.open(fileName.c_str());

	// if (bpm_estimate < secondary_bpm_estimate)
	// {
	// 	ofs << bpm_estimate << "\t" << secondary_bpm_estimate << "\t" << bpm_amp / strength << endl;
	// 	cout << bpm_estimate << "\t" << secondary_bpm_estimate << "\t" << bpm_amp / strength << endl;
	// }
	
	// else 
	// {
	// 	ofs << secondary_bpm_estimate << "\t" << bpm_estimate << "\t" << secondary_bpm_amp / strength << endl;
	// 	cout << bpm_estimate << "\t" << secondary_bpm_estimate << "\t" << bpm_amp / strength << endl;
	// }



	cout << bpm_estimate << endl;
	ofs << bpm_estimate << endl;
	


	
	ofs.close();

	delete beatTracker;
}




void 
tempo_aim_flux1(string sfName, float ground_truth_tempo, string resName, bool haveCollections) 
{
  cout << "Aim flux 1" << endl;
  
  MarSystemManager mng;
	

  MarSystem *onset_strength = mng.create("Series/onset_strength");
  
  MarSystem *accum = mng.create("Accumulator/accum");
  
  MarSystem *fluxnet = mng.create("Series/fluxnet");
  fluxnet->addMarSystem(mng.create("SoundFileSource", "src"));
  fluxnet->addMarSystem(mng.create("Stereo2Mono", "s2m"));
  fluxnet->addMarSystem(mng.create("ShiftInput", "si"));	
  fluxnet->addMarSystem(mng.create("AimPZFC2/aimpzfc"));
  fluxnet->addMarSystem(mng.create("AimHCL2/aimhcl2"));
  fluxnet->addMarSystem(mng.create("Sum/aimsum"));
  fluxnet->addMarSystem(mng.create("Flux", "flux"));
  fluxnet->updControl("Sum/aimsum/mrs_string/mode", "sum_observations");
  fluxnet->updControl("ShiftInput/si/mrs_natural/winSize", 256);

  
  accum->addMarSystem(fluxnet);
  



  onset_strength->addMarSystem(accum);
  onset_strength->addMarSystem(mng.create("ShiftInput/si2"));
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/Flux/flux/mrs_string/mode", "DixonDAFX06");
  onset_strength->updControl("mrs_natural/inSamples", 128);




  onset_strength->updControl("Accumulator/accum/mrs_natural/nTimes", 128);	  
  onset_strength->updControl("ShiftInput/si2/mrs_natural/winSize", 2048);

  onset_strength->updControl("Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_string/filename", sfName);

  
  if (pluginName != EMPTYSTRING)
  {
	ofstream ofs;
	ofs.open(pluginName.c_str());
	ofs << *onset_strength << endl;
	ofs.close();
	pluginName = EMPTYSTRING;
  }


  
  int ticks = 0;


  
  cout << *onset_strength << endl;
  while (1) 
    {
      cout << "starting tick" << endl;
      onset_strength->tick();
      cout << "end tick" << endl;
      ticks++;

      mrs_bool foo;
	  if (!onset_strength->getctrl("Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
	  {
		break;
	  }
      cout << "ticks = " << ticks << endl;
  }

  
  delete fluxnet;
}




void
tempo_histoSumBands(string sfName, float ground_truth_tempo, string resName, bool haveCollections)
{
	MarSystemManager mng;
	mrs_real srate = 0.0;


	// prepare network
	MarSystem *total = mng.create("Series", "src");
	total->addMarSystem(mng.create("SoundFileSource", "src"));
	total->addMarSystem(mng.create("Stereo2Mono", "s2m"));
	total->addMarSystem(mng.create("ShiftInput", "si"));
	// total->addMarSystem(mng.create("AudioSink", "dest"));
	total->addMarSystem(mng.create("DownSampler", "dsr1"));
	total->addMarSystem(mng.create("WaveletPyramid", "wvpt"));

	// implicit fanout
	total->addMarSystem(mng.create("WaveletBands", "wvbnds"));

	
	total->addMarSystem(mng.create("FullWaveRectifier", "fwr"));
	total->addMarSystem(mng.create("OnePole", "lpf"));
	total->addMarSystem(mng.create("Reverse", "reverse"));
	total->addMarSystem(mng.create("OnePole", "lpf1"));
	total->addMarSystem(mng.create("Reverse", "reverse1"));
	total->addMarSystem(mng.create("Norm", "norm"));
	
	
	// implicit fanin
	total->addMarSystem(mng.create("Sum", "sum"));
	total->addMarSystem(mng.create("DownSampler", "ds"));
	total->addMarSystem(mng.create("Delta", "delta"));
	// total->addMarSystem(mng.create("BeatPhase", "beatphase"));
	total->addMarSystem(mng.create("AutoCorrelation", "acr"));
	total->addMarSystem(mng.create("BeatHistogram", "histo"));


	MarSystem* hfanout = mng.create("Fanout", "hfanout");
	hfanout->addMarSystem(mng.create("Gain", "id1"));
	hfanout->addMarSystem(mng.create("TimeStretch", "tsc1"));
	// hfanout->addMarSystem(mng.create("TimeStretch", "tsc2"));
	
	hfanout->updControl("Gain/id1/mrs_real/gain", 2.0);
	
	total->addMarSystem(hfanout);
	total->addMarSystem(mng.create("Sum", "hsum"));
	total->addMarSystem(mng.create("Peaker", "pkr"));
	total->addMarSystem(mng.create("MaxArgMax", "mxr"));
	
	// update the controls
	// input filename with hopSize/winSize
	total->updControl("SoundFileSource/src/mrs_string/filename", sfName);
	srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();

	mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
	mrs_natural hopSize = winSize / 8;

	total->updControl("mrs_natural/inSamples", hopSize);
	total->updControl("ShiftInput/si/mrs_natural/winSize", winSize);
	total->updControl("DownSampler/dsr1/mrs_natural/factor", 8);
	
	// wavelt filterbank envelope extraction controls
	total->updControl("WaveletPyramid/wvpt/mrs_bool/forward", true);
	total->updControl("OnePole/lpf/mrs_real/alpha", 0.99f);
	total->updControl("OnePole/lpf1/mrs_real/alpha", 0.99f);
	mrs_natural factor = 2;
	total->updControl("DownSampler/ds/mrs_natural/factor", factor);
	factor = 16;
	
	total->updControl("MaxArgMax/mxr/mrs_natural/nMaximums", 2);
	
	total->updControl("Peaker/pkr/mrs_natural/peakNeighbors", 10);
	total->updControl("Peaker/pkr/mrs_real/peakSpacing", 0.1);
	total->updControl("Peaker/pkr/mrs_natural/peakStart", 50);
	total->updControl("Peaker/pkr/mrs_natural/peakEnd", 170);
	
	total->updControl("Peaker/pkr/mrs_real/peakStrength", 0.65);
	// total->updControl("Peaker/pkr/mrs_bool/peakHarmonics", true);

	total->updControl("AutoCorrelation/acr/mrs_real/magcompress", 0.5); 
	
	total->updControl("BeatHistogram/histo/mrs_natural/startBin", 0);
	total->updControl("BeatHistogram/histo/mrs_natural/endBin", 200);
	total->updControl("Fanout/hfanout/TimeStretch/tsc1/mrs_real/factor", 0.5);
	// total->updControl("Fanout/hfanout/TimeStretch/tsc2/mrs_real/factor", 0.33);

	

	total->linkControl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
	total->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
	total->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

	// prepare vectors for processing
	realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
				 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
	realvec estimate(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
					 total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());


	

	mrs_real bin;
	mrs_natural onSamples;
//
//	int numPlayed =0;
//	mrs_natural wc=0;
//	mrs_natural samplesPlayed = 0;
//	mrs_natural repeatId = 1;

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
	  // for (int i=0; i< 400; ++i)
	  {
	    total->process(iwin, estimate);
	    bin = estimate(1);
	    bpms.push_back(bin);
	    // total->updControl("BeatPhase/beatphase/mrs_real/tempo", bin);
	  }
	
	  
	
	
	
	evaluate_estimated_tempo(sfName, bpms[bpms.size()-1], ground_truth_tempo);
	delete total;
}



void
tempo_histoSumBandsQ(string sfName, float ground_truth_tempo, string resName, bool haveCollections)
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
	total->updControl("SoundFileSource/src/mrs_string/filename", sfName);
	srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();

	mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
	mrs_natural hopSize = winSize / 8;

	total->updControl("mrs_natural/inSamples", hopSize);
	total->updControl("ShiftInput/si/mrs_natural/winSize", winSize);

	total->updControl("DownSampler/dsr1/mrs_natural/factor", 8);

	total->updControl("ConstQFiltering/cqf/mrs_natural/channels", 5);
	total->updControl("ConstQFiltering/cqf/mrs_real/qValue", 4.0);
	total->updControl("ConstQFiltering/cqf/mrs_real/lowFreq", 41.2);
	total->updControl("ConstQFiltering/cqf/mrs_real/highFreq", 1318.5);
	total->updControl("ConstQFiltering/cqf/mrs_natural/width", winSize/ 8);
	
	
	// wavelt filterbank envelope extraction controls
	// total->updControl("WaveletPyramid/wvpt/mrs_bool/forward", true);
	total->updControl("OnePole/lpf/mrs_real/alpha", 0.99f);
	total->updControl("OnePole/lpf1/mrs_real/alpha", 0.99f);
	mrs_natural factor = 2;
	total->updControl("DownSampler/ds/mrs_natural/factor", factor);
	factor = 16;
	
	total->updControl("MaxArgMax/mxr/mrs_natural/nMaximums", 2);
	
	total->updControl("Peaker/pkr/mrs_natural/peakNeighbors", 10);
	total->updControl("Peaker/pkr/mrs_real/peakSpacing", 0.1);
	total->updControl("Peaker/pkr/mrs_natural/peakStart", 50);
	total->updControl("Peaker/pkr/mrs_natural/peakEnd", 180);
	
	total->updControl("Peaker/pkr/mrs_real/peakStrength", 0.65);
	total->updControl("Peaker/pkr/mrs_bool/peakHarmonics", true);
	
	total->updControl("BeatHistogram/histo/mrs_natural/startBin", 0);
	total->updControl("BeatHistogram/histo/mrs_natural/endBin", 200);

	total->linkControl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
	total->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
	total->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

	// prepare vectors for processing
	realvec iwin(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
				 total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
	realvec estimate(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
					 total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());




	mrs_real bin;
	mrs_natural onSamples;

//	int numPlayed =0;
//	mrs_natural wc=0;
//	mrs_natural samplesPlayed = 0;
//	mrs_natural repeatId = 1;

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
    {
		total->process(iwin, estimate);
		bin = estimate(1);
		bpms.push_back(bin);
    }
  
	float predicted_tempo;
	predicted_tempo = bpms[bpms.size()-1];

	evaluate_estimated_tempo(sfName, predicted_tempo, ground_truth_tempo);
	
	delete total;
}




void
tempo_medianSumBands(string sfName, float ground_truth_tempo, string resName, bool haveCollections)
{
	MarSystemManager mng;
	//mrs_natural nChannels;
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

	total->updControl("SoundFileSource/src/mrs_string/filename", sfName);
	srate = total->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
	
	mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
	mrs_natural hopSize = winSize / 8;

	total->updControl("mrs_natural/inSamples", hopSize);
	total->updControl("ShiftInput/si/mrs_natural/winSize", winSize);
	total->updControl("DownSampler/dsr1/mrs_natural/factor", 8);
	
	
	// wavelt filterbank envelope extraction controls
	total->updControl("WaveletPyramid/wvpt/mrs_bool/forward", true);
	total->updControl("OnePole/lpf/mrs_real/alpha", 0.99f);
	total->updControl("OnePole/lpf1/mrs_real/alpha", 0.99f);
	mrs_natural factor = 2;
	total->updControl("DownSampler/ds/mrs_natural/factor", factor);
	factor = 16;
	
	
	total->updControl("MaxArgMax/mxr/mrs_natural/nMaximums", 2);

	total->updControl("Peaker/pkr/mrs_natural/peakNeighbors", 10);
	total->updControl("Peaker/pkr/mrs_real/peakSpacing", 0.1);
	total->updControl("Peaker/pkr/mrs_natural/peakStart", 50);
	total->updControl("Peaker/pkr/mrs_natural/peakEnd", 180);

	total->updControl("Peaker/pkr/mrs_real/peakStrength", 0.65);
	total->updControl("Peaker/pkr/mrs_bool/peakHarmonics", true);

	total->updControl("BeatHistogram/histo/mrs_natural/startBin", 0);
	total->updControl("BeatHistogram/histo/mrs_natural/endBin", 200);

	
	total->linkControl("mrs_string/filename", "SoundFileSource/src/mrs_string/filename");
	total->linkControl("mrs_natural/pos", "SoundFileSource/src/mrs_natural/pos");
	total->linkControl("mrs_bool/hasData", "SoundFileSource/src/mrs_bool/hasData");

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
	//mrs_natural repeatId = 1;
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

	evaluate_estimated_tempo(sfName, bpms[bpms.size()/2], ground_truth_tempo);
    
  delete total;
}



void
tempo_bcWavelet(string sfName, string resName, bool haveCollections)
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
		total->updControl("Gain/normGain/mrs_real/gain", 0.05);
	}

	total->addMarSystem(mng.create("FullWaveRectifier", "fwr1"));
	total->addMarSystem(mng.create("ClipAudioRange", "clp"));


	cout << "NETWORK PREPARED" << endl;


	// prepare filename for reading
	total->updControl("SoundFileSource/src/mrs_string/filename", sfName);
	srate = total->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
	mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
	mrs_natural hopSize = winSize;
	nChannels = total->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();
	srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();
	offset = (mrs_natural) (start * srate * nChannels);
	duration = (mrs_natural) (length * srate * nChannels);
	total->updControl("mrs_natural/inSamples", hopSize);
	total->updControl("SoundFileSource/src/mrs_natural/pos", offset);

	// wavelt filterbank envelope extraction controls
	total->updControl("WaveletPyramid/wvpt/mrs_bool/forward", true);
	total->updControl("OnePole/lpf/mrs_real/alpha", 0.99f);

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
	lowpkr->updControl("mrs_natural/inSamples", inSamples);
	lowpkr->updControl("mrs_real/peakSpacing", 0.1);
	lowpkr->updControl("mrs_real/peakStrength", 0.5);
	lowpkr->updControl("mrs_natural/peakStart", 0);
	lowpkr->updControl("mrs_natural/peakEnd", inSamples);
	lowpkr->updControl("mrs_real/peakGain", 1.0);


	MarSystem* hipkr = mng.create("PeakerAdaptive", "hipkr");
	hipkr->updControl("mrs_natural/inSamples", inSamples);
	hipkr->updControl("mrs_real/peakSpacing", 0.05);
	hipkr->updControl("mrs_real/peakStrength", 0.6);
	hipkr->updControl("mrs_natural/peakStart", 0);
	hipkr->updControl("mrs_natural/peakEnd", inSamples);
	hipkr->updControl("mrs_real/peakGain", 1.0);




	lowdest->updControl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
	hidest->updControl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
	plowdest->updControl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));
	phidest->updControl("mrs_natural/inSamples", total->getctrl("mrs_natural/onSamples"));

	lowdest->updControl("mrs_real/israte", srate);
	lowdest->updControl("mrs_string/filename", "lowband.wav");

	hidest->updControl("mrs_real/israte", srate);
	hidest->updControl("mrs_string/filename", "hiband.wav");

	plowdest->updControl("mrs_real/israte", srate);
	plowdest->updControl("mrs_string/filename", "plowband.wav");

	phidest->updControl("mrs_real/israte", srate);
	phidest->updControl("mrs_string/filename", "phiband.wav");

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
	mix->updControl("SoundFileSource/orsrc/mrs_bool/mute", true);
	mix->addMarSystem(mng.create("SoundFileSource", "bdsrc"));
	mix->addMarSystem(mng.create("SoundFileSource", "sdsrc"));

	playback->addMarSystem(mix);
	playback->addMarSystem(mng.create("Sum", "sum"));
	playback->addMarSystem(mng.create("SoundFileSink", "adest"));
	cout << "SOUNDFILESINK srate = " << srate << endl;




	playback->updControl("Fanout/mix/SoundFileSource/orsrc/mrs_string/filename", sfName);


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


	playback->updControl("SoundFileSink/adest/mrs_real/israte", srate);
	playback->updControl("SoundFileSink/adest/mrs_string/filename", "boomchick.wav");


	cout << "******PLAYBACK******" << endl;

	while(playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_bool/hasData")->to<mrs_bool>())
    {
		if (lowtimes[lowtindex] < samplesPlayed)
		{
			lowtindex++;

			if (lowtindex > 1)
				cout << "IOI = " << lowtimes[lowtindex] - lowtimes[lowtindex-1] << endl;
			playback->updControl("Fanout/mix/SoundFileSource/bdsrc/mrs_string/filename", bdname);
			playback->updControl("Fanout/mix/SoundFileSource/bdsrc/mrs_natural/pos", 0);
		}

		if (hitimes[hitindex] < samplesPlayed)
		{
			hitindex++;
			playback->updControl("Fanout/mix/SoundFileSource/sdsrc/mrs_string/filename", sdname);
			playback->updControl("Fanout/mix/SoundFileSource/sdsrc/mrs_natural/pos", 0);
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
tempo_bcFilter(string sfName, string resName, bool haveCollections)
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
	lfilter->updControl("Filter/llfilter/mrs_realvec/ncoeffs", bl);
	lfilter->updControl("Filter/llfilter/mrs_realvec/dcoeffs", al);
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
	hfilter->updControl("Filter/hhfilter/mrs_realvec/ncoeffs", bh);
	hfilter->updControl("Filter/hhfilter/mrs_realvec/dcoeffs", ah);
	filters->addMarSystem(hfilter);

	total->addMarSystem(filters);

	// prepare filename for reading
	total->updControl("SoundFileSource/src/mrs_string/filename", sfName);
	// total->updControl("AudioSink/dest/mrs_bool/initAudio", true);

	srate = total->getctrl("SoundFileSource/src/mrs_real/osrate")->to<mrs_real>();
	mrs_natural ch = total->getctrl("SoundFileSource/src/mrs_natural/onObservations")->to<mrs_natural>();
	mrs_real tg = 1.0 / ch;
	total->updControl("Gain/tgain/mrs_real/gain", tg);
	mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 2048;

	mrs_natural hopSize = winSize;
	nChannels = total->getctrl("SoundFileSource/src/mrs_natural/nChannels")->to<mrs_natural>();
	srate = total->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();
	offset = (mrs_natural) (start * srate * nChannels);
	duration = (mrs_natural) (length * srate * nChannels);
	total->updControl("mrs_natural/inSamples", hopSize);
	total->updControl("SoundFileSource/src/mrs_natural/pos", offset);

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
	lowpkr->updControl("mrs_natural/inSamples", inSamples);
	lowpkr->updControl("mrs_real/peakSpacing", 0.3);
	lowpkr->updControl("mrs_real/peakStrength", 0.7);
	lowpkr->updControl("mrs_natural/peakStart", 0);
	lowpkr->updControl("mrs_natural/peakEnd", inSamples);
	lowpkr->updControl("mrs_real/peakGain", 1.0);
	lowpkr->updControl("mrs_natural/peakStrengthReset", 4);
	lowpkr->updControl("mrs_real/peakDecay", 0.9);

	MarSystem* hipkr = mng.create("PeakerAdaptive", "hipkr");
	hipkr->updControl("mrs_natural/inSamples", inSamples);
	hipkr->updControl("mrs_real/peakSpacing", 0.3);
	hipkr->updControl("mrs_real/peakStrength", 0.7);
	hipkr->updControl("mrs_natural/peakStart", 0);
	hipkr->updControl("mrs_natural/peakEnd", inSamples);
	hipkr->updControl("mrs_real/peakGain", 1.0);
	hipkr->updControl("mrs_natural/peakStrengthReset", 4);
	hipkr->updControl("mrs_real/peakDecay", 0.9);

	lowdest->updControl("mrs_natural/inSamples",
					 total->getctrl("mrs_natural/onSamples"));
	hidest->updControl("mrs_natural/inSamples",
					total->getctrl("mrs_natural/onSamples"));
	plowdest->updControl("mrs_natural/inSamples",
					  total->getctrl("mrs_natural/onSamples"));
	phidest->updControl("mrs_natural/inSamples",
					 total->getctrl("mrs_natural/onSamples"));

	lowdest->updControl("mrs_real/israte", srate);
	lowdest->updControl("mrs_string/filename", "lowband.wav");

	hidest->updControl("mrs_real/israte", srate);
	hidest->updControl("mrs_string/filename", "hiband.wav");

	plowdest->updControl("mrs_real/israte", srate);
	plowdest->updControl("mrs_string/filename", "plowband.wav");

	phidest->updControl("mrs_real/israte", srate);
	phidest->updControl("mrs_string/filename", "phiband.wav");

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
#ifdef MARSYA_MIDIIO
				total->updControl("MidiOutput/devibot/mrs_natural/byte2", DEVIBOT_NA);
				total->updControl("MidiOutput/devibot/mrs_natural/byte3", 50);
				total->updControl("MidiOutput/devibot/mrs_natural/byte1", 144);
				total->updControl("MidiOutput/devibot/mrs_bool/sendMessage", true);
#endif //MARSYAS_MIDIIO
			}
		}
		for (mrs_natural t=0; t < onSamples; t++)
		{
			if (phiwin(0,t) > 0.0)
			{
				hitimes.push_back(samplesPlayed+t);
#ifdef MARSYAS_MIDIO
				total->updControl("MidiOutput/devibot/mrs_natural/byte2", DEVIBOT_GE);
				total->updControl("MidiOutput/devibot/mrs_natural/byte3", 50);
				total->updControl("MidiOutput/devibot/mrs_natural/byte1", 144);
				total->updControl("MidiOutput/devibot/mrs_bool/sendMessage", true);
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
	mix->updControl("SoundFileSource/orsrc/mrs_bool/mute", true);
	mix->addMarSystem(mng.create("SoundFileSource", "bdsrc"));
	mix->addMarSystem(mng.create("SoundFileSource", "sdsrc"));

	playback->addMarSystem(mix);
	playback->addMarSystem(mng.create("Sum", "sum"));
	playback->addMarSystem(mng.create("SoundFileSink", "adest"));
	playback->addMarSystem(mng.create("AudioSink", "dest"));
	playback->addMarSystem(mng.create("MidiOutput", "devibot"));

	cout << "SOUNDFILESINK srate = " << srate << endl;

	playback->updControl("Fanout/mix/SoundFileSource/orsrc/mrs_string/filename", sfName);

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


	playback->updControl("SoundFileSink/adest/mrs_real/israte", srate);
	playback->updControl("SoundFileSink/adest/mrs_string/filename", "boomchick.wav");

	playback->updControl("Fanout/mix/SoundFileSource/bdsrc/mrs_string/filename", bdname);
	playback->updControl("Fanout/mix/SoundFileSource/bdsrc/mrs_natural/pos", 0);


	playback->updControl("AudioSink/dest/mrs_bool/initAudio", true);
	while(playback->getctrl("Fanout/mix/SoundFileSource/orsrc/mrs_bool/hasData")->to<mrs_bool>())
    {
		if (lowtimes[lowtindex] < samplesPlayed)
		{
			lowtindex++;

			if (lowtindex > 1)

				cout << "IOI = " << lowtimes[lowtindex] - lowtimes[lowtindex-1] << endl;
			// Robot Control
#ifdef MARSYAS_MIDIIO
			playback->updControl("MidiOutput/devibot/mrs_natural/byte2", DEVIBOT_GE);
			playback->updControl("MidiOutput/devibot/mrs_natural/byte3", 50);
			playback->updControl("MidiOutput/devibot/mrs_natural/byte1", 144);
			playback->updControl("MidiOutput/devibot/mrs_bool/sendMessage", true);
#endif

			// Bass Drum Play back
			playback->updControl("Fanout/mix/SoundFileSource/bdsrc/mrs_string/filename", bdname);
			playback->updControl("Fanout/mix/SoundFileSource/bdsrc/mrs_natural/pos", 0);
		}

		if (hitimes[hitindex] < samplesPlayed)
		{
			hitindex++;

			// Robot Control
#ifdef MARSYAS_MIDIO
			playback->updControl("MidiOutput/devibot/mrs_natural/byte2", DEVIBOT_NA);
			playback->updControl("MidiOutput/devibot/mrs_natural/byte3", 50);
			playback->updControl("MidiOutput/devibot/mrs_natural/byte1", 144);
			playback->updControl("MidiOutput/devibot/mrs_bool/sendMessage", true);
#endif

			// Snare Drum PlayBack
			playback->updControl("Fanout/mix/SoundFileSource/sdsrc/mrs_string/filename", sdname);
			playback->updControl("Fanout/mix/SoundFileSource/sdsrc/mrs_natural/pos", 0);
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
	net->updControl("SoundFileSource/src/mrs_string/filename", sfName);
	mrs_real srate = net->getctrl("SoundFileSource/src/mrs_real/israte")->to<mrs_real>();
	mrs_natural winSize = (mrs_natural)(srate / 22050.0) * 65536;
	
	net->updControl("mrs_natural/inSamples", winSize);

	ofstream ofs;
	ofs.open("net.mpl");
	ofs << *net << endl;
	
	ofs.close();
	
	
	net->tick();
	const mrs_realvec& data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();

	
	float max = 0.0;
	int max_i;
	
	for (int i=0; i < winSize/4; ++i) 
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
	
	
	for (int i = max_i + start; i <= max_i + end; ++i)
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
	
	for (int i=0; i < winSize/4; ++i) 
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
	
	for (int i = max_i + start; i <= max_i + end; ++i)
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


mrs_bool
readGTBeatsFile(MarSystem* beattracker, mrs_string gtBeatsFile, mrs_string audioFile, mrs_bool startPoint)
{	
	mrs_natural file = gtBeatsFile.rfind(".", gtBeatsFile.length()-1);

	mrs_bool readFileOK = true;
	mrs_string line;
	ifstream inStream;
	FileName inputFileGT(audioFile);
	
	//if gtBeatsFile = directory => add extension (.txt or .beats) to the end of filepath
	if(file == -1)
	{
		ostringstream oss;	
		//FILE* file;
		oss << gtBeatsFile << inputFileGT.nameNoExt() << ".txt";
		//check if ground-truth file, with .txt extension, exists (ifnot try with .beats extension)
		if(fopen(oss.str().c_str(), "r"))
		{
			inStream.open(oss.str().c_str());
			getline (inStream, line);
			phase_ = strtod(line.substr(0, line.find(" ")).c_str(), NULL);

			beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/gtBeatsFile", oss.str());
		}

		//else, check if ground-truth file, with .beats extension, exists (ifnot return to normal induction)
		else
		{
			oss.str("");
			oss << gtBeatsFile << inputFileGT.nameNoExt() << ".beats";
			if(fopen(oss.str().c_str(), "r"))
			{
				inStream.open(oss.str().c_str());
				getline (inStream, line);
				phase_ = strtod(line.substr(0, line.find(" ")).c_str(), NULL);

				beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/gtBeatsFile", oss.str());
			}
			else
			{
				beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "regular");
				cout << "\nGround-truth beat times file do not exists! -> " << gtBeatsFile << "(.txt/.beats)" 
					<< "\nRunning normal induction..." << endl;
				readFileOK = false;

				exit(1);
			}
		}	
	}
	
	//if gtBeatsFile = file => check if it is .txt or .beats file
	else
	{
		//check if .txt or .beats extension
		mrs_string extension = gtBeatsFile.substr(file+1, gtBeatsFile.length()-1);
		if(strcmp(extension.c_str(), "txt") == 0 || strcmp(extension.c_str(), "beats") == 0)
		{
			//if file exists
			if(fopen(gtBeatsFile.c_str(), "r"))
			{
				inStream.open(gtBeatsFile.c_str());
				getline (inStream, line);
				phase_ = strtod(line.substr(0, line.find(" ")).c_str(), NULL);

				beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/gtBeatsFile", gtBeatsFile);
			}
			else
			{
				beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "regular");
				cout << "\nGround-truth beat times file do not exists! -> " << gtBeatsFile
					<< "\nRunning normal induction..." << endl;
				readFileOK = false;

				exit(1);
			}
		}

		//if not run regular induction
		else
		{
			beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "regular");
			cout << "\nGround-truth beat times file ." << extension 
				<< " extension unknown! Spectify a .txt or .beats file." 
				<< "\nRunning normal induction..." << endl;
			readFileOK = false;

			exit(1);
		}
	}

	if(startPoint && readFileOK && phase_ > induction_time)
	{
		//phase + 0.5 for extending induction till the inputed start beat point
		induction_time = phase_ + 0.5;
	}

	return readFileOK;
}


void
tempo_ibt(string sfName, float ground_truth_tempo, string outputTxt, bool haveCollections)
{

	mrs_bool micinputopt = false;
	mrs_string givefirst2beats = "-1";
	mrs_string givefirst1beat = "-1";
	mrs_string givefirst2beats_startpoint = "-1";
	mrs_string givefirst1beat_startpoint = "-1";
	mrs_bool inductionoutopt = false;
	mrs_bool dumbinductionopt = false;
	mrs_bool logfileopt = false;
	mrs_string execPath = "";
	mrs_bool noncausalopt = false;
	
	
	MarSystemManager mng;

	// assemble the processing network 
	MarSystem* audioflow = mng.create("Series", "audioflow");
	if(micinputopt) //capture audio via microphone
		audioflow->addMarSystem(mng.create("AudioSource", "micsrc"));
	else
		audioflow->addMarSystem(mng.create("SoundFileSource", "src"));
		
		audioflow->addMarSystem(mng.create("Stereo2Mono", "src")); //replace by a "Monofier" MarSystem (to be created) [!]
			
			MarSystem* beattracker = mng.create("FlowThru","beattracker");
				//beattracker->addMarSystem(mng.create("Stereo2Mono", "src")); //replace by a "Monofier" MarSystem (to be created) [!]
				
				MarSystem* onsetdetectionfunction = mng.create("Series", "onsetdetectionfunction");
					onsetdetectionfunction->addMarSystem(mng.create("ShiftInput", "si")); 
					onsetdetectionfunction->addMarSystem(mng.create("Windowing", "win")); 
					onsetdetectionfunction->addMarSystem(mng.create("Spectrum","spk"));
					onsetdetectionfunction->addMarSystem(mng.create("PowerSpectrum", "pspk"));
					onsetdetectionfunction->addMarSystem(mng.create("Flux", "flux"));
				
				//if(sonicOutFlux)
				//	onsetdetectionfunction->addMarSystem(mng.create("SonicVisualiserSink", "sonicsink"));
							
			beattracker->addMarSystem(onsetdetectionfunction);
			beattracker->addMarSystem(mng.create("ShiftInput", "acc"));

				MarSystem* normfiltering = mng.create("Series", "normfiltering");
					normfiltering->addMarSystem(mng.create("Filter","filt1"));
					
				//if(sonicOutFluxFilter)
				//	normfiltering->addMarSystem(mng.create("SonicVisualiserSink", "sonicsinkfilt"));
					
					normfiltering->addMarSystem(mng.create("Reverse","rev1"));
					normfiltering->addMarSystem(mng.create("Filter","filt2"));
					normfiltering->addMarSystem(mng.create("Reverse","rev2"));
		
				//if(sonicOutFluxFilter)
				//	normfiltering->addMarSystem(mng.create("SonicVisualiserSink", "sonicsinkfilt"));

			beattracker->addMarSystem(normfiltering);

			MarSystem* tempoinduction = mng.create("FlowThru", "tempoinduction");

				MarSystem* tempohypotheses = mng.create("Fanout", "tempohypotheses");
					
					MarSystem* tempo = mng.create("Series", "tempo");
						tempo->addMarSystem(mng.create("AutoCorrelation","acf"));
						tempo->addMarSystem(mng.create("Peaker", "pkr"));
						tempo->addMarSystem(mng.create("MaxArgMax", "mxr"));
						
					tempohypotheses->addMarSystem(tempo);

					MarSystem* phase = mng.create("Series", "phase");
						phase->addMarSystem(mng.create("PeakerOnset","pkronset"));
						phase->addMarSystem(mng.create("OnsetTimes","onsettimes"));

					tempohypotheses->addMarSystem(phase);

				tempoinduction->addMarSystem(tempohypotheses);
				tempoinduction->addMarSystem(mng.create("TempoHypotheses", "tempohyp"));
		
			beattracker->addMarSystem(tempoinduction);

			MarSystem* initialhypotheses = mng.create("FlowThru", "initialhypotheses");
				initialhypotheses->addMarSystem(mng.create("PhaseLock", "phaselock"));
				
			beattracker->addMarSystem(initialhypotheses);
			
			MarSystem* agentpool = mng.create("Fanout", "agentpool");
				for(int i = 0; i < NR_AGENTS; ++i)
				{
					ostringstream oss;
					oss << "agent" << i;
					agentpool->addMarSystem(mng.create("BeatAgent", oss.str()));
				}

			beattracker->addMarSystem(agentpool);
			beattracker->addMarSystem(mng.create("BeatReferee", "br"));
			if(strcmp(output.c_str(), "none") != 0)
				beattracker->addMarSystem(mng.create("BeatTimesSink", "sink"));

		audioflow->addMarSystem(beattracker);

	//for synthesizing clicks (on beats) with audio and playing/recording
		MarSystem* IBTsystem = NULL;
	if(audiofileopt || audioopt)
	{
		audioflow->addMarSystem(mng.create("Gain","gainaudio"));

		MarSystem* beatmix;
		//if audiofile and not audio play mode, with mic input, then only play clicks
		if(micinputopt && ((!audiofileopt && audioopt) || (audiofileopt && audioopt)))
			beatmix = mng.create("Series","beatmix");
		else //if audiofile without audio play mode, with mic input, then save audio + clicks
			beatmix = mng.create("Fanout","beatmix");
		
			beatmix->addMarSystem(audioflow);
			
			MarSystem* beatsynth = mng.create("Series","beatsynth");
				beatsynth->addMarSystem(mng.create("NoiseSource","noisesrc"));
				beatsynth->addMarSystem(mng.create("ADSR","env"));
				beatsynth->addMarSystem(mng.create("Gain", "gainbeats"));
			beatmix->addMarSystem(beatsynth);
		
		IBTsystem = mng.create("Series", "IBTsystem");
		IBTsystem->addMarSystem(beatmix);
		
		if(audioopt) //for playing audio with clicks
			IBTsystem->addMarSystem(mng.create("AudioSink", "output"));
		if(audiofileopt) //for saving audio file with clicks
			IBTsystem->addMarSystem(mng.create("SoundFileSink", "fdest"));
	}

	///////////////////////////////////////////////////////////////////////////////////////
	//link controls
	///////////////////////////////////////////////////////////////////////////////////////
	//if(!micinputopt)
	//	IBTsystem->linkControl("mrs_bool/hasData", 
	//		"Fanout/beatmix/Series/audioflow/SoundFileSource/src/mrs_bool/hasData");

	//Link LookAheadSamples used in PeakerOnset for compensation when retriving the actual initial OnsetTimes
	tempoinduction->linkControl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_natural/lookAheadSamples", 
		"Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/lookAheadSamples");
	//Link accumulator/ShiftInput size to OnsetTimes for compensating the timing in order to the considered accumulator size
	beattracker->linkControl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/accSize",
		"ShiftInput/acc/mrs_natural/winSize");

	//Pass hypotheses matrix (from tempoinduction stage) to PhaseLock
	beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_realvec/beatHypotheses", 
		"FlowThru/tempoinduction/mrs_realvec/innerOut");
	//link backtrace option
	beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_bool/backtrace", 
		"BeatReferee/br/mrs_bool/backtrace");
	//link corFactor
	beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/corFactor", 
		"BeatReferee/br/mrs_real/corFactor");
	
	//Pass initital hypotheses to BeatReferee
	beattracker->linkControl("BeatReferee/br/mrs_realvec/beatHypotheses", 
		"FlowThru/initialhypotheses/mrs_realvec/innerOut");

	//PhaseLock nr of BPM hypotheses = nr MaxArgMax from ACF
	beattracker->linkControl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nMaximums", 
		"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPeriods");
	//TempoHypotheses nr of BPMs = nr MaxArgMax from ACF
	beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPeriods",  
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPeriodHyps");
	//OnsetTimes nr of BPMs = nr MaxArgMax from ACF (this is to avoid FanOut crash!)
	beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPeriodHyps", 
		"FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/nPeriods");
	
	//PhaseLock nr of Phases per BPM = nr of OnsetTimes considered
	beattracker->linkControl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/n1stOnsets", 
		"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPhases");
	//TempoHypotheses nr of Beat hypotheses = nr of OnsetTimes considered
	beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/nPhases", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPhasesPerPeriod");
	//nr of MaxArgMax Phases per BPM = nr OnsetTimes considered (this is to avoid FanOut crash!)
	beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/nrPhasesPerPeriod", 
		"FlowThru/tempoinduction/Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/fanoutLength");

	//Pass enabled (muted) BeatAgents (from FanOut) to the BeatReferee
	beattracker->linkControl("Fanout/agentpool/mrs_realvec/muted", "BeatReferee/br/mrs_realvec/mutedAgents");
	//Pass tempohypotheses Fanout muted vector to the BeatReferee, for disabling induction after induction timming
	beattracker->linkControl("FlowThru/tempoinduction/Fanout/tempohypotheses/mrs_realvec/muted", 
		"BeatReferee/br/mrs_realvec/inductionEnabler");

	//Link agentControl matrix from the BeatReferee to each agent in the pool
	//Also link period limits from PhaseLock
	for(int i = 0; i < NR_AGENTS; ++i)
	{
		ostringstream oss;
		oss << "agent" << i;
		beattracker->linkControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_realvec/agentControl", 
			"BeatReferee/br/mrs_realvec/agentControl");
		//and BeatAgent
		beattracker->linkControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_natural/minPeriod",
			"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/minPeriod");
		beattracker->linkControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_natural/maxPeriod",
			"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/maxPeriod");
	}

	//Defines tempo induction time after which the BeatAgents' hypotheses are populated:
	//PhaseLock timing = induction time
	beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime", 
		"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime");	
	//BeatReferee timing = induction time
	beattracker->linkControl("BeatReferee/br/mrs_natural/inductionTime", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime");
	//Link Period Limits to PhaseLock
	beattracker->linkControl("BeatReferee/br/mrs_natural/minPeriod", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/minPeriod");
	beattracker->linkControl("BeatReferee/br/mrs_natural/maxPeriod", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/maxPeriod");
	//OnsetTimes timing = induction time
	beattracker->linkControl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/inductionTime", 
		"BeatReferee/br/mrs_natural/inductionTime");

	//Link score function from a BeatAgent (all have the same) to PhaseLock
	//Link also tolerance margins
	beattracker->linkControl("Fanout/agentpool/BeatAgent/agent0/mrs_string/scoreFunc", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/scoreFunc");
	beattracker->linkControl("Fanout/agentpool/BeatAgent/agent0/mrs_real/innerMargin", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/innerMargin");
	beattracker->linkControl("Fanout/agentpool/BeatAgent/agent0/mrs_real/lftOutterMargin", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/lftOutterMargin");
	beattracker->linkControl("Fanout/agentpool/BeatAgent/agent0/mrs_real/rgtOutterMargin", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/rgtOutterMargin");

	//Link BPM conversion parameters to BeatReferee:
	beattracker->linkControl("BeatReferee/br/mrs_natural/hopSize", "mrs_natural/inSamples");

	//Link BPM conversion parameters to TempoHypotheses
	beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/hopSize", 
		"BeatReferee/br/mrs_natural/hopSize");
	beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs", 
		"BeatReferee/br/mrs_real/srcFs");

	//Link TickCounter from BeatRefree -> for updating IBT's timer
	beattracker->linkControl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/tickCount",
		"BeatReferee/br/mrs_natural/tickCount");
	beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/tickCount",	
		"FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/tickCount");
	beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/tickCount", 
		"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/tickCount");	
	
	//link frames2seconds adjustment from BeatTimesSink to PhaseLock
	beattracker->linkControl("BeatTimesSink/sink/mrs_natural/adjustment",
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/adjustment");
	//link dumbinduction to PhaseLock
	beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_bool/dumbInduction",
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_bool/dumbInduction");

	//Link Beat Output Sink parameters with the used ones:
	if(strcmp(output.c_str(), "none") != 0)
	{
		beattracker->linkControl("BeatTimesSink/sink/mrs_natural/hopSize", 
			"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/hopSize");
		beattracker->linkControl("BeatTimesSink/sink/mrs_real/srcFs", 
			"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs");
		beattracker->linkControl("BeatTimesSink/sink/mrs_natural/winSize", 
			"Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize");
		beattracker->linkControl("BeatTimesSink/sink/mrs_natural/tickCount", 
			"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/tickCount");
	}
	
	
	//Link SonicVisualiserSink parameters with the used ones:
	/*
	if(sonicOutFlux)
	{
		beattracker->linkControl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_natural/hopSize", 
			"BeatReferee/br/mrs_natural/hopSize");
		beattracker->linkControl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_real/srcFs", 
			"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs");
	}
	if(sonicOutFluxFilter)
	{
		beattracker->linkControl("Series/normfiltering/SonicVisualiserSink/sonicsinkfilt/mrs_natural/hopSize", 
			"BeatReferee/br/mrs_natural/hopSize");
		beattracker->linkControl("Series/normfiltering/SonicVisualiserSink/sonicsinkfilt/mrs_real/srcFs", 
			"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs");
	}
	*/

	//link beatdetected with noise ADSR -> for clicking when beat:
	if(audiofileopt || audioopt)
	{
		if(micinputopt && ((!audiofileopt && audioopt) || (audiofileopt && audioopt))) //if audiofile and audio play mode, with mic input, then only play clicks
			IBTsystem->linkControl("Series/beatmix/Series/audioflow/FlowThru/beattracker/BeatReferee/br/mrs_real/beatDetected", 
				"Series/beatmix/Series/beatsynth/ADSR/env/mrs_real/nton");
		else //if audiofile without audio play mode, with mic input, then save audio + clicks
			IBTsystem->linkControl("Fanout/beatmix/Series/audioflow/FlowThru/beattracker/BeatReferee/br/mrs_real/beatDetected", 
				"Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/nton");

	}

	//link sampling rate and hopsize for BPM conversion (in PhaseLock)
	beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/hopSize", 
			"BeatReferee/br/mrs_natural/hopSize");
	beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/srcFs", 
			"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs");

	//For Induction Ground-Truth in "PhaseLock"
	if(strcmp(givefirst2beats.c_str(), "-1") != 0 || strcmp(givefirst1beat.c_str(), "-1") != 0 || 
		strcmp(givefirst2beats_startpoint.c_str(), "-1") != 0 || strcmp(givefirst1beat_startpoint.c_str(), "-1") != 0)
	{
		//if normal induction_gt:
		if(strcmp(givefirst2beats.c_str(), "-1") != 0)
		{
			if(readGTBeatsFile(beattracker, givefirst2beats, sfName, false))
				beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "givefirst2beats");
			else givefirst2beats = "-1";
		}
		//if induction_gt starting tracking at given first beat
		else if(strcmp(givefirst2beats_startpoint.c_str(), "-1") != 0)
		{
			if(readGTBeatsFile(beattracker, givefirst2beats_startpoint, sfName, true))
			{
				beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "givefirst2beats_startpoint");
				backtraceopt = true;
			}
			else givefirst2beats_startpoint = "-1";
		}
		else if(strcmp(givefirst1beat.c_str(), "-1") != 0)
		{
			if(readGTBeatsFile(beattracker, givefirst1beat, sfName, true))
			{
				beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "givefirst1beat");
			}
			else givefirst1beat = "-1";
		}
		else if(strcmp(givefirst1beat_startpoint.c_str(), "-1") != 0)
		{
			if(readGTBeatsFile(beattracker, givefirst1beat_startpoint, sfName, true))
			{
				beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "givefirst1beat_startpoint");
				backtraceopt = true;
			}
			else givefirst1beat_startpoint = "-1";
		}
	}

	//if requested output of induction best period hypothesis link output directory
	if(inductionoutopt)
		beattracker->linkControl("BeatTimesSink/sink/mrs_string/destFileName", 
			"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/inductionOut");

	//link Filters coeffs
	beattracker->linkControl("Series/normfiltering/Filter/filt2/mrs_realvec/ncoeffs",
					   "Series/normfiltering/Filter/filt1/mrs_realvec/ncoeffs");
	beattracker->linkControl("Series/normfiltering/Filter/filt2/mrs_realvec/dcoeffs",
					   "Series/normfiltering/Filter/filt1/mrs_realvec/dcoeffs");

	//link bestFinalAgentHistory from BeatReferee to BeatTimesSink
	beattracker->linkControl("BeatTimesSink/sink/mrs_realvec/bestFinalAgentHistory", 
		"BeatReferee/br/mrs_realvec/bestFinalAgentHistory");
	//link inputSize for knowing when musical piece finishes
	beattracker->linkControl("BeatTimesSink/sink/mrs_natural/soundFileSize", 
		"BeatReferee/br/mrs_natural/soundFileSize");
	/*
	if(sonicOutFluxFilter)
		beattracker->linkControl("Series/normfiltering/SonicVisualiserSink/sonicsinkfilt/mrs_natural/soundFileSize", 
			"BeatTimesSink/sink/mrs_natural/soundFileSize");
	if(sonicOutFlux)
		beattracker->linkControl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_natural/soundFileSize", 
			"BeatTimesSink/sink/mrs_natural/soundFileSize");
	*/
	//link non-causal mode flag
	beattracker->linkControl("BeatTimesSink/sink/mrs_bool/nonCausal", 
		"BeatReferee/br/mrs_bool/nonCausal");


	///////////////////////////////////////////////////////////////////////////////////////
	// update controls
	///////////////////////////////////////////////////////////////////////////////////////

	//in non-causal mode always backtrace
	if(noncausalopt)
		backtraceopt = true;
	
	mrs_natural inputSize;
	if(micinputopt) //if in mic mode
	{
		mrs_real micRate = 44100.0;
		mrs_real length = 1000.0; //length of microphone capture (big number for "endless" capturing)
		audioflow->updControl("mrs_real/israte", micRate);
		audioflow->updControl("AudioSource/micsrc/mrs_natural/nChannels", 2);
		audioflow->updControl("AudioSource/micsrc/mrs_bool/initAudio", true);
		
		inputSize = (mrs_natural) ceil(length * micRate);
	}
	else //if in soundfile mode
	{
		audioflow->updControl("SoundFileSource/src/mrs_string/filename", sfName);
		inputSize = audioflow->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>(); //sound input file size (in samples)
	}

	//best result till now are using dB power Spectrum!
	beattracker->updControl("Series/onsetdetectionfunction/PowerSpectrum/pspk/mrs_string/spectrumType", "magnitude");

	beattracker->updControl("Series/onsetdetectionfunction/Flux/flux/mrs_string/mode", "DixonDAFX06");

	beattracker->updControl("mrs_natural/inSamples", HOPSIZE);
	beattracker->updControl("Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize", WINSIZE);

	mrs_real fsSrc = beattracker->getctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_real/israte")->to<mrs_real>();

	//induction time (in nr. of ticks) -> -1 because starting index on accumulator is 0 and it finnishes at accSize-1
	//So IBT's tick time notion starts also on 0 and finnishes on sound_file_size(in_frames)-1.
	mrs_natural adjustment = HOPSIZE / 2; //linked to BeatTimesSink
	//adjustment = (winSize_ - hopSize_) + floor((mrs_real) winSize_/2);
	
	mrs_natural inductionTickCount = ((mrs_natural) ceil((induction_time * fsSrc + adjustment) / HOPSIZE)) -1;
	//to avoid induction time greater than input file size
	//(in this case the induction time will equal the file size)
	if((inputSize / HOPSIZE) < inductionTickCount)
		inductionTickCount = (inputSize / HOPSIZE) -1;

	beattracker->updControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime", inductionTickCount);
	beattracker->updControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_bool/dumbInduction", dumbinductionopt);

	mrs_natural metricalChangeTime = inputSize / HOPSIZE; //if metricalChangeTime = -1 it shall equalize the full input file size (in ticks)
	if(metrical_change_time != -1.0 && metrical_change_time >= 0.0)
		metricalChangeTime = ((mrs_natural) (metrical_change_time * fsSrc) / HOPSIZE) + 1; //allowed metrical change time (in nr. of ticks)
	
	//Size of accumulator equals inductionTime + 1 -> [0, inductionTime]
	//beattracker->updControl("ShiftInput/acc/mrs_natural/winSize", inductionTickCount+1);
	//Size of accumulator equals two times the inductionTime for better filtering (must be bigger than inductionTime)
	//(and to avoid strange slow memory behaviour with inductionTime=5)

	mrs_natural accSize = 2*inductionTickCount;
	if(accSize > (inputSize / HOPSIZE)) //to avoid having an accumulator greater then the file size
		accSize = (inputSize / HOPSIZE) +1; 

	beattracker->updControl("ShiftInput/acc/mrs_natural/winSize", accSize);

	mrs_natural pkinS = tempoinduction->getctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
	mrs_natural peakEnd = (mrs_natural)((60.0 * fsSrc)/(MIN_BPM * HOPSIZE)); //MinBPM (in frames)
	mrs_natural peakStart = (mrs_natural) ((60.0 * fsSrc)/(MAX_BPM * HOPSIZE));  //MaxBPM (in frames)
	
	//mrs_real peakSpacing = ceil(((peakEnd-peakStart) * 4.0) / ((mrs_real)(MAX_BPM-MIN_BPM))) / pkinS;
	
	mrs_real peakSpacing = ((mrs_natural) (fsSrc/HOPSIZE) * (1.0-(60.0/64.0))) / (pkinS * 1.0); //spacing between peaks (4BPMs at 60BPM resolution)
	//mrs_real peakSpacing = ((mrs_natural) (peakEnd-peakStart) / (2*BPM_HYPOTHESES)) / (pkinS * 1.0);  //nrPeaks <= 2*nrBPMs

	//cout << "PkinS: " << pkinS << "; peakEnd: " << peakEnd << "; peakStart: " << peakStart << "; peakSpacing: " << peakSpacing << endl;

	tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
	tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakStrength", 0.75); //0.75
	tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakStart", peakStart);
	tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakEnd", peakEnd);
	tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakGain", 2.0);
	

	tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nMaximums", BPM_HYPOTHESES);

	mrs_natural lookAheadSamples = 6; //(higher than 3 due to unconsistencies on the very beginning of the filter window)
	mrs_real thres = 1.2;//1.3; //1.75 (1.2)

	tempoinduction->updControl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_natural/lookAheadSamples", lookAheadSamples);
	tempoinduction->updControl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_real/threshold", thres);
	
	tempoinduction->updControl("Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/n1stOnsets", PHASE_HYPOTHESES);
	
	//Pass chosen score_function to each BeatAgent in the pool:
	for(int i = 0; i < NR_AGENTS; ++i)
	{
		ostringstream oss, oss2;
		oss << "agent" << i;
		oss2 << "Agent" << i;
		beattracker->updControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_string/scoreFunc", score_function);

		beattracker->updControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_real/lftOutterMargin", LFT_OUTTER_MARGIN);
		beattracker->updControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_real/rgtOutterMargin", RGT_OUTTER_MARGIN);
		beattracker->updControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_real/innerMargin", INNER_MARGIN);
		
		//THIS IS TO REMOVE -> SEE INOBSNAMES IN BEATAGENT!!
		beattracker->updControl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_string/identity", oss2.str());
	}

	mrs_natural minPeriod = (mrs_natural) floor(60.0 / (MAX_BPM * HOPSIZE) * fsSrc);
	mrs_natural maxPeriod = (mrs_natural) ceil(60.0 / (MIN_BPM * HOPSIZE) * fsSrc);

	beattracker->updControl("BeatReferee/br/mrs_real/srcFs", fsSrc);
	beattracker->updControl("BeatReferee/br/mrs_natural/minPeriod", minPeriod);
	beattracker->updControl("BeatReferee/br/mrs_natural/maxPeriod", maxPeriod);
	beattracker->updControl("BeatReferee/br/mrs_real/obsoleteFactor", OBSOLETE_FACTOR);
	beattracker->updControl("BeatReferee/br/mrs_natural/lostFactor", LOST_FACTOR);
	beattracker->updControl("BeatReferee/br/mrs_real/childrenScoreFactor", CHILDREN_SCORE_FACTOR);
	beattracker->updControl("BeatReferee/br/mrs_real/bestFactor", BEST_FACTOR);
	beattracker->updControl("BeatReferee/br/mrs_real/corFactor", CORRECTION_FACTOR);
	beattracker->updControl("BeatReferee/br/mrs_real/child1Factor", (mrs_real) CHILD1_FACTOR);
	beattracker->updControl("BeatReferee/br/mrs_real/child2Factor", (mrs_real) CHILD2_FACTOR);
	beattracker->updControl("BeatReferee/br/mrs_real/child3Factor", (mrs_real) CHILD3_FACTOR);
	beattracker->updControl("BeatReferee/br/mrs_natural/eqPeriod", EQ_PERIOD);
	beattracker->updControl("BeatReferee/br/mrs_natural/eqPhase", EQ_PHASE);
	beattracker->updControl("BeatReferee/br/mrs_natural/metricalChangeTime", metricalChangeTime);
	beattracker->updControl("BeatReferee/br/mrs_bool/backtrace", backtraceopt);
	beattracker->updControl("BeatReferee/br/mrs_bool/logFile", logfileopt);
	beattracker->updControl("BeatReferee/br/mrs_natural/soundFileSize", (mrs_natural) ((inputSize / HOPSIZE)));
	beattracker->updControl("BeatReferee/br/mrs_bool/nonCausal", noncausalopt);

	ostringstream path;
	FileName outputFile(sfName);
	//if no outputTxt dir defined -> exec dir is assumed:
	if(strcmp(outputTxt.c_str(), "") == 0)
		path << execPath << outputFile.nameNoExt();
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

	if(logfileopt)
		beattracker->updControl("BeatReferee/br/mrs_string/logFileName", path.str() + "_log.txt");

	//adjustment used in TickCount calculation above
	beattracker->updControl("BeatTimesSink/sink/mrs_natural/adjustment", adjustment);
	output = "medianTempo";
	
	if(strcmp(output.c_str(), "none") != 0)
	{
		beattracker->updControl("BeatTimesSink/sink/mrs_string/destFileName", path.str());
		
		if(strcmp(output.c_str(), "beats") == 0)
		{
			beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "beatTimes");
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
		}
		else if(strcmp(output.c_str(), "medianTempo") == 0)
		{
			beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "medianTempo");
			// cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
		}
		else if(strcmp(output.c_str(), "meanTempo") == 0)
		{
			beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "meanTempo");
			cout << "MeanTempo Output: " << path.str() << "_meanTempo.txt" << endl;
		}
		else if(strcmp(output.c_str(), "beats+medianTempo") == 0)
		{
			beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "beats+medianTempo");
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
			cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
		}
		else if(strcmp(output.c_str(), "beats+meanTempo") == 0)
		{
			beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "beats+meanTempo");
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
			cout << "MeanTempo Output: " << path.str() << "_meanTempo.txt" << endl;
		}
		else if(strcmp(output.c_str(), "beats+meanTempo+medianTempo") == 0)
		{
			beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "beats+meanTempo+medianTempo");
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
			cout << "MeanTempo Output: " << path.str() << "_meanTempo.txt" << endl;
		}
		else
		{
			cout << "Incorrect annotation output defined - beats+tempo files will be created:" << endl;
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
			cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
			beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "beats+tempo");
		}
	}
	
	
	//SonicVisualiser Controls:
	/*
	if(sonicOutFlux)
	{
		beattracker->updControl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_string/mode", "frames");
		beattracker->updControl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_string/destFileName", path.str() + "_onsetFunction.txt");
		
		//if(backtraceopt)
		//	beattracker->updControl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_natural/offset", inductionTickCount);
		//else
		//	beattracker->updControl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_natural/offset", 0);
	}
	if(sonicOutFluxFilter)
	{
		beattracker->updControl("Series/normfiltering/SonicVisualiserSink/sonicsinkfilt/mrs_string/mode", "frames");
		beattracker->updControl("Series/normfiltering/SonicVisualiserSink/sonicsinkfilt/mrs_string/destFileName", path.str() + "_onsetFunctionFilt.txt");
	}
	*/

	//set audio/onset resynth balance and ADSR params for onset sound
	if(audiofileopt || audioopt)
	{
		if(micinputopt && ((!audiofileopt && audioopt) || (audiofileopt && audioopt)))
		{
			IBTsystem->updControl("Series/beatmix/Series/audioflow/Gain/gainaudio/mrs_real/gain", 0.6);
			IBTsystem->updControl("Series/beatmix/Series/beatsynth/Gain/gainbeats/mrs_real/gain", 1.2);
			IBTsystem->updControl("Series/beatmix/Series/beatsynth/ADSR/env/mrs_real/aTarget", 1.0);
 			IBTsystem->updControl("Series/beatmix/Series/beatsynth/ADSR/env/mrs_real/aTime", WINSIZE/80/fsSrc);
 			IBTsystem->updControl("Series/beatmix/Series/beatsynth/ADSR/env/mrs_real/susLevel", 0.0);
 			IBTsystem->updControl("Series/beatmix/Series/beatsynth/ADSR/env/mrs_real/dTime", WINSIZE/4/fsSrc);
		}
		else
		{
			IBTsystem->updControl("Fanout/beatmix/Series/audioflow/Gain/gainaudio/mrs_real/gain", 0.6);
			IBTsystem->updControl("Fanout/beatmix/Series/beatsynth/Gain/gainbeats/mrs_real/gain", 1.2);
			IBTsystem->updControl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/aTarget", 1.0);
 			IBTsystem->updControl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/aTime", WINSIZE/80/fsSrc);
 			IBTsystem->updControl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/susLevel", 0.0);
 			IBTsystem->updControl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/dTime", WINSIZE/4/fsSrc);
		}
	}

	//for saving file with audio+clicks (on beats):
	if(audiofileopt)
	{
		IBTsystem->updControl("SoundFileSink/fdest/mrs_string/filename", path.str() + "_beats.wav");
		cout << "Audiofile w/ beats being saved: " << path.str() << "_beats.wav" << endl;
	}

	/*
	//configure zero-phase Butterworth filter of Flux time series (from J.P.Bello TASLP paper)
	// Coefficients taken from MATLAB butter(2, 0.28)
	realvec bcoeffs(1,3);
	bcoeffs(0) = 0.1174;
	bcoeffs(1) = 0.2347;
	bcoeffs(2) = 0.1174;
	beattracker->updControl("Series/normfiltering/Filter/filt1/mrs_realvec/ncoeffs", bcoeffs);
	realvec acoeffs(1,3);
	acoeffs(0) = 1.0;
	acoeffs(1) = -0.8252;
	acoeffs(2) = 0.2946;
	beattracker->updControl("Series/normfiltering/Filter/filt1/mrs_realvec/dcoeffs", acoeffs);
	*/

	
	// Coefficients taken from MATLAB butter(2, 0.18)
	realvec bcoeffs(1,3);
	bcoeffs(0) = 0.0564;
	bcoeffs(1) = 0.1129;
	bcoeffs(2) = 0.0564;
	beattracker->updControl("Series/normfiltering/Filter/filt1/mrs_realvec/ncoeffs", bcoeffs);
	realvec acoeffs(1,3);
	acoeffs(0) = 1.0000;
	acoeffs(1) = -1.2247;
	acoeffs(2) = 0.4504;
	beattracker->updControl("Series/normfiltering/Filter/filt1/mrs_realvec/dcoeffs", acoeffs);
	
    /*                   
	// Coefficients taken from MATLAB butter(2, 0.1)
	realvec bcoeffs(1,3);
	bcoeffs(0) = 0.0201;
	bcoeffs(1) = 0.0402;
	bcoeffs(2) = 0.0201;
	beattracker->updControl("Series/normfiltering/Filter/filt1/mrs_realvec/ncoeffs", bcoeffs);
	realvec acoeffs(1,3);
	acoeffs(0) = 1.0000;
	acoeffs(1) = -1.5610;
	acoeffs(2) = 0.6414;
	beattracker->updControl("Series/normfiltering/Filter/filt1/mrs_realvec/dcoeffs", acoeffs);
	*/

	//ostringstream onsetFunction [-> for inputing sonicvisualiser spectral flux];
	//onsetFunction << "C:\\Users\\Joao Lobato\\Desktop\\onsetFunctions\\" << outputFile.nameNoExt() << "_vamp_vamp-aubio_aubioonset_detectionfunction.csv";

	//MATLAB Engine inits
	//used for autocorrelation.m
	//mrs_natural winSize = WINSIZE;
	//mrs_natural hopSize = HOPSIZE;
	//MATLAB_EVAL("clear;");
	//MATLAB_EVAL("FluxTS = [];");
	//MATLAB_EVAL("srcAudio = [];");
	//MATLAB_EVAL("FinalBeats=[];");
	//MATLAB_PUT(winSize, "winSize");
	//MATLAB_PUT(hopSize, "hopSize");
	/*
	MATLAB_PUT(induction_time, "timmbing");
	MATLAB_PUT(fsSrc, "SrcFs");
	MATLAB_PUT(inductionTickCount, "inductionTickCount");
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

	inputSize = (mrs_natural) (inputSize / HOPSIZE); //inputSize in ticks

	//suming inductionTickCount because with backtrace it will analyse two times the inductionWindow
	if(backtraceopt)
		inputSize += inductionTickCount;

	//while(IBTsystem->getctrl("mrs_bool/hasData")->to<mrs_bool>())
	while(frameCount <= inputSize)
	{	
		//cout << "FrameCount: " << frameCount << endl;
		if(audiofileopt || audioopt)
			IBTsystem->tick();
		else
			audioflow->tick();

		if(frameCount == 1)
		{
			if(micinputopt)
				cout << "Capturing Audio......" << endl;
			if(audioopt && !backtraceopt)
			{
				IBTsystem->updControl("AudioSink/output/mrs_bool/initAudio", true);
				if(!micinputopt)
					cout << "Playing Audio........" << endl;
			}
		}

		//Just after induction:
		if(frameCount == inductionTickCount)
		{	
			//for playing audio (with clicks on beats):
			// cout << "done" << endl;

			//if backtrace and not in mic mode
			if(backtraceopt && !micinputopt)
			{
				//Restart reading audio file
				audioflow->updControl("SoundFileSource/src/mrs_natural/pos", 0);
				
				if(strcmp(givefirst2beats_startpoint.c_str(), "-1") == 0 && !noncausalopt)
					cout << "Backtraced!" << endl;
				//else
				//	cout << "Forcing tracking startpoint at: " << phase_ << " (s)" << endl;
				
				if(audioopt)
					IBTsystem->updControl("AudioSink/output/mrs_bool/initAudio", true);
			}
			if(audioopt)
			{
				if(micinputopt) //in microphone mode only plays clicks
					cout << "Playing beat taps........" << endl;
				else
					cout << "Playing audio with beat taps........" << endl;
			}
			
			// if(!noncausalopt)
				// cout << "Real-Time Beat Tracking........" << endl;
			// else
			// cout << "Off-Line Beat Tracking........" << endl;
		}
		//Display percentage of processing complete...
		//printf("  %d % \r", (mrs_natural) frameCount*100/inputSize);
		//cout << (mrs_natural) frameCount*100/inputSize << "%" << endl;
		
		frameCount++;
	}
	



	mrs_real predicted_tempo = beattracker->getctrl("BeatTimesSink/sink/mrs_real/tempo")->to<mrs_real>();	
	

	mrs_realvec tempos(1);
	
	tempos(0) = predicted_tempo;
	
	evaluate_estimated_tempo(sfName, tempos, ground_truth_tempo);
	  
	delete IBTsystem;
	delete audioflow;
  
}









// Play a collection l of soundfiles
void tempo(string inFname, string outFname, string label, string method, bool haveCollections)
{
	MRSDIAG("tempo.cpp - tempo");

	// For each file in collection estimate tempo
	string sfName = inFname;
	string resName = outFname;


	/* resName = sfName.substr(0,sfName.rfind(".", sfName.length()));
	   resName += "Marsyas";
	   resName += ".txt";
	*/

	istringstream iss(label);
	float ground_truth_tempo; 
	iss >> ground_truth_tempo;
	


	if (method == "MEDIAN_SUMBANDS")
    {
		tempo_medianSumBands(sfName, ground_truth_tempo, resName, haveCollections);
    }
	else if (method == "MEDIAN_MULTIBANDS")
    {
		tempo_medianMultiBands(sfName,ground_truth_tempo, resName, haveCollections);
    }
	else if (method == "FLUX")
	{
		tempo_flux(sfName, ground_truth_tempo, resName, haveCollections);
	} 
	else if (method == "AIM_FLUX") 
	{
	  tempo_aim_flux(sfName, ground_truth_tempo, resName, haveCollections);
	  
	}
	
	else if (method == "AIM")
	  {
	    tempo_aim(sfName, ground_truth_tempo, resName, haveCollections);
	  }
	else if (method == "HISTO_SUMBANDS")
    {
		tempo_histoSumBands(sfName, ground_truth_tempo, resName, haveCollections);
    }
	else if (method == "HISTO_SUMBANDSQ")
	{
		tempo_histoSumBandsQ(sfName,ground_truth_tempo, resName, haveCollections);
	}
	
	else if (method == "IBT")
    {
		// cout << "TEMPO INDUCTION USING THE INESC BEAT TRACKER" << endl;
		induction_time = 5.0;
		metrical_change_time = 5.0;
		score_function = "regular"; 
		output = "beats+tempo"; 
		tempo_ibt(sfName, ground_truth_tempo, resName, haveCollections);
    }
	else if (method == "WAVELETS")
    {
		tempo_wavelets(sfName, resName, haveCollections);
    }
	else if (method == "BOOMCHICK_WAVELET")
    {
		cout << "BOOM-CHICK Wavelet RHYTHM EXTRACTION method " << endl;
		tempo_bcWavelet(sfName, resName, haveCollections);
    }
	else if (method == "BOOMCHICK_FILTER")
    {
		cout << "BOOM-CHICK Filter RHYTHM EXTRACTION method " << endl;
		tempo_bcFilter(sfName, resName, haveCollections);
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
	cmd_options.addRealOption("band", "bd", 0.0);
	cmd_options.addBoolOption("beats", "b", false);
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
	beatsopt_ = cmd_options.getBoolOption("beats");
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
		method = "FLUX";
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
		correct_harmonic_mirex_predictions = 0;
		correct_mirex_predictions = 0;
		
		total_instances = 0;
		total_differences = 0.0;
		total_errors = 0;


		errors_.create(4);
		
		for (size_t i=0; i < l.size(); ++i)
		{
			tempo(l.entry(i), "default.txt", l.labelEntry(i), method, haveCollections);
		}


		ofstream evil_collection;
		evil_collection.open("evil.mf");
		for (int i=0; i< (mrs_natural)wrong_filenames_.size(); i++)
		  evil_collection << wrong_filenames_[i] << "\t" << wrong_filenames_tempos_[i] << "-"  << ground_truth_tempos_[i] << endl;
		evil_collection.close();
		



	}
	else
	{
		for (vector<string>::iterator sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
		{
			string sfname = *sfi;
			tempo(*sfi, "default.txt", "0.0" , method, haveCollections);
		}
	}

	// This will crash unless the user knows to specify an outfile.
	//tempo(soundfiles[0], soundfiles[1], method);

	exit(0);
}
