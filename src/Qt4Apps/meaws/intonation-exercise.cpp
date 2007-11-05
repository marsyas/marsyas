#include <iostream>
using namespace std;

#include "intonation-exercise.h"
#include <QFile>
#include <QTextStream>
#include "Transcriber.h"

IntonationExercise::IntonationExercise() {
}

IntonationExercise::~IntonationExercise() {
}

int IntonationExercise::getBackend() {
	return BACKEND_PITCHES_AMPLITUDES;
}

void IntonationExercise::open(QString exerciseFilename) {
	Exercise::open(exerciseFilename);

	// load exercise answers
	mrs_natural one, two;
	int i=0;
	exerAnswer.create(16,2);
	//exerAnswer.create(100,2);
	QString loadFilename;
	loadFilename = exerciseFilename;
	loadFilename.replace(loadFilename.size()-4,4,".txt");
	QFile loadFile(loadFilename);

	if (loadFile.open(QFile::ReadOnly))
	{
		QTextStream answerText(&loadFile);
		while (!answerText.atEnd())
		{
			answerText>>one>>two;
			exerAnswer.stretchWrite(i,0,one);
			exerAnswer.stretchWrite(i,1,two);
			i++;
		}
	}
	loadFile.close();
	exerAnswer.stretch(i,2);


	//  TODO: probably remove.
	// **** read lilypond input
	loadFilename = exerciseFilename;
	loadFilename.replace(loadFilename.size()-4,4,".ly");
	loadFile.setFileName(loadFilename);

    loadFile.open(QIODevice::ReadOnly | QIODevice::Text);
    lily_input = (QTextStream(&loadFile).readAll()).split('\n');
    loadFile.close();

}

void IntonationExercise::addTry() {
	IntonationTry *newTry = new IntonationTry();
	newTry->setAnswer(exerAnswer);
	newTry->setLily(lily_input);
	Exercise::addTryAbstract(newTry);
}

void IntonationExercise::delTry() {
	Exercise::delTryAbstract();
}

QString IntonationExercise::exercisesDir() {
	QString toReturn(MEAWS_DIR);
	toReturn.append("data/intonation/");
	return toReturn;
}

QString IntonationExercise::getMessage() {
	QString toReturn( "Selected attempt " + QString::number(currentTry_) );
	return toReturn;
}

bool IntonationExercise::displayAnalysis(MarBackend *results) {
	tries_->at(currentTry_)->displayAnalysis(results);
	return true;
}

