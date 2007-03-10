#include <iostream>
using namespace std;

#include "../analyze.h"

int main() {
	Analyze* analyze;
  analyze = new Analyze();
//  analyze->writePitches("~/data/exer-1.wav");
  analyze->calcDurations();
//  analyze->calcNotes();
}

