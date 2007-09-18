#include <iostream>
using namespace std;

#include "intonation-try.h"
#include "Transcriber.h"

IntonationTry::IntonationTry()
{
	tryLayout = new QVBoxLayout();
	tryLayout->setContentsMargins(0,0,0,0);
	
    pitchPlot = new QtMarPlot();
	pitchPlot->setBackgroundColor(QColor(255,0,0));
    tryLayout->addWidget(pitchPlot);
    tryArea->setLayout(tryLayout);

	connect(pitchPlot, SIGNAL(clicked()), this, SLOT(clicked()));
}

IntonationTry::~IntonationTry()
{
}

void IntonationTry::setAnswer(const realvec answers)
{
	exerAnswer = answers;
}

bool IntonationTry::displayAnalysis(MarBackend *results) {

	realvec pitches = results->getMidiPitches();
	realvec amps = results->getAmplitudes();
	realvec bounds(2);
	bounds(0) = 0;
	bounds(1) = pitches.getSize();
	Transcriber::pitchSegment(pitches, bounds);
	realvec notes;
	notes = Transcriber::getNotes(pitches, amps, bounds);
	cout<<notes;

	realvec *data = new realvec;
	(*data) = pitches;

	pitchPlot->setData(data);
	pitchPlot->setVertical(57,73);
	pitchPlot->setPlotName("pitches");
	pitchPlot->setCenterLine(false);

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

