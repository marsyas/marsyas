#include <iostream>
using namespace std;

#include "exerciseIntonation.h"
#include <QFile>
#include <QTextStream>

ExerciseIntonation::ExerciseIntonation() {
	resultLabel = NULL;
}

ExerciseIntonation::~ExerciseIntonation() {
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

int ExerciseIntonation::getType() {
	return TYPE_INTONATION;
}

void ExerciseIntonation::setupDisplay() {
	resultLabel = new QLabel;
	resultLabel->setText("Intonation Exercise");

	resultArea->addWidget(resultLabel);
}

QString ExerciseIntonation::exercisesDir() {
	QString toReturn(MEAWS_DIR);
	toReturn.append("data/intonation/");
	return toReturn;
}

void ExerciseIntonation::displayAnalysis(MarBackend *results) {
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

	system("/Users/gperciva/progs/python/libbabelpond/reldurs.py /tmp/notes.txt");
	sleep(1);
	system("cd /tmp; lilypond -dpreview tempscore.ly");
	sleep(5);

	resultLabel->setPixmap(QPixmap::fromImage(QImage("/tmp/tempscore.preview.png")));

}


