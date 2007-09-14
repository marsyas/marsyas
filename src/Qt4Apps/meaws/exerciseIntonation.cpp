#include <iostream>
using namespace std;

#include "exerciseIntonation.h"
#include <QFile>
#include <QTextStream>
#include "Transcriber.h"



ExerciseIntonation::ExerciseIntonation() {
	resultLabel = NULL;
}

ExerciseIntonation::~ExerciseIntonation() {
	if (instructionImageLabel != NULL) {
		instructionLayout->removeWidget(instructionImageLabel);
		delete instructionImageLabel;
		instructionImageLabel = NULL;
	}
	if (resultLabel != NULL) {
		resultLayout->removeWidget(resultLabel);
		delete resultLabel;
		resultLabel = NULL;
	}
}

int ExerciseIntonation::getType() {
	return TYPE_INTONATION;
}

void ExerciseIntonation::open(QString exerciseFilename) {
	Exercise::open(exerciseFilename);

	// load exercise answers
	QString answerFile = exerciseFilename;
	int size = answerFile.size();
	answerFile.replace(size-4,4,".txt");
	cout<<qPrintable(answerFile)<<endl;
}

void ExerciseIntonation::setupDisplay() {
	resultLayout = new QVBoxLayout;

//	resultLabel = new QLabel;
//	resultLabel->setText("Intonation Exercise");
//	resultArea->addWidget(resultLabel,0,0);

	foo = new QtMarPlot();
	foo->setBackgroundColor(QColor(255,0,0));
	resultLayout->addWidget(foo);
	resultArea->setLayout(resultLayout);
}

QString ExerciseIntonation::exercisesDir() {
	QString toReturn(MEAWS_DIR);
	toReturn.append("data/intonation/");
	return toReturn;
}

QString ExerciseIntonation::getMessage() {
	// TODO: another totally fake demo for MISTIC.
	QString toReturn("Grade: 76\%");
	return toReturn;
}

bool ExerciseIntonation::displayAnalysis(MarBackend *results) {
	realvec pitches = results->getMidiPitches();
	realvec amps = results->getAmplitudes();

/*
	realvec *bounds = new realvec(2);
	(*bounds)(0) = 0;
	(*bounds)(1) = pitches.getSize();
	Transcriber::ignoreOctaves(&pitches);
	Transcriber::pitchSegment(&pitches, bounds);
	realvec *notes;
	notes = Transcriber::getNotes(&pitches, &amps, bounds);
	cout<<(*notes);
*/
	realvec *data = new realvec;
	(*data) = pitches;
	foo->setData(data);
	foo->setVertical(0,80);
	foo->setPlotName("pitches");

//	cout<<pitches<<endl;
//	cout<<amps<<endl;



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

