#ifndef MEAWS_TRY_ABSTRACT_H
#define MEAWS_TRY_ABSTRACT_H

#include "defs.h"
#include <QObject>
#include <QString>
#include <QGridLayout>
#include <QLabel>
#include <QDir>

class Try : public QObject {
	Q_OBJECT

public:
	Try();
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

