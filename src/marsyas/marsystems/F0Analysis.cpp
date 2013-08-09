#include <map>
#include <algorithm>
#include "F0Analysis.h"

#define ROUND(x)	(mrs_natural)floor(x+0.5)


using std::ostringstream;
using std::vector;
using std::abs;

using namespace Marsyas;

F0Analysis::F0Analysis(mrs_string inName)
  :MarSystem("F0Analysis",inName)
{
  addControls();
}

F0Analysis::F0Analysis(const F0Analysis& inToCopy)
  :MarSystem(inToCopy)
{
  ctrl_SampleRate_ = getctrl("mrs_real/SampleRate");
  ctrl_NrOfHarmonics_ = getctrl("mrs_natural/NrOfHarmonics");
  ctrl_F0Weight_ = getctrl("mrs_real/F0Weight");
  ctrl_Attenuation_ = getctrl("mrs_real/Attenuation");
  ctrl_Tolerance_ = getctrl("mrs_real/Tolerance");
  ctrl_LowestF0_ = getctrl("mrs_real/LowestF0");
  ctrl_Compression_ = getctrl("mrs_real/Compression");

  SampleRate_ = inToCopy.SampleRate_;
  NrOfHarmonics_ = inToCopy.NrOfHarmonics_;
  F0Weight_ = inToCopy.F0Weight_;
  Attenuation_ = inToCopy.Attenuation_;
  Tolerance_ = inToCopy.Tolerance_;
  LowestF0_ = inToCopy.LowestF0_;
  Compression_ = inToCopy.Compression_;
}

F0Analysis::~F0Analysis() {}

MarSystem* F0Analysis::clone() const
{
  return new F0Analysis(*this);
}

void F0Analysis::addControls()
{
  addctrl("mrs_real/SampleRate",8000.f,ctrl_SampleRate_);
  addctrl("mrs_natural/NrOfHarmonics",5,ctrl_NrOfHarmonics_);
  addctrl("mrs_real/F0Weight",0.5,ctrl_F0Weight_);
  addctrl("mrs_real/Attenuation",0.75,ctrl_Attenuation_);
  addctrl("mrs_real/Tolerance",0.03,ctrl_Tolerance_);
  addctrl("mrs_real/LowestF0",100.,ctrl_LowestF0_);
  addctrl("mrs_real/Compression",0.5,ctrl_Compression_);
  addctrl("mrs_real/ChordEvidence",0.);

  ctrl_SampleRate_->setState(true);
  ctrl_NrOfHarmonics_->setState(true);
  ctrl_F0Weight_->setState(true);
  ctrl_Attenuation_->setState(true);
  ctrl_Tolerance_->setState(true);
  ctrl_LowestF0_->setState(true);
  ctrl_Compression_->setState(true);

  SampleRate_ = 8000.f;
  NrOfHarmonics_ = 5;
  F0Weight_ = 0.5;
  Attenuation_ = 0.75;
  Tolerance_ = 0.03;
  LowestF0_ = 100.;
  Compression_ = 0.5;
}

void F0Analysis::myUpdate(MarControlPtr inSender)
{
  SampleRate_ = ctrl_SampleRate_->to<mrs_real>();
  NrOfHarmonics_ = ctrl_NrOfHarmonics_->to<mrs_natural>();
  F0Weight_ = ctrl_F0Weight_->to<mrs_real>();
  Attenuation_ = ctrl_Attenuation_->to<mrs_real>();
  Tolerance_ = ctrl_Tolerance_->to<mrs_real>();
  LowestF0_ = ctrl_LowestF0_->to<mrs_real>();
  Compression_ = ctrl_Compression_->to<mrs_real>();

  MarSystem::myUpdate(inSender);
}

void F0Analysis::myProcess(realvec& inVec, realvec& outVec)
{
  F2Fs theF0ToFks;		// Map between F0 and higher harmonics
  HarmMap theHarmSums;
  FindCandidateF0s(inVec, theHarmSums, theF0ToFks);
  SelectUnrelatedF0s(inVec, theHarmSums, theF0ToFks, outVec);
  updControl("mrs_real/ChordEvidence",ChordEvidence_);
}

bool F0Analysis::FindCandidateF0s(const realvec& inPeaks,
                                  HarmMap& outHarmSums, F2Fs& outF0ToFks) const
{
  /* For each F0 > FLower, search for harmonically related freqs Fks
     1. Compute harmonic sum (S) and store (S,F0) in the map outHarmSums
     2. Store all Fks assigned to F0 in m_F0ToFks */
  outHarmSums.clear();
  outF0ToFks.clear();

  mrs_real theFreqStep = SampleRate_/(2.*inPeaks.getSize());
  for (mrs_natural i=0; i<inPeaks.getSize(); ++i) {
    mrs_real theF0 = (mrs_real)i*theFreqStep;

    // F0 > FLower
    if (inPeaks(i)>0 && theF0 >= LowestF0_) {

      // Compute harmonic sum & energy
      vector<double> theAssignedFks;
      mrs_real theSum = 0.0f, theNormFactor = 0.0f;
      for (mrs_natural j=i+1; j<inPeaks.getSize(); j++) {
        if (inPeaks(j)>0) {
          mrs_real theFk = (mrs_real)j*theFreqStep;

          /* Check whether Fk is one of the considered harmonics of F0:
             1. Compute k, the closest integer to Fk/F0
             2. Check whether |Fk/k-F0| <= tolerance x F0 */
          int k = ROUND((mrs_real)j/(mrs_real)i);
          if (k > 1 && k <= NrOfHarmonics_ &&
              abs(theFk/(mrs_real)k-theF0) <= Tolerance_*theF0) {

            theAssignedFks.push_back(theFk);
            double tmp = pow(Attenuation_,(double)k);
            theSum += pow(inPeaks(j),Compression_)*tmp;
            theNormFactor += tmp;
          }
        }
      }

      // Add F0
      if (theAssignedFks.size()>0) {
        outHarmSums[pow(inPeaks(i),Compression_*F0Weight_)
                    *pow(theSum/theNormFactor,1.-F0Weight_)] = theF0;
        outF0ToFks[theF0] = theAssignedFks;
      }
    }
  }
  return true;
}

bool F0Analysis::SelectUnrelatedF0s(const realvec& inPeaks,
                                    const HarmMap inHarmSums, const F2Fs& inF0ToFks, realvec& outNoteEvidence)
{
  outNoteEvidence.setval(0);

  FreqMap thePeaks;
  mrs_real theFreqStep = SampleRate_/(2.*inPeaks.getSize());
  for (mrs_natural i=0; i<inPeaks.getSize(); ++i)
    if (inPeaks(i)>0)
      thePeaks[(mrs_real)i*theFreqStep] = inPeaks(i);

  ChordEvidence_ = 0.0f;
  mrs_natural theNrOfPitches = 0;
  if (!inHarmSums.empty()) {
    /* Select candidates F0 (called Fc) that are not harmonically related.
       At first, select Fc with the highest harmonic sum (S) value.
       At second, select the other Fcs in decreasing order of S while
       Fc is not related to a selected Fc (HARMONIC or SUBHARMONIC)*/

    HarmMap::const_iterator Cand;	// Candidate F0
    FreqMap::iterator Sel;			// Selected F0

    // Add first candidate
    Cand = inHarmSums.begin();
    double F0c = Cand->second;
    outNoteEvidence(ROUND(F0c/theFreqStep)) =
      ComputePowerOfF0(thePeaks, inF0ToFks, F0c);
    theNrOfPitches++;
    Cand++;

    // Proceed with other candidates
    mrs_real theHypPower = ComputePowerOfF0(thePeaks, inF0ToFks, F0c);
    mrs_real theAllPower = ComputePowerOfInput(thePeaks);
    while(Cand!=inHarmSums.end()) {

      // Check relationship with selected F0
      F0c = Cand->second;
      bool theRelFlag = false;

      for (mrs_natural i=0; i<outNoteEvidence.getSize(); ++i)
      {
        if (outNoteEvidence(i) > 0)
        {
          double F0 = (mrs_real)i*theFreqStep;
          int k = ROUND(F0c/F0);
          int l = ROUND(F0/F0c);

          // theRelFlag = true if Sel and Cand are harmonically related
          theRelFlag = (k > 0 && k <= NrOfHarmonics_ &&
                        abs(F0c/(double)k-F0) <= Tolerance_*F0) ||
                       (l > 0 && l <= NrOfHarmonics_ &&
                        abs((double)l*F0c-F0) <= Tolerance_*F0);

          if (theRelFlag)
            break;
        }
      }

      if (!theRelFlag)
      {
        outNoteEvidence(ROUND(F0c/theFreqStep)) = ComputePowerOfF0(thePeaks, inF0ToFks, F0c);
        theHypPower = ComputePowerOfHyp(thePeaks, inF0ToFks, outNoteEvidence);
        theNrOfPitches++;
      }
      Cand++;
    }

    // Normalize note relevances
    mrs_real theFactor = 0.0f;
    for (mrs_natural i=0; i<outNoteEvidence.getSize(); ++i)
      theFactor += outNoteEvidence(i);
    for (mrs_natural i=0; i<outNoteEvidence.getSize(); ++i)
      outNoteEvidence(i) /= theFactor;

    // Compute chord evidence if nr of notes >= 2
    if (theNrOfPitches>=2)
      ChordEvidence_= theHypPower/theAllPower;
  }
  return true;
}

mrs_real F0Analysis::ComputePowerOfF0(const FreqMap inPeaks, const F2Fs& inF0ToFks, double inF0) const {
  FreqMap::const_iterator iter1 = inPeaks.find(inF0);
  mrs_real thePower = pow(iter1->second,Compression_);

  F2Fs::const_iterator iter2 = inF0ToFks.find(inF0);
  vector<double> theFks = iter2->second;
  for (unsigned long i=0; i<theFks.size(); ++i) {
    iter1 = inPeaks.find(theFks[i]);
    thePower += pow(iter1->second,Compression_);
  }
  return thePower;
}

mrs_real F0Analysis::ComputePowerOfInput(const FreqMap inPeaks) const {
  mrs_real thePower = 0.0f;
  FreqMap::const_iterator iter;
  for (iter=inPeaks.begin(); iter!=inPeaks.end(); iter++)
    thePower += iter->second * iter->second;
  return thePower;
}

mrs_real F0Analysis::ComputePowerOfHyp(const FreqMap inPeaks,
                                       const F2Fs& inF0ToFks, realvec& inNoteEvidence) const
{
  /* For each selected candidate F0, search the assigned higher
     harmonics and store them in the vector Tmp*/
  vector<double> Tmp;
  mrs_real theFreqStep = SampleRate_/(2*inNoteEvidence.getSize());
  for (mrs_natural i=0; i<inNoteEvidence.getSize(); ++i)
  {
    if (inNoteEvidence(i) > 0)
    {
      F2Fs::const_iterator iter2 =
        inF0ToFks.find((mrs_real)i*theFreqStep);
      vector<double> theHarm = iter2->second;
      for (unsigned long i=0; i<theHarm.size(); ++i)
        Tmp.push_back(theHarm[i]);
    }
  }

  // Remove duplicate frequencies
  sort(Tmp.begin(), Tmp.end());
  unique(Tmp.begin(), Tmp.end());

  // Compute power of unique frequencies
  mrs_real thePower = 0.0f;
  FreqMap::const_iterator iter;
  for (size_t i=0; i<Tmp.size(); ++i) {
    iter = inPeaks.find(Tmp[i]);
    thePower += iter->second * iter->second;
  }
  return thePower;
}
