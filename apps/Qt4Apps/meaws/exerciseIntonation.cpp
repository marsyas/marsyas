#include <iostream>
using namespace std;

#include "exerciseIntonation.h"

ExerciseIntonation::ExerciseIntonation() {
}

void ExerciseIntonation::setupDisplay() {
	resultLabel = new QLabel;
	resultLabel->setText("Intonation Exercise");

	resultArea->addWidget(resultLabel);
}


