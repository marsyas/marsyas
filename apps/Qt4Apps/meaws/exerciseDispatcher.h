#ifndef MEAWS_EXERCISE_DISPATCHER_H
#define MEAWS_EXERCISE_DISPATCHER_H

#include "defs.h"
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
	void setInstructionArea(QLayout *getInstructionArea);
	void setResultArea(QLayout *getResultArea);

signals:
	void enableActions(int state);

private:
	QString exerciseName;
	QLayout *instructionArea;
	QLayout *resultArea;

	QLabel *imageLabel;
};
#endif

