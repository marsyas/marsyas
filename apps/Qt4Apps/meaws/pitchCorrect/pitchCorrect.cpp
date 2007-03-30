#include <iostream>
using namespace std;

#include "../analyze.h"

int main(int argc, const char **argv) {
	cout<<"Usage: pitchCorrect FILENAME.wav  (expected notes.txt)"<<endl;
	cout<<"(expected notes.txt) is optional"<<endl;
	string audioFileName, exerFileName;
	if (argc<2) { cout<<"Please enter filename."<<endl; exit(1); } else
		{ audioFileName = argv[1]; }
	if (argc>=3) {
		exerFileName = argv[2];
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
  analyze->calcDurations();
  analyze->calcNotes();

	analyze->initHarms();
	analyze->addHarmsBasic();

	analyze->calcMultipliers(); // do this AFTER addharms() !!!
	analyze->writePitches();
	analyze->writeNotes();
	analyze->writeHarmData();
	delete analyze;
}

