#include <iostream>
using namespace std;

#include "intonation-try.h"
#include "Transcriber.h"

IntonationTry::IntonationTry()
{
	tryLayout_ = new QVBoxLayout();
	tryLayout_->setContentsMargins(0,0,0,0);
	tryArea_->setLayout(tryLayout_);

	pitchPlot = new QtMarIntonationBars();
	pitchPlot->setBackgroundColor(QColor(255,255,255));
	tryLayout_->addWidget(pitchPlot);
}

IntonationTry::~IntonationTry()
{
}

void IntonationTry::setAnswer(const realvec answers)
{
	exerAnswer = answers;
}

void IntonationTry::setLily(const QStringList lilyInput)
{
	lilyInput_ = QStringList( lilyInput );
}

void IntonationTry::colorNote(int note, double error, double direction)
{
	int line=note+10;
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


// format of errors:
// [ absolute intonation mistake, end frame, error direction ]
void IntonationTry::calcErrors(const realvec& pitches, const realvec&
                               bounds, realvec& errors)
{
	errors.allocate(exerAnswer.getRows()-1,3);
	mrs_real expected;
	realvec notePitches;
	mrs_natural noteStart, noteLength;
	mrs_real noteError;
	mrs_real noteDirectionError;
	mrs_real deltaError;

	mrs_natural exerNote, i;
	for (exerNote=0; exerNote < exerAnswer.getRows()-1; exerNote++)
	{
		// find the boundaires of the note
		i = exerNote;
		while ( (bounds(i) < exerAnswer(exerNote,1)+bounds(0)) &&
		        (i < bounds.getSize()) )
			i++;
		noteStart = (mrs_natural) bounds(i);
		i = exerNote;

		while ( (bounds(i) <= exerAnswer(exerNote+1,1)+bounds(0)) &&
		        (i < bounds.getSize()) )
			i++;
		noteLength = (mrs_natural) (bounds(i) - noteStart);


		notePitches = pitches.getSubVector(noteStart, noteLength);

		expected = exerAnswer(exerNote,0);
		noteError = 0;
		noteDirectionError = 0;
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
			//    I don't understand this, and it's my own code.
			//    :(  -gp
			// normalize display of error: 1.0 = 1/4 tone wrong.
			deltaError /= (6.0*noteLength);

			noteError += fabs(deltaError);
			noteDirectionError += deltaError;
//			cout<<notePitches(i)<<"\t"<<deltaError<<endl;
		}

		// set note error
		errors(exerNote,0) = noteError;

		// set onset
		errors(exerNote,1) = noteStart - bounds(0) + noteLength -1;

		//cout<<noteError<<"\t"<<noteDirectionError<<endl;
		// set note direction
		if (noteDirectionError < -0.5*noteError)
			errors(exerNote,2) = -1;
		else if (noteDirectionError > 0.5*noteError)
			errors(exerNote,2) = 1;
		else
			errors(exerNote,2) = 0;

		//colorNote(exerNote,noteError,noteError);
	}

/*
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
*/
//	cout<<errors;
//	return errors;
}


bool IntonationTry::displayAnalysis(MarBackend *results)
{
// get info from backend
	realvec pitches = results->getMidiPitches();
	realvec amps = results->getAmplitudes();
	realvec bounds;
	Transcriber::pitchSegment(pitches, bounds);
	// shift the exercise times to match the beginning of audio exercise
	Transcriber::discardBeginEndSilences(pitches, amps, bounds);
	calcErrors(pitches, bounds, errors);

	pitchPlot->setBarData(&errors);
	//cout<<errors;

/*
	for (int i=0; i<exerAnswer.getRows(); i++)
		exerAnswer(i,1) = exerAnswer(i,1) + bounds(0);


	cout<<errors;
*/

/*
	realvec *data = new realvec;
	(*data) = pitches;

//	pitchPlot->setData(data);
//	pitchPlot->setVertical(57,73);
	pitchPlot->setPlotName("pitches");
	pitchPlot->setCenterLine(true);

	pitchPlot->setBars(true);

    mrs_natural exerLength = (mrs_natural)
		exerAnswer(exerAnswer.getRows()-1,1);

    mrs_natural j=0;
    realvec* answerVec = new realvec(exerLength);
    for (mrs_natural i=0; i<exerLength; i++)
        if ( i==(exerAnswer(j,1)) ) {
            j++;
            (*answerVec)(i)=1.0;
        } else {
            (*answerVec)(i)=0.0;
        }
    data->stretch(answerVec->getSize());
//	cout<<(*answerVec)<<endl;
//	pitchPlot->setOtherData(answerVec);
	cout<<errors;
	errors.stretch(errors.getSize(), 3);
	cout<<errors;
	//pitchPlot->setData(&errors);
*/
// LILYPOND STUFF
/*
	system("cd /tmp; lilypond -dpreview out.ly");

	QLabel* resultLabel = new QLabel;
	resultLabel->setPixmap(QPixmap::fromImage(QImage("/tmp/out.preview.png")));
	tryLayout_->addWidget(resultLabel);
*/
	return true;
}

void
IntonationTry::doubleclicked()
{
	pitchPlot->show();
//	graph->show();
}


