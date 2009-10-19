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
  
#define INDUCTION_TIME 5 //Time (in seconds) of induction before tracking. Has to be > 60/MIN_BPM (5)
#define BPM_HYPOTHESES 6 //Nr. of initial BPM hypotheses (must be <= than the nr. of agents) (6)
#define PHASE_HYPOTHESES 20//Nr. of phases per BPM hypothesis (20)
#define MIN_BPM 50 //minimum tempo considered, in BPMs (50)
#define MAX_BPM 250 //maximum tempo considered, in BPMs (250)
#define NR_AGENTS 50 //Nr. of agents in the pool (50)
#define LFT_OUTTER_MARGIN 0.20 //The size of the outer half-window (in % of IBI) before the predicted beat time (0.20)
#define RGT_OUTTER_MARGIN 0.40 //The size of the outer half-window (in % of IBI) after the predicted beat time (0.40)
#define INNER_MARGIN 4.0 //Inner tolerance window margin size (in ticks) (4.0)
#define OBSOLETE_FACTOR 1.5 //An agent is killed if, at any time, the difference between its score and the bestScore is below OBSOLETE_FACTOR * bestScore (1.5)
#define CHILD_FACTOR 0.01 //(Inertia1) Each created agent imports its father score decremented by the current dScore divided by this factor (0.05)
#define BEST_FACTOR 1.01 //(Inertia2) Mutiple of the bestScore an agent's score must have for replacing the current best agent (1.15)
#define EQ_PERIOD 0 //Period threshold which identifies two agents as predicting the same period (IBI, in ticks) (1)
#define EQ_PHASE 0 //Period threshold which identifies two agents as predicting the same phase (beat time, in ticks) (2)

#define WINSIZE 2048 //2048
#define HOPSIZE 512 //512

CommandLineOptions cmd_options;

mrs_string score_function;
mrs_real induction_time;
mrs_string execPath;
mrs_natural helpopt;
mrs_natural usageopt;
mrs_natural verboseopt;
mrs_natural audioopt;
mrs_natural audiofileopt;

void 
initOptions()
{
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addBoolOption("verbose", "v", false);
	cmd_options.addBoolOption("audio", "a", false);
	cmd_options.addBoolOption("audiofile", "f", false);
	cmd_options.addStringOption("score_function", "s", "regular");
}

void 
loadOptions()
{
	helpopt = cmd_options.getBoolOption("help");
	usageopt = cmd_options.getBoolOption("usage");
	verboseopt = cmd_options.getBoolOption("verbose");
	audioopt = cmd_options.getBoolOption("audio");
	audiofileopt = cmd_options.getBoolOption("audiofile");
	score_function = cmd_options.getStringOption("score_function");
}

void 
ibt_regular(mrs_string sfName, mrs_string outputTxt)
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

					//onsetdetectionfunction->addMarSystem(mng.create("SonicVisualiserSink", "sonicsink"));
							
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
	IBTsystem->linkctrl("mrs_bool/notEmpty", 
		"Fanout/beatmix/Series/audioflow/SoundFileSource/src/mrs_bool/notEmpty");

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
		"FlowThru/tempoinduction/Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nPhases");

	//Pass enabled (muted) BeatAgents (from FanOut) to the BeatReferee
	beattracker->linkctrl("Fanout/agentpool/mrs_realvec/muted", "BeatReferee/br/mrs_realvec/muted");
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
	//TempoHypotheses indTime = induction time
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime", 
		"ShiftInput/acc/mrs_natural/winSize");
	//PhaseLock timming = induction time
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime", 
		"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime");	
	//BeatReferee timming = induction time
	beattracker->linkctrl("BeatReferee/br/mrs_natural/inductionTime", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime");

	//Link BPM conversion parameters to BeatReferee:
	beattracker->linkctrl("BeatReferee/br/mrs_natural/hopSize", "mrs_natural/inSamples");

	//Link Output Sink parameters with the used ones:
	beattracker->linkctrl("BeatTimesSink/sink/mrs_natural/hopSize", "BeatReferee/br/mrs_natural/hopSize");
	beattracker->linkctrl("BeatTimesSink/sink/mrs_real/srcFs", "BeatReferee/br/mrs_real/srcFs");
	beattracker->linkctrl("BeatTimesSink/sink/mrs_natural/winSize", 
		"Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize");
	beattracker->linkctrl("BeatTimesSink/sink/mrs_natural/tickCount", "BeatReferee/br/mrs_natural/tickCount");

	//Link SonicVisualiserSink parameters with the used ones:
	/*
	beattracker->linkctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_natural/hopSize", 
		"BeatTimesSink/sink/mrs_natural/hopSize");
	beattracker->linkctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_real/srcFs", 
		"BeatTimesSink/sink/mrs_real/srcFs");
	*/

	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/hopSize", 
		"BeatTimesSink/sink/mrs_natural/hopSize");
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs", 
		"BeatTimesSink/sink/mrs_real/srcFs");

	//link beatdetected with noise ADSR -> for clicking when beat:
	IBTsystem->linkctrl("Fanout/beatmix/Series/audioflow/FlowThru/beattracker/BeatReferee/br/mrs_real/beatDetected", 
		"Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/nton");


	///////////////////////////////////////////////////////////////////////////////////////
	// update controls
	///////////////////////////////////////////////////////////////////////////////////////
	//FileName outputFile(sfName);
	audioflow->updctrl("SoundFileSource/src/mrs_string/filename", sfName);

	//best result till now are using dB power Spectrum!
	beattracker->updctrl("Series/onsetdetectionfunction/PowerSpectrum/pspk/mrs_string/spectrumType", "magnitude");

	beattracker->updctrl("Series/onsetdetectionfunction/Flux/flux/mrs_string/mode", "DixonDAFX06");

	beattracker->updctrl("mrs_natural/inSamples", HOPSIZE);
	beattracker->updctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize", WINSIZE);

	mrs_real fsSrc = beattracker->getctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_real/israte")->to<mrs_real>();

	
	mrs_natural inductionTickCount = (induction_time * fsSrc) / HOPSIZE; //induction time (in nr. of ticks)
	mrs_natural inputSize = audioflow->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>(); //(in samples)
	
	//to avoid induction time greater than input file size
	//(in this case the induction time will equal the file size)
	if((inputSize / HOPSIZE) < inductionTickCount)
		inductionTickCount = inputSize / HOPSIZE;
	
	beattracker->updctrl("ShiftInput/acc/mrs_natural/winSize", inductionTickCount);

	mrs_natural pkinS = tempoinduction->getctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
	mrs_natural peakEnd = (mrs_natural)((60.0 * fsSrc)/(MIN_BPM * HOPSIZE)); //50BPM (in frames)
	mrs_natural peakStart = (mrs_natural) ((60.0 * fsSrc)/(MAX_BPM * HOPSIZE));  //400BPM (in frames)
	mrs_real peakSpacing = ((mrs_natural) ((60.0 * fsSrc)/(60 * HOPSIZE)) //4BMP resolution
		- ((60.0 * fsSrc)/(64 * HOPSIZE))) / (pkinS * 1.0);

	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakStrength", 0.75);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakStart", peakStart);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakEnd", peakEnd);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakGain", 2.0);

	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nMaximums", BPM_HYPOTHESES);

	mrs_natural lookAheadSamples = 9; //multiple of 3
	mrs_real thres = 1.75;

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
	beattracker->updctrl("BeatReferee/br/mrs_real/childFactor", CHILD_FACTOR);
	beattracker->updctrl("BeatReferee/br/mrs_real/bestFactor", BEST_FACTOR);
	beattracker->updctrl("BeatReferee/br/mrs_natural/eqPeriod", EQ_PERIOD);
	beattracker->updctrl("BeatReferee/br/mrs_natural/eqPhase", EQ_PHASE);

	FileName outputFile(sfName);
	ostringstream path;

	#ifdef MARSYAS_WIN32
		path << execPath << "\\";
	#else
		path << execPath << "/";
	#endif

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

	beattracker->updctrl("BeatTimesSink/sink/mrs_string/destFileName", path.str());
	//beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "medianTempo");
	//beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "beatTimes");
	beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "all");

	
	//beattracker->updctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_string/mode", "frames");
	//beattracker->updctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_string/destFileName", path.str() + "_onsetFunction.txt");

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
		cout << "Audio File with Beats Saved: " << path.str() << "_beats.wav" << endl;
	}

	//MATLAB Engine inits
	//used for autocorrelation.m
	mrs_natural winSize = WINSIZE;
	mrs_natural hopSize = HOPSIZE;

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
	inputSize = (inputSize / HOPSIZE) + inductionTickCount; //inputSize in ticks
	
	//while(IBTsystem->getctrl("mrs_bool/notEmpty")->to<mrs_bool>())
	cout << "Induction........";
	while(frameCount <= inputSize)
	{	
		IBTsystem->tick();
 
		frameCount++;
		//Just after induction:
		if(frameCount == inductionTickCount)
		{
			//Restart reading audio file
			audioflow->updctrl("SoundFileSource/src/mrs_natural/pos", 0);
			//for playing audio (with clicks on beats):
			cout << "done" << endl;
			cout << "Beat Tracking........" << endl;

			if(audioopt)
			{
				IBTsystem->updctrl("AudioSink/output/mrs_bool/initAudio", true);
				cout << "Playing Audio with beat taps........" << endl;
			}
		}
		//Display percentage of processing complete...
		//cout << (mrs_natural) frameCount*100/inputSize << "%" << endl;
	}

	cout << "Finish!" << endl;
}

void
ibt_average(mrs_string sfName, mrs_string mode, mrs_string outputTxt)
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

					//onsetdetectionfunction->addMarSystem(mng.create("SonicVisualiserSink", "sonicsink"));
							
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
			beattracker->addMarSystem(mng.create("BeatRefereeAvg", "br"));
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
	IBTsystem->linkctrl("mrs_bool/notEmpty", 
		"Fanout/beatmix/Series/audioflow/SoundFileSource/src/mrs_bool/notEmpty");

	//Link LookAheadSamples used in PeakerOnset for compensation when retriving the actual initial OnsetTimes
	tempoinduction->linkctrl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_natural/lookAheadSamples", 
		"Fanout/tempohypotheses/Series/phase/OnsetTimes/OnsetTimes/mrs_natural/lookAheadSamples");

	//Pass hypotheses matrix (from tempoinduction stage) to PhaseLock
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_realvec/beatHypotheses", 
		"FlowThru/tempoinduction/mrs_realvec/innerOut");
	
	//Pass initital hypotheses to BeatRefereeAvg
	beattracker->linkctrl("BeatRefereeAvg/br/mrs_realvec/beatHypotheses", 
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
		"FlowThru/tempoinduction/Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nPhases");

	//Pass enabled (muted) BeatAgents (from FanOut) to the BeatRefereeAvg
	beattracker->linkctrl("Fanout/agentpool/mrs_realvec/muted", "BeatRefereeAvg/br/mrs_realvec/muted");
	//Pass tempohypotheses Fanout muted vector to the BeatRefereeAvg, for disabling induction after induction timming
	beattracker->linkctrl("FlowThru/tempoinduction/Fanout/tempohypotheses/mrs_realvec/muted", 
		"BeatRefereeAvg/br/mrs_realvec/inductionEnabler");

	//Link agentControl matrix from the BeatRefereeAvg to each agent in the pool
	for(int i = 0; i < NR_AGENTS; i++)
	{
		ostringstream oss;
		oss << "agent" << i;
		beattracker->linkctrl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_realvec/agentControl", 
			"BeatRefereeAvg/br/mrs_realvec/agentControl");
	}

	//Defines tempo induction time after which the BeatAgents' hypotheses are populated:
	//TempoHypotheses indTime = induction time
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime", 
		"ShiftInput/acc/mrs_natural/winSize");
	//PhaseLock timming = induction time
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime", 
		"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime");	
	//BeatRefereeAvg timming = induction time
	beattracker->linkctrl("BeatRefereeAvg/br/mrs_natural/inductionTime", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime");

	//Link BPM conversion parameters to BeatRefereeAvg:
	beattracker->linkctrl("BeatRefereeAvg/br/mrs_natural/hopSize", "mrs_natural/inSamples");

	//Link Output Sink parameters with the used ones:
	beattracker->linkctrl("BeatTimesSink/sink/mrs_natural/hopSize", "BeatRefereeAvg/br/mrs_natural/hopSize");
	beattracker->linkctrl("BeatTimesSink/sink/mrs_real/srcFs", "BeatRefereeAvg/br/mrs_real/srcFs");
	beattracker->linkctrl("BeatTimesSink/sink/mrs_natural/winSize", 
		"Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize");
	beattracker->linkctrl("BeatTimesSink/sink/mrs_natural/tickCount", "BeatRefereeAvg/br/mrs_natural/tickCount");

	//Link SonicVisualiserSink parameters with the used ones:
	/*
	beattracker->linkctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_natural/hopSize", 
		"BeatTimesSink/sink/mrs_natural/hopSize");
	beattracker->linkctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_real/srcFs", 
		"BeatTimesSink/sink/mrs_real/srcFs");
	*/

	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/hopSize", 
		"BeatTimesSink/sink/mrs_natural/hopSize");
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs", 
		"BeatTimesSink/sink/mrs_real/srcFs");

	//link beatdetected with noise ADSR -> for clicking when beat:
	IBTsystem->linkctrl("Fanout/beatmix/Series/audioflow/FlowThru/beattracker/BeatRefereeAvg/br/mrs_real/beatDetected", 
		"Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/nton");


	///////////////////////////////////////////////////////////////////////////////////////
	// update controls
	///////////////////////////////////////////////////////////////////////////////////////
	//FileName outputFile(sfName);
	audioflow->updctrl("SoundFileSource/src/mrs_string/filename", sfName);

	//best result till now are using dB power Spectrum!
	beattracker->updctrl("Series/onsetdetectionfunction/PowerSpectrum/pspk/mrs_string/spectrumType", "magnitude");

	beattracker->updctrl("Series/onsetdetectionfunction/Flux/flux/mrs_string/mode", "DixonDAFX06");

	beattracker->updctrl("mrs_natural/inSamples", HOPSIZE);
	beattracker->updctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize", WINSIZE);

	mrs_real fsSrc = beattracker->getctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_real/israte")->to<mrs_real>();

	mrs_natural inductionTickCount = (induction_time * fsSrc) / HOPSIZE; //induction time (in nr. of ticks)
	mrs_natural inputSize = audioflow->getctrl("SoundFileSource/src/mrs_natural/size")->to<mrs_natural>(); //(in samples)
	
	//to avoid induction time greater than input file size
	//(in this case the induction time will equal the file size)
	if((inputSize / HOPSIZE) < inductionTickCount)
		inductionTickCount = inputSize / HOPSIZE;

	beattracker->updctrl("ShiftInput/acc/mrs_natural/winSize", inductionTickCount);

	mrs_natural pkinS = tempoinduction->getctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
	mrs_natural peakEnd = (mrs_natural)((60.0 * fsSrc)/(MIN_BPM * HOPSIZE)); //50BPM (in frames)
	mrs_natural peakStart = (mrs_natural) ((60.0 * fsSrc)/(MAX_BPM * HOPSIZE));  //400BPM (in frames)
	mrs_real peakSpacing = ((mrs_natural) ((60.0 * fsSrc)/(60 * HOPSIZE)) //4BMP resolution
		- ((60.0 * fsSrc)/(64 * HOPSIZE))) / (pkinS * 1.0);

	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakStrength", 0.75);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakStart", peakStart);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakEnd", peakEnd);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakGain", 2.0);

	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nMaximums", BPM_HYPOTHESES);

	mrs_natural lookAheadSamples = 9; //multiple of 3
	mrs_real thres = 1.75;

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

	beattracker->updctrl("BeatRefereeAvg/br/mrs_real/srcFs", fsSrc);
	beattracker->updctrl("BeatRefereeAvg/br/mrs_natural/minTempo", MIN_BPM);
	beattracker->updctrl("BeatRefereeAvg/br/mrs_natural/maxTempo", MAX_BPM);
	beattracker->updctrl("BeatRefereeAvg/br/mrs_real/obsoleteFactor", OBSOLETE_FACTOR);
	beattracker->updctrl("BeatRefereeAvg/br/mrs_real/childFactor", CHILD_FACTOR);
	beattracker->updctrl("BeatRefereeAvg/br/mrs_real/bestFactor", BEST_FACTOR);
	beattracker->updctrl("BeatRefereeAvg/br/mrs_natural/eqPeriod", EQ_PERIOD);
	beattracker->updctrl("BeatRefereeAvg/br/mrs_natural/eqPhase", EQ_PHASE);

	
	FileName outputFile(sfName);
	ostringstream path;

	#ifdef MARSYAS_WIN32
		path << execPath << "\\";
	#else
		path << execPath << "/";
	#endif

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

	beattracker->updctrl("BeatTimesSink/sink/mrs_string/destFileName", path.str());
	//beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "medianTempo");
	//beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "beatTimes");
	beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "all");

	/*
	beattracker->updctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_string/mode", "seconds");
	beattracker->updctrl("Series/onsetdetectionfunction/SonicVisualiserSink/sonicsink/mrs_string/destFileName", 
		path.str() + "_onsetFunction.txt");
	*/

	//set audio/onset resynth balance and ADSR params for onset sound
	IBTsystem->updctrl("Fanout/beatmix/Series/audioflow/Gain/gainaudio/mrs_real/gain", 0.6);
	IBTsystem->updctrl("Fanout/beatmix/Series/beatsynth/Gain/gainbeats/mrs_real/gain", 0.8);
	IBTsystem->updctrl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/aTarget", 1.0);
 	IBTsystem->updctrl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/aTime", WINSIZE/80/fsSrc);
 	IBTsystem->updctrl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/susLevel", 0.0);
 	IBTsystem->updctrl("Fanout/beatmix/Series/beatsynth/ADSR/env/mrs_real/dTime", WINSIZE/4/fsSrc);

	//for saving file with audio+clicks (on beats):
	if(audiofileopt)
	{
		IBTsystem->updctrl("SoundFileSink/fdest/mrs_string/filename", path.str() + "_beats.wav");
		cout << "Audio File with Beats Saved: " << path.str() << "_beats.wav" << endl;
	}

	//MATLAB Engine inits
	//used for autocorrelation.m
	//MATLAB_EVAL("FluxTS = [];");
	/*
	mrs_natural winSize = WINSIZE;
	mrs_natural hopSize = HOPSIZE;
	MATLAB_EVAL("clear;");
	MATLAB_PUT(induction_time, "timming");
	MATLAB_PUT(fsSrc, "SrcFs");
	MATLAB_PUT(inductionTickCount, "inductionTickCount");
	MATLAB_PUT(winSize, "winSize");
	MATLAB_PUT(hopSize, "hopSize");
	MATLAB_EVAL("srcAudio = [];");
	MATLAB_EVAL("FluxTS = [];");
	MATLAB_EVAL("FinalTS = [];");
	MATLAB_EVAL("BeatAgentTS=[];");
	MATLAB_EVAL("BeatAgentsTS=[];");
	MATLAB_EVAL("FinalBeats=[];");
	MATLAB_EVAL("bestAgentScore=[];");
	MATLAB_EVAL("Flux_FilterTS=[];");
	*/

	///////////////////////////////////////////////////////////////////////////////////////
	//process input file (till EOF)
	///////////////////////////////////////////////////////////////////////////////////////
	mrs_natural frameCount = 0;
	inputSize = (inputSize / HOPSIZE) + inductionTickCount; //inputSize in ticks
	
	cout << "Induction........";
	while(frameCount <= inputSize)
	{	
		IBTsystem->tick();
 
		frameCount++;
		//Just after induction:
		if(frameCount == inductionTickCount)
		{
			//Restart reading audio file
			audioflow->updctrl("SoundFileSource/src/mrs_natural/pos", HOPSIZE);
			//for playing audio (with clicks on beats):
			cout << "done" << endl;
			cout << "Beat Tracking........" << endl;

			if(audioopt)
			{
				IBTsystem->updctrl("AudioSink/output/mrs_bool/initAudio", true);
				cout << "Playing Audio with beat taps........" << endl;
			}
		}
		//Display percentage of processing complete...
		//cout << (mrs_natural) frameCount*100/inputSize << "%" << endl;
	}

	cout << "Finish!" << endl;
}

int
main(int argc, const char **argv)
{
	MRSDIAG("SF+ACF.cpp - Main");

	if (argc == 1)
	{
	//      printUsage(progName);
	  exit(1);
	}

	initOptions(); //initialize app options
	cmd_options.readOptions(argc,argv);
	loadOptions(); //load app options from command line

	/* // print help or usage
	if (helpopt) 
	printHelp(progName);

	if (usageopt)
	printUsage(progName);
	*/
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

	induction_time = INDUCTION_TIME;
	cout << "SoundFile: " << sfName << "\nInductionTime: " << induction_time << "s\nScoreFunction: " << 
	  score_function << endl;

	if(strcmp(score_function.c_str(), "avgCorr") == 0)
	{
	  score_function = "correlation";
	  ibt_average(sfName, "avgCorr", outputTxt);
	}
	else if(strcmp(score_function.c_str(), "avgRegular") == 0)
	{
	  score_function = "regular";
	  ibt_average(sfName, "avgRegular", outputTxt);
	}
	else if(strcmp(score_function.c_str(), "avgSquareCorr") == 0)
	{
	  score_function = "squareCorr";
	  ibt_average(sfName, "avgSquareCorr", outputTxt);
	}
	else
	  ibt_regular(sfName, outputTxt);

	return 0;
}
