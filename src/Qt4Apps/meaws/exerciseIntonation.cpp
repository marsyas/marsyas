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
	mrs_natural one, two;
	mrs_natural frameSum=0;
	mrs_natural frame;
	int i=0;
	exerAnswer.create(100,2);
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
			exerAnswer(i,0) = one;
			exerAnswer(i,1) = two;
//			exerAnswer.stretchWrite(i,0,one);
//			exerAnswer.stretchWrite(i,1,two);
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

void ExerciseIntonation::setupDisplay() {
	resultLayout = new QVBoxLayout;

//	resultLabel = new QLabel;
//	resultLabel->setText("Intonation Exercise");
//	resultArea->addWidget(resultLabel,0,0);

	foo = new QtMarPlot();
	foo->setBackgroundColor(QColor(255,0,0));
	resultLayout->addWidget(foo);
	resultArea->setLayout(resultLayout);

	Try *newTry = new Try();
	tries = new QList<Try *>;
	tries->append(newTry);

	resultLayout->addWidget( (tries->takeFirst())->getDisplay() );
//zz
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
	realvec bounds(2);
	bounds(0) = 0;
	bounds(1) = pitches.getSize();
	Transcriber::pitchSegment(pitches, bounds);
	realvec notes;
	notes = Transcriber::getNotes(pitches, amps, bounds);
//	cout<<notes;

	realvec *data = new realvec;
	(*data) = pitches;
	foo->setData(data);
	foo->setVertical(0,80);
	foo->setPlotName("pitches");

	realvec mistakes;
	mistakes.create(exerAnswer.getRows());
	mrs_natural expected;
	mrs_real detected;
	mrs_real mistake;
	int j=0;
	int start;
	for (int i=0; i<notes.getRows(); i++)
	{
		start = (mrs_natural) notes(i,1);
		while ( exerAnswer(j,1) <= notes(i,1) )
		{
			if (j>exerAnswer.getRows()-1)
				break;
			expected = (mrs_natural) exerAnswer(j,0);
			//cout<<"Correct: "<<exerAnswer(j,0)<<"  "<<exerAnswer(j,1)<<endl;
			j++;
		}
		//cout<<"\t"<<notes(i,0)<<" "<<notes(i,1)<<endl;
		detected = notes(i,0);
		mistake = fmod(detected,12.0) - (expected % 12);
		if (mistake > 6)
			mistake -= 12.0;
		//cout<<"**** "<<j<<"   "<<mistake<<endl;
		if (j>exerAnswer.getRows()-1)
			break;
		mistakes(j) += mistake;
	}
	cout<<mistakes;

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

