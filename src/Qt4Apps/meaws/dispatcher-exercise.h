#ifndef MEAWS_EXERCISE_DISPATCHER_H
#define MEAWS_EXERCISE_DISPATCHER_H

#include "defs.h"
#include "intonation-exercise.h"
#include "rhythm-exercise.h"
//#include "exerciseControl.h"
//#include "exerciseShift.h"
#include "backend.h"

#include <QDialog>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QtGui>

class ExerciseDispatcher : public QDialog
{
	Q_OBJECT

public:
	ExerciseDispatcher(QFrame *getCentralFrame);
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

	void addTry()
	{
		evaluation->addTry();
	};
	void delTry()
	{
		evaluation->delTry();
	};

signals:
	void enableActions(int state);
	void attemptRunning(bool running);

private:
	bool chooseEvaluation();

	// basic GUI frames
	QVBoxLayout *layout;
	QFrame *instructionArea;
	QFrame *resultArea;

	// actual Meaws objects
	Exercise *evaluation;
	MarBackend *marBackend;

	// left-over garbage (?)
	QString exerciseName;
	bool attemptRunningBool;

	QString statusMessage;
};
#endif

