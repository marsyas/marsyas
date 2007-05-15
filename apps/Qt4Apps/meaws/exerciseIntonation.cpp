#include <iostream>
using namespace std;

#include "exerciseIntonation.h"

ExerciseIntonation::ExerciseIntonation() {
}
ExerciseIntonation::~ExerciseIntonation() {
	instructionArea->removeWidget(instructionImageLabel);
	delete (instructionImageLabel);;

	resultArea->removeWidget(resultLabel);
	delete resultLabel;
}

int ExerciseIntonation::getType() {
	return TYPE_INTONATION;
}

void ExerciseIntonation::setupDisplay() {
	resultLabel = new QLabel;
	resultLabel->setText("Intonation Exercise");

	resultArea->addWidget(resultLabel);
}

QString ExerciseIntonation::exercisesDir() {
	QString toReturn(MEAWS_DIR);
	toReturn.append("data/intonation/");
	return toReturn;
}


