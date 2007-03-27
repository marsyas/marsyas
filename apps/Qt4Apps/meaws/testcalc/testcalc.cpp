#include <iostream>
using namespace std;

#include "../analyze.h"

int main() {
	Analyze* analyze;
//  analyze = new Analyze("/Users/gperciva/data/separation/mono-loud-piano-good-int.wav", "music/exer.txt");
  analyze = new Analyze("/Users/gperciva/data/exer-2.wav", "music/exer.txt");
//  analyze = new Analyze("/Users/gperciva/data/sit-test.wav", "music/exer.txt");
  analyze->calcDurations();
  analyze->calcNotes();
	analyze->writePitches();

//	analyze->printNotes();
//	cout<<"-----------"<<endl;
	analyze->initHarms();
	analyze->addHarmsBasic();
	analyze->writeNotes();
	analyze->printNotes();
	delete analyze;
}

