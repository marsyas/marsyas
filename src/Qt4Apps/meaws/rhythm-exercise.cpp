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
	exerAnswer.create(16,2);
	//exerAnswer.create(100,2);
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
//			exerAnswer(i,0) = one;
//			exerAnswer(i,1) = two;
			exerAnswer.stretchWrite(i,0,one);
			exerAnswer.stretchWrite(i,1,two);
			i++;
		}
	}
	answerFile.close();
	exerAnswer.stretch(i-1,2);
/*
	for (i=0; i<exerAnswer.getRows(); i++)
		exerAnswer(i,1) =
			(mrs_natural) (exerAnswer(i,1)*44100.0/512.0/384.00);
	cout<<exerAnswer;
*/

	// **** read lilypond input
	// FIXME: filename
	QString lilyFile(MEAWS_DIR);
	lilyFile.append("data/rhythm/scale.ly");
    QFile in_file(lilyFile);
    in_file.open(QIODevice::ReadOnly | QIODevice::Text);
    lily_input = (QTextStream(&in_file).readAll()).split('\n');
    in_file.close();
/*
	QString temp;
    for (int i = 0; i < lily_input.size(); ++i) {
        temp = lily_input.at(i);
        cout<<qPrintable(temp)<<endl;
    }
*/
}

void RhythmExercise::addTry() {
	RhythmTry *newTry = new RhythmTry();
	newTry->setAnswer(exerAnswer);
	newTry->setLily(lily_input);
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
	QString toReturn( "Selected attempt " + QString::number(current_) );
	return toReturn;
}

bool RhythmExercise::displayAnalysis(MarBackend *results) {
	tries_->at(current_)->displayAnalysis(results);
	return true;
}

