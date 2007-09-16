#ifndef MEAWS_EXERCISE_INTONATION_H
#define MEAWS_EXERCISE_INTONATION_H

#include "defs.h"
#include "abstract-exercise.h"
#include "intonation-try.h"


#include "QtMarPlot.h"

class IntonationExercise : public Exercise {
public:
	IntonationExercise();
	~IntonationExercise();
	int getType();

	void open(QString exerciseFilename);
	void setupDisplay();
	QString exercisesDir();
	QString getMessage();

	bool displayAnalysis(MarBackend *results);

private:
	QLabel *resultLabel;
	QtMarPlot *foo;
	realvec exerAnswer;

};
#endif

