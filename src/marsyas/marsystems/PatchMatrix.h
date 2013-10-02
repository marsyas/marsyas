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

#ifndef MARSYAS_PatchMatrix_H
#define MARSYAS_PatchMatrix_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class PatchMatrix
	\ingroup Processing
    \brief Linear Combine Each input channel (i.e. observation) with each other channel
			the weights for the combinations can be provided through the weights control:
			each column of the weights control contains the weight for the input channel (i.e. row)
			with the same number indexwise. Sets of weight for each channel are expected to be found
			within same channel of the weights control.
			A constant vector can also be provided in the consts control to set the offset for each
			channel respectively.

	Controls:
	- \b mrs_realvec/consts [w] : vector (i.e. one dimensional realvec) of constants
									each of which is added to its channel.
	- \b mrs_realvec/weights [w] : weights matrix.
*/


class PatchMatrix: public MarSystem
{
private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  mrs_bool use_consts_;
  mrs_bool use_weights_;
  MarControlPtr ctrl_consts_;
  MarControlPtr ctrl_weights_;
  void myUpdate(MarControlPtr sender);

public:
  PatchMatrix(std::string name);
  PatchMatrix(const PatchMatrix& a);
  ~PatchMatrix();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
