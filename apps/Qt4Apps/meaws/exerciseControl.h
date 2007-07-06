#ifndef MEAWS_EXERCISE_CONTROL_H
#define MEAWS_EXERCISE_CONTROL_H

#include "defs.h"
#include "exerciseAbstract.h"

//temporary
#include "../QtMarPlot.h"

class ExerciseControl : public Exercise {
public:
	ExerciseControl();
	~ExerciseControl();
	int getType();

	void setupDisplay();
	QString exercisesDir();
	QString getMessage();

	bool displayAnalysis(MarBackend *results);

private:
//	QLabel *displayPitches;
//	QLabel *displayAmplitude;

// temporary
    QtMarPlot *displayPitches;
    QtMarPlot *displayAmplitude;
	realvec myPitches;
};
#endif

