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

	int getBackend();
	QString exercisesDir();

	void open(QString exerciseFilename);
	void addTry();
	void delTry();

	QString getMessage();
	bool displayAnalysis(MarBackend *results);

public slots:
	void button(mrs_natural selected);

private:
	realvec exerAnswer;


	QStringList lily_input;

};
#endif

