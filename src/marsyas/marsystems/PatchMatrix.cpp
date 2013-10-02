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
#include "PatchMatrix.h"

using namespace Marsyas;


//#define MTLB_DBG_LOG

PatchMatrix::PatchMatrix(mrs_string name):MarSystem("PatchMatrix", name)
{
  //Add any specific controls needed by PatchMatrix
  //(default controls all MarSystems should have
  //were already added by MarSystem::addControl(),
  //called by :MarSystem(name) constructor).
  //If no specific controls are needed by a MarSystem
  //there is no need to implement and call this addControl()
  //method (see for e.g. Rms.cpp)
  addControls();
  use_consts_=false;
  use_weights_=false;
}

PatchMatrix::PatchMatrix(const PatchMatrix& a) : MarSystem(a)
{
  // For any MarControlPtr in a MarSystem
  // it is necessary to perform this getctrl
  // in the copy constructor in order for cloning to work
  ctrl_weights_ = getctrl("mrs_realvec/weights");
  ctrl_consts_ = getctrl("mrs_realvec/consts");

  use_consts_=a.use_consts_;
  use_weights_=a.use_weights_;
}

PatchMatrix::~PatchMatrix()
{

}

MarSystem*
PatchMatrix::clone() const
{
  return new PatchMatrix(*this);
}

void
PatchMatrix::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_realvec/consts", realvec(), ctrl_consts_);
  addctrl("mrs_realvec/weights", realvec(), ctrl_weights_);
  //setControlState("mrs_realvec/consts",true);
  setControlState("mrs_realvec/weights",true);
}

void
PatchMatrix::myUpdate(MarControlPtr sender)
{

  MarSystem::myUpdate(sender);

  if(ctrl_weights_->to<mrs_realvec>().getSize()!=0)
  {
    use_weights_=true;
    ctrl_onObservations_->setValue(ctrl_weights_->to<mrs_realvec>().getRows(),NOUPDATE);
  }
}


void
PatchMatrix::myProcess(realvec& in, realvec& out)
{
  //get a local copy of the current PatchMatrix control's values
  //(they will be used for this entire processing, even if it's
  //changed by a different thread)
  mrs_realvec PatchMatrixValue = ctrl_weights_->to<mrs_realvec>();
  mrs_realvec patchConstValues = ctrl_consts_->to<mrs_realvec>();

  if(PatchMatrixValue.getSize()!=0) use_weights_=true;
  if(patchConstValues.getSize()!=0) use_consts_=true;


#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(in, "in");
  MATLAB_EVAL("figure(11),plot(in'),axis('tight'),grid on");
#endif
#endif

  if (use_weights_)
  {
    mrs_realvec::matrixMulti(PatchMatrixValue,in,out);
  }

  if (use_consts_)
  {
    out += patchConstValues;
  }

#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(out, "out");
  MATLAB_EVAL("figure(12),plot(out'),axis('tight'),grid on");
#endif
#endif

}








