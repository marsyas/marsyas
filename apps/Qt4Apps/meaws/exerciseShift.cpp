#include <iostream>
using namespace std;

#include "exerciseShift.h"
#include <QFile>
#include <QTextStream>



ExerciseShift::ExerciseShift() {
	score = 0;
    displayPitches = NULL;
}

ExerciseShift::~ExerciseShift() {
	if (instructionImageLabel != NULL) {
		instructionArea->removeWidget(instructionImageLabel);
		delete instructionImageLabel;
		instructionImageLabel = NULL;
	}
    if (displayPitches != NULL) {
        resultArea->removeWidget(displayPitches);
        delete displayPitches;
        displayPitches = NULL;
    }
}

int ExerciseShift::getType() {
	return TYPE_SHIFT;
}

void ExerciseShift::setupDisplay() {
    displayPitches = new QtMarPlot();
    displayPitches->setPlotName("Shifting Pitches");
    displayPitches->setBackgroundColor(QColor(255,255,255));
    displayPitches->setPixelWidth(2);

	resultArea->addWidget(displayPitches);
}

QString ExerciseShift::exercisesDir() {
	QString toReturn(MEAWS_DIR);
	toReturn.append("data/shift/");
	return toReturn;
}

QString ExerciseShift::getMessage() {
	// TODO: another totally fake demo for MISTIC.
	QString toReturn("Grade: " + QString::number(score) + "\%");
	return toReturn;
}

bool ExerciseShift::displayAnalysis(MarBackend *results) {
	pitches = results->getMidiPitches();
	mrs_real sum = 0.0;
	mrs_natural count = 0;
	for (mrs_natural i=0; i<pitches.getSize(); i++) {
		if (pitches(i)>70) {
			sum += pitches(i);
			count++;
		}
	}
	mrs_real average = sum/count;
	sum = 0.0;
	count++;
	for (mrs_natural i=0; i<pitches.getSize(); i++) {
		pitches(i) = pitches(i)-average;
		if (pitches(i)>-3) {
			sum += fabs( pitches(i) );
			count++;
		}
		//cout<<pitches(i)<<endl;
	}
	displayPitches->setVertical(-1,1);
    displayPitches->setData( &pitches );

	score = 100*( 1 - (sum)/(count*0.5) );

	return true;
}

