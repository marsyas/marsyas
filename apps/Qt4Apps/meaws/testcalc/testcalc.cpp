#include <iostream>
using namespace std;

#include "../analyze.h"

int main() {
	Analyze* analyze;
  analyze = new Analyze();
//  analyze->getPitches("/Users/gperciva/data/exer-2.wav");
//	analyze->metroDurations();
  analyze->calcDurations();
//  analyze->calcNotes();
}

