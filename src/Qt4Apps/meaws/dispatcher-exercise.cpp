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
	evaluation_ = NULL;
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
		if (evaluation_ != NULL)
			delete evaluation_;
		if (item=="Intonation test") evaluation_ = new IntonationExercise();
		if (item=="Rhythm test") evaluation_ = new RhythmExercise();
//		if (item=="Sound control test") evaluation_ = new ExerciseControl();
//		if (item=="Shifting test") evaluation_ = new ExerciseShift();
		connect(evaluation_,SIGNAL(analysisDone()), this,
		        SLOT(analysisDone()));
		return true;
	}
	return false;
}

void ExerciseDispatcher::open()
{
	if (chooseEvaluation())
	{
		QString openFilename = QFileDialog::getOpenFileName(0,tr("Open file"),
		                       evaluation_->exercisesDir(), tr("Exercises (*.png)"));
		if (!openFilename.isEmpty())
		{
			evaluation_->open(openFilename);
			evaluation_->setupDisplay(instructionArea_, resultArea_);
			evaluation_->addTry();
			if (marBackend_ != NULL)
				delete marBackend_;
			marBackend_ = new MarBackend(evaluation_->getBackend());
			connect(marBackend_, SIGNAL(setAttempt(bool)), this, SLOT(setAttempt(bool)));
			enableActions(MEAWS_READY_EXERCISE);
		}
		else
		{
			close();
		}
	}
}

void ExerciseDispatcher::close()
{
	if (marBackend_ != NULL)
	{
		delete marBackend_;
		marBackend_ = NULL;
	}
	if (evaluation_ != NULL)
	{
		delete evaluation_;
		evaluation_ = NULL;
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


void ExerciseDispatcher::openAttempt()
{
	QString openFilename = QFileDialog::getOpenFileName(0,tr("Open Attempt"),
	                       "/home/gperciva/data/");
	if (!openFilename.isEmpty())
	{
		marBackend_->open( qPrintable(openFilename) );
	}

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
	// to be removed DEBUG   -Mathieu
// what's to be removed?  Was this already removed?  -gp
// stuff that mght be accidentally commiited -gp.
	if ( marBackend_->analyze() )
	{
		evaluation_->displayAnalysis( marBackend_ );
		analysisDone();
	}
}

void ExerciseDispatcher::analysisDone()
{
	statusMessage_ = evaluation_->getMessage();
	enableActions(MEAWS_TRY_PAUSED);
}

