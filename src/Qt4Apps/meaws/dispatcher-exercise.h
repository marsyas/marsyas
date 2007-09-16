#ifndef MEAWS_EXERCISE_DISPATCHER_H
#define MEAWS_EXERCISE_DISPATCHER_H

#include "defs.h"
#include "intonation-exercise.h"
//#include "exerciseControl.h"
//#include "exerciseShift.h"
#include "backend.h"

#include <QDialog>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QtGui>

class ExerciseDispatcher : public QDialog {
	Q_OBJECT

public:
	ExerciseDispatcher(QFrame *getInstructionArea, QFrame *getResultArea);
	~ExerciseDispatcher();

	QString getMessage();

public slots:
	void open();
	void close();
	void toggleAttempt();
	void setAttempt(bool running);
	void openAttempt();

	void playFile();

	void analyze(); // temp
	void analysisDone(); // even tempier

signals:
	void enableActions(int state);
	void attemptRunning(bool running);

private:
	bool chooseEvaluation();

	QString exerciseName;
	QFrame *instructionArea;
	QFrame *resultArea;

	Exercise *evaluation;
	bool attemptRunningBool;
	MarBackend *marBackend;

	QString statusMessage;
};
#endif

