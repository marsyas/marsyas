#include <iostream>
using namespace std;

#include "exerciseAbstract.h"

Exercise::Exercise() {
	instructionArea = NULL;
	resultArea = NULL;
}

void Exercise::setArea(QGridLayout *getInstructionArea, QGridLayout *getResultArea) {
	instructionArea = getInstructionArea;
	resultArea = getResultArea;
}



