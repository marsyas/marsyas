#ifndef MEAWS_TRY_INTONATION_H
#define MEAWS_TRY_INTONATION_H

#include "defs.h"
#include <QObject>
#include <QString>
#include <QGridLayout>
#include <QLabel>
#include <QDir>
#include "abstract-try.h"

class IntonationTry : public Try {
public:
	IntonationTry();
	~IntonationTry();
	QLabel* getDisplay();

/*
	void setArea(QFrame *getInstructionArea, QFrame *getResultArea);
	virtual void setupDisplay() = 0;
	virtual QString exercisesDir() = 0;
	virtual int getType() = 0;
	virtual bool displayAnalysis(MarBackend *results) = 0;
	virtual QString getMessage() = 0;
	virtual void open(QString exerciseFilename);
*/

//signals:
//	void analysisDone();

protected:
	QLabel *resultDisplay;
};
#endif

