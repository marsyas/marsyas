#include "Pitch2Chroma.h"
#include <map>
#include <algorithm>

using std::ostringstream;
using std::max;
using std::min;

using namespace Marsyas;

Pitch2Chroma::Pitch2Chroma(mrs_string inName)
  :MarSystem("Pitch2Chroma",inName)
{
  addControls();
}

Pitch2Chroma::Pitch2Chroma(const Pitch2Chroma& inToCopy)
  :MarSystem(inToCopy)
{
  ctrl_SampleRate_ = getctrl("mrs_real/SampleRate");
  ctrl_LowestPitch_ = getctrl("mrs_real/LowestPitch");
  ctrl_NotesPerOctave_ = getctrl("mrs_natural/NotesPerOctave");
  ctrl_NrOfNotes_ = getctrl("mrs_natural/NrOfNotes");
  ctrl_RefChromaIndex_ = getctrl("mrs_natural/RefChromaIndex_");

  SampleRate_ = inToCopy.SampleRate_;
  LowestPitch_ = inToCopy.LowestPitch_;
  NotesPerOctave_ = inToCopy.NotesPerOctave_;
  NrOfNotes_ = inToCopy.NrOfNotes_;
  RefChromaIndex_ = inToCopy.RefChromaIndex_;
}

Pitch2Chroma::~Pitch2Chroma() {}

MarSystem* Pitch2Chroma::clone() const
{
  return new Pitch2Chroma(*this);
}

void Pitch2Chroma::addControls()
{
  addctrl("mrs_real/SampleRate",8000.,ctrl_SampleRate_);
  addctrl("mrs_real/LowestPitch",27.5,ctrl_LowestPitch_);
  addctrl("mrs_natural/NotesPerOctave",12,ctrl_NotesPerOctave_);
  addctrl("mrs_natural/NrOfNotes",88,ctrl_NrOfNotes_);
  addctrl("mrs_natural/RefChromaIndex_",6,ctrl_RefChromaIndex_);

  ctrl_SampleRate_->setState(true);
  ctrl_LowestPitch_->setState(true);
  ctrl_NotesPerOctave_->setState(true);
  ctrl_NrOfNotes_->setState(true);
  ctrl_RefChromaIndex_->setState(true);

  SampleRate_ = 8000.;
  LowestPitch_ = 27.5;
  NotesPerOctave_ = 12;
  NrOfNotes_ = 88;
  RefChromaIndex_ = 6;
}

void Pitch2Chroma::myUpdate(MarControlPtr inSender)
{
  MarSystem::myUpdate(inSender);
  ctrl_onObservations_->setValue(NotesPerOctave_,NOUPDATE);

  // Update member variables
  SampleRate_ = ctrl_SampleRate_->to<mrs_real>();
  LowestPitch_ = ctrl_LowestPitch_->to<mrs_real>();
  NotesPerOctave_ = ctrl_NotesPerOctave_->to<mrs_natural>();
  NrOfNotes_ = ctrl_NrOfNotes_->to<mrs_natural>();
  RefChromaIndex_ = ctrl_RefChromaIndex_->to<mrs_natural>();

  UpdatePitchToNoteTransform();
  UpdateNoteToChromaTransform();
}

void Pitch2Chroma::UpdatePitchToNoteTransform()
{
  // Define pitch to note transformation
  PitchToNoteTransform_.create(NrOfNotes_,inObservations_);
  StartAndEndIndex_.create(NrOfNotes_,2);

  mrs_real theLOGFc = LowestPitch_;
  mrs_real theLOGFStep = pow(2.,1./(mrs_real)NotesPerOctave_);
  mrs_real theLinFStep = SampleRate_/(2.f*(mrs_real)inObservations_);
  for (int i=0; i<NrOfNotes_; ++i)
  {
    // Define begin and end frequency
    mrs_real theLOGFb = theLOGFc/sqrt(theLOGFStep);
    mrs_real theLOGFe = theLOGFc*sqrt(theLOGFStep);

    // Convert begin and end frequency to FFT index
    StartAndEndIndex_(i,0) = max((int)ceil(theLOGFb/theLinFStep),0);
    StartAndEndIndex_(i,1) = min((int)floor(theLOGFe/theLinFStep),(int)inObservations_-1);

    for (int j=(mrs_natural)StartAndEndIndex_(i,0); j<=(mrs_natural)StartAndEndIndex_(i,1); j++)
    {
      double theBinFreq = (mrs_real)j*theLinFStep;
      if (theBinFreq <= theLOGFc)
        // Ramp up
        PitchToNoteTransform_(i,j) = (theBinFreq-theLOGFb)/(theLOGFc-theLOGFb);
      else
        // Ramp down
        PitchToNoteTransform_(i,j) = 1.-(theBinFreq-theLOGFc)/(theLOGFe-theLOGFc);
    }

    theLOGFc *= theLOGFStep;
  }
}

void Pitch2Chroma::UpdateNoteToChromaTransform()
{
  // Define note to chroma transformation
  NoteToChromaTransform_.create(NotesPerOctave_,NrOfNotes_);

  for (int i=0; i<NotesPerOctave_; ++i)
    for (int j=i; j<NrOfNotes_; j+=NotesPerOctave_)
    {
      int theChromaIndex = (7*i+RefChromaIndex_-1)%NotesPerOctave_;
      NoteToChromaTransform_(theChromaIndex,j) = 1.;
    }
}

void Pitch2Chroma::myProcess(realvec& inVec, realvec& outVec)
{
  mrs_natural t,o;
  outVec.setval(0.);
  for(o=0; o<onObservations_; o++)
    for(t=0; t<inSamples_; t++)
      for(int i=0; i<NrOfNotes_; ++i)
        for(int j=(mrs_natural)StartAndEndIndex_(i,0); j<=(mrs_natural)StartAndEndIndex_(i,1); j++)
          outVec(o,t) += NoteToChromaTransform_(o,i)*
                         PitchToNoteTransform_(i,j)*inVec(j,t);

  if (outVec.sum() != 0)
    outVec /= outVec.sum();
}
