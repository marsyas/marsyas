#include <iostream>
using namespace std;

#include "rhythm-exercise.h"
#include <QFile>
#include <QTextStream>
#include "Transcriber.h"


RhythmExercise::RhythmExercise() {
}

RhythmExercise::~RhythmExercise() {
}

int RhythmExercise::getBackend() {
	return (BACKEND_AMPLITUDES);
}

void RhythmExercise::open(QString exerciseFilename) {
	Exercise::open(exerciseFilename);

	// load exercise answers
	mrs_natural one, two;
	int i=0;
	exerAnswer.create(16);
	QString answerFilename = exerciseFilename;
	int size = answerFilename.size();
	answerFilename.replace(size-4,4,".txt");
	QFile answerFile(answerFilename);
	if (answerFile.open(QFile::ReadOnly))
	{
		QTextStream answerText(&answerFile);
		while (!answerText.atEnd())
		{
			answerText>>one>>two;
			//  not needed for rhythms
			//exerAnswer.stretchWrite(i,0,one);
			exerAnswer.stretchWrite(i,two);
			i++;
		}
	}
	answerFile.close();
	exerAnswer.stretch(i-1);
}

void RhythmExercise::addTry() {
	RhythmTry *newTry = new RhythmTry();
	newTry->setAnswer(exerAnswer);
	Exercise::addTryAbstract(newTry);
}

void RhythmExercise::delTry() {
	Exercise::delTryAbstract();
}

QString RhythmExercise::exercisesDir() {
	QString toReturn(MEAWS_DIR);
	toReturn.append("data/rhythm/");
	return toReturn;
}

QString RhythmExercise::getMessage() {
	if (currentTry_ < 0)
		return "";
	QString toReturn( "Selected attempt " + QString::number(currentTry_) );
	toReturn.append(". Score: " +
		QString::number( tries_->at(currentTry_)->getScore()) );
	toReturn.append("%");
	return toReturn;
}

bool RhythmExercise::displayAnalysis(MarBackend *results) {
	tries_->at(currentTry_)->displayAnalysis(results);
	return true;
}

