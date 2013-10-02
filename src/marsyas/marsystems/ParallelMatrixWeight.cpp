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

#include "../common_source.h"
#include "ParallelMatrixWeight.h"

using std::ostringstream;
using namespace Marsyas;

//#define MTLB_DBG_LOG

ParallelMatrixWeight::ParallelMatrixWeight(mrs_string name) : MarSystem("ParallelMatrixWeight", name)
{
  /// Add any specific controls needed by this MarSystem.
  // Default controls that all MarSystems should have (like "inSamples"
  // and "onObservations"), are already added by MarSystem::addControl(),
  // which is already called by the constructor MarSystem::MarSystem(name).
  // If no specific controls are needed by a MarSystem there is no need to
  // implement and call this addControl() method (see for e.g. Rms.cpp)
  addControls();
}

ParallelMatrixWeight::ParallelMatrixWeight(const ParallelMatrixWeight& a) : MarSystem(a)
{
  // IMPORTANT!
  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  // Otherwise this would result in trying to deallocate them twice!
  ctrl_weights_ = getctrl("mrs_realvec/weights");
}


ParallelMatrixWeight::~ParallelMatrixWeight()
{
}

MarSystem*
ParallelMatrixWeight::clone() const
{
  // Every MarSystem should do this.
  return new ParallelMatrixWeight(*this);
}

void
ParallelMatrixWeight::addControls()
{
  mrs_realvec tmp(1);
  tmp(0)	= 1;

  addctrl("mrs_realvec/weights", tmp, ctrl_weights_);
}

void
ParallelMatrixWeight::myUpdate(MarControlPtr sender)
{
  MRSDIAG("ParallelMatrixWeight.cpp - ParallelMatrixWeight:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);
}

void
ParallelMatrixWeight::myProcess(realvec& in, realvec& out)
{
  mrs_realvec weights		= ctrl_weights_->to<mrs_realvec> ();
  mrs_natural		k,i,j,
                numRows		= weights.getRows (),
                 numCols		= weights.getCols (),
                  intRows,
                  intCols;

  if (numRows == 0)
  {
    out.setval(0);
    return;
  }

  if (in.getRows () % numRows)
  {
    MRSWARN("ParallelMatrixWeight: dimension mismatch");
    MRSASSERT(false);
    out.setval(0);
    return;
  }

  intRows		= in.getRows () / numRows,
   intCols		= in.getCols ();

  out = in;

  if (numCols == 1)
  {
    for (k = 0; k < numRows; k++)
    {
      mrs_real	weight	= weights(k);
      for (i = 0; i < intRows; i++)
      {
        for (j = 0; j < intCols; j++)
        {
          out(k*intRows+i,j)	*= weight;
        }
      }
    }
  }
  else
  {
    if (in.getCols () % numCols || in.getRows () != numRows)
    {
      MRSWARN("ParallelMatrixWeight: dimension mismatch");
      MRSASSERT(false);
      out.setval(0);
      return;
    }

    out		*= weights;
  }
#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(in, "in");
  MATLAB_PUT(out, "out");
  MATLAB_PUT(weights, "weights");
  MATLAB_EVAL("figure(2);subplot(221),imagesc(in),colorbar;subplot(222),imagesc(out),colorbar;subplot(212),imagesc(weights),colorbar;");
#endif
#endif
}
