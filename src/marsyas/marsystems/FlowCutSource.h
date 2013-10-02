/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MARSYAS_FLOWCUTSOURCE_H
#define MARSYAS_FLOWCUTSOURCE_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class FlowCutSource
	\ingroup IO
    \brief break the data flow consistency by entering a black hole in the network

	Break the data flow consistency by entering a black hole in the
network.  Overwrites the output with 0s.

	Controls:
	- \b mrs_natural/setSamples [w] : sets the number of samples.
	- \b mrs_natural/setObservations [w] : sets the number of
	  observations.
	- \b mrs_real/setRate [w] : sets the sample rate.
*/


class FlowCutSource: public MarSystem
{
private:
  //Add specific controls needed by this MarSystem.
  void addControls();

  // lmartins: since this is the default MarSystem::myUpdate(MarControlPtr sender)
  // (i.e. does not alters input data format) it's not needed to
  // override it here! Use the default implementation defined in
  // MarSystem::myUpdate(MarControlPtr sender).
  //
  //void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_FlowCutSource_;
  MarControlPtr ctrl_RMScalc_;
  MarControlPtr ctrl_inRMS_;
  MarControlPtr ctrl_outRMS_;

  //RMS value for each in/out channel
  realvec inRMS_;
  realvec outRMS_;

  void myUpdate(MarControlPtr sender);

public:
  FlowCutSource(std::string name);
  FlowCutSource(const FlowCutSource& a);
  ~FlowCutSource();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
