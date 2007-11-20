#include <iostream>
using namespace std;

#include "dispatcher.h"

Dispatcher::Dispatcher(QObject* mainWindow)
{
	user_ = new User();
//	metro_ = new Metro();

	string audioFile = "data/sd.wav";
//	metro_ = new Metro(this, audioFile);
//	connect(visualMetroBeatAct_, SIGNAL(triggered()), metro_,
//	        SLOT(toggleBigMetro()));
//	metro_->setIcon(visualMetroBeatAct_);

//	connectMain(mainWindow);

/*
	centralLayout_ = new QVBoxLayout();
	centralLayout_->setContentsMargins(1,1,1,1);
	centralFrame->setLayout(centralLayout_);
*/

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
//	if (centralLayout_ != NULL)
//		delete centralLayout_;
}

//void Dispatcher::connectMain(QObject* mainWindow)
//{
//}

bool Dispatcher::close()
{
	return true;
}

/*
bool Dispatcher::chooseEvaluation()
{
	QStringList items;
	items << tr("Rhythm test") << tr("Intonation test");
//	items << tr("Intonation test") << tr("Sound control test") <<
//tr("Shifting test");
	bool ok;
	QString item = QInputDialog::getItem(this, tr("Choose testing method"),
	                                     tr("TestingMethod:"), items,
					0, false, &ok);
	if (ok && !item.isEmpty())
	{
		if (exercise_ != NULL)
			delete exercise_;
		if (item=="Rhythm test") exercise_ = new RhythmExercise();
		if (item=="Intonation test") exercise_ = new IntonationExercise();
//		if (item=="Sound control test") exercise_ = new ExerciseControl();
//		if (item=="Shifting test") exercise_ = new ExerciseShift();
		return true;
	}
	return false;
}

void Dispatcher::open()
{
	if (chooseEvaluation())
	{
		QString openFilename = QFileDialog::getOpenFileName(
				0,tr("Open file"), exercise_->exercisesDir(),
				tr("Exercises (*.png)"));
		if (!openFilename.isEmpty())
		{
			connect(exercise_,SIGNAL(analysisDone()),
				this, SLOT(analysisDone()));
			connect(exercise_,SIGNAL(newTry()),
				this, SLOT(newTry()));
			exercise_->open(openFilename);
			//exercise_->setupDisplay(centralLayout_);
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
		}
		else
		{
			close();
		}
	}
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

