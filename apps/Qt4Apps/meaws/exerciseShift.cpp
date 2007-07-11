#include <iostream>
using namespace std;

#include "exerciseShift.h"
#include <QFile>
#include <QTextStream>



ExerciseShift::ExerciseShift() {
	resultLabel = NULL;
}

ExerciseShift::~ExerciseShift() {
	if (instructionImageLabel != NULL) {
		instructionArea->removeWidget(instructionImageLabel);
		delete instructionImageLabel;
		instructionImageLabel = NULL;
	}
	if (resultLabel != NULL) {
		resultArea->removeWidget(resultLabel);
		delete resultLabel;
		resultLabel = NULL;
	}
}

int ExerciseShift::getType() {
	return TYPE_SHIFT;
}

void ExerciseShift::setupDisplay() {
	resultLabel = new QLabel;
	resultLabel->setText("Shifting Exercise");

	resultArea->addWidget(resultLabel);
}

QString ExerciseShift::exercisesDir() {
	QString toReturn(MEAWS_DIR);
	toReturn.append("data/shift/");
	return toReturn;
}

QString ExerciseShift::getMessage() {
	// TODO: another totally fake demo for MISTIC.
	QString toReturn("Grade: 76\%");
	return toReturn;
}

bool ExerciseShift::displayAnalysis(MarBackend *results) {
	// TODO: this is a totally fake demo for the MISTIC talk.
	resultLabel->setPixmap(QPixmap::fromImage(QImage(MEAWS_DIR+"data/scale1.preview.png")));


/*
	realvec durations = results->getDurations();
	realvec notes = results->getNotes();

	QFile out_file("/tmp/notes.txt");
	out_file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&out_file);

	for (int i=0; i<durations.getSize(); i++) {
		if (notes(i)>0) {
			out<<(int) floor(notes(i)+0.5)<<"\t"<<durations(i)<<endl;;
			cout<<(int) floor(notes(i)+0.5)<<"\t"<<durations(i)<<endl;;
		}
	}
	out_file.close();
*/

/*
#ifndef MARSYAS_WIN32 // [ML] this is ugly and sleep does not exist in Win32 !!
	system("/Users/gperciva/progs/python/libbabelpond/reldurs.py /tmp/notes.txt");
	sleep(1);
	system("cd /tmp; lilypond -dpreview tempscore.ly");
	sleep(5);
#endif

	resultLabel->setPixmap(QPixmap::fromImage(QImage("/tmp/tempscore.preview.png")));
*/
	return true;
}

