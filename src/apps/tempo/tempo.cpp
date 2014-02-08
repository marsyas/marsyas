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


#include <marsyas/common_source.h>
#include <cstdio>
#include <cstdlib>
#include <algorithm>


#include <marsyas/FileName.h>
#include <marsyas/Collection.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/marsystems/Esitar.h>
#include <marsyas/mididevices.h>
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


#define BPM_HYPOTHESES 6 //Nr. of initial BPM hypotheses (must be <= than the nr. of agents) (6)
#define PHASE_HYPOTHESES 30//Nr. of phases per BPM hypothesis (30)
#define MIN_BPM 40 //minimum tempo considered, in BPMs (50)
#define MAX_BPM 180 //maximum tempo considered, in BPMs (250)
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
#define TRIGGER_BEST_FACTOR 1.0 //Proportion of the current best agent score which is inherited by the agents created at each triggered induction [shouldn't be much higher than 1, for not inflating scores two much] (1.0)
#define SUPERVISED_TRIGGER_THRES 0.03 //Degree (in percentage) of mean bestScore decrease to trigger a new induction in supervised induction mode (0.03)
#define BEAT_TRANSITION_TOL 0.6 //Tolerance for handling beats at transitions between agents [-1 for unconsider it]: (0.6)
//In causal mode, if between two consecutive beats there is over a BEAT_TRANSITION_TOL decrease in current IBI the second beat is unconsidered;
//In non-causal mode, if between a son's first beat and its father's last there is over a BEAT_TRANSITION_TOL descrease on the father last IBI the son's first beat is unconsidered;
//In non-causal mode, if between a son's first beat and its father's last there is over a BEAT_TRANSITION_TOL increase on the father last IBI the son's first beat shall be its father's next beat, and the second beat shall be its assigned first.


// 0: no doubling at all
// 1: single threshold (bpm > x => double)
// 2: SVM-based doubling
#define POST_DOUBLING 2

#define WRITE_INTERMEDIATE 0
#define DISPLAY_SVM 1


#define WINSIZE 1024 //(2048?)
#define HOPSIZE 512 //(512)



using namespace std;
using namespace Marsyas;
MarSystemManager mng;

vector<mrs_string> predicted_filenames_;
vector<float> predicted_filenames_tempos_;
vector<mrs_string> wrong_filenames_;
vector<float> wrong_filenames_tempos_;
vector<float> wrong_ground_truth_tempos_;
vector<float> ground_truth_tempos_;


mrs_string output;
mrs_bool audiofileopt;
mrs_real toleranceopt_;
mrs_real induction_time;
mrs_real metrical_change_time;
mrs_string groundtruth_induction = "-1";
mrs_string induction_mode = "-1";
mrs_string groundtruth_file = "-1";
mrs_natural triggergt_tol = 5;



mrs_string score_function;
mrs_bool audioopt;
mrs_bool backtraceopt;
mrs_real phase_;
mrs_realvec errors_;
mrs_string fileName;
mrs_string pluginName = EMPTYSTRING;
mrs_string methodopt;
mrs_string predictedopt_;
mrs_string predictedOutopt_;
mrs_string wrong_filename_opt_;


mrs_natural minBPM_;
mrs_natural maxBPM_;
mrs_real sup_thres = SUPERVISED_TRIGGER_THRES;
mrs_bool avoid_metrical_changes = false;



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

mrs_real
next_power_two(mrs_real x)
{
  return pow(2, ceil( log(x) / log( (mrs_real) 2.0)));
}


int
printUsage(mrs_string progName)
{
  MRSDIAG("tempo.cpp - printUsage");
  cerr << "Usage : " << progName << " [-m method] [-g gain] [-o offset(samples)] [-d duration(samples)] [-s start(seconds)] [-l length(seconds)] [-f outputfile] [-p pluginName] [-r repetitions] file1 file2 file3" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format or collections " << endl;
  return 1;
}

int
printHelp(mrs_string progName)
{
  MRSDIAG("tempo.cpp - printHelp");
  cerr << "tempo, MARSYAS, Copyright George Tzanetakis " << endl;
  cerr << "--------------------------------------------" << endl;
  cerr << "Estimates the tempo for the sound files provided as arguments " << endl;
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
  cerr << "STEM" << endl;
  cerr << "TEST_OSS_FLUX" << endl;
  cerr << "AIM_FLUX" << endl;
  cerr << "AIM" << endl;
  cerr << "PREDICTED" << endl;
  cerr << "BOOMCHICK_WAVELET" << endl;
  cerr << "BOOMCHICK_FILTER" << endl;

  return 1;
}

void
evaluate_estimated_tempo(mrs_string sfName, mrs_realvec tempos, float ground_truth_tempo, mrs_real tolerance)
{


  mrs_real predicted_tempo = tempos(0);

  float diff1 = fabs(predicted_tempo - ground_truth_tempo);
  float diff2 = fabs(2 * predicted_tempo - ground_truth_tempo);
  float diff3 = fabs(0.5 * predicted_tempo - ground_truth_tempo);
  float diff4 = fabs(3 * predicted_tempo - ground_truth_tempo);
  float diff5 = fabs(0.33 * predicted_tempo - ground_truth_tempo);


  // float diff1 = fabs(predicted_tempo - ground_truth_tempo);
  // float diff2 = fabs(predicted_tempo - 2 * ground_truth_tempo);
  // float diff3 = fabs(predicted_tempo - 0.5 * ground_truth_tempo);
  // float diff4 = fabs(predicted_tempo - 3 * ground_truth_tempo);
  // float diff5 = fabs(predicted_tempo - 0.33 * ground_truth_tempo);



  cout << sfName << "\t" << predicted_tempo << ":" << ground_truth_tempo <<  "---" << diff1 << ":" << diff2 << ":" << diff3 << ":" << diff4 << ":" << diff5 << endl;
  cout << sfName << "\t" << predicted_tempo << endl;

  //mrs_real accPerc = 4.0;


  if (diff1 < 0.5)
    correct_predictions++;

  // if ((predicted_tempo >= ground_truth_tempo * (1 - accPerc / 100.0))
  // &&(predicted_tempo <= ground_truth_tempo * (1 + accPerc/ 100.0)))
  if (diff1 <= tolerance * ground_truth_tempo)
    correct_mirex_predictions++;


  if (diff2 <= tolerance * ground_truth_tempo)
    errors_(0) = errors_(0) + 1;
  if (diff3 <= tolerance * ground_truth_tempo)
  {
    errors_(1) = errors_(1) + 1;
  }

  if (diff4 <= tolerance * ground_truth_tempo)
    errors_(2) = errors_(2) + 1;
  if (diff5 <= tolerance * ground_truth_tempo)
    errors_(3) = errors_(3) + 1;



  if ((diff1 <= tolerance * ground_truth_tempo)||(diff2 <= tolerance * ground_truth_tempo)||(diff3 <= tolerance * ground_truth_tempo)||(diff4 <= tolerance * ground_truth_tempo)||(diff5 <= tolerance * ground_truth_tempo))
  {
    correct_harmonic_mirex_predictions++;
  }
  else
  {
    wrong_filenames_.push_back(sfName);
    wrong_filenames_tempos_.push_back(predicted_tempo);
    wrong_ground_truth_tempos_.push_back(ground_truth_tempo);
  }


  predicted_filenames_.push_back(sfName);
  predicted_filenames_tempos_.push_back(predicted_tempo);
  ground_truth_tempos_.push_back(ground_truth_tempo);




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
  }








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
void tempo_medianMultiBands(mrs_string sfName, float ground_truth_tempo, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) resName;
  (void) haveCollections;



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
  total->addMarSystem(mng.create("Windowing", "autowin"));
  total->updControl("Windowing/autowin/mrs_string/type", "Hanning");
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
  evaluate_estimated_tempo(sfName, realvec(1,1,predicted_tempo),
                           ground_truth_tempo, tolerance);

  delete total;
}





void
tempo_wavelets(mrs_string sfName, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) resName;
  (void) haveCollections;
  (void) tolerance;

  //MarSystemManager mng;

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
tempo_aim(mrs_string sfName, float ground_truth_tempo, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) resName;

  cout << "Tempo-aim" << endl;


  //MarSystemManager mng;

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
    evaluate_estimated_tempo(sfName, tempos, ground_truth_tempo, tolerance);
  }

  ofstream ofs;
  ofs.open(fileName.c_str());
  cout << bpm_estimate << endl;
  ofs << bpm_estimate << endl;

  ofs.close();

  delete net;

}




MarSystem *onset_strength_signal_flux(mrs_string sfName)
{
  //MarSystemManager mng;

  /* Onset strength calcuates the onset strength signal whose
     individual values are computed using the fluxnet. The
     resulting onset strength signal is filtered to smooth out
     adjacent peaks.  This moves the locations of the true peaks
     by a fixed number of samples
     */
  MarSystem *onset_strength = mng.create("Series/onset_strength");
  MarSystem *accum = mng.create("Accumulator/accum");
  MarSystem *fluxnet = mng.create("Series/fluxnet");

  fluxnet->addMarSystem(mng.create("SoundFileSource/src"));
  fluxnet->addMarSystem(mng.create("MixToMono/m2m"));

  // fluxnet->addMarSystem(mng.create("DownSampler/tds"));
  fluxnet->addMarSystem(mng.create("ShiftInput/si"));	       // overlap for the spectral flux
  fluxnet->addMarSystem(mng.create("Windowing/windowing1"));
  fluxnet->addMarSystem(mng.create("Spectrum/spk"));
  fluxnet->addMarSystem(mng.create("PowerSpectrum/pspk"));


  //fluxnet->addMarSystem(mng.create("TriangularFilterBank/tfb"));
  // fluxnet->addMarSystem(mng.create("Sum/triangsum"));

  fluxnet->addMarSystem(mng.create("Flux/flux"));
  //fluxnet->addMarSystem(mng.create("Gain/gain5"));
  //fluxnet->updControl("Flux/flux/mrs_string/mode", "multichannel");
  //fluxnet->addMarSystem(mng.create("Delta/delta"));
  //fluxnet->updControl("Delta/delta/mrs_bool/positive", true);

  fluxnet->addMarSystem(mng.create("Filter", "filt1"));

  accum->addMarSystem(fluxnet);
  onset_strength->addMarSystem(accum);

  // parameters for the onset strength signal
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/PowerSpectrum/pspk/mrs_string/spectrumType", "logmagnitude");
  // onset_strength->updControl("Accumulator/accum/Series/fluxnet/Windowing/windowing1/mrs_string/type", "Blackman-Harris");
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/Flux/flux/mrs_string/mode", "Laroche2003");


  // The filter object in Marsyas is implemented as a direct form II
  // structure. This is a canonical form which has the minimum number
  // of delay elements.

  // 15th order
  //   import scipy.signal
  //   b,a = scipy.signal.firwin(16, 3.0 / (344.53125/2.0))
  /*
     mrs_realvec bcoeffs(1, 16);
     bcoeffs(0) = 0.0095530348472755;
     bcoeffs(1) = 0.0144032200641954;
     bcoeffs(2) = 0.0280928524023415;
     bcoeffs(3) = 0.0483763511689087;
     bcoeffs(4) = 0.0717941373871591;
     bcoeffs(5) = 0.0942851675260588;
     bcoeffs(6) = 0.1119142142273117;
     bcoeffs(7) = 0.1215810223767492;
     bcoeffs(8) = 0.1215810223767492;
     bcoeffs(9) = 0.1119142142273117;
     bcoeffs(10) = 0.0942851675260588;
     bcoeffs(11) = 0.0717941373871592;
     bcoeffs(12) = 0.0483763511689087;
     bcoeffs(13) = 0.0280928524023415;
     bcoeffs(14) = 0.0144032200641954;
     bcoeffs(15) = 0.0095530348472755;
     */
  /*
      //   b,a = scipy.signal.firwin(16, 10.0 / (344.53125/2.0))
      mrs_realvec bcoeffs(1, 16);
      bcoeffs(0) = 0.0073773298534980;
      bcoeffs(1) = 0.0120567511070207;
      bcoeffs(2) = 0.0251341506152936;
      bcoeffs(3) = 0.0456735164211478;
      bcoeffs(4) = 0.0706923687612440;
      bcoeffs(5) = 0.0957577801640946;
      bcoeffs(6) = 0.1160031971306242;
      bcoeffs(7) = 0.1273049059470771;
      bcoeffs(8) = 0.1273049059470771;
      bcoeffs(9) = 0.1160031971306242;
      bcoeffs(10) = 0.0957577801640946;
      bcoeffs(11) = 0.0706923687612441;
      bcoeffs(12) = 0.0456735164211478;
      bcoeffs(13) = 0.0251341506152936;
      bcoeffs(14) = 0.0120567511070207;
      bcoeffs(15) = 0.0073773298534980;
  */
  //   b,a = scipy.signal.firwin(15, 6.0 / (344.53125/2.0))
  mrs_realvec bcoeffs(1, 15);
  bcoeffs(0) = 0.0096350145101721;
  bcoeffs(1) = 0.0155332463596257;
  bcoeffs(2) = 0.0320375094304201;
  bcoeffs(3) = 0.0563964822180710;
  bcoeffs(4) = 0.0839538807274150;
  bcoeffs(5) = 0.1091494735026997;
  bcoeffs(6) = 0.1267578095297140;
  bcoeffs(7) = 0.1330731674437646;
  bcoeffs(8) = 0.1267578095297140;
  bcoeffs(9) = 0.1091494735026998;
  bcoeffs(10) = 0.0839538807274150;
  bcoeffs(11) = 0.0563964822180711;
  bcoeffs(12) = 0.0320375094304201;
  bcoeffs(13) = 0.0155332463596257;
  bcoeffs(14) = 0.0096350145101721;


  fluxnet->updControl("Filter/filt1/mrs_realvec/ncoeffs", bcoeffs);
  //fluxnet->updControl("Filter/filt1/mrs_realvec/dcoeffs", acoeffs);


  //onset_strength->linkControl(
  // "mrs_string/filename",
  // "Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_string/filename");
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_string/filename", sfName);
  onset_strength->linkControl(
    "Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_real/osrate",
    "mrs_real/file_srate");

  onset_strength->linkControl(
    "Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_bool/hasData",
    "mrs_bool/hasData");


  //   updated values, for variable sample rates.  ms = milliseconds
  //   these will be rounded up to the nearest power of 2 (in samples)
  mrs_real oss_hop_ms = 2.9;     // for flux calculation
  mrs_real oss_win_ms = 5.8;     // for flux calculation

  mrs_real srate = onset_strength->getControl("mrs_real/file_srate")->to<mrs_real>();
  mrs_natural oss_hop_size = (mrs_natural) next_power_two(srate * oss_hop_ms * 0.001);
  mrs_natural oss_win_size = (mrs_natural) next_power_two(srate * oss_win_ms * 0.001);
#if 0
  cout<<"OSS sizes:\t"<<oss_hop_size<<"\t"<<oss_win_size<<endl;
#endif
  onset_strength->updControl("mrs_natural/inSamples", oss_hop_size);
  fluxnet->updControl("ShiftInput/si/mrs_natural/winSize", oss_win_size);


  return onset_strength;
}

void
test_oss_flux(mrs_string sfName, float ground_truth_tempo, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) ground_truth_tempo;
  (void) resName;
  (void) haveCollections;
  (void) tolerance;
  cout << "Writing OSS to onset_strength.txt" << endl;

  MarSystem *onset_strength = onset_strength_signal_flux(sfName);
  onset_strength->updControl("Accumulator/accum/mrs_natural/nTimes", 1);

  onset_strength->addMarSystem(mng.create("PlotSink", "plotsink"));
  onset_strength->updControl("PlotSink/plotsink/mrs_string/filename",
                             "onset_strength.txt");
  onset_strength->updControl("PlotSink/plotsink/mrs_bool/sequence", false);
  onset_strength->updControl("PlotSink/plotsink/mrs_bool/single_file", true);
  onset_strength->updControl("PlotSink/plotsink/mrs_bool/no_ticks", true);


  MarControlPtr hasData = onset_strength->getctrl("mrs_bool/hasData");
  while (hasData->isTrue()) {
    onset_strength->tick();
  }
  delete onset_strength;
}

mrs_real energy_in_histo_range(realvec histo,
                               mrs_real factor, mrs_real low, mrs_real high)
{
  // manual rounding because MSVC doesn't support round()
  mrs_natural index_low  = low * factor + 0.5;
  mrs_natural index_high = high * factor + 0.5;
  if (high == 1.0) {
    index_high = histo.getCols()-1;
  }
  if (index_high > histo.getCols()-1) {
    index_high = histo.getCols()-1;
  }
  if (index_low < 0) {
    index_low = 0.0;
  }
  //cout<<low<<"\t"<<high<<"\t"<<factor<<"\t";
  //cout<<index_low<<"\t"<<index_high<<endl;
  mrs_real sum = 0.0;
  for (mrs_natural i=index_low; i < index_high + 1; i++) {
    sum += histo(i);
  }
  return sum;
}

const int INFO_SIZE = 10;
realvec info_histogram(mrs_natural bpm, realvec histo,
                       mrs_real factor, mrs_real tolerance)
{
  realvec info(INFO_SIZE);
  info.setval(0.0);
  mrs_natural size = histo.getCols();

  // global maximum (will be a peak)
  //mrs_real bpm1 = 0;
  mrs_real bpm2 = 0;
  mrs_real bpm3 = 0;
  mrs_real str1 = 0;
  mrs_real str2 = 0;
  mrs_real str3 = 0;
  for (int i=1; i < size-1; i++)
  {
    if (histo(i) > str1)
    {
      str1 = histo(i);
      //bpm1 = i / factor;
    }
  }

  // second-highest maximum (need to ensure peak-ness)
  for (int i=1; i < size-1; i++)
  {
    if ((histo(i) > str2) && (histo(i) < str1) &&
        (histo(i-1) < histo(i)) && (histo(i+1) < histo(i)))
    {
      str2 = histo(i);
      bpm2 = i / factor;
    }
  }

  // third-highest maximum (need to ensure peak-ness)
  for (int i=1; i < size-1; i++)
  {
    if ((histo(i) > str3) && (histo(i) < str1) && (histo(i) < str2) &&
        (histo(i-1) < histo(i)) && (histo(i+1) < histo(i)))
    {
      str3 = histo(i);
      bpm3 = i / factor;
    }
  }

  // number of values
  mrs_natural num_non_zero = 0;
  for (int i=1; i < size-1; i++)
  {
    if (histo(i) > 0)
    {
      num_non_zero += 1;
    }
  }


  // energy over / under
  mrs_real energy_total  = energy_in_histo_range(histo, factor,
                           0, 1.0 );
  mrs_real energy_under = energy_in_histo_range(histo, factor,
                          0.0, bpm*(1.0 - tolerance) ) / energy_total;
  mrs_real energy_over  = energy_in_histo_range(histo, factor,
                          bpm*(1.0 + tolerance), 1.0 ) / energy_total;

  mrs_real str05 = energy_in_histo_range(histo, factor,
                                         0.5*bpm*(1.0 - tolerance), 0.5*bpm*(1.0 + tolerance) ) / energy_total;
  mrs_real str10 = energy_in_histo_range(histo, factor,
                                         1.0*bpm*(1.0 - tolerance), 1.0*bpm*(1.0 + tolerance) ) / energy_total;
  mrs_real str20 = energy_in_histo_range(histo, factor,
                                         2.0*bpm*(1.0 - tolerance), 2.0*bpm*(1.0 + tolerance) ) / energy_total;

  info(0) = energy_under;
  info(1) = energy_over;
  info(2) = 1.0 - (energy_under + energy_over);
  info(3) = str05;
  info(4) = str10;
  info(5) = str20;
  info(6) = 1.0 - (str05 + str10 + str20);
  info(7) = bpm2 / bpm;
  info(8) = bpm3 / bpm;
  info(9) = num_non_zero;
  return info;
}



/* tempo induction based on the following steps:
   1) onset strength signal calcalation based on spectral flux
      (see onset_strength_signal_flux function)
   2) unbiased autorrelation
   3) warping to Beat Histrogram
   4) peak picking for selecting tempo candidates
   5) tempo candidate rescoring by cross-correlation with the onset
   strength signal
   6) secondary Beat Histogram for final tempo induction
*/


void
tempo_stem(mrs_string sfName, float ground_truth_tempo, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) resName;
  cout << "SIMPLE TEMPO ESTIMATION METHOD (STEM)" << endl;

  //MarSystemManager mng;

  MarSystem *onset_strength = onset_strength_signal_flux(sfName);
  MarSystem *beatTracker = mng.create("Series/beatTracker");

#if WRITE_INTERMEDIATE
  onset_strength->addMarSystem(mng.create("PlotSink", "plotsink"));
  onset_strength->updControl("PlotSink/plotsink/mrs_string/filename",
                             "out/onset_signal_strength.txt");
  onset_strength->updControl("PlotSink/plotsink/mrs_bool/sequence", false);
  onset_strength->updControl("PlotSink/plotsink/mrs_bool/single_file", true);
  onset_strength->updControl("PlotSink/plotsink/mrs_bool/no_ticks", true);
#endif


  onset_strength->addMarSystem(mng.create("ShiftInput/si2"));   // overlap for the onset strength signal


  beatTracker->addMarSystem(onset_strength);

  /* After the onset signal is calculated it is used for initial
     tempo induction based mapping an enhanced autocorrelation to a
     beat histogram BH and selecting the peaks of the BH as tempo candidates.
     The FlowThru composite is used to propagate the onset strength signal
     to the BeatPhase MarSystem which finds the beat locations and rescores
     the tempo candidates in order to select the tempo. The tempo induction
     is performed as the inner process of the FlowThru composite and the tempo
     candidates are linked to the BeatPhase tempo candidates.
     */

  MarSystem *tempoInduction = mng.create("FlowThru/tempoInduction");


  /* MarSystem *adaptive_median_threshold = mng.create("Fanout/adaptive_median_threshold");
     adaptive_median_threshold->addMarSystem(mng.create("MedianFilter", "medianfilter"));
     adaptive_median_threshold->addMarSystem(mng.create("Gain/subtract_gain"));
     adaptive_median_threshold->updControl("Gain/subtract_gain/mrs_real/gain", -1.0);
     adaptive_median_threshold->updControl("MedianFilter/medianfilter/mrs_natural/WindowSize", 10);
     tempoInduction->addMarSystem(adaptive_median_threshold);
     tempoInduction->addMarSystem(mng.create("Sum/asum"));
     tempoInduction->addMarSystem(mng.create("HalfWaveRectifier", "hwr"));
     */


  tempoInduction->addMarSystem(mng.create("AutoCorrelation", "acr"));
  tempoInduction->addMarSystem(mng.create("BeatHistogram", "histo"));


  //  enhance the BH harmonic peaks
  MarSystem* hfanout = mng.create("Fanout", "hfanout");
  hfanout->addMarSystem(mng.create("Gain", "id1"));
  hfanout->addMarSystem(mng.create("TimeStretch", "tsc1"));
  hfanout->addMarSystem(mng.create("TimeStretch", "tsc2"));
  tempoInduction->addMarSystem(hfanout);
  tempoInduction->addMarSystem(mng.create("Sum", "hsum"));

#if 0
  tempoInduction->addMarSystem(mng.create("PlotSink", "plotsink_hbh"));
  tempoInduction->updControl("PlotSink/plotsink_hbh/mrs_string/filename",
                             "hbh-combo.txt");
  tempoInduction->updControl("PlotSink/plotsink_hbh/mrs_bool/sequence", false);
  tempoInduction->updControl("PlotSink/plotsink_hbh/mrs_bool/single_file", true);
#endif


  // Select the peaks
  tempoInduction->addMarSystem(mng.create("Peaker", "pkr1"));
  tempoInduction->addMarSystem(mng.create("MaxArgMax", "mxr1"));

  // Using the tempo induction block calculate the Beat Locations
  beatTracker->addMarSystem(tempoInduction);
  // beatTracker->addMarSystem(mng.create("ShiftInput/si3"));
  beatTracker->addMarSystem(mng.create("BeatPhase/beatphase"));
  beatTracker->addMarSystem(mng.create("Gain/id"));

  //mrs_natural hop_ms = 5.8;     // for flux calculation
  //mrs_natural bhop_ms = 5.8;    // for onset strength signal
  mrs_real hop_ms = 2.9;     // for flux calculation
  mrs_real bhop_ms = 2.9;    // for onset strength signal
  mrs_real bwin_ms = 46.4; // 46.4;	 // for onset strength signal
  // mrs_natural bp_winSize = 8192; // for onset strength signal for the beat locations
  mrs_natural nCandidates = 10;  // number of tempo candidates
  mrs_natural factor = 4;

  // parameters for BH pick peaking
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakNeighbors", 2);
  tempoInduction->updControl("Peaker/pkr1/mrs_real/peakSpacing", 0.0);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakStart", factor*MIN_BPM+1);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakEnd", factor*MAX_BPM-1);
  tempoInduction->updControl("MaxArgMax/mxr1/mrs_natural/interpolation", 0);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/interpolation", 0);
  beatTracker->updControl("FlowThru/tempoInduction/MaxArgMax/mxr1/mrs_natural/nMaximums", nCandidates);

  // autocorrelation parameters
  tempoInduction->updControl("AutoCorrelation/acr/mrs_real/magcompress", 0.5);
  tempoInduction->updControl("AutoCorrelation/acr/mrs_bool/setr0to0", true);
  tempoInduction->updControl("AutoCorrelation/acr/mrs_bool/setr0to1", true);

  // beat histogram parameters
  tempoInduction->updControl("BeatHistogram/histo/mrs_natural/startBin", 0);
  tempoInduction->updControl("BeatHistogram/histo/mrs_natural/endBin",
                             factor*MAX_BPM);
  tempoInduction->updControl("BeatHistogram/histo/mrs_real/factor", (mrs_real)factor);
  tempoInduction->updControl("BeatHistogram/histo/mrs_real/alpha", 0.0);

  tempoInduction->updControl("Fanout/hfanout/TimeStretch/tsc1/mrs_real/factor", 0.5);
  tempoInduction->updControl("Fanout/hfanout/TimeStretch/tsc2/mrs_real/factor", 0.25);
  tempoInduction->updControl("Fanout/hfanout/Gain/id1/mrs_real/gain", 1.0);

  // set the filename, hop and window size
  mrs_real srate = onset_strength->getControl("mrs_real/file_srate")->to<mrs_real>();

  mrs_natural hopSize = (mrs_natural) next_power_two(srate * hop_ms * 0.001);
  mrs_natural bhopSize = (mrs_natural) next_power_two(srate * bhop_ms * 0.001);
  mrs_natural bwinSize = (mrs_natural) next_power_two(srate * bwin_ms * 0.001);
#if 0
  cout<<"sizes:"<<endl;
  cout<<hopSize<<endl;
  cout<<bhopSize<<endl;
  cout<<bwinSize<<"\t"<<srate * bwin_ms * 0.001<<endl;
#endif

  beatTracker->updControl("mrs_natural/inSamples", hopSize);

  onset_strength->updControl("Accumulator/accum/mrs_natural/nTimes", bhopSize);
  onset_strength->updControl("ShiftInput/si2/mrs_natural/winSize", bwinSize);
  // beatTracker->updControl("ShiftInput/si3/mrs_natural/winSize", bp_winSize);

  // onset_strength->updControl("Accumulator/accum/Series/fluxnet/DownSampler/tds/mrs_natural/factor", 2);

  // BeatPhase estimates a tempo based on rescoring the tempo candidates
  // of the tempo induction phase by cross-correlating pulse trains
  // with the onset strength signal
  beatTracker->updControl("BeatPhase/beatphase/mrs_real/factor", (mrs_real)factor);
  beatTracker->updControl("BeatPhase/beatphase/mrs_natural/bhopSize", bhopSize);
  beatTracker->updControl("BeatPhase/beatphase/mrs_natural/bwinSize", bwinSize);
  beatTracker->updControl("BeatPhase/beatphase/mrs_natural/nCandidates", nCandidates);
  beatTracker->updControl("BeatPhase/beatphase/mrs_real/ground_truth_tempo", ground_truth_tempo);
  beatTracker->linkControl("BeatPhase/beatphase/mrs_realvec/tempo_candidates",
                           "FlowThru/tempoInduction/MaxArgMax/mxr1/mrs_realvec/processedData");


  mrs_realvec tempos(nCandidates);  // tempo estimates from the BH
  mrs_realvec temposcores(nCandidates);

  mrs_realvec bphase;	 // secondary beat histogram for selecting the best tempo estimate from BeatPhase
  const int BPHASE_SIZE = MAX_BPM;
  bphase.create(BPHASE_SIZE);
  bphase.setval(0.0);


  // output plugin that can be used with MarMonitors for debugging
  if (pluginName != EMPTYSTRING)
  {
    ofstream ofs;
    ofs.open(pluginName.c_str());
    ofs << *beatTracker << endl;
    ofs.close();
    pluginName = EMPTYSTRING;
  }



  //mrs_real bh_estimate = 0;
  //mrs_real bh_estimate2 = 0;


  mrs_natural size_in_bytes = onset_strength->getctrl("Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_natural/size")->to<mrs_natural>();
  mrs_natural num_ticks = (size_in_bytes / (hopSize * bhopSize)) + 1;


  mrs_natural ticks = 0;

  // beginning: tick until the input BH buffer is full
  mrs_natural begin_tick_num = bwinSize / bhopSize - 1;
  for (; ticks<begin_tick_num; ticks++) {
    beatTracker->tick();
    //mrs_realvec input = onset_strength->getControl("mrs_realvec/processedData")->to<mrs_realvec>();
    //cout<<"begin:\t"<<input(0,0)<<"\t"<<input(0,2047)<<endl;
  }

  // reset anything needed for real processing
  tempoInduction->updControl("BeatHistogram/histo/mrs_bool/reset", true);




#if WRITE_INTERMEDIATE
  ofstream out_bh;
  out_bh.open("out/beat_histogram.txt");
#endif
  // middle: actual data, the input BH buffer is full
  mrs_natural end_tick_num = num_ticks - 1;
  for (; ticks<end_tick_num; ticks++) {
    beatTracker->tick();
    //mrs_realvec input = onset_strength->getControl("mrs_realvec/processedData")->to<mrs_realvec>();
    //cout<<"middle:\t"<<input(0,0)<<"\t"<<input(0,2047)<<endl;

#if WRITE_INTERMEDIATE
    mrs_realvec bh_candidates = beatTracker->getctrl("FlowThru/tempoInduction/MaxArgMax/mxr1/mrs_realvec/processedData")->to<mrs_realvec>();
    for (int k=0; k < nCandidates; k++)
    {
      tempos(k) = bh_candidates(2*k+1) / factor;
      if (k < nCandidates-1) {
        out_bh << setiosflags(ios::fixed) << std::setprecision(2) << tempos(k) << "  ";
      } else {
        out_bh << tempos(k) << endl;
      }
    }
#endif

    // store best score in bphase
    tempos = beatTracker->getControl("BeatPhase/beatphase/mrs_realvec/tempos")->to<mrs_realvec>();
    temposcores = beatTracker->getControl("BeatPhase/beatphase/mrs_realvec/tempo_scores")->to<mrs_realvec>();
    mrs_natural bpm = tempos(0) + 0.5;
    bphase(bpm) += temposcores(0);

#if 0
    printf("%li\t%f\n", bpm, temposcores(0));
//zz
#endif
    /*
            if (ticks == (15+begin_tick_num)) {
                cout<<tempos<<endl;
                cout<<temposcores<<endl;
                exit(1);
            }
    */
  }
#if WRITE_INTERMEDIATE
  out_bh.close();
#endif

  // Find the max bin of the histogram created from the
  // BeatPhase tempo candidates
  mrs_real bhmax = 0.0;
  mrs_natural max_i = 0;

#if WRITE_INTERMEDIATE
  bphase.writeText("out/beat_phase.txt");
#endif


  // global maximum (will be a peak)
  for (int i=1; i < BPHASE_SIZE-1; i++)
  {
    if (bphase(i) > bhmax)
    {
      bhmax = bphase(i);
      max_i = i;
    }
  }

  mrs_real heuristic_tempo = max_i;


  mrs_natural num_features = 1*INFO_SIZE + 2;
  realvec features(num_features);
  realvec features_normalized(num_features);
  features_normalized.setval(0.0);

  realvec from_bp = info_histogram(heuristic_tempo, bphase,
                                   1.0, 0.05);
  for (int i=0; i<INFO_SIZE; i++) {
    features(i) = from_bp(i);
  }

  features(num_features - 2) = heuristic_tempo;
  features(num_features - 1) = ground_truth_tempo;

  // generated through post-processing
  // scripts/large-evaluators/make-mf.py
  const mrs_real mins[] = { 0.0, 0.0, 0.0507398, 0.0, 0.0670043, 0.0, -4.44089e-16, 0.0, 0.0, 1.0, 41.0, 0 };
  const mrs_real maxs[] = { 0.875346, 0.932996, 1.0, 0.535128, 1.0, 0.738602, 0.919375, 3.93182, 4.02439, 93.0, 178.0, 0 };
  const mrs_real svm_weights[] = {
    1.1071, -0.8404, -0.1949, -0.2892,
    -0.2094, 2.1781, -1.369, -0.4589,
    -0.8486, -0.3786, -7.953, 0,
  };
  double svm_sum = 2.1748;

  for (int i=0; i<features.getCols(); i++) {
    if (mins[i] == maxs[i]) {
      continue;
    }
    features_normalized(i) = (features(i) - mins[i]) / (maxs[i] - mins[i]);
  }
  mrs_real mult = 1.0;

  // -1 because the final "feature" is the ground truth (for
  // calibration) and of course we don't include that in the SVM
  for (int i=0; i<features_normalized.getCols() - 1; i++) {
    svm_sum += (features_normalized(i) * svm_weights[i]);
  }

#if DISPLAY_SVM
  std::ostringstream features_text;
  features_text << "features_orig:\t";
  for (int i=0; i < features.getCols(); i++) {
    features_text << features(i) << "\t";
  }
  cout << features_text.str() << endl;

  std::ostringstream features_normalized_text;
  features_normalized_text << "features_normalized:\t";
  for (int i=0; i < features_normalized.getCols(); i++) {
    features_normalized_text << features_normalized(i) << "\t";
  }
  cout << features_normalized_text.str() << endl;

  cout<<"svm_sum:\t"<<svm_sum<<endl;
#endif

#if WRITE_INTERMEDIATE
  ofstream out_svm;
  out_svm.open("out/doubling_heuristic_svm.txt");

  ostringstream features_text2;
  features_text2 << "features_orig:\t";
  for (int i=0; i < features.getCols(); i++) {
    features_text2 << features(i) << "\t";
  }
  out_svm << features_text2.str() << endl;

  std::ostringstream features_normalized_text2;
  features_normalized_text2 << "features_normalized:\t";
  for (int i=0; i < features_normalized.getCols(); i++) {
    features_normalized_text2 << features_normalized(i) << "\t";
  }
  out_svm << features_normalized_text2.str() << endl;

  out_svm << svm_sum << endl;
#endif


#if POST_DOUBLING == 2
  if (svm_sum > 0) {
    mult = 2.0;
    //cout<<"doubling!"<<endl;
  }
  tempos(0) = mult * heuristic_tempo;
#endif

#if POST_DOUBLING == 1
  if (heuristic_tempo <= 72.5) {
    heuristic_tempo *= 2;
  }
  tempos(0) = heuristic_tempo;
#endif

#if POST_DOUBLING == 0
  tempos(0) = heuristic_tempo;
#endif



  if (haveCollections)
    evaluate_estimated_tempo(sfName, tempos, ground_truth_tempo, tolerance);

  ofstream ofs;
  ofs.open(fileName.c_str());
  // cout << tempos(0) << endl;
  ofs << tempos(0) << endl;
  ofs.close();
  cout << "Estimated tempo = " << tempos(0) << endl;


  delete beatTracker;
}

void
tempo_predicted(mrs_string sfName, float predicted_tempo, float ground_truth_tempo, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) resName;

  cout << "Using predicted tempo collection" << endl;

  if (ground_truth_tempo < 90.0)
    cout << sfName << "\t" << "sslow" << endl;
  else
    cout << sfName << "\t" << "sfast" << endl;


  mrs_realvec tempos(1);
  tempos(0) = predicted_tempo;
  if (haveCollections)
    evaluate_estimated_tempo(sfName, tempos, ground_truth_tempo, tolerance);

}


void
tempo_aim_flux2(mrs_string sfName, float ground_truth_tempo, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) resName;

  //MarSystemManager mng;


  MarSystem *beatTracker = mng.create("Series/beatTracker");

  /* Onset strength calcuates the onset strength signal whose individual
   values are computed using the fluxnet and are accumulated. The resulting
   onset strength signal is forward/backward filtered to smooth out
   adjacent peaks without move the location of the true onset peaks
  */
  MarSystem *onset_strength = mng.create("Series/onset_strength");
  MarSystem *accum = mng.create("Accumulator/accum");
  MarSystem *fluxnet = mng.create("Series/fluxnet");
  fluxnet->addMarSystem(mng.create("SoundFileSource/src"));
  fluxnet->addMarSystem(mng.create("Stereo2Mono/s2m"));
  fluxnet->addMarSystem(mng.create("ShiftInput/si"));	       // overlap for the spectral flux

  fluxnet->addMarSystem(mng.create("AimPZFC2/aimpzfc"));
  fluxnet->addMarSystem(mng.create("AimHCL2/aimhcl"));
  fluxnet->addMarSystem(mng.create("Sum/aimsum"));
  fluxnet->updControl("Sum/aimsum/mrs_string/mode", "sum_observations");





  fluxnet->addMarSystem(mng.create("Flux/flux"));
  accum->addMarSystem(fluxnet);
  onset_strength->addMarSystem(accum);


  onset_strength->addMarSystem(mng.create("ShiftInput/si2"));   // overlap for the onset strength signal
  onset_strength->addMarSystem(mng.create("Filter", "filt1"));
  onset_strength->addMarSystem(mng.create("Reverse", "reverse1"));
  onset_strength->addMarSystem(mng.create("Filter", "filt2"));
  onset_strength->addMarSystem(mng.create("Reverse", "reverse2"));


  beatTracker->addMarSystem(onset_strength);

  /* After the onset signal is calculated it is used for initial
   tempo induction based mapping an enhanced autocorrelation to a
   beat histogram BH and selecting the peaks of the BH as tempo candidates.
   The FlowThru composite is used to propagate the onset strength signal
   to the BeatPhase MarSystem which finds the beat locations and rescores
   the tempo candidates in order to select the tempo. The tempo induction
   is performed as the inner process of the FlowThru composite and the tempo
   candidates are linked to the BeatPhase tempo candidates.
  */

  MarSystem *tempoInduction = mng.create("FlowThru/tempoInduction");

  // MarSystem *adaptive_median_threshold = mng.create("Fanout/adaptive_median_threshold");
  // adaptive_median_threshold->addMarSystem(mng.create("MedianFilter", "medianfilter"));
  // adaptive_median_threshold->addMarSystem(mng.create("Gain/subtract_gain"));
  // adaptive_median_threshold->updControl("Gain/subtract_gain/mrs_real/gain", -1.0);
  // adaptive_median_threshold->updControl("MedianFilter/medianfilter/mrs_natural/WindowSize", 10);
  // tempoInduction->addMarSystem(adaptive_median_threshold);
  // tempoInduction->addMarSystem(mng.create("Sum/asum"));
  // tempoInduction->addMarSystem(mng.create("HalfWaveRectifier", "hwr"));



  tempoInduction->addMarSystem(mng.create("AutoCorrelation", "acr"));
  tempoInduction->addMarSystem(mng.create("BeatHistogram", "histo"));

  //  enhance the BH harmonic peaks
  MarSystem* hfanout = mng.create("Fanout", "hfanout");
  hfanout->addMarSystem(mng.create("Gain", "id1"));
  hfanout->addMarSystem(mng.create("TimeStretch", "tsc1"));
  tempoInduction->addMarSystem(hfanout);
  tempoInduction->addMarSystem(mng.create("Sum", "hsum"));

  // Select the peaks
  tempoInduction->addMarSystem(mng.create("Peaker", "pkr1"));
  tempoInduction->addMarSystem(mng.create("MaxArgMax", "mxr1"));

  // Using the tempo induction block calculate the Beat Locations
  beatTracker->addMarSystem(tempoInduction);
  // beatTracker->addMarSystem(mng.create("ShiftInput/si3"));
  beatTracker->addMarSystem(mng.create("BeatPhase/beatphase"));
  beatTracker->addMarSystem(mng.create("Gain/id"));


  mrs_natural winSize = 256;     // for flux calculation
  mrs_natural hopSize = 128;     // for flux calculation
  mrs_natural bhopSize = 128;    // for onset strength signal
  mrs_natural bwinSize = 2048;	 // for onset strength signal
  // mrs_natural bp_winSize = 8192; // for onset strength signal for the beat locations
  mrs_natural nCandidates = 8;  // number of tempo candidates

  onset_strength->updControl("Accumulator/accum/mrs_natural/nTimes", bhopSize);
  onset_strength->updControl("ShiftInput/si2/mrs_natural/winSize",bwinSize);
  // beatTracker->updControl("ShiftInput/si3/mrs_natural/winSize", bp_winSize);

  // parameters for the onset strength signal
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/Flux/flux/mrs_string/mode", "Laroche2003");

  // filter coefficients for forward/backward filtering
  realvec bcoeffs(1,3);
  bcoeffs(0) = 0.0564;
  bcoeffs(1) = 0.1129;
  bcoeffs(2) = 0.0564;
  onset_strength->updControl("Filter/filt1/mrs_realvec/ncoeffs", bcoeffs);
  onset_strength->updControl("Filter/filt2/mrs_realvec/ncoeffs", bcoeffs);
  realvec acoeffs(1,3);
  acoeffs(0) = 1.0000;
  acoeffs(1) = -1.2247;
  acoeffs(2) = 0.4504;
  onset_strength->updControl("Filter/filt1/mrs_realvec/dcoeffs", acoeffs);
  onset_strength->updControl("Filter/filt2/mrs_realvec/dcoeffs", acoeffs);

  // parameters for BH pick peaking
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakNeighbors", 10);
  tempoInduction->updControl("Peaker/pkr1/mrs_real/peakSpacing", 0.0);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakStart", 200);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/peakEnd", 720);
  tempoInduction->updControl("MaxArgMax/mxr1/mrs_natural/interpolation", 0);
  tempoInduction->updControl("Peaker/pkr1/mrs_natural/interpolation", 0);
  beatTracker->updControl("FlowThru/tempoInduction/MaxArgMax/mxr1/mrs_natural/nMaximums", nCandidates);

  // autocorrelation parameters
  tempoInduction->updControl("AutoCorrelation/acr/mrs_real/magcompress", 0.5);
  tempoInduction->updControl("AutoCorrelation/acr/mrs_bool/setr0to0", true);
  tempoInduction->updControl("AutoCorrelation/acr/mrs_bool/setr0to1", true);

  // beat histogram parameters
  tempoInduction->updControl("BeatHistogram/histo/mrs_natural/startBin", 0);
  tempoInduction->updControl("BeatHistogram/histo/mrs_natural/endBin", 800);
  tempoInduction->updControl("BeatHistogram/histo/mrs_real/factor", 16.0);
  tempoInduction->updControl("Fanout/hfanout/TimeStretch/tsc1/mrs_real/factor", 0.5);
  tempoInduction->updControl("Fanout/hfanout/Gain/id1/mrs_real/gain", 1.0);

  // set the filename, hop and window size
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_string/filename", sfName);
  onset_strength->updControl("Accumulator/accum/Series/fluxnet/ShiftInput/si/mrs_natural/winSize", winSize);
  beatTracker->updControl("mrs_natural/inSamples", hopSize);

  // BeatPhase estimates a tempo based on rescoring the tempo candidates
  // of the tempo induction phase by cross-correlating pulse trains
  // with the onset strength signal
  beatTracker->updControl("BeatPhase/beatphase/mrs_natural/bhopSize", bhopSize);
  beatTracker->updControl("BeatPhase/beatphase/mrs_natural/bwinSize", bwinSize);
  beatTracker->updControl("BeatPhase/beatphase/mrs_natural/nCandidates", nCandidates);
  beatTracker->updControl("BeatPhase/beatphase/mrs_real/ground_truth_tempo", ground_truth_tempo);
  beatTracker->linkControl("BeatPhase/beatphase/mrs_realvec/tempo_candidates",
                           "FlowThru/tempoInduction/MaxArgMax/mxr1/mrs_realvec/processedData");


  // the number of ticks it takes to have a full onset strength
  // signal without zeroes due to overlap
  int extra_ticks = bwinSize/bhopSize;


  mrs_realvec tempos(nCandidates);  // tempo estimates from the BH
  mrs_realvec temposcores(nCandidates);

  mrs_real phase_tempo;	 // tempo estimate calculated by the BeatPhase MarSystem
  mrs_realvec bhisto;	 // secondary beat histogram for selecting the best tempo estimate from BeatPhase
  bhisto.create(200);


  // output plugin that can be used with MarMonitors for debugging
  if (pluginName != EMPTYSTRING)
  {
    ofstream ofs;
    ofs.open(pluginName.c_str());
    ofs << *beatTracker << endl;
    ofs.close();
    pluginName = EMPTYSTRING;
  }




  int ticks = 0;

  int size_in_bytes = onset_strength->getctrl("Accumulator/accum/Series/fluxnet/SoundFileSource/src/mrs_natural/size")->to<mrs_natural>();
  int num_ticks = (size_in_bytes  / (hopSize * bhopSize)) +1;



  mrs_real bh_estimate;
  mrs_real bh_estimate2;


  while (1)
  {
    // reset the histogram after the first initial ticks that don't contain
    // enough onset strength signal for accurate estimation
    if (ticks == extra_ticks)
      tempoInduction->updControl("BeatHistogram/histo/mrs_bool/reset", true);

    // tick the network and get a tempo estimates
    beatTracker->tick();
    ticks++;

    mrs_realvec bh_candidates = beatTracker->getctrl("FlowThru/tempoInduction/MaxArgMax/mxr1/mrs_realvec/processedData")->to<mrs_realvec>();
    for (int k=0; k < nCandidates; k++)
    {
      tempos(k) = bh_candidates(2*k+1) * 0.25;
    }

    bh_estimate = tempos(0);
    bh_estimate2 = tempos(1);

    // tempo estimation using cross-correlation of candidate pulse trains to the onset strength signal
    phase_tempo = beatTracker->getControl("BeatPhase/beatphase/mrs_real/phase_tempo")->to<mrs_real>();

    tempos = beatTracker->getControl("BeatPhase/beatphase/mrs_realvec/tempos")->to<mrs_realvec>();
    temposcores = beatTracker->getControl("BeatPhase/beatphase/mrs_realvec/tempo_scores")->to<mrs_realvec>();

    if (ticks >= extra_ticks)
    {
      bhisto(phase_tempo) += temposcores(0);
    }

    if (num_ticks - ticks < 1)
    {
      break;
    }

  }



  // Find the max bin of the histogram created from the
  // BeatPhase tempo candidates
  mrs_real bhmax = 0.0;
  mrs_real bhmax2 = 0.0;

  mrs_natural max_i = 0;
  mrs_natural max_i2 = 0;

  for (int i=0; i < 200; i++)
  {
    if (bhisto(i) > bhmax)
    {
      bhmax = bhisto(i);
      max_i = i;
    }

    if ((bhisto(i) > bhmax2) && (bhisto(i) < bhmax))
    {
      bhmax2 = bhisto(i);
      max_i2 = i;
    }

  }
  mrs_real bhmaxt = max_i;
  mrs_real bhmaxt2 = max_i2;


  tempos(0) = bhmaxt;
  tempos(1) = bhmaxt2;
  tempos(2) = bh_estimate;
  tempos(3) = bh_estimate2;

  for (int i=0; i < 4; i++)
  {
    for (int j=0; j < 4; j++)
    {
      // if there are two tempo estimates with a ratio of 2 pick the higher
      // one if the lower one is less than 70 BPM
      if (i != j)
      {
        if ((fabs(2 * tempos(i) - tempos(j)) < 0.04 * tempos(j)) && (tempos(i) < 70))
          tempos(0) = tempos(j);
      }




    }

  }


  if (haveCollections)
    evaluate_estimated_tempo(sfName, tempos, ground_truth_tempo, tolerance);

  ofstream ofs;
  ofs.open(fileName.c_str());
  cout << tempos(0) << endl;
  ofs << tempos(0) << endl;
  ofs.close();

  delete beatTracker;



}




void
tempo_histoSumBands(mrs_string sfName, float ground_truth_tempo, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) resName;
  (void) haveCollections;

  //MarSystemManager mng;
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
  //mrs_natural onSamples;
  //
  //	int numPlayed =0;
  //	mrs_natural wc=0;
  //	mrs_natural samplesPlayed = 0;
  //	mrs_natural repeatId = 1;

  // vector of bpm estimate used to calculate median
  vector<mrs_real> bpms;
  //onSamples = total->getctrl("ShiftInput/si/mrs_natural/onSamples")->to<mrs_natural>();

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

  evaluate_estimated_tempo(sfName, realvec(1,1,bpms[bpms.size()-1]),
      ground_truth_tempo, tolerance);

  delete total;
}



void
tempo_histoSumBandsQ(mrs_string sfName, float ground_truth_tempo, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) resName;
  (void) haveCollections;

  //MarSystemManager mng;
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
  //mrs_natural onSamples;

  //	int numPlayed =0;
  //	mrs_natural wc=0;
  //	mrs_natural samplesPlayed = 0;
  //	mrs_natural repeatId = 1;

  // vector of bpm estimate used to calculate median
  vector<mrs_real> bpms;
  //onSamples = total->getctrl("ShiftInput/si/mrs_natural/onSamples")->to<mrs_natural>();

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

  evaluate_estimated_tempo(sfName, realvec(1,1,predicted_tempo),
                           ground_truth_tempo, tolerance);

  delete total;
}




void
tempo_medianSumBands(mrs_string sfName, float ground_truth_tempo, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) resName;
  (void) haveCollections;

  //MarSystemManager mng;
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

  evaluate_estimated_tempo(sfName, realvec(1,1,bpms[bpms.size()/2]),
      ground_truth_tempo, tolerance);

  delete total;
}



void
tempo_bcWavelet(mrs_string sfName, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) resName;
  (void) haveCollections;
  (void) tolerance;
  //MarSystemManager mng;
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


  mrs_string sdname;
  mrs_string bdname;

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
tempo_bcFilter(mrs_string sfName, mrs_string resName, bool haveCollections, mrs_real tolerance)
{
  (void) resName;
  (void) haveCollections;
  (void) tolerance;

  cout << "BOOMCICK_Filter PROCESSING" << endl;

  //MarSystemManager mng;
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

  mrs_string sdname;
  mrs_string bdname;

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
refine(mrs_string sfName, float predicted_tempo)
{
  return predicted_tempo;

  cout << "Refining tempo for " << sfName << endl;

  predicted_tempo ++;

  cout << "Initial tempo = " << predicted_tempo << endl;

  //MarSystemManager mng;

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
  mrs_string::size_type file = gtBeatsFile.rfind(".", gtBeatsFile.length()-1);

  mrs_bool readFileOK = true;
  mrs_string line;
  ifstream inStream;
  FileName inputFileGT(audioFile);

  //if gtBeatsFile = directory => add extension (.txt or .beats) to the end of filepath
  if(file == mrs_string::npos)
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

        return 1;
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

        return 1;
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

      return 1;
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
tempo_ibt(mrs_string sfName, float ground_truth_tempo, mrs_string outputTxt, bool haveCollections, mrs_real tolerance)
{
  (void) haveCollections;
  //MarSystemManager mng;


  mrs_bool micinputopt = false;
  mrs_string givefirst2beats = "-1";
  mrs_string givefirst1beat = "-1";
  mrs_string givefirst2beats_startpoint = "-1";
  mrs_string givefirst1beat_startpoint = "-1";
  mrs_bool inductionoutopt = false;
  mrs_bool dumbinductionopt = false;
  //mrs_bool logfileopt = false;
  mrs_string execPath = "";
  mrs_bool noncausalopt = false;








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


  minBPM_ = MIN_BPM;
  maxBPM_ = MAX_BPM;

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
  beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/accSize",
                           "FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/accSize");

  //Pass hypotheses matrix (from tempoinduction stage) to PhaseLock
  beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_realvec/beatHypotheses",
                           "FlowThru/tempoinduction/mrs_realvec/innerOut");
  //link backtrace option
  beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_bool/backtrace",
                           "BeatReferee/br/mrs_bool/backtrace");
  beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/curBestScore",
                           "BeatReferee/br/mrs_real/curBestScore");
  //link corFactor
  beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/corFactor",
                           "BeatReferee/br/mrs_real/corFactor");
  //link triggerInduction
  beattracker->linkControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_bool/triggerInduction",
                           "BeatReferee/br/mrs_bool/triggerInduction");
  beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_bool/triggerInduction",
                           "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_bool/triggerInduction");
  beattracker->linkControl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_bool/triggerInduction",
                           "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_bool/triggerInduction");
  //link ground-truth file (used at BeatReferee in trigger induction mode)
  beattracker->linkControl("BeatReferee/br/mrs_string/gtBeatsFile",
                           "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/gtBeatsFile");
  //link gt induction mode from PhaseLock to BeatReferee
  beattracker->linkControl("BeatReferee/br/mrs_string/gtInductionMode",
                           "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode");

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
  //Link Period Limits to PhaseLock and TempoHypotheses
  beattracker->linkControl("BeatReferee/br/mrs_natural/minPeriod",
                           "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/minPeriod");
  beattracker->linkControl("BeatReferee/br/mrs_natural/maxPeriod",
                           "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/maxPeriod");
  beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/minPeriod",
                           "BeatReferee/br/mrs_natural/minPeriod");
  beattracker->linkControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/maxPeriod",
                           "BeatReferee/br/mrs_natural/maxPeriod");
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

  //link frames2seconds adjustment from BeatTimesSink to PhaseLock and BeatReferee
  beattracker->linkControl("BeatReferee/br/mrs_natural/adjustment",
                           "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/adjustment");
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

  //if ground-truth induction or ground-truth induction mode
  if(!strcmp(groundtruth_induction.c_str(), "-1") == 0 || strcmp(induction_mode.c_str(), "groundtruth") == 0)
  {
    //if not in ground-truth induction mode
    //&& if gt induction mode different than the defined
    if(!strcmp(induction_mode.c_str(), "groundtruth") == 0 &&
        (!strcmp(groundtruth_induction.c_str(), "2b2") == 0 && !strcmp(groundtruth_induction.c_str(), "2b") == 0 &&
         !strcmp(groundtruth_induction.c_str(), "1b1") == 0 && !strcmp(groundtruth_induction.c_str(), "1b") == 0 &&
         !strcmp(groundtruth_induction.c_str(), "p") == 0 && !strcmp(groundtruth_induction.c_str(), "p_mr") == 0 &&
         !strcmp(groundtruth_induction.c_str(), "p_nr") == 0))
    {
      cerr << "Invalid induction groundtruth mode - Please define one of the following: \"2b2\";\"1b1\";\"2b\";\"1b\";\"p\";\"p_mr\";\"p_nr\"" << endl;
      cerr << "Running \"regular\" induction..." << endl;
      beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "regular");
    }
    else
    {
      if(strcmp(groundtruth_file.c_str(), "-1") == 0) //if ground-truth file not defined
      {
        //force regular induction
        beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "regular");
        cerr << "Please specify the ground-truth beat-times file path via -gt \"path/file.(beats\\txt)\"" << endl;
        cerr << "Running \"regular\" induction..." << endl;
        if(strcmp(induction_mode.c_str(), "groundtruth") == 0) //and requested ground-truth induction operation
        {
          cout << "Running \"single\" induction operation" << endl;
          induction_mode = "single"; //force single induction mode
        }
      }
      else
      {
        mrs_bool readGTFileOk = false;

        //check if ground-truth induction fixed (unchanged) within induction window
        if(strcmp(groundtruth_induction.c_str(), "2b2") == 0 || strcmp(groundtruth_induction.c_str(), "1b1") == 0)
          readGTFileOk = readGTBeatsFile(beattracker, groundtruth_file, sfName, true);
        else readGTFileOk = readGTBeatsFile(beattracker, groundtruth_file, sfName, false);

        if(readGTFileOk) //if could read beat-times gt file (if couldn't read => regular induction)
        {
          if(strcmp(groundtruth_induction.c_str(), "-1") == 0) //if no induction mode defined
          {
            //assume regular induction mode
            groundtruth_induction = "regular";
            cerr << "Trigger Induction: no induction mode defined (define it via -gti option) -> \"regular\" assumed" << endl;
          }

          //assign requested induction mode
          beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", groundtruth_induction);

          cout << "Beat-Times Ground-Truth File: " << groundtruth_file << endl;
        }
        else
          beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "regular");
      }
    }
  }

  //if requested induction operation (other than ground-truth - treated above)
  if(!strcmp(induction_mode.c_str(), "-1") == 0)
  {
    //avoid different commands than the defined -> force regular if different
    if(!strcmp(induction_mode.c_str(), "single") == 0
        && !strcmp(induction_mode.c_str(), "repeated-reset") == 0 && !strcmp(induction_mode.c_str(), "repeated-regen") == 0
        && !strcmp(induction_mode.c_str(), "random-reset") == 0 && !strcmp(induction_mode.c_str(), "random-regen") == 0
        && !strcmp(induction_mode.c_str(), "auto-reset") == 0 && !strcmp(induction_mode.c_str(), "auto-regen") == 0
        && !strcmp(induction_mode.c_str(), "givetransitions-reset") == 0 && !strcmp(induction_mode.c_str(), "givetransitions-regen") == 0)
    {
      cerr << "Induction Mode: re-define induction_mode value as one of the following: \"single\", \"auto-reset\", \"auto-regen\", \"repeated-reset\", \"repeated-regen\", \"random-reset\", \"random-regen\", \"givetransitions-reset\", \"givetransitions-regen\" -> \"single\" assumed." << endl;
      induction_mode = "single";
    }

    cout << "Requested induction in \"" << induction_mode << "\" operation" << endl;

    //handle beat error tolerance, used in trigger groundtruth mode (and assure that beat-times ground file is passed)
    if(strcmp(induction_mode.c_str(), "groundtruth") == 0)
    {
      if(triggergt_tol == 5)
        cerr << "Beat Error Tolerance: " <<
             triggergt_tol << " (if wanted different tolerance please define it through -tigt_tol option)" << endl;
      else
        cout << "Beat Error Tolerance: " << triggergt_tol << endl;
    }
  }

  beattracker->linkControl("BeatReferee/br/mrs_bool/resetFeatWindow", "ShiftInput/acc/mrs_bool/clean");

  beattracker->linkControl("BeatReferee/br/mrs_string/destFileName", "BeatTimesSink/sink/mrs_string/destFileName");

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


  //set the default non-causal tempo range to [50-250]BPM
  //(by default in causal mode tempo is limited to the range [81-160]BPM)
  //(if the selected tempo range differs from the causal default [81-160]BPM then don't activate non-causal default [50-250]BPM.)
  if(noncausalopt && ((strcmp(induction_mode.c_str(), "-1") == 0) || (strcmp(induction_mode.c_str(), "single") == 0)) && minBPM_ == 81 && maxBPM_ == 160)
  {
    minBPM_ = 50;
    maxBPM_ = 250;
  }
  //if avoid metrical changes set the tempo range within one octave at [81-160]BPM
  if(avoid_metrical_changes)
  {
    minBPM_ = 81;
    maxBPM_ = 160;
    cerr << "Avoid metrical changes mode activated (default in causal operation)." << endl;
  }
  cerr << "Considered tempo in the range [" << minBPM_ << "-" << maxBPM_ << "]BPM." << endl;


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
    audioflow->updControl("mrs_natural/inSamples", HOPSIZE);
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
  beattracker->updControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_bool/dumbInductionRequest", dumbinductionopt);

  //Size of accumulator equals inductionTime + 1 -> [0, inductionTime]
  //beattracker->updControl("ShiftInput/acc/mrs_natural/winSize", inductionTickCount+1);
  //Size of accumulator equals two times the inductionTime for better filtering (must be bigger than inductionTime)
  //(and to avoid strange slow memory behaviour with inductionTime=5)

  mrs_natural accSize = 2*inductionTickCount;
  if(accSize > (inputSize / HOPSIZE)) //to avoid having an accumulator greater then the file size
    accSize = (inputSize / HOPSIZE) +1;

  beattracker->updControl("ShiftInput/acc/mrs_natural/winSize", accSize);

  mrs_natural pkinS = tempoinduction->getctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural peakEnd = (mrs_natural)((60.0 * fsSrc)/(minBPM_ * HOPSIZE)); //MinBPM (in frames)
  mrs_natural peakStart = (mrs_natural) ((60.0 * fsSrc)/(maxBPM_ * HOPSIZE));  //MaxBPM (in frames)

  //mrs_real peakSpacing = ceil(((peakEnd-peakStart) * 4.0) / ((mrs_real)(MAX_BPM-MIN_BPM))) / pkinS;

  mrs_real peakSpacing = ((mrs_natural) (fsSrc/HOPSIZE) * (1.0-(60.0/64.0))) / (pkinS * 1.0); //spacing between peaks (4BPMs at 60BPM resolution)
  //mrs_real peakSpacing = ((mrs_natural) (peakEnd-peakStart) / (2*BPM_HYPOTHESES)) / (pkinS * 1.0);  //nrPeaks <= 2*nrBPMs

  //cout << "PkinS: " << pkinS << "; peakEnd: " << peakEnd << "; peakStart: " << peakStart << "; peakSpacing: " << peakSpacing << endl;

  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakStrength", 0.75); //0.75
  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakStart", peakStart);
  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakEnd", peakEnd);
  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakGain", 2.0);

  //for only considering the 2nd half of the induction window in the ACF calculation
  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/AutoCorrelation/acf/mrs_real/lowCutoff", 0.5);
  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/AutoCorrelation/acf/mrs_real/highCutoff", 1.0);
  beattracker->updControl("ShiftInput/acc/mrs_real/lowCleanLimit", 0.0);
  beattracker->updControl("ShiftInput/acc/mrs_real/highCleanLimit", 0.5);

  tempoinduction->updControl("Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nMaximums", BPM_HYPOTHESES);

  mrs_natural minPeriod = (mrs_natural) floor(60.0 / (maxBPM_ * HOPSIZE) * fsSrc);
  mrs_natural maxPeriod = (mrs_natural) ceil(60.0 / (minBPM_ * HOPSIZE) * fsSrc);

  mrs_natural lookAheadSamples = 20; //minPeriod; //(higher than 9 due to unconsistencies on the very beginning of the filter window)
  mrs_real thres = 1.1;//1.3; //1.75 (1.1)
  //mrs_natural lookAheadSamples = 6;
  //mrs_real thres = 1.2;

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

  beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/triggerBestScoreFactor", TRIGGER_BEST_FACTOR);


  if(strcmp(induction_mode.c_str(), "auto-reset") == 0 || strcmp(induction_mode.c_str(), "repeated-reset") == 0
      || strcmp(induction_mode.c_str(), "random-reset") == 0 || strcmp(induction_mode.c_str(), "givetransitions-reset") == 0)
    beattracker->updControl("BeatReferee/br/mrs_bool/resetAfterNewInduction", true);
  else if(strcmp(induction_mode.c_str(), "auto-regen") == 0 || strcmp(induction_mode.c_str(), "repeated-regen") == 0
          || strcmp(induction_mode.c_str(), "random-regen") == 0 || strcmp(induction_mode.c_str(), "givetransitions-regen") == 0)
    beattracker->updControl("BeatReferee/br/mrs_bool/resetAfterNewInduction", false);
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
  beattracker->updControl("BeatReferee/br/mrs_bool/backtrace", backtraceopt);
  beattracker->updControl("BeatReferee/br/mrs_natural/soundFileSize", (mrs_natural) ((inputSize / HOPSIZE)));
  beattracker->updControl("BeatReferee/br/mrs_bool/nonCausal", noncausalopt);
  beattracker->updControl("BeatReferee/br/mrs_natural/triggerGtTolerance", triggergt_tol);
  beattracker->updControl("BeatReferee/br/mrs_real/beatTransitionTol", BEAT_TRANSITION_TOL);
  beattracker->updControl("BeatReferee/br/mrs_real/supervisedTriggerThres", sup_thres);
  //if(noncausalopt) beattracker->updControl("BeatReferee/br/mrs_bool/resetAfterNewInduction", false);
  //else beattracker->updControl("BeatReferee/br/mrs_bool/resetAfterNewInduction", true);
  //map induction_mode to previous name convention
  if(strcmp(induction_mode.c_str(), "auto-reset") == 0 || strcmp(induction_mode.c_str(), "auto-regen") == 0)
    induction_mode = "supervised";
  else if(strcmp(induction_mode.c_str(), "repeated-reset") == 0 || strcmp(induction_mode.c_str(), "repeated-regen") == 0)
    induction_mode = "repeated";
  else if(strcmp(induction_mode.c_str(), "random-reset") == 0 || strcmp(induction_mode.c_str(), "random-regen") == 0)
    induction_mode = "random";
  else if(strcmp(induction_mode.c_str(), "givetransitions-reset") == 0 || strcmp(induction_mode.c_str(), "givetransitions-regen") == 0)
    induction_mode = "givetransitions";
  beattracker->updControl("BeatReferee/br/mrs_string/inductionMode", induction_mode);


  ostringstream path;
  FileName outputFile(sfName);

  //if no outputTxt dir defined -> exec dir is assumed:
  if(strcmp(outputTxt.c_str(), "") == 0)
    path << execPath << outputFile.nameNoExt();
  else
  {
    path.str("");
    mrs_string::size_type loc;
    loc = outputTxt.rfind(".txt", outputTxt.length()-1);

    if(loc == mrs_string::npos) //if only output dir defined -> append filename:
      path << outputTxt << outputFile.nameNoExt();
    else
    {
      outputTxt = outputTxt.substr(0, loc);
      path << outputTxt;
    }
  }

  //set the file with the groundtruth times of trigger
  if(strcmp(induction_mode.c_str(), "givetransitions") == 0)
  {
    ostringstream triggerFilePath;
    triggerFilePath << outputFile.path() << outputFile.nameNoExt() << "_trigger.txt";

    beattracker->updControl("BeatReferee/br/mrs_string/triggerTimesFile", triggerFilePath.str());
  }

  //adjustment used in TickCount calculation above
  beattracker->updControl("BeatTimesSink/sink/mrs_natural/adjustment", adjustment);
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
      cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
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
      cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
    }
    else
    {
      cerr << "Incorrect annotation output defined - beats+medianTempo files will be created:" << endl;
      cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
      cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
      beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "beats+medianTempo");
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
    IBTsystem->updControl("mrs_natural/inSamples", HOPSIZE);
    if((HOPSIZE != 512 || fsSrc != 44100) && audioopt) //Why different sampling_rates don't work on AudioSink?? - Is it only on LINUX??
      cerr << "MARSYAS seems to not support playing audio at sampling rates different than 44100 and hop sizes different than 512. Try -f option instead." << endl;
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

  /*
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
  */

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
  //MATLAB_EVAL("Onsets = zeros(1,862);");
  //MATLAB_EVAL("Onsets2 = zeros(1,862);");
  //MATLAB_EVAL("Induction = zeros(1,862);");
  //MATLAB_EVAL("Induction2 = zeros(1,862);");
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
      cout << "Induction........";
    }

    //Just after induction:
    if(frameCount == inductionTickCount) //CHANGE TO TRIGGER!!!
    {
      //cout << "done" << endl;

      //if backtrace and not in mic mode
      if(backtraceopt && !micinputopt)
      {
        //regress to given soundfile position (in samples)
        audioflow->updControl("SoundFileSource/src/mrs_natural/pos", 0);

        //if in 2bs mode and in causal tracking -> force backtrace to assure starting beat as ground-truth
        if(strcmp(groundtruth_induction.c_str(), "2b2") == 0 && !noncausalopt)
          cout << "Backtraced!" << endl;
        //else
        //	cout << "Forcing tracking startpoint at: " << phase_ << " (s)" << endl;

        if(audioopt) //for playing audio (with clicks on beats):
          IBTsystem->updControl("AudioSink/output/mrs_bool/initAudio", true);
      }
      if(audioopt)
      {
        if(micinputopt) //in microphone mode only plays clicks
          cout << "Playing beat taps........" << endl;
        else
          cout << "Playing audio with beat taps........" << endl;
      }

      if(!noncausalopt)
      {
        cout << "Real-Time Beat Tracking........" << endl;
      }
      else
      {
        cout << "Off-Line Beat Tracking........" << endl;
      }
    }
    //Display percentage of processing complete...
    //printf("  %d % \r", (mrs_natural) frameCount*100/inputSize);
    //cout << (mrs_natural) frameCount*100/inputSize << "%" << endl;

    frameCount++;
  }
  cout << "Finish!" << endl;

  cout << "PREDICTED TEMPO = " << endl;

  mrs_real predicted_tempo = beattracker->getctrl("BeatTimesSink/sink/mrs_real/tempo")->to<mrs_real>();
  cout << "PREDICTED TEMPO = " << predicted_tempo << endl;


  mrs_realvec tempos(1);

  tempos(0) = predicted_tempo;

  evaluate_estimated_tempo(sfName, tempos, ground_truth_tempo, tolerance);

  delete IBTsystem;
  delete audioflow;

}









// Process a collection l of soundfiles
void tempo(mrs_string inFname, mrs_string outFname, mrs_string prlabel, mrs_string label, mrs_string method, mrs_bool haveCollections, mrs_real tolerance)
{
  MRSDIAG("tempo.cpp - tempo");

  // For each file in collection estimate tempo
  mrs_string sfName = inFname;
  mrs_string resName = outFname;




  istringstream iss(label);
  float ground_truth_tempo;
  iss >> ground_truth_tempo;

  istringstream iss1(prlabel);
  mrs_real predicted_tempo;
  iss1 >> predicted_tempo;



  if (method == "MEDIAN_SUMBANDS")
  {
    tempo_medianSumBands(sfName, ground_truth_tempo, resName, haveCollections, tolerance);
  }
  else if (method == "MEDIAN_MULTIBANDS")
  {
    tempo_medianMultiBands(sfName,ground_truth_tempo, resName, haveCollections, tolerance);
  }
  else if (method == "STEM")
  {
    tempo_stem(sfName, ground_truth_tempo, resName, haveCollections, tolerance);
  }
  else if (method == "TEST_OSS_FLUX")
  {
    test_oss_flux(sfName, ground_truth_tempo, resName, haveCollections, tolerance);
  }
  else if (method == "AIM_FLUX")
  {
    tempo_aim_flux2(sfName, ground_truth_tempo, resName, haveCollections, tolerance);
  }
  else if (method == "AIM")
  {
    tempo_aim(sfName, ground_truth_tempo, resName, haveCollections, tolerance);
  }
  else if (method == "HISTO_SUMBANDS")
  {
    tempo_histoSumBands(sfName, ground_truth_tempo, resName, haveCollections, tolerance);
  }
  else if (method == "HISTO_SUMBANDSQ")
  {
    tempo_histoSumBandsQ(sfName,ground_truth_tempo, resName, haveCollections, tolerance);
  }
  else if (method == "IBT")
  {
    // cout << "TEMPO INDUCTION USING THE INESC BEAT TRACKER" << endl;
    induction_time = 5.0;
    metrical_change_time = 5.0;
    score_function = "regular";
    output = "beats+tempo";
    tempo_ibt(sfName, ground_truth_tempo, resName, haveCollections, tolerance);
  }
  else if (method == "WAVELETS")
  {
    tempo_wavelets(sfName, resName, haveCollections, tolerance);
  }
  else if (method == "BOOMCHICK_WAVELET")
  {
    cout << "BOOM-CHICK Wavelet RHYTHM EXTRACTION method " << endl;
    tempo_bcWavelet(sfName, resName, haveCollections, tolerance);
  }
  else if (method == "BOOMCHICK_FILTER")
  {
    cout << "BOOM-CHICK Filter RHYTHM EXTRACTION method " << endl;
    tempo_bcFilter(sfName, resName, haveCollections, tolerance);
  }
  else if (method == "PREDICTED")
  {
    cout << "PREDICTED COLLECTION - JUST EVALUATION" << endl;
    tempo_predicted(sfName, predicted_tempo, ground_truth_tempo, resName, haveCollections, tolerance);


  }

  else
    cout << "Unsupported tempo induction method " << endl;



}



void
readCollection(Collection& l, mrs_string name)
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
    mrs_string warn;
    warn += "Problem reading collection ";
    warn += name;
    warn += " - tried both default mf directory and current working directory";
    MRSWARN(warn);
    exit (1);

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
  cmd_options.addStringOption("predictedinput", "pi", EMPTYSTRING);
  cmd_options.addStringOption("predictedoutput", "po", EMPTYSTRING);
  cmd_options.addRealOption("tolerance", "t", 0.04);
  cmd_options.addStringOption("wrongoutput", "wo", EMPTYSTRING);
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
  predictedopt_ = cmd_options.getStringOption("predictedinput");
  predictedOutopt_ = cmd_options.getStringOption("predictedoutput");
  toleranceopt_ = cmd_options.getRealOption("tolerance");
  wrong_filename_opt_ = cmd_options.getStringOption("wrongoutput");

}











int
main(int argc, const char **argv)
{

  MRSDIAG("tempo.cpp - main");

  mrs_string progName = argv[0];
  if (argc == 1)
  {
    return printUsage(progName);
  }

  initOptions();
  if (! cmd_options.readOptions(argc,argv) )
  {
    return printUsage(progName);
  }
  loadOptions();

  if (helpopt)
    return printHelp(progName);

  if (usageopt)
    return printUsage(progName);

  mrs_string method;
  if (methodopt == EMPTYSTRING)
    method = "STEM";
  else
    method = methodopt;

  vector<mrs_string> soundfiles = cmd_options.getRemaining();

  FileName inputfile(soundfiles[0]);
  FileName wrong_filename;
  if (wrong_filename_opt_ != EMPTYSTRING) {
    wrong_filename = wrong_filename_opt_;
  } else {
    wrong_filename = inputfile.nameNoExt() + "-wrong.mf";
  }

  bool haveCollections = true;
  if (inputfile.ext() == "mf")
    haveCollections = true;
  else
    haveCollections = false;

  mrs_real tolerance = toleranceopt_;

  // collection code for batch processing
  if (haveCollections)
  {

    Collection l;
    Collection pr;

    vector<mrs_string>::iterator sfi;
    for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
    {
      mrs_string sfname = *sfi;
      readCollection(l, sfname);
    }

    if (predictedopt_ != EMPTYSTRING)
      readCollection(pr, predictedopt_);

    correct_predictions = 0;
    correct_harmonic_predictions = 0;
    correct_harmonic_mirex_predictions = 0;
    correct_mirex_predictions = 0;

    total_instances = 0;
    total_differences = 0.0;
    total_errors = 0;

    errors_.create(4);




    for (mrs_natural i=0; i < l.size(); ++i)
    {
      if (predictedopt_ == EMPTYSTRING)
      {
        tempo(l.entry(i), "default.txt", l.labelEntry(i), l.labelEntry(i), method, haveCollections, tolerance);
      }
      else
      {
        tempo(l.entry(i), "default.txt", pr.labelEntry(i), l.labelEntry(i), method, haveCollections, tolerance);
      }

    }



    ofstream predicted_collection;
    predicted_collection.open(predictedOutopt_.c_str());
    for (int i=0; i< (mrs_natural)predicted_filenames_.size(); i++)
    {
      predicted_collection << predicted_filenames_[i] << "\t" << predicted_filenames_tempos_[i] << endl;
      // predicted_collection << "#GROUND TRUTH = " << ground_truth_tempos_[i] << endl;
    }
    predicted_collection.close();


    ofstream wrong_collection;
    wrong_collection.open(wrong_filename.fullname().c_str());
    for (int i=0; i< (mrs_natural)wrong_filenames_.size(); i++)
    {
      wrong_collection << wrong_filenames_[i] << "\t" << wrong_filenames_tempos_[i] << endl;
      wrong_collection << "#GROUND TRUTH = " << wrong_ground_truth_tempos_[i] << endl;
    }
    wrong_collection.close();


  }
  else
  {
    for (vector<mrs_string>::iterator sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
    {
      mrs_string sfname = *sfi;
      tempo(*sfi, "default.txt", "0.0", "0.0", method, haveCollections, tolerance);
    }
  }

  return 0;
}
