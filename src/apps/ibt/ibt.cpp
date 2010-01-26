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
#include <string>
#include <iomanip> 

#include "common.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"
#include "FileName.h"

#include "MarSystemTemplateBasic.h"
#include "MarSystemTemplateAdvanced.h"
#include "EvValUpd.h"
#include "Collection.h"
#include "NumericLib.h"
#include <string.h>

#ifdef MARSYAS_MIDIIO
#include "RtMidi.h"
#endif 

#ifdef MARSYAS_AUDIOIO
#include "RtAudio3.h"
#endif 

using namespace std;
using namespace Marsyas;

#pragma warning(disable: 4244)  //disable double to float warning
#pragma warning(disable: 4100) //disable argc warning
  
//============================== IBT FUNCTIONAL PARAMETERS ==============================

//#define SCORE_FUNCTION "regular" //the score function (heuristics) which conducts the beat tracking ("regular")
//#define INDUCTION_TIME 5.0 //Time (in seconds) of the beat induction step, before tracking. Has to be > 60/MIN_BPM (5.0)
//#define METRICAL_CHANGE_TIME 0.0 //Initial time (in seconds) allowed for eventual metrical changes within tracking (if 0.0 no metrical changes allowed at all; if -1.0 metrical changes would be allowed along all musical piece) (5.0)
#define BPM_HYPOTHESES 6 //Nr. of initial BPM hypotheses (must be <= than the nr. of agents) (6)
#define PHASE_HYPOTHESES 20//Nr. of phases per BPM hypothesis (20)
#define MIN_BPM 50 //minimum tempo considered, in BPMs (50)
#define MAX_BPM 250 //maximum tempo considered, in BPMs (250)
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

//=======================================================================================

CommandLineOptions cmd_options;

mrs_string score_function;
mrs_string output;
mrs_string induction_gt;
mrs_real induction_time;
mrs_real metrical_change_time;
mrs_string execPath;
mrs_natural helpopt;
mrs_natural usageopt;
mrs_bool audioopt;
mrs_bool audiofileopt;
mrs_bool backtraceopt;
mrs_bool sonicOut = false;

void 
printUsage(string progName)
{
	MRSDIAG("ibt.cpp - printUsage");
	cerr << "Usage : " << progName << 
		" [-s scoreFunction] [-t inductionTime (in secs)] [-m metricalChangeTime (in secs)] [-b backtrace] [-o annotationsOutput] [-a play_w/_beats] [-f outputFile_w/_beats] [-igt induction_ground-truth] fileName outDir" << endl;
	cerr << "where fileName is a sound file in a MARSYAS supported format and outDir the directory where the annotation files (beats + tempo) shall be saved (ibt.exe dir by default)." << endl;
	cerr << endl;
	exit(1); 
}

void 
printHelp(string progName)
{
	MRSDIAG("ibt.cpp - printHelp");
	cerr << "ibt, MARSYAS, Copyright George Tzanetakis " << endl;
	cerr << "--------------------------------------------------------" << endl;
	cerr << "Detect the beat times (and the tempo - median IBI) in the sound file provided as argument" << endl;
	cerr << endl;
	cerr << "Usage : " << progName << " fileName" << endl;
	cerr << endl;
	cerr << "where file is a sound file in a Marsyas supported format" << endl;
	cerr << "Help Options:" << endl;
	cerr << "-u --usage          : display short usage info" << endl;
	cerr << "-h --help           : display this information " << endl;
	cerr << "-s --score_function : heuristics which conducts the beat tracking" << endl;
	cerr << "-t --induction_time : time (in secs) dispended in the initial induction stage" << endl;
	cerr << "-m --metrical_time  : initial time (in secs) allowing tracking metrical changes (0 not allowing at all; -1 for the whole music)" << endl;
	cerr << "-b --backtrace      : after induction backtrace the analysis to the beginning" << endl;
	cerr << "-a --audio          : play the original sound mixed with the synthesized beats" << endl;
	cerr << "-f --audiofile      : output the original sound mixed with the synthesized beats (as fileName_beats.*)" << endl;
	cerr << "-igt --induction_gt : replace induction stage with ground-truth (two first beats from beatTimes file - .txt or .beats - from the directory given as argument)" << endl;
	cerr << "-o --annot_output   : output the predicted beat times and/or the (median) tempo (\"beats+tempo\"-default; \"beats\"; \"tempo\"; \"none\")" << endl;
	cerr << "Available Score Functions: " << endl;
	cerr << "\"regular\" (default)" << endl;
	cerr << "\"correlation\"" << endl;
	cerr << "\"squareCorr\"" << endl;
	exit(1);
}

void 
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addBoolOption("audio", "a", false);
	cmd_options.addBoolOption("audiofile", "f", false);
	cmd_options.addBoolOption("backtrace", "b", false);
	cmd_options.addStringOption("induction_gt", "igt", "-1");
	cmd_options.addStringOption("output", "o", "beats+tempo");
	//the score function (heuristics) which conducts the beat tracking ("regular" by default)
	cmd_options.addStringOption("score_function", "s", "regular");
	//Time (in seconds) of induction before tracking. Has to be > 60/MIN_BPM (5.0 by default)
	cmd_options.addRealOption("induction_time", "t", 5.0);
	//initial time (in secs) allowed for eventual tracking metrical changes (0 not allowing at all; -1 for the whole music)" (5.0 by default)
	cmd_options.addRealOption("metrical_change_time", "m", -1);
}

void 
loadOptions()
{
	helpopt = cmd_options.getBoolOption("help");
	usageopt = cmd_options.getBoolOption("usage");
	audioopt = cmd_options.getBoolOption("audio");
	audiofileopt = cmd_options.getBoolOption("audiofile");
	backtraceopt = cmd_options.getBoolOption("backtrace");
	induction_gt = cmd_options.getStringOption("induction_gt");
	output = cmd_options.getStringOption("output");
	score_function = cmd_options.getStringOption("score_function");
	induction_time = cmd_options.getRealOption("induction_time");
	metrical_change_time = cmd_options.getRealOption("metrical_change_time");
}

mrs_bool
existsFile(mrs_string fileName)
{
	if (FILE * file = fopen(fileName.c_str(), "r"))
	{
		fclose(file);
		return true;
	}
	else 
	{
		cout << "Bad or nonexistent file! Please specify a supported one." << endl;
		return false;
	}
}

void
ibt(mrs_string sfName, mrs_string outputTxt)
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
	if(strcmp(output.c_str(), "none") != 0)
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
	if(strcmp(output.c_str(), "none") != 0)
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

	
	//For Induction Ground-Truth in "PhaseLock"
	if(strcmp(induction_gt.c_str(), "-1") != 0) //enabled induction ground-truth
	{
		beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/hopSize", 
							  "BeatReferee/br/mrs_natural/hopSize");
		beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/srcFs", 
							  "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs");

		ostringstream pathGT; //induction ground-truth path (retrieved from argument)
		FileName outputFileGT(sfName);
		pathGT << induction_gt.c_str() << outputFileGT.nameNoExt();

		beattracker->updctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "ground-truth");
		beattracker->updctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/gtBeatsFile", pathGT.str());
	}

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
	if(strcmp(outputTxt.c_str(), "") == 0)
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

	if(strcmp(output.c_str(), "none") != 0)
	{
		beattracker->updctrl("BeatTimesSink/sink/mrs_string/destFileName", path.str());
		
		if(strcmp(output.c_str(), "beats") == 0)
		{
			beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "beatTimes");
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
		}
		else if(strcmp(output.c_str(), "tempo") == 0)
		{
			beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "medianTempo");
			cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
		}
		else if(strcmp(output.c_str(), "beats+tempo") == 0)
		{
			beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "beats+tempo");
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
			cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
		}
		else
		{
			cout << "Incorrect annotation output defined - beats+tempo files will be created:" << endl;
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
			cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
			beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "beats+tempo");
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
			cout << "Induction........";
		}

		//Just after induction:
		if(frameCount == inductionTickCount)
		{	
			//for playing audio (with clicks on beats):
			cout << "done" << endl;

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
			
			cout << "Beat Tracking........" << endl;
		}
		//Display percentage of processing complete...
		//printf("  %d % \r", (mrs_natural) frameCount*100/inputSize);
		//cout << (mrs_natural) frameCount*100/inputSize << "%" << endl;
		
		frameCount++;
	}
	cout << "Finish!" << endl;
}

int
main(int argc, const char **argv)
{
	MRSDIAG("SF+ACF.cpp - Main");

	if (argc == 1)
	{
		printUsage("ibt");
		exit(1);
	}

	initOptions(); //initialize app options
	cmd_options.readOptions(argc,argv);
	loadOptions(); //load app options from command line

	// print help or usage
	if (helpopt) 
		printHelp("ibt");

	if (usageopt)
		printUsage("ibt");
	
	execPath = string(argv[0]);
#ifdef MARSYAS_WIN32
	execPath = execPath.substr(0, execPath.rfind('\\'));
#else 
	execPath = execPath.substr(0, execPath.rfind('/'));
#endif

	vector<string> soundfiles = cmd_options.getRemaining();
	mrs_string sfName = "";
	mrs_string outputTxt = "";

	if (soundfiles.size() > 0)
		sfName = soundfiles[0];
	if (soundfiles.size() > 1)  
		outputTxt = soundfiles[1];

	FileName outputFile(sfName);
	if(strcmp(outputFile.ext().c_str(), "wav") != 0 && strcmp(outputFile.ext().c_str(), "mp3") != 0 && 
	   strcmp(outputFile.ext().c_str(), "au") != 0 && strcmp(outputFile.ext().c_str(), "raw") != 0)
	{
		if(strcmp(outputFile.ext().c_str(), "mf") == 0 || strcmp(outputFile.ext().c_str(), "txt") == 0)
		{
			ifstream inStream;
			mrs_natural i = 0;
			mrs_string lineFile;
			inStream.open(outputFile.fullname().c_str());
			cout << "InductionTime: " << induction_time << "secs"
				 << "\nScoreFunction: " << score_function;
			if(metrical_change_time == -1)
				cout << "\nMetrical Change: permitted througout all piece" << endl;
			else
				cout << "\nMetrical Change: initial " << metrical_change_time << "secs" << endl;
			
			cout << "\nInputing Collection " << sfName << "..." << endl;

			while(!inStream.eof())
			{
				getline (inStream, lineFile);
				if(lineFile != "")
				{
					cout << "SoundFile" << i << ": " << lineFile << endl;
					if(existsFile(lineFile))
						ibt(lineFile, outputTxt);
					i++;
				}
			}
		}
		else
			cout << "Unsupported or nonspecified audio format!" << endl;
	}

	else
	{
		cout << "SoundFile: " << sfName << endl;
		if(existsFile(sfName))
		{
			cout << "InductionTime: " << induction_time << "secs"
				 << "\nScoreFunction: " << score_function;
			if(metrical_change_time == -1)
				cout << "\nMetrical Change: permitted througout all piece" << endl;
			else
				cout << "\nMetrical Change: initial " << metrical_change_time << "secs" << endl;
			
			ibt(sfName, outputTxt);
		}
		else exit(0);
	}

	cout << "All Done!" << endl;
	exit(0);
}
