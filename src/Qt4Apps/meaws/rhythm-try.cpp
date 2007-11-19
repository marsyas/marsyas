#include <iostream>
using namespace std;

#include "rhythm-try.h"
#include "Transcriber.h"

RhythmTry::RhythmTry()
{
	tryLayout_ = new QVBoxLayout();
	tryLayout_->setContentsMargins(0,0,0,0);
	tryArea_->setLayout(tryLayout_);

	pitchPlot = new QtMarRhythmLines();
	tryLayout_->addWidget(pitchPlot);

	score_ = -1;
}

RhythmTry::~RhythmTry()
{
}

void RhythmTry::setAnswer(const realvec answers)
{
	exerAnswer = answers;
}

void RhythmTry::selected(bool selected)
{
	if (selected) {
		pitchPlot->setBackgroundColor(QColor(0,234,234));
	} else {
		pitchPlot->setBackgroundColor(QColor(255,255,255));
	}
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

mrs_natural
RhythmTry::calcOffsetAndScore(realvec exerciseOnsets, realvec audioOnsets)
{
	mrs_real SCALE = 0.1;
	mrs_real bestOffset = 0;

	for (mrs_natural j=0; j<audioOnsets.getSize()-1; j++) {
		mrs_real offset = exerciseOnsets(j) - audioOnsets(j);
		mrs_real offsetScore = 100.0;
		for (mrs_natural i=0; i<exerciseOnsets.getSize()-1; i++) {
			mrs_real expected = exerciseOnsets(i);
			mrs_real detected = ( audioOnsets(i) + offset);

			mrs_real noteScore = fabs( expected - detected );
			noteScore = pow(noteScore,1.5) * SCALE;
			//cout<<expected<<"\t"<<detected<<"\t"<<noteScore<<endl;
			offsetScore -= noteScore;
		}
		if (offsetScore > score_) {
			score_ = offsetScore;
			bestOffset = offset;
			//cout<<"shifting by "<<offset<<" score: "<<offsetScore<<endl;
		}
		//cout<<"score: "<<offsetScore<<endl;
	}
	if (score_ < 0)
		score_ = 0;
	return (mrs_natural) bestOffset;
}

void RhythmTry::displayAnalysis(MarBackend *results)
{
	// get info from backend
	realvec amps = results->getAmplitudes();
	realvec bounds;
	Transcriber::ampSegment(amps, bounds);
	// shift the exercise times to match the beginning of audio exercise
	Transcriber::discardBeginEndSilencesAmpsOnly(amps, bounds);

	cout<<"Notes founds: "<<bounds.getSize()-1<<endl;
	cout<<"    expected: "<<exerAnswer.getSize()-1<<endl;
	// shift detected onsets to produce highest score
	mrs_natural offset = calcOffsetAndScore(exerAnswer, bounds);

	// display expected onsets
	pitchPlot->setExpectedLines(exerAnswer);

	// display the detected onsets
	mrs_natural start = (mrs_natural) bounds(0)-1;
	if (start < 0)
		start = 0;
	mrs_natural length = (mrs_natural) (bounds(bounds.getSize()-1)
- bounds(0));
	realvec data = amps.getSubVector(start,length);
	offset = (mrs_natural) ( -1*(offset + bounds(0)) );
	pitchPlot->setDetectedOffset(offset);
	pitchPlot->setData(data);

}

/*
void
RhythmTry::doubleclicked()
{
//	pitchPlot->show();
//	graph->show();
}
*/


mrs_real
RhythmTry::getScore() {
	return score_;
}

