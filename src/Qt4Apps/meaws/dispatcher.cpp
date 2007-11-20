#include <iostream>
using namespace std;

#include "dispatcher.h"

ExerciseDispatcher::ExerciseDispatcher(QFrame *centralFrame)
{
	centralLayout_ = new QVBoxLayout();
	centralLayout_->setContentsMargins(1,1,1,1);
	centralFrame->setLayout(centralLayout_);

	attemptRunningBool_ = false;
	marBackend_ = NULL;
	exercise_ = NULL;
	statusMessage_ = "ready";
}

ExerciseDispatcher::~ExerciseDispatcher()
{
	close();
	if (centralLayout_ != NULL)
		delete centralLayout_;
}

bool ExerciseDispatcher::chooseEvaluation()
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

void ExerciseDispatcher::open()
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
			exercise_->setupDisplay(centralLayout_);
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
			enableActions(MEAWS_READY_EXERCISE);
		}
		else
		{
			close();
		}
	}
}

void ExerciseDispatcher::newTry() {
	cout<<"dispatcher: new try"<<endl;
	if (marBackend_ != NULL) {
		marBackend_->newTry(exercise_->getBackend());
		cout<<"use backend: "<<exercise_->getBackend();
	}
}

void ExerciseDispatcher::close()
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
	enableActions(MEAWS_READY_USER);
}

void ExerciseDispatcher::toggleAttempt()
{
	setAttempt(!attemptRunningBool_);
}

void ExerciseDispatcher::setAttempt(bool running)
{
	// if the attempt state has changed
	if (running != attemptRunningBool_)
	{
		if (running)
		{
			attemptRunningBool_ = true;
			marBackend_->start();
			enableActions(MEAWS_TRY_RUNNING);
		}
		else
		{
			attemptRunningBool_ = false;
			marBackend_->stop();
			enableActions(MEAWS_TRY_PAUSED);
		}
	}
}


bool ExerciseDispatcher::openAttempt()
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


void ExerciseDispatcher::playFile()
{
	marBackend_->playFile();
	setAttempt(true);
}

QString ExerciseDispatcher::getMessage()
{
	return statusMessage_;
}

void ExerciseDispatcher::analyze()
{
	if ( marBackend_->analyze() )
	{
		exercise_->displayAnalysis( marBackend_ );
		analysisDone();
	}
}

void ExerciseDispatcher::analysisDone()
{
	statusMessage_ = exercise_->getMessage();
	enableActions(MEAWS_TRY_PAUSED);
}

