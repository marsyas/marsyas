#include "Spectrum2ACMChroma.h"
#include <marsyas/system/MarSystemManager.h>
#include "Series.h"

#include "MedianFilter.h"
#include "PeakInObservation.h"
#include "Negative.h"
#include "Signum.h"
#include "F0Analysis.h"
#include "Pitch2Chroma.h"

using namespace std;
using namespace Marsyas;

// Initialize 'mrs_real' constants outside class definition
// const mrs_real Spectrum2ACMChroma::RaiseFactor_ = 2.;
// const mrs_real Spectrum2ACMChroma::Hysteresis_ = sqrt(2.);
// const mrs_real Spectrum2ACMChroma::Tolerance_ = 0.03;
// const mrs_real Spectrum2ACMChroma::Attenuation_ = 0.75;
// const mrs_real Spectrum2ACMChroma::PowerOfAmplitude_ = 0.5;
// const mrs_real Spectrum2ACMChroma::Diapason_ = 440.;

Spectrum2ACMChroma::Spectrum2ACMChroma(mrs_string inName)
  :MarSystem("Spectrum2ACMChroma",inName)
{
  addControls();

  MarSystem* theNewSystem;
  MarSystemManager theManager;

  // Add new MarSystems to manager
  MarSystem* theDummy;
  theDummy = new MedianFilter("Anything");
  theManager.registerPrototype("MedianFilter",theDummy);
  theDummy = new PeakInObservation("Anything");
  theManager.registerPrototype("PeakInObservation",theDummy);
  theDummy = new Negative("Anything");
  theManager.registerPrototype("Negative",theDummy);
  theDummy = new Signum("Anything");
  theManager.registerPrototype("Signum",theDummy);
  theDummy = new F0Analysis("Anything");
  theManager.registerPrototype("F0Analysis",theDummy);
  theDummy = new Pitch2Chroma("Anything");
  theManager.registerPrototype("Pitch2Chroma",theDummy);

  // ------------------------ DEFINE CHROMA EXTRACTOR ------------------
  Spectrum2ACMChromaNet_ = theManager.create("Series","SER1");

  // --- 8.1 Compute peaks minus background in spectrum
  MarSystem* theFan2 = theManager.create("FanOutIn","FAN2");

  // ------ 8.1.1 Compute background spectrum
  MarSystem* theSeries3 = theManager.create("Series","SER3");

  // --------- 8.1.1a Median filter
  theNewSystem = theManager.create("MedianFilter","MedianFilter");
  theSeries3->addMarSystem(theNewSystem);

  // --------- 8.1.1b Amplify background spectrum
  theNewSystem = theManager.create("Gain","Gain");
  theSeries3->addMarSystem(theNewSystem);

  // --------- 8.1.1c Reverse sign of background spectrum (for subtraction)
  theNewSystem = theManager.create("Negative","Negative");
  theSeries3->addMarSystem(theNewSystem);

  // ------ 8.1.2 Compute peaks in spectrum
  theNewSystem = theManager.create("PeakInObservation","FindPeaks");

  // --- Determine the positions of the salient peaks
  //     Therefore, subtract peaks and background via sum in fanOutIn
  //     (Default combinator: +)
  theFan2->addMarSystem(theNewSystem);
  theFan2->addMarSystem(theSeries3);

  // --- 8.2 Derive booleans from salient peaks
  MarSystem* theSeries2 = theManager.create("Series","SER2");
  theSeries2->addMarSystem(theFan2);

  theNewSystem = theManager.create("Signum","Sign");
  theSeries2->addMarSystem(theNewSystem);

  // --- 8.3 Compute salient peaks
  //         Therefore, multiply booleans with spectrum via multiply in fanOutIn
  MarSystem* theFan1 = theManager.create("FanOutIn","FAN1");
  theFan1->addMarSystem(theSeries2);

  theNewSystem = theManager.create("Gain","Gain");	// No function
  theFan1->addMarSystem(theNewSystem);

  Spectrum2ACMChromaNet_->addMarSystem(theFan1);

  // 9. Perform pitch analysis
  theNewSystem = theManager.create("F0Analysis","F0Analysis");
  Spectrum2ACMChromaNet_->addMarSystem(theNewSystem);

  // 10. Map pitches to chroma
  theNewSystem = theManager.create("Pitch2Chroma","Pitch2Chroma");
  Spectrum2ACMChromaNet_->addMarSystem(theNewSystem);

  // ------------------------ DEFINE FIXED PARAMETERS ------------------
  mrs_string theControlString = "FanOutIn/FAN1/Series/SER2/FanOutIn/FAN2/"
                                "Series/SER3/Gain/Gain/mrs_real/gain";
  Spectrum2ACMChromaNet_->updControl(theControlString,2.);

  theControlString = "FanOutIn/FAN1/Series/SER2/FanOutIn/FAN2/"
                     "PeakInObservation/FindPeaks/mrs_real/HystFactor";
  Spectrum2ACMChromaNet_->updControl(theControlString,sqrt(2.));

  theControlString = "FanOutIn/FAN1/Gain/Gain/mrs_real/gain";
  Spectrum2ACMChromaNet_->updControl(theControlString,1.);

  theControlString = "FanOutIn/FAN1/mrs_string/combinator";
  Spectrum2ACMChromaNet_->updControl(theControlString,"*");
}

Spectrum2ACMChroma::Spectrum2ACMChroma(const Spectrum2ACMChroma& inToCopy)
  :MarSystem(inToCopy)
{
  Spectrum2ACMChromaNet_ = inToCopy.Spectrum2ACMChromaNet_->clone();

  // Copy MarControllers
  ctrl_NrOfHarmonics_ = getControl("mrs_natural/NrOfHarmonics");
  ctrl_F0Weight_ = getControl("mrs_real/F0Weight");
  ctrl_LowestF0_ = getControl("mrs_real/LowestF0");

  // Copy member variables
  NrOfHarmonics_ = inToCopy.NrOfHarmonics_;
  F0Weight_ = inToCopy.F0Weight_;
  LowestF0_ = inToCopy.LowestF0_;
}

Spectrum2ACMChroma::~Spectrum2ACMChroma()
{
  delete Spectrum2ACMChromaNet_;
}

MarSystem* Spectrum2ACMChroma::clone() const
{
  // Difference between COPY and CLONE?
  return new Spectrum2ACMChroma(*this);
}

void Spectrum2ACMChroma::addControls()
{
  // Why in special addControls() function?

  // Add MarControllers with default values
  addctrl("mrs_natural/NrOfHarmonics",5,ctrl_NrOfHarmonics_);
  addctrl("mrs_real/F0Weight",0.5,ctrl_F0Weight_);
  addctrl("mrs_real/LowestF0",100.,ctrl_LowestF0_);
  addctrl("mrs_real/ChordEvidence",0.);

  // Call myUpdate when controller changes
  ctrl_NrOfHarmonics_->setState(true);
  ctrl_F0Weight_->setState(true);
  ctrl_LowestF0_->setState(true);

  // Set member variables to default values
  NrOfHarmonics_ = 5;
  F0Weight_ = 0.5;
  LowestF0_ = 100.;
}

void Spectrum2ACMChroma::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  // output variables may change during course of program
  // therefore, declare them in myUpdate (not in constructor)
  // UPDATE is necessary! (why?)

  // Copy THIS input specs to input specs of Spectrum2ACMChromaNet
  updControl(Spectrum2ACMChromaNet_->ctrl_inSamples_, ctrl_inSamples_);
  updControl(Spectrum2ACMChromaNet_->ctrl_inObservations_, ctrl_inObservations_);
  updControl(Spectrum2ACMChromaNet_->ctrl_israte_, ctrl_israte_);

  // Copy output specs of Spectrum2ACMChromaNet to THIS output specs
  updControl(ctrl_onSamples_, Spectrum2ACMChromaNet_->ctrl_onSamples_);
  updControl(ctrl_onObservations_, Spectrum2ACMChromaNet_->ctrl_onObservations_);
  updControl(ctrl_osrate_, Spectrum2ACMChromaNet_->ctrl_osrate_);

  // Update member variables
  NrOfHarmonics_ = ctrl_NrOfHarmonics_->to<mrs_natural>();
  F0Weight_ = ctrl_F0Weight_->to<mrs_real>();
  LowestF0_ = ctrl_LowestF0_->to<mrs_real>();

  // Define MarSystem parameters that depend on bin width
  mrs_real theFrameSize = 0.08;	//!! should depend on input framesize !!

  // israte_ = FFTSize/Fs = width of bins in input spectrum
  mrs_natural theWindowSize =
    (mrs_natural)floor((mrs_real)80/israte_+0.5);
  mrs_string theControlString = "FanOutIn/FAN1/Series/SER2/FanOutIn/FAN2/"
                                "Series/SER3/MedianFilter/MedianFilter/mrs_natural/WindowSize";
  Spectrum2ACMChromaNet_->updControl(theControlString,theWindowSize);

  mrs_natural theHystLength =
    (mrs_natural)floor(2./(theFrameSize*israte_)+0.5);
  theControlString = "FanOutIn/FAN1/Series/SER2/FanOutIn/FAN2/"
                     "PeakInObservation/FindPeaks/mrs_natural/HystLength";
  Spectrum2ACMChromaNet_->updControl(theControlString,theHystLength);

  // All notes are referred to the diapason:
  // 1. search for the pitch of the lowest note >= the lowest pitch
  //    The lowest pitch is defined by the user
  // 2. search for the pitch of the highest note <= the highest pitch
  //    The highest pitch is defined by the algorithm and equals Fs/4

  // 1.1 compute the note index of the diapason referred to the min. pitch
  mrs_natural theDiapInd = (mrs_natural)(1 + floor(12. * log(440./100.)/log(2.)));
  // 1.2 compute the pitch of the lowest note given the note index of the diapason
  mrs_real theLowestNote = 440. * pow(2., (mrs_real)(1-theDiapInd)/12.);
  // 2 compute the note index of the pitch of the highest note = the number of notes
  // !! Use Nyquist frequency of input signal instead of 1000Hz!!
  mrs_natural theMaxInd = (mrs_natural)(theDiapInd + floor(12.*log(1000./(440.))/log(2.)));

  theControlString = "Pitch2Chroma/Pitch2Chroma/mrs_real/LowestPitch";
  Spectrum2ACMChromaNet_->updControl(theControlString,theLowestNote);

  theControlString = "Pitch2Chroma/Pitch2Chroma/mrs_natural/NrOfNotes";
  Spectrum2ACMChromaNet_->updControl(theControlString,theMaxInd);
}

void Spectrum2ACMChroma::myProcess(realvec& inSpectrum, realvec& outChroma)
{
  Spectrum2ACMChromaNet_->process(inSpectrum,outChroma);

  // Update output variable
  mrs_string theControlString = "F0Analysis/F0Analysis/mrs_real/ChordEvidence";
  MarControlPtr theControlPtr = Spectrum2ACMChromaNet_->getctrl(theControlString);
  updControl("mrs_real/ChordEvidence",theControlPtr->to<mrs_real>());
}
