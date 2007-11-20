#include <iostream>
using namespace std;

#include "dispatcher.h"

Dispatcher::Dispatcher(QObject* mainWindow, QFrame* centralFrame)
{
	user_ = new User();
//	metro_ = new Metro();
	exercise_ = NULL;

//	string audioFile = "data/sd.wav";
//	metro_ = new Metro(this, audioFile);
//	connect(visualMetroBeatAct_, SIGNAL(triggered()), metro_,
//	        SLOT(toggleBigMetro()));
//	metro_->setIcon(visualMetroBeatAct_);

//	connectMain(mainWindow);

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
//	connect(exercise_,SIGNAL(analysisDone()),
//	        this, SLOT(analysisDone()));
//	connect(exercise_,SIGNAL(newTry()),
//	        this, SLOT(newTry()));
	exercise_->setupDisplay(centralFrame_);
/*
	exercise_->addTry();
	if (marBackend_ != NULL)
		delete marBackend_;
	marBackend_ = new MarBackend();
	// FIXME: order of creation
	marBackend_->newTry(exercise_->getBackend());

	cout<<"made backend"<<endl;
	connect(marBackend_, SIGNAL(setAttempt(bool)),
	        this, SLOT(setAttempt(bool)));
	connect(marBackend_, SIGNAL(gotAudio()),
	        this, SLOT(analyze()));
	updateMain(MEAWS_READY_EXERCISE);
*/
}


/*
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

void Dispatcher::toggleAttempt()
{
	setAttempt(!attemptRunningBool_);
}

void Dispatcher::setAttempt(bool running)
{
	// if the attempt state has changed
	if (running != attemptRunningBool_)
	{
		if (running)
		{
			attemptRunningBool_ = true;
			marBackend_->start();
			updateMain(MEAWS_TRY_RUNNING);
		}
		else
		{
			attemptRunningBool_ = false;
			marBackend_->stop();
			updateMain(MEAWS_TRY_PAUSED);
		}
	}
}


bool Dispatcher::openAttempt()
{
	QString openFilename = QFileDialog::getOpenFileName(0,tr("Open Attempt"),
	                       "/home/gperciva/data/");
	if (!openFilename.isEmpty())
	{
		marBackend_->openTry(exercise_->getBackend(), qPrintable(openFilename) );
		return true;
	}
	return false;
}


void Dispatcher::playFile()
{
	marBackend_->playFile();
	setAttempt(true);
}

QString Dispatcher::getMessage()
{
	return statusMessage_;
}

void Dispatcher::analyze()
{
	if ( marBackend_->analyze() )
	{
		exercise_->displayAnalysis( marBackend_ );
		analysisDone();
	}
}

void Dispatcher::analysisDone()
{
	statusMessage_ = exercise_->getMessage();
	updateMain(MEAWS_TRY_PAUSED);
}
*/

