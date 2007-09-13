#include <iostream>
using namespace std;

#include "exerciseControl.h"

#include "PowerSpectrum.h"
#include "Series.h"
#include "Power.h"
#include "Spectrum.h"
#include "Windowing.h"

#include <QGroupBox>
#include <sstream>

ExerciseControl::ExerciseControl() {
	displayPitches = NULL;
	displayAmplitudes = NULL;
	hopSize = 8;
	exerciseState = straightPiano;
	noteButton = new QToolButton*[5];
}

ExerciseControl::~ExerciseControl() {
	if (instructionImageLabel != NULL) {
		instructionLayout->removeWidget(instructionImageLabel);
		delete (instructionImageLabel);
		instructionImageLabel = NULL;
	}
	if (displayPitches != NULL) {
		resultLayout->removeWidget(displayPitches);
		delete displayPitches;
		displayPitches = NULL;
	}
	if (displayAmplitudes != NULL) {
		resultLayout->removeWidget(displayAmplitudes);
		delete displayAmplitudes;
		displayAmplitudes = NULL;
	}
}

int ExerciseControl::getType() {
	return TYPE_CONTROL;
}

void ExerciseControl::setupDisplay() {
	displayPitches = new QtMarPlot();
	displayPitches->setPlotName("Pitch");
	displayPitches->setBackgroundColor(QColor(255,255,255));
//	displayPitches->setBackgroundColor(QColor(0,255,0));
	displayPitches->setPixelWidth(3);
	displayAmplitudes = new QtMarPlot();
	displayAmplitudes->setPlotName("Amplitude");
	displayAmplitudes->setBackgroundColor(QColor(255,255,255));
//	displayAmplitudes->setBackgroundColor(QColor(0,0,255));
	displayAmplitudes->setPixelWidth(3);
	/*
	displayPitches = new QLabel;
	displayAmplitude = new QLabel;
	displayPitches->setText("Display Pitches here");
	displayAmplitude->setText("Display Ampitudes here");
	resultLayout->addWidget(displayPitches,0,0);
	resultLayout->addWidget(displayAmplitude,0,1);
	*/
	//QHBoxLayout *displayLayout = new QHBoxLayout;
	resultLayout = new QHBoxLayout;
	resultLayout->addWidget(displayPitches);
	resultLayout->addWidget(displayAmplitudes);
	resultArea->setLayout(resultLayout);
	//resultLayout->addLayout(displayLayout);

	//	resultsDisplay = new MeawsDisplay();
	//	mainLayout->addLayout(resultsDisplay);
}

void ExerciseControl::open(QString exerciseFilename) {
	QString noteImageBaseFilename = exerciseFilename.remove(".png");
	QString noteImageFilename;
	QPixmap image;

	instructionLayout = new QHBoxLayout;
	notes = new QButtonGroup;
	instructionLayout->setSpacing(0);
	for (int i=0; i<5; i++) {
		noteButton[i] = new QToolButton;
		noteImageFilename = noteImageBaseFilename+"-"+QString::number(i+1)+".png";
		//cout<<qPrintable(noteButtonFilename)<<endl;
		image = QPixmap::fromImage(QImage( noteImageFilename ));
		//noteButton[i]->setIcon(QPixmap::fromImage(QImage( noteImageFilename )));
		noteButton[i]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		noteButton[i]->setIcon( image );
		noteButton[i]->setIconSize( image.size() );
		// noteButton[i]->setScaledContents(false);
//		noteButton[i]->setMaximumHeight(120);
		notes->addButton(noteButton[i]);
		notes->setId( noteButton[i],i);
		instructionLayout->addWidget(noteButton[i]);
//,0,i,Qt::AlignLeft|Qt::AlignTop);
//		instructionLayout->addWidget(noteButton[i],0,i,Qt::AlignLeft|Qt::AlignTop);
	}

	//	instructionLayout->addWidget(notes);
	instructionArea->setLayout(instructionLayout);
	connect(notes, SIGNAL(buttonClicked(int)), this, SLOT(setNote(int)));

	// set size
	int ml, mt, mr, mb;
	instructionLayout->getContentsMargins(&ml, &mt, &mr, &mb);
	mt = 0;
	mb = 0;
	instructionLayout->setContentsMargins(ml, mt, mr, mb);

	instructionArea->setMaximumHeight(image.height()+mt+mb);
	instructionArea->setMinimumHeight(image.height()+mt+mb);
}
//zz

void ExerciseControl::setNote(int noteNumber) {
	exerciseState = (exerciseControlType) noteNumber;
	displayAnalysis(NULL);

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
	mrs_natural i, j=0;
	// TODO: stops warning messages
	(void) i;
	(void) j;

	// import results
	if(results)
	{
		overallPitches = results->getPitches();
		overallAmplitudes = results->getAmplitudes();

	/*	ofstream dump ;
		dump.open("pitch");
		dump << overallPitches;
		dump.close();
		dump.open("amps");
		dump << overallAmplitudes;
		dump.close();*/

	}

	cout << overallPitches;
	//overallPitches.read("pitch");
	//overallAmplitudes.read("amps");


	//MATLAB_PUT(overallPitches, "pitch");
	//MATLAB_PUT(overallAmplitudes, "amp");
	//MATLAB_EVAL("subplot(2, 1, 1); plot(pitch); subplot(2, 1, 2); plot(amp); ");

	selectExercisePerformance();

	displayPitches->setVertical(myPitches.median()-0.04,myPitches.median()+0.02);
	displayPitches->setData( &myPitches );

	displayAmplitudes->setVertical(0,myAmplitudes.maxval()+1);
	displayAmplitudes->setData( &myAmplitudes);

    evaluatePerformance();
	cout << "Results: " << pitchError << " " << amplitudeError;

	std::stringstream ss;
	ss << "Results: " << pitchError << " " << amplitudeError;
	resultString = ss.str();

	emit analysisDone();
	return 0 ;
}

void ExerciseControl::evaluatePerformance() {
	switch(exerciseState)
	{
	case straightPiano:
		pitchError = evaluateStraight(myPitches, myWeight);
		amplitudeError = evaluateStraight(myAmplitudes, myWeight);
		break;
	case straightMezzo:
		pitchError = evaluateStraight(myPitches, myWeight);
		amplitudeError = evaluateStraight(myAmplitudes, myWeight);
		break;
	case straightForte:
		pitchError = evaluateStraight(myPitches, myWeight);
		amplitudeError = evaluateStraight(myAmplitudes, myWeight);
		break;
	case crescendoDecrescendo:
		pitchError = evaluateStraight(myPitches, myWeight);
		amplitudeError = evaluateCrescendoDecrescendo(myAmplitudes, myWeight);
		break;
	case vibrato:
		pitchError = evaluateVibrato(myPitches, myWeight);
		amplitudeError = evaluateVibrato(myAmplitudes, myWeight);
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

	// TODO: compile fix, might be yucky.  -gp
	realvec vecSubtracted = vec - vecLinear;
	return slidingWeightedDeviation( vecSubtracted, weight)/(vec.maxval()-vec.minval());
}

mrs_real ExerciseControl::evaluateVibrato(realvec &vec, realvec &weight)
{
	realvec window(hopSize*2), windowWeight(hopSize*2);
		realvec vibratoFrequency (vec.getSize()-window.getSize()), vibratoWeight(vec.getSize()-window.getSize());
realvec spectrum(2048, 1), power(1);

	window.setval(0);

	if(vec.getSize() != weight.getSize())
	{
		MRSERR("weigtedDeviation computation, data and weight must have the same dimensions\n");
		return 0;
	}

	// create necesary marsystems for vibrato analysis
	Series vibSeries("vibSeries");
	vibSeries.addMarSystem(new Windowing("window"));
	vibSeries.addMarSystem(new Spectrum("fft"));
	vibSeries.addMarSystem(new PowerSpectrum("ps"));
	
	vibSeries.updctrl("mrs_natural/inSamples", window.getCols());
	vibSeries.updctrl("mrs_natural/inObservations", window.getRows());
	vibSeries.updctrl("Windowing/window/mrs_string/type", "Hanning");
	vibSeries.updctrl("Windowing/window/mrs_natural/zeroPadding", 4096);
// FIXME!!!
//lmartins: THIS IS NOW DIFFERENTLY IMPLEMENTED IN Windowing.cpp -> go there and figure out how ;-)

	Power p("Power");
	p.updctrl("mrs_natural/inSamples", windowWeight.getCols());
	p.updctrl("mrs_natural/inObservations", windowWeight.getRows());
	p.updctrl("mrs_natural/onSamples", power.getCols());
	p.updctrl("mrs_natural/onObservations", power.getRows());

	for (mrs_natural i=0 ; i<vec.getSize()-window.getSize() ; i++)
	{
		

		for (mrs_natural j=0 ; j<window.getSize() ; j++)
		{
			window(j) = vec(i+j);
			windowWeight(j) = weight(i+j);
		}
		window-=window.mean();
		vibSeries.process(window, spectrum);
		p.process(windowWeight, power);

		mrs_natural indexMax=0, k=0;
		while(spectrum(k+1)<spectrum(k) && k<spectrum.getSize()-1) spectrum(k++) = 0;

	
		spectrum.maxval(&indexMax);
		vibratoFrequency(i) = indexMax/2048.0;
		vibratoWeight(i) = power(0);
	}

	/*	MATLAB_PUT(vibratoFrequency, "pitch");
	    MATLAB_PUT(vibratoWeight, "amp");
	    MATLAB_EVAL("subplot(2, 1, 1); plot(pitch); subplot(2, 1, 2); plot(amp); ");*/


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

	for (mrs_natural i=0 ; i<vec.getSize()-hopSize*2 ; i+=hopSize)
	{
		for (mrs_natural j=0 ; j<hopSize*2 ; j++)
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

void ExerciseControl::selectExercisePerformance()
{
	mrs_natural startSectionSilence=0, endSectionSilence=0, accSilence=0, delaySilence = 10, i;
	// TODO: avoids warning
	(void) endSectionSilence;
	mrs_natural startSectionActivity=0, endSectionActivity=0, accActivity=0, delayActivity = 10;
	mrs_natural currentType= -1;

	myPitches.stretch(0);

	// search for consequent non zeros pitches followed by zeros
	for (i=0 ; i<overallPitches.getSize() ; i++)
	{
		if(overallPitches(i) && !startSectionActivity)
			startSectionActivity=i;
		if(overallPitches(i) && startSectionActivity)
			accActivity++;
		if(!overallPitches(i) && accActivity>delayActivity && !startSectionSilence)
			startSectionSilence=i;
		if(!overallPitches(i) && startSectionActivity)
			endSectionActivity=i;
		if(!overallPitches(i) && startSectionSilence)
			accSilence++;
		if(accSilence>delaySilence || i==overallPitches.getSize()-1)
		{
			currentType++;

			if(i==overallPitches.getSize()-1)
				startSectionSilence = overallPitches.getSize()-1;

			if(((exerciseControlType) currentType) == exerciseState)
			{
				myPitches.stretch(startSectionSilence-startSectionActivity);
				myAmplitudes.stretch(startSectionSilence-startSectionActivity);

				for (mrs_natural j=0 ; j<myPitches.getSize() ; j++)
				{
					myPitches(j) = overallPitches(startSectionActivity+j) ;
					myAmplitudes(j) = overallAmplitudes(startSectionActivity+j) ;
				}
				break;
			}
			startSectionActivity=0;
			startSectionSilence=0;
			accActivity=0;
			accSilence=0;
		}
	}
	// select the needed performance following the exercice type
	//myPitches.stretch(tmpP.getSize());
	//myAmplitudes.stretch(tmpP.getSize());

	//// remove zeros pitches
	//for(i=0 ; i< tmpP.getSize() ; i++)
	//	if(tmpP(i))
	//	{
	//myPitches(j) = tmpP(i);
	//myAmplitudes(j++) = tmpA(i);
	//}
	//	myPitches.stretch(j);
	//	myAmplitudes.stretch(j);
	
	// deal with octave errors
	if(myPitches.getSize() != 0)
	{
	mrs_real medianPitch = myPitches.median();
	for (mrs_natural i=0 ; i<myPitches.getSize() ; i++)
	{
		if(myPitches(i)>medianPitch*1.5)
			myPitches(i)/=2;
        if(myPitches(i)<medianPitch*.75)
			myPitches(i)*=2;
	}
	
		MATLAB_PUT(myPitches, "pitch");
	MATLAB_PUT(myAmplitudes, "amp");
	MATLAB_EVAL("subplot(2, 1, 1); plot(pitch); subplot(2, 1, 2); plot(amp); ");

myWeight = myAmplitudes;

	if(exerciseState != vibrato)
	{
    myPitches.apply(hertz2bark);
	myAmplitudes.apply(amplitude2dB);
	}
	}
}

