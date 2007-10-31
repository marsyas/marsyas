#include <iostream>
using namespace std;

#include "rhythm-try.h"
#include "Transcriber.h"

RhythmTry::RhythmTry()
{
	tryLayout_ = new QVBoxLayout();
	tryLayout_->setContentsMargins(0,0,0,0);
	tryArea_->setLayout(tryLayout_);

	pitchPlot = new QtMarPlot();
	tryLayout_->addWidget(pitchPlot);

}

RhythmTry::~RhythmTry()
{
}

void RhythmTry::setAnswer(const realvec answers)
{
	exerAnswer = answers;
}

void RhythmTry::setLily(const QStringList lilyInput)
{
	lilyInput_ = QStringList( lilyInput );
}

void RhythmTry::colorNote(int note, double error, double direction)
{
	int line=note+3;
	QString color = "black";

	if (error < -0.002)
		color = "Medium Blue";
	if (error < -0.006)
		color = "Dodger Blue";
	if (error < -0.02)
		color = "Light Sky Blue";

	if (error > 0.002)
		color = "Light Salmon";
	if (error > 0.006)
		color = "tomato";
	if (error > 0.02)
		color = "red";

	color.insert(0,"\\colorNote #\"");
	color.append("\" ");
	QString lily_line = lilyInput_.at(line);
	lily_line.insert(0,color);

	if (color != "\\colorNote #\"black\" ")
	{
		if (direction<0)
			color="^\\down";
		if (direction>0)
			color="_\\up";
		lily_line.append(color);
	}

	lilyInput_.replace(line,lily_line);
}

void RhythmTry::calcErrors(const realvec& pitches, const realvec&
                               bounds)
{
	mrs_real expected;
	realvec notePitches;
	mrs_natural noteStart, noteLength;
	mrs_real noteError;
	mrs_real deltaError;

	mrs_natural exerNote, i;
	for (exerNote=0; exerNote < exerAnswer.getRows()-1; exerNote++)
	{
		// find the boundaires of the note
		i = exerNote;
		while ( (bounds(i) < exerAnswer(exerNote,1)) &&
		        (i < bounds.getSize()) )
			i++;
		noteStart = (mrs_natural) bounds(i);
		i = exerNote;

		while ( (bounds(i) <= exerAnswer(exerNote+1,1)) &&
		        (i < bounds.getSize()) )
			i++;
		noteLength = (mrs_natural) (bounds(i) - noteStart);


		notePitches = pitches.getSubVector(noteStart, noteLength);
		/*
				cout<<"note "<<exerNote<<"\t"<<noteStart<<"\t"<<noteLength+noteStart<<endl;
				cout<<"\t"<<Transcriber::findMedianWithoutZeros(0,notePitches.getSize(),notePitches)<<"\t"<<exerAnswer(exerNote,0)<<endl;
				cout<<endl;
		*/
		expected = exerAnswer(exerNote,0);
		noteError = 0;
		for (i=0; i<notePitches.getSize(); i++)
		{
			if (notePitches(i) == 0)
				continue;
			deltaError = notePitches(i)-expected;
			deltaError = fmod( deltaError, 12);
			if (deltaError < -6)
				deltaError += 12;
			if (deltaError > 6)
				deltaError -= 12;
			noteError += deltaError;
//			cout<<notePitches(i)<<"\t"<<deltaError<<endl;
		}
		// normalize display of error: 1.0= 1/4 tone wrong.
		noteError = noteError / (6.0*noteLength);
		//cout<<exerNote<<" "<<noteError<<endl;

// TODO: fix direction of error
		colorNote(exerNote,noteError,noteError);
	}


	// WRITE LILYPOND FILE OUT
	// FIXME: filename
	QString temp;
	QFile out_file("/tmp/out.ly");
	out_file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&out_file);

	for (i = 0; i < lilyInput_.size(); ++i)
	{
		temp = lilyInput_.at(i);
		out<<qPrintable(temp)<<endl;
	}
	out_file.close();
}


bool RhythmTry::displayAnalysis(MarBackend *results)
{
	//cout<<"LOLCAT SEZ: im in ur beat analysis"<<endl;

// get info from backend
	realvec amps = results->getAmplitudes();
	realvec bounds;
	Transcriber::ampSegment(amps, bounds);
	// shift the exercise times to match the beginning of audio exercise
	Transcriber::discardBeginEndSilencesAmpsOnly(amps, bounds);
	//cout<<"divided"<<endl;

	mrs_natural start = (mrs_natural) bounds(0);
	mrs_natural length = (mrs_natural) (bounds(bounds.getSize()-1)
- bounds(0));
	realvec *data = new realvec;
	(*data) = amps.getSubVector(start,length);
	
	pitchPlot->setVertical(0,1);
	pitchPlot->setPlotName("onsets");
	pitchPlot->setCenterLine(false);
	pitchPlot->setImpulses(true);
	//cout<<"plot done"<<endl;

	mrs_natural exerLength = (mrs_natural) exerAnswer(exerAnswer.getRows()-1,1);

	mrs_natural j=0;
	realvec* answerVec = new realvec(exerLength);
	for (int i=0; i<exerLength; i++)
		if ( i==(exerAnswer(j,1)) ) {
			j++;
			(*answerVec)(i)=1.0;
		} else {
			(*answerVec)(i)=0.0;
		}
	data->stretch(answerVec->getSize());

	pitchPlot->setData(data);
	pitchPlot->setOtherData(answerVec);
//	cout<<(*answerVec);

	for (int i=0; i<exerAnswer.getRows(); i++)
		exerAnswer(i,1) = exerAnswer(i,1) + bounds(0);
/*

	calcErrors(pitches, bounds);

	pitchPlot->setData(data);
	pitchPlot->setVertical(57,73);
	pitchPlot->setPlotName("pitches");
	pitchPlot->setCenterLine(false);
*/
//	graph->setBuffer(*data);

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
	#endif
	*/

/*
	system("cd /tmp; lilypond -dpreview out.ly");

	QLabel* resultLabel = new QLabel;
	resultLabel->setPixmap(QPixmap::fromImage(QImage("/tmp/out.preview.png")));
	tryLayout->addWidget(resultLabel);
*/
	return true;
}

/*
void
RhythmTry::doubleclicked()
{
//	pitchPlot->show();
//	graph->show();
}
*/

