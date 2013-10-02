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

#ifndef MARSYAS_DEINTERLEAVE_H
#define MARSYAS_DEINTERLEAVE_H

#include <marsyas/system/MarSystem.h>
#include "PatchMatrix.h"

namespace Marsyas
{
/**
	\ingroup Processing Basic
	\brief Reorder input along observations.

	This is designed, so that its output can potentially be meaningfully split by the Parallel MarSystem.

	if you want to deinterleave along samples:
	transpose, then deinterleave using this marsystem, then transpose again (using the Transposer MarSystem).

	The \b input realvec is assumed to be organized in numSets equal modulo sets -
	i.e. in a manner of:
	\code
	n*numSets,n*numSets+1,...,n*numSets+(numSets-1)
	\endcode

	This marsystem ensures that the \b output will be ordered with the sets grouped together -
	i.e. in a manner of:
	\code
	numSet[0][0],numSet[0][1],numSet[0][2],...,numSet[0][inObervations/numsets],
	numSet[1][0],numSet[1][1],numSet[1][2],...,numSet[1][inObervations/numsets],
	...,
	numSet[numSets-1][0],numSet[numSets-1][1],numSet[numSets-1][2],...,numSet[numSets-1][inObervations/numsets]
	\endcode

	Note that, if the number of observations is not divisible by the number of Sets, then the first "rest" groups
	will have one element more than the remaining groups.
	(With "rest" being the rest of the division of the number of observations by the number of Sets).

	For example, if there are 8 input rows (i.e. observations), then this marsystem would -
	for three sets (numSets - control set to 3) - reorder the elements of the 8 elements
	(ordered 0th,1st,2nd,3rd,4th,5th,6th,7th) of each row to the new order 0th,3rd,6th,1st,4th,7th,2nd,5th.

	Controls:
	- \b mrs_real/numSets [w] : deinterleave this number of observations.;
*/



class Deinterleave: public MarSystem
{
private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  MarControlPtr ctrl_numSets_;
  void myUpdate(MarControlPtr sender);

  PatchMatrix* pat_;

  mrs_realvec weights_;
  mrs_realvec outindex_;
  mrs_realvec inindex_;

public:
  Deinterleave(std::string name);
  Deinterleave(const Deinterleave& a);
  ~Deinterleave();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
