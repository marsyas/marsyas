#include <iostream>
using namespace std;

#include "exerciseDispatcher.h"

ExerciseDispatcher::ExerciseDispatcher() {
}

ExerciseDispatcher::~ExerciseDispatcher() {
}

void ExerciseDispatcher::setArea(QGridLayout *getInstructionArea, QGridLayout *getResultArea){
	instructionArea = getInstructionArea;
	resultArea = getResultArea;
}

bool ExerciseDispatcher::chooseEvaluation() {
	QStringList items;
	items << tr("Intonation test") << tr("Sound control test");
	bool ok;
	QString item = QInputDialog::getItem(this, tr("Choose testing method"),
		tr("TestingMethod:"), items, 0, false, &ok);
	if (ok && !item.isEmpty()) {
		if (item=="Intonation test") evaluation = new ExerciseIntonation();
		if (item=="Sound control test") evaluation = new ExerciseControl();
		evaluation->setArea(instructionArea, resultArea);
		evaluation->setupDisplay();
		return true;
	}
	return false;
}

void ExerciseDispatcher::open() {
	if (chooseEvaluation()) {
		QString openFilename = QFileDialog::getOpenFileName(0,tr("Save file"),
			evaluation->exercisesDir());
		if (!openFilename.isEmpty()) {
			evaluation->open(openFilename);
			enableActions(MEAWS_READY_EXERCISE);
		}
	}
}

