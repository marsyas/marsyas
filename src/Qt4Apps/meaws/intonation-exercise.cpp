#include <iostream>
using namespace std;

#include "intonation-exercise.h"
#include <QFile>
#include <QTextStream>
#include "Transcriber.h"


IntonationExercise::IntonationExercise() {
	tries = new QList<IntonationTry *>;

	resultGroup = new QButtonGroup;

	nextNumber = 0;
}

IntonationExercise::~IntonationExercise() {
	delete tries;
}

int IntonationExercise::getType() {
	return TYPE_INTONATION;
}

void IntonationExercise::open(QString exerciseFilename) {
	Exercise::open(exerciseFilename);

	// load exercise answers
	mrs_natural one, two;
	mrs_natural frameSum=0;
	mrs_natural frame;
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
	for (i=0; i<exerAnswer.getRows(); i++)
	{
		frame = (mrs_natural) ( exerAnswer(i,1)*44100.0/512.0 /2.0);
		exerAnswer(i,1) = frameSum;
		frameSum += frame;
	}
//	cout<<exerAnswer;
}

void IntonationExercise::addTry() {
	IntonationTry *newTry = new IntonationTry();
	resultLayout->addWidget( newTry->getDisplay() );
	tries->append(newTry);
	tries->at(0)->setAnswer(exerAnswer);

	nextNumber++;
	newTry->getPlot()->setReportNumber(nextNumber);

	connect(newTry->getPlot(), SIGNAL(report(mrs_natural)),
		this, SLOT(button(mrs_natural)));
}

void IntonationExercise::button(mrs_natural selected)
{
	cout<<selected<<endl;
	//cout<<resultGroup->checkedButton()<<endl;
}

QString IntonationExercise::exercisesDir() {
	QString toReturn(MEAWS_DIR);
	toReturn.append("data/intonation/");
	return toReturn;
}

QString IntonationExercise::getMessage() {
	// TODO: another totally fake demo for MISTIC.
	QString toReturn("Grade: 76\%");
	return toReturn;
}

bool IntonationExercise::displayAnalysis(MarBackend *results) {
	tries->at(0)->displayAnalysis(results);
	addTry();
	return true;
}

