#include <iostream>
using namespace std;

#include "exerciseControl.h"

#include "PowerSpectrum.h"
#include "Power.h"

#include <sstream>

ExerciseControl::ExerciseControl() {
	hopSize = 8;
}

ExerciseControl::~ExerciseControl() {
	instructionArea->removeWidget(instructionImageLabel);
	delete (instructionImageLabel);;

	resultArea->removeWidget(displayPitches);
	delete displayPitches;
	resultArea->removeWidget(displayAmplitudes);
	delete displayAmplitudes;
}

int ExerciseControl::getType() {
	return TYPE_CONTROL;
}

void ExerciseControl::setupDisplay() {
	displayPitches = new QtMarPlot();
	displayPitches->setPlotName("Pitch");
	displayPitches->setBackgroundColor(QColor(255,255,255));
	displayPitches->setPixelWidth(2);
	displayAmplitudes = new QtMarPlot();
	displayAmplitudes->setPlotName("Amplitude");
	displayAmplitudes->setBackgroundColor(QColor(255,255,255));
	displayAmplitudes->setPixelWidth(2);
	/*
	displayPitches = new QLabel;
	displayAmplitude = new QLabel;
	displayPitches->setText("Display Pitches here");
	displayAmplitude->setText("Display Ampitudes here");
	resultArea->addWidget(displayPitches,0,0);
	resultArea->addWidget(displayAmplitude,0,1);
	*/
	//QHBoxLayout *displayLayout = new QHBoxLayout;
	resultArea->addWidget(displayPitches,0,0);
	resultArea->addWidget(displayAmplitudes,0,1);
	//resultArea->addLayout(displayLayout);

	//	resultsDisplay = new MeawsDisplay();
	//	mainLayout->addLayout(resultsDisplay);
}


QString ExerciseControl::exercisesDir() {
	QString toReturn(MEAWS_DIR);
	toReturn.append("data/control/");
	return toReturn;
}

QString ExerciseControl::getMessage() {
	QString toReturn(resultString.c_str());
	return toReturn;
}

bool ExerciseControl::displayAnalysis(MarBackend *results) {
	myPitches = results->getPitches();	
	displayPitches->setVertical(0,200);
	displayPitches->setData( &myPitches );

	myAmplitudes = results->getAmplitudes();	
	displayAmplitudes->setVertical(0,200);
	displayAmplitudes->setData( &myAmplitudes);

	evaluatePerformance(results, straightMezzo);

	std::stringstream ss;
	ss << "Results: " << pitchError << " " << amplitudeError;
	resultString = ss.str();

	return 0 ;
}

void ExerciseControl::evaluatePerformance(MarBackend *results, exerciseControlType type) {
	switch(type)
	{
	case straightPiano:
		pitchError = evaluateStraight(myPitches, myAmplitudes);
		amplitudeError = evaluateStraight(myAmplitudes, myAmplitudes);
		break;
	case straightMezzo:
		pitchError = evaluateStraight(myPitches, myAmplitudes);
		amplitudeError = evaluateStraight(myAmplitudes, myAmplitudes);
		break;
	case straightForte:
		pitchError = evaluateStraight(myPitches, myAmplitudes);
		amplitudeError = evaluateStraight(myAmplitudes, myAmplitudes);
		break;
	case crescendoDecrescendo:
		pitchError = evaluateStraight(myPitches, myAmplitudes);
		amplitudeError = evaluateCrescendoDecrescendo(myAmplitudes, myAmplitudes);
		break;
	case vibrato:
		pitchError = evaluateVibrato(myPitches, myAmplitudes);
		amplitudeError = evaluateVibrato(myAmplitudes, myAmplitudes);
		break;
	}
}

mrs_real ExerciseControl::evaluateStraight(realvec &vec, realvec &weight)
{
	return slidingWeightedDeviation(vec, weight)/vec.mean();
}

mrs_real ExerciseControl::evaluateCrescendoDecrescendo(realvec &vec, realvec &weight)
{
	mrs_natural maxIndex;

	mrs_real vecMax = vec.maxval(&maxIndex);
	mrs_real slope1 = (vecMax-vec(0))/(maxIndex);
	mrs_real slope2 = (vecMax-vec(vec.getSize()-1))/(vec.getSize()-maxIndex);

	realvec vecLinear(vec.getSize());

	for (mrs_natural i=0 ; i<maxIndex ; i++)
		vecLinear(i) = slope1*(i+vec(0));
	for (mrs_natural i=maxIndex ; i<vec.getSize() ; i++)
		vecLinear(i) = slope2*(vec.getSize()-i)+vec(vec.getSize()-1);

	return slidingWeightedDeviation(vec-vecLinear, weight)/(vec.maxval()-vec.minval());
}

mrs_real ExerciseControl::evaluateVibrato(realvec &vec, realvec &weight)
{
	realvec vibratoFrequency (vec.getSize()), vibratoWeight(vec.getSize());
	realvec window(4096), windowWeight(hopSize*2);
	realvec spectrum(4096), power(1);

	window.setval(0);

	if(vec.getSize() != weight.getSize())
	{
		MRSERR("weigtedDeviation computation, data and weight must have the same dimensions\n");
		return 0;
	}

	// create necesary marsystems for vibrato analysis
	PowerSpectrum ps("PowerSpectrum");
	ps.updctrl("mrs_natural/inSamples", window.getCols());
	ps.updctrl("mrs_natural/inObservations", window.getRows());
	ps.updctrl("mrs_natural/onSamples", spectrum.getCols());
	ps.updctrl("mrs_natural/onObservations", spectrum.getRows());

	Power p("Power");
	p.updctrl("mrs_natural/inSamples", windowWeight.getCols());
	p.updctrl("mrs_natural/inObservations", windowWeight.getRows());
	p.updctrl("mrs_natural/onSamples", power.getCols());
	p.updctrl("mrs_natural/onObservations", power.getRows());

	for (mrs_natural i=0 ; i<vec.getSize() ; i++)
	{
		mrs_real meanValue = 0 ;
		for (mrs_natural j=0 ; j<vec.getSize() ; j++)
			meanValue += vec(i+j);
		meanValue/=vec.getSize();

		for (mrs_natural j=0 ; j<vec.getSize() ; j++)
		{
			window(j) = vec(i+j)-meanValue;
			windowWeight(j) = weight(i+j);
		}
		ps.process(window, spectrum);
		p.process(windowWeight, power);

		mrs_natural indexMax=0;
		spectrum.maxval(&indexMax);
		vibratoFrequency(i) = indexMax/4096*44100;
		vibratoWeight(i) = power(0);
	}

	return slidingWeightedDeviation(vibratoFrequency, vibratoWeight);
}

mrs_real ExerciseControl::slidingWeightedDeviation(realvec &vec, realvec &weight)
{
	mrs_real res=0;
	realvec window(hopSize*2), windowWeight(hopSize*2);

	if(vec.getSize() != weight.getSize())
	{
		MRSERR("weigtedDeviation computation, data and weight must have the same dimensions\n");
		return 0;
	}

	for (mrs_natural i=0 ; i<vec.getSize() ; i+=hopSize)
	{
		for (mrs_natural j=0 ; j<vec.getSize() ; j+=hopSize)
		{
			window(j) = vec(i+j);
			windowWeight(j) = weight(i+j);
		}
		res += weightedDeviation(window, windowWeight);
	}
	return res/vec.getSize()*hopSize;
}

mrs_real ExerciseControl::weightedDeviation(realvec &vec, realvec &weight)
{
	mrs_real meanData = vec.mean(), res=0;

	if(vec.getSize() != weight.getSize())
	{
		MRSERR("weigtedDeviation computation, data and weight must have the same dimensions\n");
		return 0;
	}

	for (mrs_natural i=0 ; i<vec.getSize() ; i++)
		res+= (vec(i)-meanData)*(vec(i)-meanData)*weight(i);

	return sqrt(res/(vec.getSize()*weight.mean()));
}
