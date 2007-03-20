#include <iostream>
using namespace std;

#include "../analyze.h"

int main() {
	Analyze* analyze;
  analyze = new Analyze("/Users/gperciva/data/exer-1.wav", "music/exer.txt");
//  analyze = new Analyze("/Users/gperciva/data/sit-test.wav", "music/exer.txt");
  analyze->calcDurations();
  analyze->calcNotes();
	analyze->writePitches();
	analyze->writeNotes();

//	analyze->printNotes();
//	cout<<"-----------"<<endl;
	analyze->addHarmonies();
	analyze->printNotes();
	delete analyze;
}

