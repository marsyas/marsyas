#ifndef MEAWS_EXERCISE_DISPATCHER_H
#define MEAWS_EXERCISE_DISPATCHER_H

#include "defs.h"
#include "exerciseIntonation.h"
//#include <QObject>
#include <QDialog>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QtGui>

class ExerciseDispatcher : public QDialog {
	Q_OBJECT

public:
	ExerciseDispatcher();
	~ExerciseDispatcher();
	
public slots:
	void open();
	void setInstructionArea(QGridLayout *getInstructionArea);
	void setResultArea(QGridLayout *getResultArea);

signals:
	void enableActions(int state);

private:
	QString exerciseName;
	QGridLayout *instructionArea;
	QGridLayout *resultArea;

	QLabel *imageLabel;
};
#endif

