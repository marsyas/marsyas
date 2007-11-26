#include <iostream>
using namespace std;

#include "dispatcher.h"

Dispatcher::Dispatcher(QFrame* centralFrame)
{
	user_ = new User();
	exercise_ = NULL;

	marBackend_ = new MarBackend();
	connect(marBackend_, SIGNAL(gotAudio()),
	        this, SLOT(analyze()));
	connect(marBackend_, SIGNAL(setAttempt(bool)),
	        this, SLOT(setAttempt(bool)));


	string audioFile = "data/sd.wav";
	metro_ = new Metro(0, audioFile);


	centralFrame_ = centralFrame;
	attemptRunningBool_ = false;

}

Dispatcher::~Dispatcher()
{
	close();
	delete exercise_;
	delete metro_;
	delete marBackend_;
}

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

QString Dispatcher::getStatus()
{
	if (exercise_ == NULL)
		return "";
	return exercise_->getMessage();
}


// Actions

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

//zz
void Dispatcher::openCampaign()
{
	QString dir(MEAWS_DIR);
	dir.append("data/campaign");
	QString filename =
	    ChooseExercise::chooseCampaign( dir );
	if (filename.isEmpty())
		return;

	Campaign foo(filename);
	cout<<qPrintable( foo.getNextExercise() );
}

void Dispatcher::setupExercise()
{
	connect(exercise_, SIGNAL(setupBackend()),
	        this, SLOT(setupBackend()));
	exercise_->setupDisplay(centralFrame_);
	exercise_->addTry();
	updateMain(MEAWS_READY_EXERCISE);
}

void Dispatcher::setupBackend()
{
	if ( exercise_->hasAudio() )
	{
		marBackend_->setBackend( BACKEND_PLAYBACK, true,
		                         exercise_->getFilename() );
	}
	else
	{
		marBackend_->setBackend( exercise_->getBackend(), false,
		                         exercise_->getFilename() );
	}
	marBackend_->setup();
}


void Dispatcher::openAttempt()
{
	QString filename = ChooseExercise::chooseAttempt();
	exercise_->setFilename( qPrintable(filename) );
	marBackend_->setBackend( exercise_->getBackend(), true,
	                         exercise_->getFilename());
	marBackend_->setup();
	marBackend_->start();
}


void Dispatcher::analyze()
{
	if ( !exercise_->hasAudio() )
	{
		if ( marBackend_->analyze() )
		{
			exercise_->displayAnalysis( marBackend_ );
			cout<<"DISPATCHER done display analysis"<<endl;
			emit updateMain(MEAWS_UPDATE);
			cout<<"DISPATCHER main updated"<<endl;
		}
	}
}


void Dispatcher::toggleAttempt()
{
	cout<<"DISPATCHER toggleAttempt"<<endl;
	setAttempt(!attemptRunningBool_);
}

void Dispatcher::setAttempt(bool running)
{
	// if the attempt state has changed
	if (running != attemptRunningBool_)
	{
		cout<<"DISPATCHER setAttempt: "<<running<<endl;
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

*/

