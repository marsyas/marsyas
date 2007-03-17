#include <iostream>
using namespace std;

#include "../analyze.h"

int main() {
	Analyze* analyze;
  analyze = new Analyze("/Users/gperciva/data/exer-1.wav", "music/exer.txt");
//  analyze->getPitches();
//	analyze->metroDurations();
  analyze->calcDurations();
//  analyze->calcNotes();
}

