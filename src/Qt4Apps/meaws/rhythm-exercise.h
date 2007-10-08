#ifndef MEAWS_EXERCISE_RHYTHM_H
#define MEAWS_EXERCISE_RHYTHM_H

#include "defs.h"
#include "abstract-exercise.h"
#include "rhythm-try.h"
#include <QButtonGroup>


class RhythmExercise : public Exercise {
public:
	RhythmExercise();
	~RhythmExercise();

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
//	QList<RhythmTry *> *tries;

//	mrs_natural current_;
	QStringList lily_input;
};
#endif

