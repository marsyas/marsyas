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
	virtual void open(QString exerciseFilename) = 0;
	void setArea(QGridLayout *getInstructionArea, QGridLayout *getResultArea);

protected:
    QGridLayout *instructionArea;
    QGridLayout *resultArea;
};
#endif

