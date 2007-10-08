#ifndef MEAWS_EXERCISE_ABSTRACT_H
#define MEAWS_EXERCISE_ABSTRACT_H

#include "defs.h"
#include <QObject>
#include <QString>
#include "abstract-try.h"


class Exercise : public QObject {
	Q_OBJECT

public:
	Exercise();
	~Exercise();

	virtual void open(QString exerciseFilename);
	virtual void setupDisplay(QFrame* instructionArea, QFrame*
resultArea);

	virtual QString exercisesDir() = 0;
	virtual int getBackend() = 0;
	virtual QString getMessage() = 0;
	virtual bool displayAnalysis(MarBackend *results) = 0;

	virtual void addTry() = 0;
	void addTryAbstract(Try* newTry);
	virtual void delTry() = 0;
	void delTryAbstract();

signals:
	void analysisDone();

public slots:
	virtual void button(mrs_natural selected) = 0;

protected:
	QLayout *instructionLayout;
	QLabel *instructionImageLabel;

	QLayout *resultLayout;
	QList<Try *> *tries;

	mrs_natural current_;

	// TODO: figure out how to find the height.  :(
	int foo;
};
#endif

