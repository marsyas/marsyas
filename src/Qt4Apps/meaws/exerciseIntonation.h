#ifndef MEAWS_EXERCISE_INTONATION_H
#define MEAWS_EXERCISE_INTONATION_H

#include "defs.h"
#include "exerciseAbstract.h"

class ExerciseIntonation : public Exercise {
public:
	ExerciseIntonation();
	~ExerciseIntonation();
	int getType();

	void setupDisplay();
	QString exercisesDir();
	QString getMessage();

	bool displayAnalysis(MarBackend *results);

private:
	QLabel *resultLabel;

};
#endif

