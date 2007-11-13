#include <iostream>
using namespace std;

#include "dispatcher-exercise.h"

ExerciseDispatcher::ExerciseDispatcher(QFrame *centralFrame)
{
	layout_ = new QVBoxLayout();
	layout_->setContentsMargins(1,1,1,1);
	centralFrame->setLayout(layout_);

// move?
	instructionArea_ = new QFrame;
	resultArea_ = new QFrame;
	layout_->addWidget(instructionArea_);
	layout_->addWidget(resultArea_);

	attemptRunningBool_ = false;
	marBackend_ = NULL;
	exercise_ = NULL;
	statusMessage_ = "ready";
}

ExerciseDispatcher::~ExerciseDispatcher()
{
	close();
	if (layout_ != NULL)
		delete layout_;
}

bool ExerciseDispatcher::chooseEvaluation()
{
	QStringList items;
	items << tr("Intonation test") << tr("Rhythm test");
//	items << tr("Intonation test") << tr("Sound control test") <<
//tr("Shifting test");
	bool ok;
	QString item = QInputDialog::getItem(this, tr("Choose testing method"),
	                                     tr("TestingMethod:"), items, 0, false, &ok);
	if (ok && !item.isEmpty())
	{
		if (exercise_ != NULL)
			delete exercise_;
		if (item=="Intonation test") exercise_ = new IntonationExercise();
		if (item=="Rhythm test") exercise_ = new RhythmExercise();
//		if (item=="Sound control test") exercise_ = new ExerciseControl();
//		if (item=="Shifting test") exercise_ = new ExerciseShift();
		connect(exercise_,SIGNAL(analysisDone()),
			this, SLOT(analysisDone()));
		connect(exercise_,SIGNAL(newTry()),
			this, SLOT(newTry()));
		return true;
	}
	return false;
}

void ExerciseDispatcher::open()
{
	if (chooseEvaluation())
	{
		QString openFilename = QFileDialog::getOpenFileName(0,tr("Open file"),
		                       exercise_->exercisesDir(), tr("Exercises (*.png)"));
		if (!openFilename.isEmpty())
		{
			exercise_->open(openFilename);
			exercise_->setupDisplay(instructionArea_, resultArea_);
			exercise_->addTry();
			if (marBackend_ != NULL)
				delete marBackend_;
			marBackend_ = new MarBackend(exercise_->getBackend());
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
	if (marBackend_ != NULL)
		marBackend_->newTry();
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
		//attemptRunning(attemptRunningBool_);
	}
}


bool ExerciseDispatcher::openAttempt()
{
	QString openFilename = QFileDialog::getOpenFileName(0,tr("Open Attempt"),
	                       "/home/gperciva/data/");
	if (!openFilename.isEmpty())
	{
		marBackend_->openTry( qPrintable(openFilename) );
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

