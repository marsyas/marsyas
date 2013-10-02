/* -*- c-basic-offset: 4 indent-tabs-mode: nil -*-  vi:set ts=8 sts=4 sw=4: */

/*
    Vamp

    An API for audio analysis and feature extraction plugins.

    Centre for Digital Music, Queen Mary, University of London.
    Copyright 2006 Chris Cannam.

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use, copy,
    modify, merge, publish, distribute, sublicense, and/or sell copies
    of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
    ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
    CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
    WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

    Except as contained in this notice, the names of the Centre for
    Digital Music; Queen Mary, University of London; and Chris Cannam
    shall not be used in advertising or otherwise to promote the sale,
    use or other dealings in this Software without prior written
    authorization.
*/

#ifndef _MARSYAS_IBTRIC_H_
#define _MARSYAS_IBTRIC_H_

#include <cstdio>
#include <cstdlib>
#include <string>
#include <iomanip>

#include <marsyas/common_source.h>
#include <marsyas/system/MarSystemManager.h>
#include <marsyas/CommandLineOptions.h>
#include <marsyas/FileName.h>

#include "MarSystemTemplateBasic.h"
#include "MarSystemTemplateAdvanced.h"
#include <marsyas/sched/EvValUpd.h>
#include <marsyas/Collection.h>
#include <marsyas/NumericLib.h>
#include <string.h>
#include "vamp-sdk/Plugin.h"
//#include <vector>

#ifdef MARSYAS_MIDIIO
#include "RtMidi.h"
#endif

#ifdef MARSYAS_AUDIOIO
#include "RtAudio3.h"
#endif

using namespace std;
using namespace Marsyas;

//#pragma warning(disable: 4244)  //disable double to float warning
//#pragma warning(disable: 4100) //disable argc warning

/**
 * Example plugin that calculates the positions and density of
 * zero-crossing points in an audio waveform.
*/

class MarsyasIBT : public Vamp::Plugin
{
public:
  MarsyasIBT(float inputSampleRate);
  virtual ~MarsyasIBT();

  bool initialise(size_t channels, size_t stepSize, size_t blockSize);
  void reset();

  InputDomain getInputDomain() const { return TimeDomain; }

  std::string getIdentifier() const;
  std::string getName() const;
  std::string getDescription() const;
  std::string getMaker() const;
  int getPluginVersion() const;
  std::string getCopyright() const;

  OutputList getOutputDescriptors() const;
  size_t getPreferredStepSize() const;
  size_t getPreferredBlockSize() const;
  ParameterList getParameterDescriptors() const;
  float getParameter(std::string name) const;
  void setParameter(std::string name, float value);

  FeatureSet process(const float *const *inputBuffers,
                     Vamp::RealTime timestamp);

  FeatureSet getRemainingFeatures();

protected:
  size_t m_stepSize;
  float m_previousSample;
  float induction_time;
  bool online_flag, output_flag, metrical_changes_flag;
  size_t nr_agents, min_bpm, max_bpm; //must be size_t for int parameters
  float stamp, prevTimestamp;
  float ibi;
  int frameCount;
  mrs_natural inductionTickCount;
  vector<vector<double> > backSave;

  MarSystemManager mng;
  MarSystem* ibt;
  MarSystem* featureNetwork;
  MarSystem* featExtractor;
  MarSystem* audioflow;
  MarSystem* beattracker;
};


#endif
