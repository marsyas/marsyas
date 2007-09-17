#include <iostream>
using namespace std;

#include "dispatcher-exercise.h"

ExerciseDispatcher::ExerciseDispatcher()
{
	instructionArea = new QFrame;
	resultArea = new QFrame;

	attemptRunningBool = false;
	marBackend = NULL;
	evaluation = NULL;
	statusMessage = "ready";
}

ExerciseDispatcher::~ExerciseDispatcher() {
	close();
}

bool ExerciseDispatcher::chooseEvaluation() {
	QStringList items;
	items << tr("Intonation test") << tr("Sound control test") <<
tr("Shifting test");
	bool ok;
	QString item = QInputDialog::getItem(this, tr("Choose testing method"),
		tr("TestingMethod:"), items, 0, false, &ok);
	if (ok && !item.isEmpty()) {
		if (evaluation != NULL)
			delete evaluation;
		if (item=="Intonation test") evaluation = new IntonationExercise();
//		if (item=="Sound control test") evaluation = new ExerciseControl();
//		if (item=="Shifting test") evaluation = new ExerciseShift();
		connect(evaluation,SIGNAL(analysisDone()), this,
SLOT(analysisDone()));
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
			evaluation->setupDisplay(instructionArea, resultArea);
			evaluation->blah();
			if (marBackend != NULL)
				delete marBackend;
			marBackend = new MarBackend(evaluation->getType());
			connect(marBackend, SIGNAL(setAttempt(bool)), this, SLOT(setAttempt(bool)));
			enableActions(MEAWS_READY_EXERCISE);
		} else {
			close();
		}
	}
}

void ExerciseDispatcher::close() {
	if (marBackend != NULL) {
		delete marBackend;
		marBackend = NULL;
	}
	if (evaluation != NULL) {
		delete evaluation;
		evaluation = NULL;
	}
	enableActions(MEAWS_READY_USER);
}

void ExerciseDispatcher::toggleAttempt() {
	setAttempt(!attemptRunningBool);
}

void ExerciseDispatcher::setAttempt(bool running) {
	// if the attempt state has changed
	if (running != attemptRunningBool) {
		if (running) {
			attemptRunningBool = true;
			marBackend->start();
		} else {
			attemptRunningBool = false;
			marBackend->stop();
		}
		attemptRunning(attemptRunningBool);
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
	setAttempt(true);
}

QString ExerciseDispatcher::getMessage() {
	return statusMessage;
}

void ExerciseDispatcher::analyze() {
    // to be removed DEBUG   -Mathieu
// what's to be removed?  Was this already removed?  -gp
// stuff that mght be accidentally commiited -gp.
		if ( marBackend->analyze() ) 
	{
		evaluation->displayAnalysis( marBackend );
		analysisDone();
	}
}

void ExerciseDispatcher::analysisDone() {
	statusMessage = evaluation->getMessage();
	enableActions(MEAWS_READY_AUDIO);
}

