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

#ifdef MARSYAS_WIN32
#pragma warning(disable: 4244)  //disable double to float warning
#pragma warning(disable: 4100) //disable argc warning
#endif 

mrs_bool sonicOutFlux = 0;
mrs_bool sonicOutFluxFilter = 0;

//============================== IBT FUNCTIONAL PARAMETERS ==============================

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

//=======================================================================================

CommandLineOptions cmd_options;

mrs_string score_function;
mrs_string output;
mrs_string givefirst2beats;
mrs_string givefirst2beats_startpoint;
mrs_string givefirst1beat;
mrs_string givefirst1beat_startpoint;
mrs_string giveinitperiod;
mrs_string giveinitperiod_metricalrel;
mrs_string giveinitperiod_nonrel;
mrs_real induction_time;
mrs_real metrical_change_time;
mrs_string execPath;
mrs_natural helpopt;
mrs_natural usageopt;
mrs_bool audioopt;
mrs_bool audiofileopt;
mrs_bool backtraceopt;
mrs_bool logfileopt;
mrs_bool noncausalopt;
mrs_bool dumbinductionopt;
mrs_bool inductionoutopt;
mrs_bool micinputopt;
mrs_natural sendudp_port;
mrs_real phase_;

void
printUsage(string progName)
{
	MRSDIAG("ibt.cpp - printUsage");
	cerr << "Usage : " << progName <<
		" [-s scoreFunction] [-t inductionTime (in secs)] [-m metricalChangeTime (in secs)] [-b backtrace] [-di dumb_induction] [-nc non-causal] [-o annotationsOutput] [-a play_w/_beats] [-f outputFile_w/_beats] [-2b givefirst2beats] [-1b givefirst1beat] [-pgt giveinitperiod] [-pgt_mr giveinitperiod+metricalrel] [-pgt_nr giveinitperiod+nonrel] [-2bs givefirst2beats_startpoint] [-1bs givefirst1beat_startpoint] [-l log_file] [-io induction_out] [-mic microphone_input] [-send_udp send_udp] fileName outDir" << endl;
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
	cerr << "-di --dumb_induction: for ignoring period induction substituting it by manual defined values" << endl;
	cerr << "-nc --non-causal    : for running in non-causal mode" << endl;
	cerr << "-a --audio          : play the original sound mixed with the synthesized beats" << endl;
	cerr << "-f --audiofile      : output the original sound mixed with the synthesized beats (as fileName_beats.*)" << endl;
	cerr << "-2b --givefirst2beats : replace induction stage with ground-truth (two first beats from beatTimes file - .txt or .beats - from the directory or file given as argument)" << endl;
	cerr << "-2bs --givefirst2beats_startpoint : equal to givefirst2beats mode but starting tracking at the first given beat time" << endl;
	cerr << "-1b --givefirst1beat : replace initial phase by the given ground-truth first beat (from beatTimes file - .txt or .beats - from the directory or file given as argument)" << endl;
	cerr << "-1bs --givefirst1beat_startpoint : equal to givefirst1beat mode but start tracking at the given phase" << endl;
	cerr << "-pgt --giveinitperiod   : replace initial period given by the ibi of the ground-truth two first beats (from beatTimes file - .txt or .beats - from the directory or file given as argument)" << endl;
	cerr << "-pgt_mr --giveinitperiod+metricalrel : equal to giveinitperiod but complementing it with metrically related tempi (2x, 1/2x, 3x, 1/3x)" << endl;
	cerr << "-pgt_nr --giveinitperiod+nonrel : equal to giveinitperiod but complementing it with non-related tempi" << endl;
	cerr << "-o --output         : what to output (predicted beat times, mean/median tempo): \"beats\", \"medianTempo\", \"meanTempo\", \"beats+medianTempo\", \"beats+meanTempo\", \"beats+meanTempo+medianTempo\" or \"none\"." << endl;
	cerr << "-l --log_file       : generate log file" << endl;
	cerr << "-io --induction_out : output best period (in BPMs) by the end of the induction stage (in the outDir directory)" << endl;
	cerr << "-mic --microphone_input : input sound via microphone interface" << endl;
	cerr << "-send_udp --send_udp : [!!WINDOWS_ONLY!!] send beats - \"beat_flag(tempo)\" - via udp sockets at defined port (in localhost) - for causal mode" << endl;
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
	cmd_options.addBoolOption("logFile", "l", false);
	cmd_options.addBoolOption("noncausal", "nc", false);
	cmd_options.addBoolOption("dumbinduction", "di", false);
	cmd_options.addBoolOption("inductionout", "io", false);
	cmd_options.addBoolOption("microphoneinput", "mic", false);
	cmd_options.addNaturalOption("sendudp", "send_udp", -1);
	cmd_options.addStringOption("givefirst2beats", "2b", "-1");
	cmd_options.addStringOption("givefirst1beat", "1b", "-1");
	cmd_options.addStringOption("giveinitperiod", "pgt", "-1");
	cmd_options.addStringOption("giveinitperiod+metricalrel", "pgt_mr", "-1");
	cmd_options.addStringOption("giveinitperiod+nonrel", "pgt_nr", "-1");
	cmd_options.addStringOption("givefirst2beats_startpoint", "2bs", "-1");
	cmd_options.addStringOption("givefirst1beat_startpoint", "1bs", "-1");
	cmd_options.addStringOption("output", "o", "beats+medianTempo");
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
	noncausalopt = cmd_options.getBoolOption("noncausal");
	sendudp_port = cmd_options.getNaturalOption("sendudp");
	dumbinductionopt = cmd_options.getBoolOption("dumbinduction");
	logfileopt = cmd_options.getBoolOption("logFile");
	inductionoutopt = cmd_options.getBoolOption("inductionout");
	micinputopt = cmd_options.getBoolOption("microphoneinput");
	givefirst2beats = cmd_options.getStringOption("givefirst2beats");
	givefirst2beats_startpoint= cmd_options.getStringOption("givefirst2beats_startpoint");
	givefirst1beat = cmd_options.getStringOption("givefirst1beat");
	givefirst1beat_startpoint = cmd_options.getStringOption("givefirst1beat_startpoint");
	giveinitperiod = cmd_options.getStringOption("giveinitperiod");
	giveinitperiod_metricalrel = cmd_options.getStringOption("giveinitperiod+metricalrel");
	giveinitperiod_nonrel = cmd_options.getStringOption("giveinitperiod+nonrel");
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

mrs_bool
readGTBeatsFile(MarSystem* beattracker, mrs_string gtBeatsFile, mrs_string audioFile, mrs_bool startPoint)
{
	mrs_natural file = (mrs_natural) gtBeatsFile.rfind(".", gtBeatsFile.length()-1);

	mrs_bool readFileOK = true;
	mrs_string line;
	ifstream inStream;
	FileName inputFileGT(audioFile);

	//if gtBeatsFile = directory => add extension (.txt or .beats) to the end of filepath
	if(file == -1)
	{
		ostringstream oss;
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
ibt(mrs_string sfName, mrs_string outputTxt)
{
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
	if(strcmp(givefirst2beats.c_str(), "-1") != 0 || strcmp(givefirst1beat.c_str(), "-1") != 0 || strcmp(giveinitperiod.c_str(), "-1") != 0 ||
		strcmp(giveinitperiod_metricalrel.c_str(), "-1") != 0 || strcmp(giveinitperiod_nonrel.c_str(), "-1") != 0 ||
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
			if(readGTBeatsFile(beattracker, givefirst1beat, sfName, false))
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
		else if(strcmp(giveinitperiod.c_str(), "-1") != 0)
		{
			if(readGTBeatsFile(beattracker, giveinitperiod, sfName, false))
			{
				beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "giveinitperiod");
			}
			else giveinitperiod = "-1";
		}
		else if(strcmp(giveinitperiod_metricalrel.c_str(), "-1") != 0)
		{
			if(readGTBeatsFile(beattracker, giveinitperiod_metricalrel, sfName, false))
			{
				beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "giveinitperiod_metricalrel");
			}
			else giveinitperiod_metricalrel = "-1";
		}
		else if(strcmp(giveinitperiod_nonrel.c_str(), "-1") != 0)
		{
			if(readGTBeatsFile(beattracker, giveinitperiod_nonrel, sfName, false))
			{
				beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "giveinitperiod_nonrel");
			}
			else giveinitperiod_nonrel = "-1";
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
		size_t loc;
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
			cout << "Incorrect annotation output defined - beats+medianTempo files will be created:" << endl;
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
			cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
			beattracker->updControl("BeatTimesSink/sink/mrs_string/mode", "beats+medianTempo");
		}
	}

	if(sendudp_port > 0)
	{
		#ifdef MARSYAS_WIN32 //sockets only available in Windows
			beattracker->updControl("BeatTimesSink/sink/mrs_natural/socketsPort", sendudp_port);
		#else
			cout << "Sockets not available in non-Windows platforms" << endl;
		#endif
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
			cout << "Induction........";
		}

		//Just after induction:
		if(frameCount == inductionTickCount)
		{
			//for playing audio (with clicks on beats):
			cout << "done" << endl;

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

			if(!noncausalopt)
			{
				cout << "Real-Time Beat Tracking........" << endl;
				if(sendudp_port > 0)
					cout << "Sending via UDP Sockets at port " << sendudp_port << endl;
			}
			else
			{
				cout << "Off-Line Beat Tracking........" << endl;
				if(sendudp_port > 0)
					cout << "Sockets not available in non-causal mode!" << endl;
			}
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
		execPath = execPath.substr(0, execPath.rfind('\\')+1);
	#else
		execPath = execPath.substr(0, execPath.rfind('/')+1);
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
		strcmp(outputFile.ext().c_str(), "au") != 0 && strcmp(outputFile.ext().c_str(), "raw") != 0 &&
		!micinputopt)
	{
		if(strcmp(outputFile.ext().c_str(), "mf") == 0 || strcmp(outputFile.ext().c_str(), "txt") == 0)
		{
			ifstream inStream;
			mrs_natural i = 0;
			mrs_string lineFile;
			inStream.open(outputFile.fullname().c_str());
			cout << "InductionTime: " << induction_time << "secs"
				<< "\nScoreFunction: " << score_function << endl;
			//if(metrical_change_time == -1)
			//	cout << "\nMetrical Change: permitted througout all piece" << endl;
			//else
			//	cout << "\nMetrical Change: initial " << metrical_change_time << "secs" << endl;

			cout << "\nInputing Collection " << sfName << "..." << endl;

			while(!inStream.eof())
			{
				getline (inStream, lineFile);
				if(lineFile != "")
				{
					cout << "SoundFile" << i << ": " << lineFile << endl;
					if(existsFile(lineFile))
						ibt(lineFile, outputTxt);
					++i;
				}
			}
		}
		else
			cout << "Unsupported or nonspecified audio format!" << endl;
	}

	else
	{
		cout << "InductionTime: " << induction_time << "secs"
			<< "\nScoreFunction: " << score_function << endl;
		//if(metrical_change_time == -1)
		//	cout << "\nMetrical Change: permitted througout all piece" << endl;
		//else
		//	cout << "\nMetrical Change: initial " << metrical_change_time << "secs" << endl;
		if(micinputopt)
		{
			sfName = "mic";
			cout << "SoundFile: Captured from Microphone" << endl;
			ibt(sfName, outputTxt);
		}

		else
		{
			cout << "SoundFile: " << sfName << endl;
			if(existsFile(sfName))
			{
				ibt(sfName, outputTxt);
			}
			else exit(0);
		}
	}

	cout << "All Done!" << endl;
	exit(0);
}
