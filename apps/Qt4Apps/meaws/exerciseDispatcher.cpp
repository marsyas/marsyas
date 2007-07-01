#include <iostream>
using namespace std;

#include "exerciseDispatcher.h"

ExerciseDispatcher::ExerciseDispatcher() {
	attemptRunningBool = false;
	marBackend = NULL;
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
		QString openFilename = QFileDialog::getOpenFileName(0,tr("Open file"),
			evaluation->exercisesDir(), tr("Exercises (*.png)"));
		if (!openFilename.isEmpty()) {
			evaluation->open(openFilename);
			marBackend = new MarBackend(evaluation->getType());
			enableActions(MEAWS_READY_EXERCISE);
		}
	}
}

void ExerciseDispatcher::close() {
	delete evaluation;
	enableActions(MEAWS_READY_USER);
}

void ExerciseDispatcher::toggleAttempt() {
	attemptRunningBool = !attemptRunningBool;
	attemptRunning(attemptRunningBool);
	if (attemptRunningBool) {
		marBackend->start();
	} else {
		marBackend->stop();
	}
}


void ExerciseDispatcher::openAttempt() {
	QString openFilename = QFileDialog::getOpenFileName(0,tr("Open Attempt"),
			"/home/gperciva/data/");
	if (!openFilename.isEmpty()) {
		marBackend->open( qPrintable(openFilename) );
	}

}

void ExerciseDispatcher::playFile() {
	marBackend->playFile();
	if (attemptRunningBool==false)
		toggleAttempt();
	cout<<"done playFile()"<<endl;
}

void ExerciseDispatcher::analyze() {
	marBackend->analyze();
}

