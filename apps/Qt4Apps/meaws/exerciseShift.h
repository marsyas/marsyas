#ifndef MEAWS_EXERCISE_SHIFT_H
#define MEAWS_EXERCISE_SHIFT_H

#include "defs.h"
#include "exerciseAbstract.h"
#include "../QtMarPlot.h"

class ExerciseShift : public Exercise {
public:
	ExerciseShift();
	~ExerciseShift();
	int getType();

	void setupDisplay();
	QString exercisesDir();
	QString getMessage();

	bool displayAnalysis(MarBackend *results);

private:
	mrs_real score;

	realvec pitches;
    QtMarPlot *displayPitches;
};
#endif

