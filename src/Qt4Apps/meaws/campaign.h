#ifndef MEAWS_CAMPAIGN_H
#define MEAWS_CAMPAIGN_H

#include "defs.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>

class Campaign: public QObject
{
	Q_OBJECT

public:
	Campaign(QString campaignFile);
	~Campaign();

	QString getNextExercise();

private:
	void openFile(QString filename);
	int passPercent;
	QList<QStringList> adventures;

	//QStringList
};
#endif

