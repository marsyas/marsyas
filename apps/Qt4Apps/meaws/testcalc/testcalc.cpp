#include <iostream>
using namespace std;

#include "../analyze.h"

int main() {
	Analyze* analyze;
  analyze = new Analyze("exer.txt");
//  analyze->getPitches("/Users/gperciva/data/exer-1.wav");
//	analyze->metroDurations();
  analyze->calcDurations();
  analyze->calcNotes();
}

