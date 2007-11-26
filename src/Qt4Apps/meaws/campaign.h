#ifndef MEAWS_CAMPAIGN_H
#define MEAWS_CAMPAIGN_H

#include "defs.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>

#include "rhythm-exercise.h"
#include "intonation-exercise.h"
//#include "exerciseControl.h"
//#include "exerciseShift.h"



class Campaign: public QObject
{
	Q_OBJECT

public:
	Campaign(QString campaignFile);
	~Campaign();

	Exercise* getNextExercise(double lastScore);

private:
	void openFile(QString filename);
	int passPercent;
	QList<QStringList> adventures;

	int currentLevel;
};
#endif

