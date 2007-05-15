#ifndef MEAWS_EXERCISE_CONTROL_H
#define MEAWS_EXERCISE_CONTROL_H

#include "defs.h"
#include "exerciseAbstract.h"

class ExerciseControl : public Exercise {
public:
	ExerciseControl();
	~ExerciseControl();

	void setupDisplay();
	QString exercisesDir();

private:
	QLabel *displayPitches;
	QLabel *displayAmplitude;
};
#endif

