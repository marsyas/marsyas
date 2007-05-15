#ifndef MEAWS_EXERCISE_ABSTRACT_H
#define MEAWS_EXERCISE_ABSTRACT_H

#include "defs.h"
#include <QObject>
#include <QString>
#include <QGridLayout>
#include <QLabel>

class Exercise : public QObject {
	Q_OBJECT

public:
	Exercise();
	void setArea(QGridLayout *getInstructionArea, QGridLayout *getResultArea);
	virtual void setupDisplay() = 0;
	virtual QString exercisesDir() = 0;
	virtual void open(QString exerciseFilename);

protected:
    QGridLayout *instructionArea;
    QGridLayout *resultArea;
	QLabel *instructionImageLabel;
};
#endif

