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

	fullPlot = new QtMarRhythmLines();
}

RhythmTry::~RhythmTry()
{
}

void RhythmTry::reset()
{
	Try::reset();
	pitchPlot->reset();
}

void RhythmTry::setAnswer(const realvec answers)
{
	exerAnswer = answers;
}

void RhythmTry::display(mrs_natural state)
{
	switch (state) {
	case ( TRY_NOTHING ):
		pitchPlot->setBackgroundColor(QColor(255,255,255));
		break;
	case ( TRY_SELECTED ):
		pitchPlot->setBackgroundColor(QColor(0,234,234));
		break;
	case ( TRY_BADNOTES ):
		pitchPlot->setBackgroundColor(QColor(255,0,0));
		break;
	}
}

void RhythmTry::calcScore(const realvec exerciseOnsets,
                    const realvec audioOnsets, mrs_natural& offset,
                    mrs_real& score)
{
//	cout<<"Notes found: "<<exerciseOnsets.getSize()-1<<endl;
//	cout<<"   expected: "<<audioOnsets.getSize()-1<<endl;
	mrs_real SCALE = 0.1;
	offset = 0;
	score = 0.0;
	for (mrs_natural j=0; j<audioOnsets.getSize()-1; j++)
	{
		mrs_real offsetPos = exerciseOnsets(j) - audioOnsets(j);
		mrs_real offsetScore = 100.0;
		for (mrs_natural i=0; i<exerciseOnsets.getSize()-1; i++)
		{
			mrs_real expected = exerciseOnsets(i);
			mrs_real detected = ( audioOnsets(i) + offsetPos);

			mrs_real noteScore = fabs( expected - detected );
			noteScore = pow(noteScore,1.5) * SCALE;
			//cout<<expected<<"\t"<<detected<<"\t"<<noteScore<<endl;
			offsetScore -= noteScore;
		}
		if (offsetScore > score)
		{
			score = offsetScore;
			offset = (mrs_natural) offsetPos;
			//cout<<"shifting by "<<offset<<" score: "<<offsetScore<<endl;
		}
		//cout<<"score: "<<offsetScore<<endl;
	}

}


mrs_natural
RhythmTry::calcOffsetAndScore(const realvec exerciseOnsets,
	realvec& audioOnsets)
{
//	cout<<"Notes found: "<<exerciseOnsets.getSize()-1<<endl;
//	cout<<"   expected: "<<audioOnsets.getSize()-1<<endl;

	mrs_natural testOffset;
	mrs_real testScore;

	calcScore(exerciseOnsets, audioOnsets, testOffset, testScore);
	if ( audioOnsets.getSize()-1 > exerciseOnsets.getSize()-1) {
		audioOnsets =
			audioOnsets.getSubVector(1,audioOnsets.getSize()-1);
		calcScore(exerciseOnsets, audioOnsets, testOffset, testScore);
	}
	if (testScore < 0)
		testScore = 0;
	score_ = testScore;
	return (mrs_natural) testOffset;
}

void RhythmTry::displayAnalysis(MarBackend *results)
{
	// get info from backend
	realvec amps = results->getAmplitudes();
	realvec bounds;
	Transcriber::ampSegment(amps, bounds);
	// shift the exercise times to match the beginning of audio exercise
	Transcriber::discardBeginEndSilencesAmpsOnly(amps, bounds);

	if (bounds.getSize() > exerAnswer.getSize() + 2) {
		display(TRY_BADNOTES);
	}
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

	cout<<offset<<endl;
	offset = (mrs_natural) (bounds(0) + offset);
	cout<<offset<<endl;

//	pitchPlot->setDetectedOffset(offset);
	pitchPlot->setData(data);

	pitchPlot->setHorizontal(offset,
		(mrs_natural) (exerAnswer(exerAnswer.getSize()-1)+offset));

	fullPlot->setData(amps);

	hasAudio_ = true;
}

void
RhythmTry::doubleclicked()
{
	fullPlot->show();
}

