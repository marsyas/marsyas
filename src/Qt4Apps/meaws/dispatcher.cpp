#include <iostream>
using namespace std;

#include "dispatcher.h"

Dispatcher::Dispatcher(QFrame* centralFrame)
{
	user_ = new User();
//	metro_ = new Metro();
	exercise_ = NULL;

	marBackend_ = new MarBackend();
	connect(marBackend_, SIGNAL(gotAudio()),
	        this, SLOT(analyze()));
	connect(marBackend_, SIGNAL(setAttempt(bool)),
	        this, SLOT(setAttempt(bool)));
	marBackend_->setFileName("foo.wav");


	attemptRunningBool_ = false;

	string audioFile = "data/sd.wav";
	metro_ = new Metro(0, audioFile);



	centralFrame_ = centralFrame;

	/*
		attemptRunningBool_ = false;
		marBackend_ = NULL;
		exercise_ = NULL;
		statusMessage_ = "ready";
	*/
}

Dispatcher::~Dispatcher()
{
	close();
}

//void Dispatcher::connectMain(QObject* mainWindow)
//{
//}

bool Dispatcher::close()
{
	// FIXME: check user and exercise first!
	return true;
}

QString Dispatcher::getTitle()
{
	QString title = "Meaws";
	QString next = user_->getName();
	if (!next.isEmpty())
		title.append(QString(" - %1").arg(next));
	return title;
}



void Dispatcher::openExercise()
{
	if (exercise_ != NULL)
		delete exercise_;
	exercise_ = ChooseExercise::chooseType();
	if (exercise_ == NULL)
		return;

	QString filename =
	    ChooseExercise::chooseFile(exercise_->exercisesDir());
	if (filename.isEmpty())
		return;

	exercise_->open(filename);
	setupExercise();
}


void Dispatcher::setupExercise()
{
	connect(exercise_,SIGNAL(setBackend(mrs_natural)),
	        marBackend_,SLOT(setBackend(mrs_natural)));
	exercise_->setupDisplay(centralFrame_);
	exercise_->addTry();
	updateMain(MEAWS_READY_EXERCISE);
}

void Dispatcher::openAttempt()
{
	QString filename =
	    ChooseExercise::chooseAttempt();
	marBackend_->setBackend(exercise_->getBackend());
	marBackend_->openTry( qPrintable(filename) );
}

void Dispatcher::analyze()
{
	if ( marBackend_->analyze() )
	{
		exercise_->displayAnalysis( marBackend_ );
		emit updateMain(MEAWS_UPDATE);
	}
}


QString Dispatcher::getStatus()
{
	if (exercise_ == NULL)
		return "";
	return exercise_->getMessage();
}



// temp
void Dispatcher::toggleAttempt()
{
	cout<<"toggleAttempt"<<endl;
	setAttempt(!attemptRunningBool_);
}

void Dispatcher::setAttempt(bool running)
{
	cout<<"setAttempt: "<<running<<endl;
	// if the attempt state has changed
	if (running != attemptRunningBool_)
	{
		if (running)
		{
			attemptRunningBool_ = true;
			marBackend_->start();
			metro_->start();
			updateMain(MEAWS_TRY_RUNNING);
		}
		else
		{
			attemptRunningBool_ = false;
			marBackend_->stop();
			metro_->stop();
			updateMain(MEAWS_TRY_PAUSED);
		}
	}
}

/*
void Dispatcher::analysisDone()
{
	statusMessage_ = exercise_->getMessage();
	updateMain(MEAWS_TRY_PAUSED);
}

void Dispatcher::newTry() {
	cout<<"dispatcher: new try"<<endl;
	if (marBackend_ != NULL) {
		marBackend_->newTry(exercise_->getBackend());
		cout<<"use backend: "<<exercise_->getBackend();
	}
}

void Dispatcher::close()
{
	if (marBackend_ != NULL)
	{
		delete marBackend_;
		marBackend_ = NULL;
	}
	if (exercise_ != NULL)
	{
		QLayoutItem* child;
		while ((child = centralLayout_->takeAt(0)) != 0)
   			delete child;
		delete exercise_;
		exercise_ = NULL;
	}
	updateMain(MEAWS_READY_USER);
}



void Dispatcher::playFile()
{
	marBackend_->playFile();
	setAttempt(true);
}

*/

