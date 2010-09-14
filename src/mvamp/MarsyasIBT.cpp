#include "MarsyasIBT.h"

//#define SCORE_FUNCTION "regular" //the score function (heuristics) which conducts the beat tracking ("regular")
//#define INDUCTION_TIME 5.0 //Time (in seconds) of the beat induction step, before tracking. Has to be > 60/min_bpm (5.0)
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

mrs_string score_function="regular";
//mrs_string output="beats+medianTempo";
mrs_string output="none"; //no output in sonicvisualiser
mrs_string givefirst2beats="-1";
mrs_string givefirst1beat="-1";
mrs_string givefirst2beats_startpoint="-1";
mrs_string givefirst1beat_startpoint="-1";
//mrs_real induction_time = 5.0;
mrs_real metrical_change_time = -1;
mrs_string execPath;
mrs_natural helpopt=false;
mrs_natural usageopt=false;
mrs_bool audioopt=false;
mrs_bool audiofileopt=false;
mrs_bool backtraceopt=false;
mrs_bool logfileopt=false;
mrs_bool noncausalopt=false;
mrs_bool dumbinductionopt=false;
mrs_bool inductionoutopt=false;
mrs_bool micinputopt=false;
mrs_real phase_;

using std::string;
using std::vector;
using std::cerr;
using std::endl;



MarsyasIBT::MarsyasIBT(float inputSampleRate) : Plugin(inputSampleRate), 
	m_stepSize(0), 
	m_previousSample(0.0f),
	induction_time(5.0),
	nr_agents(NR_AGENTS),
	min_bpm(MIN_BPM),
	max_bpm(MAX_BPM),
	output_flag(0)
{
	prevTimestamp = 0.0;
	frameCount = 0; //count number of processed frames
}

MarsyasIBT::~MarsyasIBT()
{
}

string
MarsyasIBT::getIdentifier() const
{
  return "marsyas_ibt";
}

string
MarsyasIBT::getName() const
{
  return "Marsyas - IBT - INESC Beat Tracker";
}

string
MarsyasIBT::getDescription() const
{
  return "Estimate beat locations and tempo causally (only estimates beats after induction)";
}

string
MarsyasIBT::getMaker() const
{
  return "Marsyas Plugins";
}

int
MarsyasIBT::getPluginVersion() const
{
  return 1;
}

string
MarsyasIBT::getCopyright() const
{
	return "Plugin by João Lobato Oliveira, Fabien Gouyon, Luis Gustavo Martins, Luis Paulo Reis. Copyright GPL v3 license";
}

bool
MarsyasIBT::initialise(size_t channels, size_t stepSize, size_t blockSize)
{
  if (channels < getMinChannelCount() ||
	  channels > getMaxChannelCount()) return false;

  m_stepSize = std::min(stepSize, blockSize);

  // Overall extraction and classification network 
  ibt = mng.create("Series", "ibt");

  // Add a realvec as the source
  featureNetwork = mng.create("Series", "featureNetwork");

  // Add a realvec as the source
  featureNetwork->addMarSystem(mng.create("RealvecSource", "src"));

  // Convert the data to mono
  featureNetwork->addMarSystem(mng.create("Stereo2Mono", "m2s"));
	
// Build the overall feature calculation network = audioflow
		//MarSystem* featExtractor = mng.create("Series", "featExtractor");

			beattracker = mng.create("FlowThru","beattracker");
			
				MarSystem* onsetdetectionfunction = mng.create("Series", "onsetdetectionfunction");
					onsetdetectionfunction->addMarSystem(mng.create("ShiftInput", "si")); 
					onsetdetectionfunction->addMarSystem(mng.create("Windowing", "win")); 
					onsetdetectionfunction->addMarSystem(mng.create("Spectrum","spk"));
					onsetdetectionfunction->addMarSystem(mng.create("PowerSpectrum", "pspk"));
					onsetdetectionfunction->addMarSystem(mng.create("Flux", "flux"));
		
			beattracker->addMarSystem(onsetdetectionfunction);
			beattracker->addMarSystem(mng.create("ShiftInput", "acc"));

				MarSystem* normfiltering = mng.create("Series", "normfiltering");
					normfiltering->addMarSystem(mng.create("Filter","filt1"));	
					normfiltering->addMarSystem(mng.create("Reverse","rev1"));
					normfiltering->addMarSystem(mng.create("Filter","filt2"));
					normfiltering->addMarSystem(mng.create("Reverse","rev2"));
		
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
				for(int i = 0; i < (int) nr_agents; i++)
				{
					ostringstream oss;
					oss << "agent" << i;
					agentpool->addMarSystem(mng.create("BeatAgent", oss.str()));
				}

			beattracker->addMarSystem(agentpool);
			beattracker->addMarSystem(mng.create("BeatReferee", "br"));
			beattracker->addMarSystem(mng.create("BeatTimesSink", "sink"));


	// Add the featureNetwork to the main ibt
	featureNetwork->addMarSystem(beattracker);
	//featureNetwork->addMarSystem(featExtractor);
	ibt->addMarSystem(featureNetwork);

	/*
	//======================TEST PARAMETERS - SAVE IN TXT FILE=========================
	ostringstream oss;
	fstream outStream;
	oss << "parameters.txt";
	outStream.open(oss.str().c_str(), ios::out|ios::trunc);
	outStream << "induction_time: " << induction_time << endl;
	outStream << "nr_agents: " << nr_agents << endl;
	outStream << "out: " << output_flag << "; output: " << output << endl;
	outStream << "minbpm: " << min_bpm << endl;
	outStream << "maxbpm: " << max_bpm << endl;
	outStream.close();
	//=====================================================
	*/

  // src has to be configured with hopSize frame length in case a
  // ShiftInput is used in the feature extraction network
  ibt->updctrl("mrs_natural/inSamples", (int)stepSize);

  // Link the "done" control of the input RealvecSource to the "done"
  // control of the ibt
  ibt->linkctrl("mrs_bool/done", "Series/featureNetwork/RealvecSource/src/mrs_bool/done");

	//remaining controls given network (audioflow) blocks with adjusted variables (e.g. blocksize instead of winsize)	
	///////////////////////////////////////////////////////////////////////////////////////
	//link controls
	//////////////////////////////////////////////////////////////////////////////////////

	//Link LookAheadSamples used in PeakerOnset for compensation when retriving the actual initial OnsetTimes
	tempoinduction->linkctrl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_natural/lookAheadSamples", 
		"Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/lookAheadSamples");
	//Link accumulator/ShiftInput size to OnsetTimes for compensating the timing in order to the considered accumulator size
	beattracker->linkctrl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/accSize",
		"ShiftInput/acc/mrs_natural/winSize");

	//Pass hypotheses matrix (from tempoinduction stage) to PhaseLock
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_realvec/beatHypotheses", 
		"FlowThru/tempoinduction/mrs_realvec/innerOut");
	//link backtrace option
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_bool/backtrace", 
		"BeatReferee/br/mrs_bool/backtrace");
	//link corFactor
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/corFactor", 
		"BeatReferee/br/mrs_real/corFactor");
	
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
		"FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/nPeriods");
	
	//PhaseLock nr of Phases per BPM = nr of OnsetTimes considered
	beattracker->linkctrl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/n1stOnsets", 
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
	//Also link period limits from PhaseLock
	for(int i = 0; i < (int) nr_agents; i++)
	{
		ostringstream oss;
		oss << "agent" << i;
		beattracker->linkctrl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_realvec/agentControl", 
			"BeatReferee/br/mrs_realvec/agentControl");
		//and BeatAgent
		beattracker->linkctrl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_natural/minPeriod",
			"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/minPeriod");
		beattracker->linkctrl("Fanout/agentpool/BeatAgent/"+oss.str()+"/mrs_natural/maxPeriod",
			"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/maxPeriod");
	}

	//Defines tempo induction time after which the BeatAgents' hypotheses are populated:
	//PhaseLock timing = induction time
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime", 
		"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime");	
	//BeatReferee timing = induction time
	beattracker->linkctrl("BeatReferee/br/mrs_natural/inductionTime", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/inductionTime");
	//Link Period Limits to PhaseLock
	beattracker->linkctrl("BeatReferee/br/mrs_natural/minPeriod", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/minPeriod");
	beattracker->linkctrl("BeatReferee/br/mrs_natural/maxPeriod", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/maxPeriod");
	//OnsetTimes timing = induction time
	beattracker->linkctrl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/inductionTime", 
		"BeatReferee/br/mrs_natural/inductionTime");

	//Link score function from a BeatAgent (all have the same) to PhaseLock
	//Link also tolerance margins
	beattracker->linkctrl("Fanout/agentpool/BeatAgent/agent0/mrs_string/scoreFunc", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/scoreFunc");
	beattracker->linkctrl("Fanout/agentpool/BeatAgent/agent0/mrs_real/innerMargin", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/innerMargin");
	beattracker->linkctrl("Fanout/agentpool/BeatAgent/agent0/mrs_real/lftOutterMargin", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/lftOutterMargin");
	beattracker->linkctrl("Fanout/agentpool/BeatAgent/agent0/mrs_real/rgtOutterMargin", 
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/rgtOutterMargin");

	//Link BPM conversion parameters to BeatReferee:
	beattracker->linkctrl("BeatReferee/br/mrs_natural/hopSize", "mrs_natural/inSamples");

	//Link BPM conversion parameters to TempoHypotheses
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/hopSize", 
		"BeatReferee/br/mrs_natural/hopSize");
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs", 
		"BeatReferee/br/mrs_real/srcFs");

	//Link TickCounter from BeatRefree -> for updating IBT's timer
	beattracker->linkctrl("FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/tickCount",
		"BeatReferee/br/mrs_natural/tickCount");
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/tickCount",	
		"FlowThru/tempoinduction/Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/tickCount");
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/tickCount", 
		"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/tickCount");	
	
	//link frames2seconds adjustment from BeatTimesSink to PhaseLock
	beattracker->linkctrl("BeatTimesSink/sink/mrs_natural/adjustment",
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/adjustment");
	//link dumbinduction to PhaseLock
	beattracker->linkctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_bool/dumbInduction",
		"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_bool/dumbInduction");

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
	

	//link sampling rate and hopsize for BPM conversion (in PhaseLock)
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/hopSize", 
			"BeatReferee/br/mrs_natural/hopSize");
	beattracker->linkctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/srcFs", 
			"FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs");

	//For Induction Ground-Truth in "PhaseLock"
	/*
	if(strcmp(givefirst2beats.c_str(), "-1") != 0 || strcmp(givefirst1beat.c_str(), "-1") != 0 || 
		strcmp(givefirst2beats_startpoint.c_str(), "-1") != 0 || strcmp(givefirst1beat_startpoint.c_str(), "-1") != 0)
	{
		//if normal induction_gt:
		if(strcmp(givefirst2beats.c_str(), "-1") != 0)
		{
			if(readGTBeatsFile(beattracker, givefirst2beats, sfName, false))
				beattracker->updctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "givefirst2beats");
			else givefirst2beats = "-1";
		}
		//if induction_gt starting tracking at given first beat
		else if(strcmp(givefirst2beats_startpoint.c_str(), "-1") != 0)
		{
			if(readGTBeatsFile(beattracker, givefirst2beats_startpoint, sfName, true))
			{
				beattracker->updctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "givefirst2beats_startpoint");
				backtraceopt = true;
			}
			else givefirst2beats_startpoint = "-1";
		}
		else if(strcmp(givefirst1beat.c_str(), "-1") != 0)
		{
			if(readGTBeatsFile(beattracker, givefirst1beat, sfName, true))
			{
				beattracker->updctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "givefirst1beat");
			}
			else givefirst1beat = "-1";
		}
		else if(strcmp(givefirst1beat_startpoint.c_str(), "-1") != 0)
		{
			if(readGTBeatsFile(beattracker, givefirst1beat_startpoint, sfName, true))
			{
				beattracker->updctrl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/mode", "givefirst1beat_startpoint");
				backtraceopt = true;
			}
			else givefirst1beat_startpoint = "-1";
		}
	}
	*/

	//if requested output of induction best period hypothesis link output directory
	if(inductionoutopt)
		beattracker->linkctrl("BeatTimesSink/sink/mrs_string/destFileName", 
			"FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_string/inductionOut");

	//link Filters coeffs
	beattracker->linkctrl("Series/normfiltering/Filter/filt2/mrs_realvec/ncoeffs",
					   "Series/normfiltering/Filter/filt1/mrs_realvec/ncoeffs");
	beattracker->linkctrl("Series/normfiltering/Filter/filt2/mrs_realvec/dcoeffs",
					   "Series/normfiltering/Filter/filt1/mrs_realvec/dcoeffs");

	//link bestFinalAgentHistory from BeatReferee to BeatTimesSink
	beattracker->linkctrl("BeatTimesSink/sink/mrs_realvec/bestFinalAgentHistory", 
		"BeatReferee/br/mrs_realvec/bestFinalAgentHistory");
	//link inputSize for knowing when musical piece finishes
	beattracker->linkctrl("BeatTimesSink/sink/mrs_natural/soundFileSize", 
		"BeatReferee/br/mrs_natural/soundFileSize");
	
	//link non-causal mode flag
	beattracker->linkctrl("BeatTimesSink/sink/mrs_bool/nonCausal", 
		"BeatReferee/br/mrs_bool/nonCausal");


	///////////////////////////////////////////////////////////////////////////////////////
	// update controls                                                                   //
	///////////////////////////////////////////////////////////////////////////////////////

	
	//best result till now are using dB power Spectrum!
	beattracker->updctrl("Series/onsetdetectionfunction/PowerSpectrum/pspk/mrs_string/spectrumType", "magnitude");

	beattracker->updctrl("Series/onsetdetectionfunction/Flux/flux/mrs_string/mode", "DixonDAFX06");

	beattracker->updctrl("mrs_natural/inSamples", (int)stepSize);
	beattracker->updctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize", (int)blockSize);


	//mrs_real fsSrc = beattracker->getctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_real/israte")->to<mrs_real>();
	mrs_real fsSrc = m_inputSampleRate;
	//HARD-CODED INPUT-SIZE -> give great value in beggining and update true value by the end of the analysis (in getRemainingFeatures())
	mrs_natural inputLength = 20000; //(in seconds)
	mrs_natural inputSize = inputLength * fsSrc;
	//cout << "fsSrc: " << fsSrc << endl;

	//induction time (in nr. of ticks) -> -1 because starting index on accumulator is 0 and it finnishes at accSize-1
	//So IBT's tick time notion starts also on 0 and finnishes on sound_file_size(in_frames)-1.
	mrs_natural adjustment = (int)stepSize / 2; //linked to BeatTimesSink
	//adjustment = (winSize_ - hopSize_) + floor((mrs_real) winSize_/2);
	
	mrs_natural inductionTickCount = ((mrs_natural) ceil((induction_time * fsSrc + adjustment) / (int)stepSize)) -1;

	//to avoid induction time greater than input file size
	//(in this case the induction time will equal the file size)
	if((inputSize / (int)stepSize) < inductionTickCount)
		inductionTickCount = (inputSize / (int)stepSize) -1;

	beattracker->updctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime", inductionTickCount);
	beattracker->updctrl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_bool/dumbInduction", dumbinductionopt);

	mrs_natural metricalChangeTime = inputSize / (int)stepSize; //if metricalChangeTime = -1 it shall equalize the full input file size (in ticks)
	if(metrical_change_time != -1.0 && metrical_change_time >= 0.0)
		metricalChangeTime = ((mrs_natural) (metrical_change_time * fsSrc) / (int)stepSize) + 1; //allowed metrical change time (in nr. of ticks)
	
	//Size of accumulator equals inductionTime + 1 -> [0, inductionTime]
	//beattracker->updctrl("ShiftInput/acc/mrs_natural/winSize", inductionTickCount+1);
	//Size of accumulator equals two times the inductionTime for better filtering (must be bigger than inductionTime)
	//(and to avoid strange slow memory behaviour with inductionTime=5)

	mrs_natural accSize = 2*inductionTickCount;
	if(accSize > (inputSize / (int)stepSize)) //to avoid having an accumulator greater then the file size
		accSize = (inputSize / (int)stepSize) +1; 

	beattracker->updctrl("ShiftInput/acc/mrs_natural/winSize", accSize);

	mrs_natural pkinS = tempoinduction->getctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
	mrs_natural peakEnd = (mrs_natural)((60.0 * fsSrc)/(min_bpm * (int)stepSize)); //MinBPM (in frames)
	mrs_natural peakStart = (mrs_natural) ((60.0 * fsSrc)/(max_bpm * (int)stepSize));  //MaxBPM (in frames)
	
	//mrs_real peakSpacing = ceil(((peakEnd-peakStart) * 4.0) / ((mrs_real)(max_bpm-min_bpm))) / pkinS;
	
	mrs_real peakSpacing = ((mrs_natural) (fsSrc/(int)stepSize) * (1.0-(60.0/64.0))) / (pkinS * 1.0); //spacing between peaks (4BPMs at 60BPM resolution)
	//mrs_real peakSpacing = ((mrs_natural) (peakEnd-peakStart) / (2*BPM_HYPOTHESES)) / (pkinS * 1.0);  //nrPeaks <= 2*nrBPMs

	//cout << "PkinS: " << pkinS << "; peakEnd: " << peakEnd << "; peakStart: " << peakStart << "; peakSpacing: " << peakSpacing << endl;

	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakSpacing", peakSpacing);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakStrength", 0.75); //0.75
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakStart", peakStart);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/peakEnd", peakEnd);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_real/peakGain", 2.0);
	
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/tempo/MaxArgMax/mxr/mrs_natural/nMaximums", BPM_HYPOTHESES);

	mrs_natural lookAheadSamples = 6; //(higher than 3 due to unconsistencies on the very beginning of the filter window)
	mrs_real thres = 1.2;//1.3; //1.75 (1.2)

	tempoinduction->updctrl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_natural/lookAheadSamples", lookAheadSamples);
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_real/threshold", thres);
	
	tempoinduction->updctrl("Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/n1stOnsets", PHASE_HYPOTHESES);
	
	//Pass chosen score_function to each BeatAgent in the pool:
	for(int i = 0; i < (int) nr_agents; i++)
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

	mrs_natural minPeriod = (mrs_natural) floor(60.0 / (max_bpm * (int)stepSize) * fsSrc);
	mrs_natural maxPeriod = (mrs_natural) ceil(60.0 / (min_bpm * (int)stepSize) * fsSrc);

	beattracker->updctrl("BeatReferee/br/mrs_real/srcFs", fsSrc);
	beattracker->updctrl("BeatReferee/br/mrs_natural/minPeriod", minPeriod);
	beattracker->updctrl("BeatReferee/br/mrs_natural/maxPeriod", maxPeriod);
	beattracker->updctrl("BeatReferee/br/mrs_real/obsoleteFactor", OBSOLETE_FACTOR);
	beattracker->updctrl("BeatReferee/br/mrs_natural/lostFactor", LOST_FACTOR);
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
	beattracker->updctrl("BeatReferee/br/mrs_bool/logFile", logfileopt);
	beattracker->updctrl("BeatReferee/br/mrs_natural/soundFileSize", (mrs_natural) ((inputSize / (int)stepSize)));
	beattracker->updctrl("BeatReferee/br/mrs_bool/nonCausal", noncausalopt);

	mrs_string sfName = "BeatOutput.wav"; //HARD-CODED!!!
	ostringstream path;
	FileName outputFile(sfName);
	mrs_string outputTxt = ""; //HARD-CODED!!!
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
		beattracker->updctrl("BeatReferee/br/mrs_string/logFileName", path.str() + "_log.txt");

	//adjustment used in TickCount calculation above
	beattracker->updctrl("BeatTimesSink/sink/mrs_natural/adjustment", adjustment);
	if(strcmp(output.c_str(), "none") != 0)
	{
		beattracker->updctrl("BeatTimesSink/sink/mrs_string/destFileName", path.str());
		
		if(strcmp(output.c_str(), "beats") == 0)
		{
			beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "beatTimes");
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
		}
		else if(strcmp(output.c_str(), "medianTempo") == 0)
		{
			beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "medianTempo");
			cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
		}
		else if(strcmp(output.c_str(), "meanTempo") == 0)
		{
			beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "meanTempo");
			cout << "MeanTempo Output: " << path.str() << "_meanTempo.txt" << endl;
		}
		else if(strcmp(output.c_str(), "beats+medianTempo") == 0)
		{
			beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "beats+medianTempo");
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
			cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
		}
		else if(strcmp(output.c_str(), "beats+meanTempo") == 0)
		{
			beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "beats+meanTempo");
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
			cout << "MeanTempo Output: " << path.str() << "_meanTempo.txt" << endl;
		}
		else if(strcmp(output.c_str(), "beats+meanTempo+medianTempo") == 0)
		{
			beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "beats+meanTempo+medianTempo");
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
			cout << "MeanTempo Output: " << path.str() << "_meanTempo.txt" << endl;
		}
		else
		{
			cout << "Incorrect annotation output defined - beats+tempo files will be created:" << endl;
			cout << "BeatTimes Output: " << path.str() << ".txt" << endl;
			cout << "MedianTempo Output: " << path.str() << "_medianTempo.txt" << endl;
			beattracker->updctrl("BeatTimesSink/sink/mrs_string/mode", "beats+tempo");
		}
	}
	

	//Coefficients taken from MATLAB butter(2, 0.18)
	realvec bcoeffs(1,3);
	bcoeffs(0) = 0.0564;
	bcoeffs(1) = 0.1129;
	bcoeffs(2) = 0.0564;
	beattracker->updctrl("Series/normfiltering/Filter/filt1/mrs_realvec/ncoeffs", bcoeffs);
	realvec acoeffs(1,3);
	acoeffs(0) = 1.0000;
	acoeffs(1) = -1.2247;
	acoeffs(2) = 0.4504;
	beattracker->updctrl("Series/normfiltering/Filter/filt1/mrs_realvec/dcoeffs", acoeffs);
	
	//cout << "Ind: " << inductionTickCount << "; fsSrc: " << fsSrc << "; adj: " << adjustment << "; hop: " << (int)stepSize << endl;
	//cout << "Win: " << blockSize << "; SI_InS: " << beattracker->getctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/inSamples")->to<mrs_natural>() << endl;
	//cout << "ACC: " << beattracker->getctrl("ShiftInput/acc/mrs_natural/winSize")->to<mrs_natural>() << endl;

	return true;
}

void
MarsyasIBT::reset()
{
  m_previousSample = 0.0f;
}

size_t
MarsyasIBT::getPreferredStepSize() const
{
    return HOPSIZE; //stepsize = hopsize
}

size_t
MarsyasIBT::getPreferredBlockSize() const
{
    return WINSIZE; //blocksize = winsize
}

//define user parameters
MarsyasIBT::ParameterList
MarsyasIBT::getParameterDescriptors() const
{
	ParameterList list;

	//Induction time:
	ParameterDescriptor desc;
	desc.identifier = "indtime"; //nr of agents
    desc.name = "Initial Induction Time";
    desc.description = "Time of Initial Induction Step";
	desc.minValue = (float) (60.0 / min_bpm);
    desc.maxValue = 60.0;
	desc.defaultValue = 5.0;
	desc.unit = "s";
    desc.isQuantized = false;
	list.push_back(desc);

	//Nr of agents:
	desc = ParameterDescriptor();
	desc.identifier = "nragents"; //nr of agents
    desc.name = "Number of Agents";
    desc.description = "Maximum amount of BeatAgents";
    desc.minValue = 1;
    desc.maxValue = 50;
	desc.defaultValue = NR_AGENTS;
    desc.isQuantized = true;
	desc.quantizeStep = 1;
    list.push_back(desc);

	//Minimum Tempo:
	desc = ParameterDescriptor();
	desc.identifier = "minbpm";
    desc.name = "Minimum Allowed Tempo (BPM)";
    desc.description = "Minimum Allowed Tempo (BPM)";
    desc.minValue = 1;
    desc.maxValue = max_bpm-1; //can't surpass max tempo
	desc.defaultValue = MIN_BPM;
    desc.isQuantized = true;
	desc.quantizeStep = 1;
    list.push_back(desc);

	//Maximum Tempo:
	desc = ParameterDescriptor();
	desc.identifier = "maxbpm";
    desc.name = "Maximum Allowed Tempo (BPM)";
    desc.description = "Maximum Allowed Tempo (BPM)";
    desc.minValue = min_bpm+1; //can't surpass min tempo
    desc.maxValue = 400; 
	desc.defaultValue = MAX_BPM;
    desc.isQuantized = true;
	desc.quantizeStep = 1;
    list.push_back(desc);

	//Output txt files:
	desc = ParameterDescriptor();
	desc.identifier = "output"; //nr of agents
    desc.name = "Output Beat and (Median) Tempo Annotations";
    desc.description = "Output Beat and (Median) Tempo Annotations";
    desc.minValue = 0;
    desc.maxValue = 1;
	desc.defaultValue = 0;
    desc.isQuantized = true;
	desc.quantizeStep = 1;
    list.push_back(desc);

	return list;
}

//get user parameters' values
float
MarsyasIBT::getParameter(std::string name) const
{
	if (name == "indtime") {
		return induction_time;
	}
    else if (name == "nragents") {
        return nr_agents;
    }
	else if (name == "minbpm") {
        return min_bpm;
	}
	else if (name == "maxbpm") {
        return max_bpm;
	}
	else if (name == "output") {
		return output_flag ? 1.0 : 0.0;
    }
    return 0.0;
}

//set user paramteres
void
MarsyasIBT::setParameter(std::string name, float value)
{
	if (name == "indtime") {
		induction_time = value;
	}
	else if (name == "nragents") {
        nr_agents = value;
    }
	else if (name == "minbpm") {
        min_bpm = value;
    }
	else if (name == "maxbpm") {
        max_bpm = value;
    }
	else if (name == "output") {
		output_flag = (int)(value+0.5);
		switch(output_flag)
		{
			case 0: output = "none"; break;
			case 1: output = "beats+medianTempo"; break;
		}
    }
}

MarsyasIBT::OutputList
MarsyasIBT::getOutputDescriptors() const
{
  OutputList list;

  OutputDescriptor output;
  output.identifier = "Beat Positions";
  output.name = "IBT";
  output.description = "Beat Positions";
 
  //for outputing time instants
  output.unit = "s";
  output.hasFixedBinCount = true;
  output.binCount = 0;
  output.sampleType = OutputDescriptor::OneSamplePerStep;//::VariableSampleRate;  
  output.sampleRate = (m_inputSampleRate / m_stepSize);
  

 /* for outputing values
  output.hasFixedBinCount = true;
  output.binCount = 1;
  output.hasKnownExtents = false;
  output.isQuantized = false;
  output.sampleType = OutputDescriptor::OneSamplePerStep;
  */
  
  list.push_back(output);

  return list;
}

MarsyasIBT::FeatureSet
MarsyasIBT::process(const float *const *inputBuffers,Vamp::RealTime timestamp)
{
  if (m_stepSize == 0) {
	cerr << "ERROR: MarsyasIBT::process: "
	     << "MarsyasIBT not been initialised"
	     << endl;
	return FeatureSet();
  }
  
  // Stuff inputBuffers into a realvec
  realvec r(m_stepSize);
  for (size_t i = 0; i < m_stepSize; ++i)
	  r(i) = inputBuffers[0][i];

  // Load the network with the data
  featureNetwork->updControl("RealvecSource/src/mrs_realvec/data", r);

  // Tick the network once, which will process one window of data
  ibt->tick();

  // Get the data out of the network
  //realvec output_realvec = featureNetwork->getControl("mrs_realvec/processedData")->to<mrs_realvec>();
  realvec output_realvec = beattracker->getControl("mrs_realvec/innerOut")->to<mrs_realvec>();
  
  // The feature we are going to return to the host
  FeatureSet returnFeatures;
  if(output_realvec(0) == 1.0) // beats are output 1
  {
	  //cout << "BEAT at: " << timestamp << endl;
	  Feature feature;
	  feature.hasTimestamp = true;
	  feature.timestamp = timestamp; //-hopSize (timestamp is hopSize ahead of real timing) -> TODO

	  stamp = strtod(timestamp.toString().c_str(), NULL); 
	  ibi = stamp - prevTimestamp;
	  prevTimestamp = stamp;
	  
	  float stampString = (60.0 / ibi); // (convert to BPMs)

	  char label[20];
	  sprintf(label, "%f", stampString);
	  feature.label = label; //tempo (BPM) label

	  //cout << "Tempo: " << stampString << "; Label: " << label << endl;
	  
	  returnFeatures[0].push_back(feature);
  }

  frameCount ++; //count for getting soundfile size to use in non-causal mode
  return returnFeatures; //CAUSAL OUTPUT (frame by frame)
}

//FOR NON-CAUSAL OUTPUT!!! - TODO:
//1- solve backtrace issue
//2- solve non-causal output
MarsyasIBT::FeatureSet
MarsyasIBT::getRemainingFeatures()
{
	if(noncausalopt)
	{
		//cout << "frameCount: " << frameCount << "; time: " << (frameCount * (m_stepSize / m_inputSampleRate)) << endl;
		beattracker->updControl("BeatReferee/br/mrs_natural/soundFileSize", frameCount); //for opearting in non-causal mode

		ibt->tick();
	}

	return FeatureSet();
}

