#include "choose-exercise.h"

Exercise* ChooseExercise::chooseType()
{
	Exercise* exercise;
	QStringList items;
	items << tr("Rhythm test") << tr("Intonation test");
//	items << tr("Intonation test") << tr("Sound control test") <<
//tr("Shifting test");
	bool ok;
	QString item = QInputDialog::getItem(0, tr("Choose testing method"),
	                                     tr("TestingMethod:"), items,
	                                     0, false, &ok);
	if (ok && !item.isEmpty())
	{
		if (item=="Rhythm test") exercise = new RhythmExercise();
		if (item=="Intonation test") exercise = new IntonationExercise();
//		if (item=="Sound control test") exercise_ = new ExerciseControl();
//		if (item=="Shifting test") exercise_ = new ExerciseShift();
		return exercise;
	}
	return NULL;
}

QString ChooseExercise::chooseCampaign(QString dir)
{
	QString openCampaign = QFileDialog::getOpenFileName(
	                           0,tr("Open Campaign"), dir,
	                           tr("Campaigns (*.txt)"));
	return openCampaign;
}

QString ChooseExercise::chooseFile(QString dir)
{
	QString openFilename = QFileDialog::getOpenFileName(
	                           0,tr("Open file"), dir,
	                           tr("Exercises (*.png)"));
	return openFilename;
}


QString ChooseExercise::chooseAttempt()
{
	// FIXME: directory
	QString openFilename = QFileDialog::getOpenFileName(0,tr("Open Attempt"),
	                       "/home/gperciva/data/");
	return openFilename;
}


