#include <iostream>
#include "CommandLineOptions.h"
using namespace std;
using namespace Marsyas;

#include "analyze.h"

CommandLineOptions cmd_options;

int helpOpt;
int usageOpt;
mrs_string fretFilename;
mrs_natural median_radius;
mrs_real new_note_midi;
mrs_real pitch_certainty;
mrs_string otherFilename;

Analyze* analyze;

void
printUsage()
{
  MRSDIAG("pitchCorrect.cpp - printUsage");
  cerr << "Usage: pitchCorrect <audiofile>" << endl;
  cerr << "where <audiofile> is a sound files in a Marsyas supported format" << endl;
  exit(1);
}

void
printHelp()
{
  MRSDIAG("pitchCorrect.cpp - printHelp");
  cerr << "pitchCorrect: Sample Program"<< endl;
  cerr << "------------------------------" << endl;
  cerr << endl;
  cerr << "Usage: pitchCorrect <audiofile>" << endl;
  cerr << endl;
  cerr << "where <audiofile> is a sound files in a Marsyas supported format." << endl;
	cerr << "You may also call pitchCorrect with only a fret file."<<endl;
  cerr << "Help Options:" << endl;
  cerr << "-u --usage      : display short usage info" << endl;
  cerr << "-h --help       : display this information" << endl;
  cerr << "-f --fret       : fret data to aid analysis (for sitar)" << endl;
	cerr << "-r --radius     : radius of median pitch calculation (10)"<<endl;
	cerr << "-n --newnote    : midi pitch value that indicates a new note (0.6)"<<endl;
	cerr << "-p --pitch      : certainty of the pitch at this frame.  Higher values mean that pitches of lower certainty are are used.  (200)"<<endl;
	cerr << "-o --other      : other option"<<endl;
  exit(1);
}

void
initOptions()
{
  cmd_options.addBoolOption("help", "h", false);
  cmd_options.addBoolOption("usage", "u", false);
  cmd_options.addStringOption("fret", "f", "");
	cmd_options.addNaturalOption("radius", "r", 10);
	cmd_options.addRealOption("newnote", "n", 0.6);
	cmd_options.addRealOption("pitch", "p", 200);
	cmd_options.addStringOption("other", "o", "");
}

void
loadOptions()
{
  helpOpt = cmd_options.getBoolOption("help");
  usageOpt = cmd_options.getBoolOption("usage");
  fretFilename = cmd_options.getStringOption("fret");
	median_radius = cmd_options.getNaturalOption("radius");
	new_note_midi = cmd_options.getRealOption("newnote");
	pitch_certainty = cmd_options.getRealOption("pitch");
  otherFilename = cmd_options.getStringOption("other");
}


void
analyzeFile(string filename)
{
	analyze->setOptions(median_radius, new_note_midi, pitch_certainty);
	analyze->loadData(filename);
	analyze->writePitches();
	analyze->calcOnsets();
	analyze->calcNotes();
	analyze->writeOnsets();
	analyze->writeNotes();
}

void
analyzeFileWithFret(string filename)
{
	analyze->setOptions(median_radius, new_note_midi, pitch_certainty);
	analyze->loadData(filename);
	analyze->writePitches();
//	analyze->calcOnsets();
	analyze->calcNotes();
	analyze->writeOnsets();
	analyze->writeNotes();
}

int main(int argc, const char **argv) {
  initOptions();
  cmd_options.readOptions(argc,argv);
  loadOptions();

  if (helpOpt)
    printHelp();
  if ( (usageOpt) || (argc==1) )
    printUsage(); 

  analyze = new Analyze();
	if (fretFilename != "")
		analyzeFile(fretFilename);
	analyze->clearPitches();

	if (otherFilename != "")
		analyze->realvecFileToPlain(otherFilename);
		

// TODO: we only want to analyze one file; it was just easier to do it
// this way.
  vector<string> soundfiles = cmd_options.getRemaining();
  vector<string>::iterator sfi;
  for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi)
    {
//			if (fretFilename == "")
				analyzeFile( *sfi );
//			else
//      	analyzeFileWithFret( *sfi );
    }

	delete analyze;
}

