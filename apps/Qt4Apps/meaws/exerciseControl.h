#ifndef MEAWS_EXERCISE_CONTROL_H
#define MEAWS_EXERCISE_CONTROL_H

#include "defs.h"
#include "exerciseAbstract.h"

class ExerciseControl : public Exercise {
public:
	ExerciseControl();
	void setupDisplay();
	QString exercisesDir();

//private:
};
#endif

