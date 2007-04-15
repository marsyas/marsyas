#include <iostream>
#include "CommandLineOptions.h"
using namespace std;
using namespace Marsyas;

#include "analyze.h"

CommandLineOptions cmd_options;

int helpOpt;
int usageOpt;
mrs_string audioFilename;
mrs_natural median_radius;
mrs_real new_note_midi;
mrs_real pitch_certainty;

void
printUsage()
{
  MRSDIAG("pitchCorrect.cpp - printUsage");
  cerr << "Usage: pitchCorrect " << "file1 file2 file3" << endl;
  cerr << endl;  cerr << "where file1, ..., fileN are sound files in a MARSYAS supported format" << endl;
  exit(1);
}

void
printHelp()
{
  MRSDIAG("pitchCorrect.cpp - printHelp");
  cerr << "pitchCorrect: Sample Program"<< endl;
  cerr << "------------------------------" << endl;
  cerr << endl;
  cerr << "Usage: pitchCorrect file1" << endl;
  cerr << endl;
  cerr << "where file1, ..., fileN are sound files in a Marsyas supported format" << endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information" << endl;
  cerr << "-a --audio      : audio file to analyze" << endl;
	cerr << "-r --radius     : radius of median pitch calculation (10)"<<endl;
	cerr << "-n --newnote    : midi pitch value that indicates a new note (0.6)"<<endl;
	cerr << "-p --pitch      : certainty of the pitch at this frame.  Higher means that less certain pitches are used.  (500)"<<endl;
  exit(1);
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addStringOption("audio", "a", "");
	cmd_options.addNaturalOption("radius", "r", 10);
	cmd_options.addRealOption("newnote", "n", 0.6);
	cmd_options.addRealOption("pitch", "p", 500);
}

void
loadOptions()
{
  helpOpt = cmd_options.getBoolOption("help");
  usageOpt = cmd_options.getBoolOption("usage");
  audioFilename = cmd_options.getStringOption("audio");
	median_radius = cmd_options.getNaturalOption("radius");
	new_note_midi = cmd_options.getRealOption("newnote");
	pitch_certainty = cmd_options.getRealOption("pitch");
}


void
analyzeFile(string filename)
{
	cout<<"Analyzing "<<filename<<endl;

	Analyze* analyze;
  analyze = new Analyze();
	analyze->setOptions(median_radius, new_note_midi, pitch_certainty);
	analyze->loadData(filename);
	analyze->writePitches();
	analyze->calcOnsets();
	analyze->calcNotes();
	analyze->writeOnsets();
	analyze->writeNotes();
	delete analyze;
}

int main(int argc, const char **argv) {
  initOptions();
  cmd_options.readOptions(argc,argv);
  loadOptions();

  if (helpOpt)
    printHelp();
  if ( (usageOpt) || (argc==1) )
    printUsage(); 

	if (audioFilename != "")
		analyzeFile(audioFilename);
}

