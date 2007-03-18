#include <iostream>
using namespace std;

#include "../analyze.h"

int main() {
	Analyze* analyze;
  analyze = new Analyze("/Users/gperciva/data/exer-3.wav", "music/exer.txt");
  analyze->calcDurations();
  analyze->calcNotes();
}

