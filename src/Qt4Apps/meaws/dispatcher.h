#ifndef MEAWS_DISPATCHER_H
#define MEAWS_DISPATCHER_H

#include "defs.h"
#include "user.h"
#include "metro.h"
#include "backend.h"

#include "rhythm-exercise.h"
#include "intonation-exercise.h"
//#include "exerciseControl.h"
//#include "exerciseShift.h"

#include <QInputDialog>
#include <QFileDialog>

class Dispatcher : public QDialog
{
	Q_OBJECT

public:
	Dispatcher(QObject* mainWindow);
	~Dispatcher();

	QObject* getUserPointer()
		{ return user_; };

	bool close();

	QString getTitle();
	QString getMessage();
/*
public slots:
	void open();
	void close();
	void toggleAttempt();
	void setAttempt(bool running);
	bool openAttempt();

	void playFile();

	void analyze();  // temp ?
	void analysisDone(); // even tempier

	void newTry();

	void addTry()
	{
		exercise_->addTry();
	};
	void delTry()
	{
		exercise_->delTry();
	};

signals:
	void updateMain(int state);
*/

private:
// main object variables
	User *user_;
	Metro *metro_;


/*
//	void connectMain(QObject* mainWindow);
	bool chooseEvaluation();

	// basic GUI frame
	QVBoxLayout *centralLayout_;

	// actual Meaws objects
	Exercise *exercise_;
	MarBackend *marBackend_;

	// left-over garbage (?)
	QString exerciseName_;
	bool attemptRunningBool_;

	QString statusMessage_;
*/
};
#endif

