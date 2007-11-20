#ifndef MEAWS_CHOOSE_EXERCISE_H
#define MEAWS_CHOOSE_EXERCISE_H

#include "defs.h"
#include "abstract-exercise.h"

#include "rhythm-exercise.h"
#include "intonation-exercise.h"
//#include "exerciseControl.h"
//#include "exerciseShift.h"

#include <QInputDialog>
#include <QFileDialog>

class ChooseExercise: public QDialog
{
	Q_OBJECT

public:
	ChooseExercise() {};

	static Exercise* chooseType();
	static QString chooseFile(QString dir);
	static QString chooseAttempt();

};
#endif

