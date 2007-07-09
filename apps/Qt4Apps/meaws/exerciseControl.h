#ifndef MEAWS_EXERCISE_CONTROL_H
#define MEAWS_EXERCISE_CONTROL_H

#include "defs.h"
#include "exerciseAbstract.h"
#include <string>
#include <QToolButton>
#include <QButtonGroup>

//temporary  ??? [ML] for how long ;-)
#include "../QtMarPlot.h"

typedef enum {
	straightPiano,
	straightMezzo,
	straightForte,
	crescendoDecrescendo,
	vibrato
} exerciseControlType ;

class ExerciseControl : public Exercise {
	Q_OBJECT

public:
	ExerciseControl();
	~ExerciseControl();
	int getType();

	void setupDisplay();
	QString exercisesDir();
	QString getMessage();

	bool displayAnalysis(MarBackend *results);
	void open(QString exerciseFilename);  // overloaded from Abstract

public slots:
	void setNote(int noteNumber);

private:
	void evaluatePerformance(MarBackend *results, exerciseControlType type);
	mrs_real evaluateStraight(realvec &vec, realvec &weight);
	mrs_real evaluateCrescendoDecrescendo(realvec &vec, realvec &weight);
	mrs_real evaluateVibrato(realvec &vec, realvec &weight);
	mrs_real slidingWeightedDeviation(realvec &vec, realvec &weight);
    mrs_real weightedDeviation(realvec &vec, realvec &weight);

//	QLabel *displayPitches;
//	QLabel *displayAmplitude;

// temporary
    QtMarPlot *displayPitches;
    QtMarPlot *displayAmplitudes;
	realvec myPitches;
	realvec myAmplitudes;
    realvec myWeight;

	mrs_natural hopSize;

	mrs_real pitchError;
	mrs_real amplitudeError;

	std::string resultString;


	QButtonGroup *notes;
	QToolButton **noteButton;

};
#endif

