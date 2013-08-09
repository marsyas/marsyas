#include "IBT.h"
#define INDUCTION_TIME 5.0 //Nr. of initial BPM hypotheses (must be <= than the nr. of agents) (6)
#define BPM_HYPOTHESES 6 //Nr. of initial BPM hypotheses (must be <= than the nr. of agents) (6)
#define PHASE_HYPOTHESES 30//Nr. of phases per BPM hypothesis (30)
#define MIN_BPM 81 //minimum tempo considered, in BPMs (50) [80 -> to prevent octave error]
#define MAX_BPM 160 //maximum tempo considered, in BPMs (250) [160 -> to prevent octave error]
#define NR_AGENTS 30 //Nr. of agents in the pool (30)
#define LFT_OUTTER_MARGIN 0.20 //(Inertia1.1) The size of the outer half-window (in % of the IBI) before the predicted beat time (0.20)
#define RGT_OUTTER_MARGIN 0.40 //(Inertia1.2) The size of the outer half-window (in % of the IBI) after the predicted beat time (0.30)
#define INNER_MARGIN 4.0 //(Inertia1.3) Inner tolerance window margin size (= half inner window size -> in ticks) (4.0)
#define OBSOLETE_FACTOR 0.8 //An agent is killed if, at any time (after the initial Xsecs-defined in BeatReferee), the difference between its score and the bestScore is below OBSOLETE_FACTOR * bestScore (0.8)
#define LOST_FACTOR 8 //An agent is killed if it become lost, i.e. if it found LOST_FACTOR consecutive beat predictions outside its inner tolerance window (8)
#define CHILDREN_SCORE_FACTOR 0.9 //(Inertia2) Each created agent imports its father score multiplied (or divided if negative) by this factor (0.8)
#define BEST_FACTOR 1.0 //(Inertia3) Mutiple of the bestScore an agent's score must have for replacing the current best agent (1.0)
#define CORRECTION_FACTOR 0.25 //(Inertia4) correction factor for compensating each agents' own {phase, period} hypothesis errors (0.25)
#define EQ_PERIOD 1 //Period threshold which identifies two agents as predicting the same period (IBI, in ticks) (1)
#define EQ_PHASE 2 //Phase threshold which identifies two agents as predicting the same phase (phase, in ticks) (2)
#define CHILD1_FACTOR 1.0 //Correction factor (error proportion-[0.0-1.0]) for compensating its father's {phase, period} hypothesis - used by child1 (2.0 - only full phase adjustment; -1 - no child considered) (1.0)
#define CHILD2_FACTOR 2.0 //Correction factor (error proportion-[0.0-1.0]) for compensating its father's {phase, period} hypothesis - used by child2 (2.0 - only full phase adjustment; -1 - no child considered) (2.0)
#define CHILD3_FACTOR 0.5 //Correction factor (error proportion-[0.0-1.0]) for compensating its father's {phase, period} hypothesis - used by child3 (2.0 - only full phase adjustment; -1 - no child considered) (0.5)
#define TRIGGER_GT_TOL 5 //Number of miss computed beats, in comparison to ground-truth beat-times, tolerated before triggering new induction (used in trigger "groundtruth" mode) -> can be defined via -tigt_tol 
#define TRIGGER_BEST_FACTOR 1.0 //Proportion of the current best agent score which is inherited by the agents created at each triggered induction [shouldn't be much higher than 1, for not inflating scores two much] (1.0)
#define SUPERVISED_TRIGGER_THRES 0.0 //Degree (in percentage) of mean bestScore decrease to trigger a new induction in supervised induction mode (0.03)
#define BEAT_TRANSITION_TOL 0.6 //Tolerance for handling beats at transitions between agents [-1 for unconsider it]: (0.6)
//In causal mode, if between two consecutive beats there is over a BEAT_TRANSITION_TOL decrease in current IBI the second beat is unconsidered;
//In non-causal mode, if between a son's first beat and its father's last there is over a BEAT_TRANSITION_TOL descrease on the father last IBI the son's first beat is unconsidered;
//In non-causal mode, if between a son's first beat and its father's last there is over a BEAT_TRANSITION_TOL increase on the father last IBI the son's first beat shall be its father's next beat, and the second beat shall be its assigned first.

#define WINSIZE 1024 //(2048?)
#define HOPSIZE 512 //(512)

using namespace Marsyas;
using namespace std;

mrs_string sfName="BeatOutput.wav"; //HARD-CODED!!!
mrs_string groundtruth_file="-1"; //for groundtruth induction
mrs_string groundtruth_induction="-1"; //for groundtruth induction
mrs_string induction_mode="-1"; //for trigger induction
mrs_natural triggergt_tol = 5; //for trigger induction
mrs_string score_function="regular";
mrs_string output="beats+medianTempo";
//mrs_string output="none"; //no output by default in sonicvisualiser
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
//mrs_string logfileopt="seconds";
mrs_string logfileopt="-1";
mrs_bool noncausalopt=false;
mrs_bool dumbinductionopt=false;
mrs_bool inductionoutopt=false;
mrs_bool micinputopt=false;
mrs_real phase_;
mrs_natural minBPM_;
mrs_natural maxBPM_;
mrs_bool avoid_metrical_changes = false;
mrs_natural nr_agents = NR_AGENTS;
mrs_natural min_bpm = MIN_BPM;
mrs_natural max_bpm = MAX_BPM;
mrs_real induction_time = INDUCTION_TIME;

//Default Parameters
mrs_natural stepSize = HOPSIZE;
mrs_natural blockSize = WINSIZE;
mrs_real m_inputSampleRate = 44100.0;

MarMaxIBT::MarMaxIBT(mrs_natural winSize, mrs_natural hopSize, mrs_real fs, mrs_real indTime, mrs_natural minBPM, mrs_natural maxBPM, mrs_string outPathName, mrs_bool stateRecovery)
{
  blockSize = winSize;
  stepSize = hopSize;
  m_inputSampleRate = fs;
  min_bpm = minBPM;
  max_bpm = maxBPM;
  induction_time = indTime;
  sfName = outPathName;

  if(strcmp(sfName.c_str(), "") == 0) output="none"; //no output if none define in Max command line
  else output="beats+medianTempo";

  if(stateRecovery) induction_mode = "supervised";
  else induction_mode = "-1";

  //cout << "winSize: " << blockSize << "; stepSize: " << hopSize << "; fs: " << m_inputSampleRate
  //	<< "; minBPM: " << min_bpm << "; maxBPM: " << max_bpm << "; indTime: " << induction_time << "; outPathName: " << sfName
  //	<< "; stateRecovery? " << stateRecovery << "; output: " << output << endl;
}

MarMaxIBT::~MarMaxIBT() {}

MarSystem* MarMaxIBT::createMarsyasNet()
{
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
  ibt->updControl("mrs_natural/inSamples", (int)stepSize);

  // Link the "done" control of the input RealvecSource to the "done"
  // control of the ibt
  ibt->linkControl("mrs_bool/done", "Series/featureNetwork/RealvecSource/src/mrs_bool/done");

  //remaining controls given network (audioflow) blocks with adjusted variables (e.g. blocksize instead of winsize)
  ///////////////////////////////////////////////////////////////////////////////////////
  //link controls
  //////////////////////////////////////////////////////////////////////////////////////

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
  //if(strcmp(output.c_str(), "none") != 0)
  //{
  beattracker->linkControl("BeatTimesSink/sink/mrs_natural/hopSize",
                           "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/hopSize");
  beattracker->linkControl("BeatTimesSink/sink/mrs_real/srcFs",
                           "FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_real/srcFs");
  beattracker->linkControl("BeatTimesSink/sink/mrs_natural/winSize",
                           "Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize");
  beattracker->linkControl("BeatTimesSink/sink/mrs_natural/tickCount",
                           "FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_natural/tickCount");
  //}


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
          readGTFileOk = false; //HARD-CODED
        //readGTFileOk = readGTBeatsFile(beattracker, groundtruth_file, sfName, true);
        else readGTFileOk = false; //HARD-CODED
        //readGTFileOk = readGTBeatsFile(beattracker, groundtruth_file, sfName, false);

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
    if(!strcmp(induction_mode.c_str(), "single") == 0 && !strcmp(induction_mode.c_str(), "repeated") == 0
        && !strcmp(induction_mode.c_str(), "random") == 0 && !strcmp(induction_mode.c_str(), "supervised") == 0
        && !strcmp(induction_mode.c_str(), "givetransitions") == 0)
    {
      cerr << "Trigger Induction: re-define trigger mode value as one of the following: \"single\", \"repeated\", \"random\", \"supervised\", \"givetransitions\" -> \"single\" assumed." << endl;
      induction_mode = "single";
    }
    else if(strcmp(induction_mode.c_str(), "repeated") == 0 || strcmp(induction_mode.c_str(), "random") == 0
            || strcmp(induction_mode.c_str(), "supervised") == 0 || strcmp(induction_mode.c_str(), "givetransitions") == 0)
    {
      avoid_metrical_changes = true; //always run repeated induction modes with metrical change avoidance
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

  //link non-causal mode flag
  beattracker->linkControl("BeatTimesSink/sink/mrs_bool/nonCausal",
                           "BeatReferee/br/mrs_bool/nonCausal");

  if(avoid_metrical_changes)
  {
    minBPM_ = 81;
    maxBPM_ = 160;
    cerr << "Avoid metrical changes mode activated (default in causal operation)." << endl;
  }
  else
  {
    minBPM_ = min_bpm;
    maxBPM_ = max_bpm;
  }
  cerr << "Considered tempo in the range [" << minBPM_ << "-" << maxBPM_ << "]BPM." << endl;


  ///////////////////////////////////////////////////////////////////////////////////////
  // update controls                                                                   //
  ///////////////////////////////////////////////////////////////////////////////////////


  //best result till now are using dB power Spectrum!
  if(noncausalopt)
    backtraceopt = true;

  //best result till now are using dB power Spectrum!
  beattracker->updControl("Series/onsetdetectionfunction/PowerSpectrum/pspk/mrs_string/spectrumType", "magnitude");

  beattracker->updControl("Series/onsetdetectionfunction/Flux/flux/mrs_string/mode", "DixonDAFX06");

  beattracker->updControl("mrs_natural/inSamples", (int)stepSize);
  beattracker->updControl("Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/winSize", (int)blockSize);


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

  inductionTickCount = ((mrs_natural) ceil((induction_time * fsSrc + adjustment) / (int)stepSize)) -1;

  //to avoid induction time greater than input file size
  //(in this case the induction time will equal the file size)
  if((inputSize / (int)stepSize) < inductionTickCount)
    inductionTickCount = (inputSize / (int)stepSize) -1;

  beattracker->updControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_natural/inductionTime", inductionTickCount);
  beattracker->updControl("FlowThru/tempoinduction/TempoHypotheses/tempohyp/mrs_bool/dumbInduction", dumbinductionopt);

  mrs_natural metricalChangeTime = inputSize / (int)stepSize; //if metricalChangeTime = -1 it shall equalize the full input file size (in ticks)
  if(metrical_change_time != -1.0 && metrical_change_time >= 0.0)
    metricalChangeTime = ((mrs_natural) (metrical_change_time * fsSrc) / (int)stepSize) + 1; //allowed metrical change time (in nr. of ticks)

  //Size of accumulator equals inductionTime + 1 -> [0, inductionTime]
  //beattracker->updControl("ShiftInput/acc/mrs_natural/winSize", inductionTickCount+1);
  //Size of accumulator equals two times the inductionTime for better filtering (must be bigger than inductionTime)
  //(and to avoid strange slow memory behaviour with inductionTime=5)

  mrs_natural accSize = 2*inductionTickCount;
  if(accSize > (inputSize / (int)stepSize)) //to avoid having an accumulator greater then the file size
    accSize = (inputSize / (int)stepSize) +1;

  beattracker->updControl("ShiftInput/acc/mrs_natural/winSize", accSize);

  mrs_natural pkinS = tempoinduction->getctrl("Fanout/tempohypotheses/Series/tempo/Peaker/pkr/mrs_natural/onSamples")->to<mrs_natural>();
  mrs_natural peakEnd = (mrs_natural)((60.0 * fsSrc)/(minBPM_ * (int)stepSize)); //MinBPM (in frames)
  mrs_natural peakStart = (mrs_natural) ((60.0 * fsSrc)/(maxBPM_ * (int)stepSize));  //MaxBPM (in frames)
  mrs_real peakSpacing = ((mrs_natural) (fsSrc/(int)stepSize) * (1.0-(60.0/64.0))) / (pkinS * 1.0); //spacing between peaks (4BPMs at 60BPM resolution)
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

  mrs_natural lookAheadSamples = 20; //(higher than 3 due to unconsistencies on the very beginning of the filter window)
  mrs_real thres = 1.1;//1.3; //1.75 (1.1)

  tempoinduction->updControl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_natural/lookAheadSamples", lookAheadSamples);
  tempoinduction->updControl("Fanout/tempohypotheses/Series/phase/PeakerOnset/pkronset/mrs_real/threshold", thres);

  tempoinduction->updControl("Fanout/tempohypotheses/Series/phase/OnsetTimes/onsettimes/mrs_natural/n1stOnsets", PHASE_HYPOTHESES);

  //Pass chosen score_function to each BeatAgent in the pool:
  for(int i = 0; i < (int) nr_agents; i++)
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

  mrs_natural minPeriod = (mrs_natural) floor(60.0 / (maxBPM_ * (int)stepSize) * fsSrc);
  mrs_natural maxPeriod = (mrs_natural) ceil(60.0 / (minBPM_ * (int)stepSize) * fsSrc);

  beattracker->updControl("FlowThru/initialhypotheses/PhaseLock/phaselock/mrs_real/triggerBestScoreFactor", TRIGGER_BEST_FACTOR);

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
  beattracker->updControl("BeatReferee/br/mrs_string/inductionMode", induction_mode);
  beattracker->updControl("BeatReferee/br/mrs_natural/triggerGtTolerance", triggergt_tol);
  beattracker->updControl("BeatReferee/br/mrs_real/beatTransitionTol", BEAT_TRANSITION_TOL);
  beattracker->updControl("BeatReferee/br/mrs_real/supervisedTriggerThres", SUPERVISED_TRIGGER_THRES);
  //reset in state-recovery causal mode and recover in state-recovery non-causal
  if(noncausalopt) beattracker->updControl("BeatReferee/br/mrs_bool/resetAfterNewInduction", false);
  else beattracker->updControl("BeatReferee/br/mrs_bool/resetAfterNewInduction", true);

  ostringstream path;
  FileName outputFile(sfName);
  mrs_string outputTxt = "";
  //if no outputTxt dir defined -> exec dir is assumed:
  if(strcmp(outputTxt.c_str(), "") == 0)
  {
    if(strcmp(outputFile.nameNoExt().c_str(), "") == 0)
      path << outputTxt << outputFile.path() << "BeatOutput";
    else
      path << outputTxt << outputFile.path() << outputFile.nameNoExt();
  }
  else
  {
    path.str("");
    mrs_natural loc;
    loc = outputTxt.rfind(".txt", outputTxt.length()-1);

    if(loc == -1) //if only output dir defined -> append filename:
      path << outputTxt << outputFile.path() << "BeatOutput";
    else
    {
      outputTxt = outputTxt.substr(0, loc);
      path << outputTxt;
    }
  }

  if(!strcmp(logfileopt.c_str(), "-1") == 0)
  {
    if(!strcmp(logfileopt.c_str(), "trigger") == 0)
    {
      if(!strcmp(logfileopt.c_str(), "frames") == 0 && !strcmp(logfileopt.c_str(), "seconds") == 0
          && !strcmp(logfileopt.c_str(), "frames+seconds") == 0)
      {
        cerr << "LogFile: Unknown time units specified... \"frames+seconds\" considered." << endl;
        logfileopt = "frames+seconds";
      }
    }

    beattracker->updControl("BeatReferee/br/mrs_string/logFile", logfileopt);
    beattracker->updControl("BeatReferee/br/mrs_string/logFileName", path.str() + "_log.txt");
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


  //Coefficients taken from MATLAB butter(2, 0.18)
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

  //cout << "Ind: " << inductionTickCount << "; fsSrc: " << fsSrc << "; adj: " << adjustment << "; hop: " << (int)stepSize << endl;
  //cout << "Win: " << blockSize << "; SI_InS: " << beattracker->getctrl("Series/onsetdetectionfunction/ShiftInput/si/mrs_natural/inSamples")->to<mrs_natural>() << endl;
  //cout << "ACC: " << beattracker->getctrl("ShiftInput/acc/mrs_natural/winSize")->to<mrs_natural>() << endl;

  return ibt;
}