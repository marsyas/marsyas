#ifndef MEAWS_EXERCISE_INTONATION_H
#define MEAWS_EXERCISE_INTONATION_H

#include "defs.h"
#include "abstract-exercise.h"
#include "intonation-try.h"
#include <QButtonGroup>


class IntonationExercise : public Exercise {
public:
	IntonationExercise();
	~IntonationExercise();

	int getType();
	QString exercisesDir();

	void open(QString exerciseFilename);
	void addTry();

	QString getMessage();
	bool displayAnalysis(MarBackend *results);

public slots:
	void button(mrs_natural selected);

private:
	realvec exerAnswer;
	QList<IntonationTry *> *tries;

	QButtonGroup* resultGroup;
	int nextNumber;
};
#endif

