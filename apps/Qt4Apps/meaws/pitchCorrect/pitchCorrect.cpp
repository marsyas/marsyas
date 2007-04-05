#include <iostream>
using namespace std;

#include "../analyze.h"

int main(int argc, const char **argv) {
// TODO: need a better way of selecting this.
	bool NOTE_SEGMENT = true;
	int mode;

	cout<<"Usage: pitchCorrect FILENAME.wav  (expected notes.txt)"<<endl;
	cout<<"(expected notes.txt) is optional"<<endl;
	string audioFileName, exerFileName;
	if (argc<3) { cout<<"Please enter filename and mode."<<endl; exit(1); } else {
		audioFileName = argv[1];
		mode = atoi( argv[2] );
	}
	if (argc>=4) {
		exerFileName = argv[3];
	} else {
		exerFileName = "";
	}
	
	cout<<"Options: "<<endl;
	cout<<"  "<<audioFileName;
	if (exerFileName != "") {
		cout<<"  "<<exerFileName;
	};
	cout<<endl;

	Analyze* analyze;
  analyze = new Analyze(audioFileName, exerFileName);

	if (NOTE_SEGMENT) {
		analyze->calcDurations();
		analyze->calcNotes();
	} else {
		analyze->calcNothing();
	}
	analyze->writePitches();
	analyze->writeNotes();

	analyze->initHarms();
	// correct pitch
	if (mode==1) {
		analyze->calcIndividualMultipliers(); // do this AFTER addharms() !!!
	}
	// correct pitch and add harmonies
	if (mode==2) {
		analyze->addHarmsBasic();
		analyze->calcIndividualMultipliers(); // do this AFTER addharms() !!!
	}
	if (mode==3) {
		analyze->makeMinor();
		analyze->calcIndividualMultipliers(); // do this AFTER addharms() !!!
	}
	if (mode==4) {
		analyze->calcIndividualMultipliers(); // do this AFTER addharms() !!!
		analyze->screwJazz();
	}

	analyze->writeHarmData();
	delete analyze;
}

