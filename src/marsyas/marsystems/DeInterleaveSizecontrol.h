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

#ifndef MARSYAS_DEINTERLEAVESIZECONTROL_H
#define MARSYAS_DEINTERLEAVESIZECONTROL_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing Basic
	\brief Reorder input observations.

	This is designed, so that its output can potentially be meaningfully split by the Parallel MarSystem.

	if you want to deinterleave samples (possibly with different channels containing different numbers of samples):
	transpose, then deinterleave using this marsystem, then transpose again (using the Transposer MarSystem).

	Same basic functionality as Deinterleave. As opposed to that one, this is not matrix multiplication based, and
	therefore has additional support for rows (observations) of different lengths.

	For example: the second input row has only 5 entries instead of the 8 of the first (rest is assumed to be zero padded) -
	then the result for three sets (numSets - control = 3) would not only reorder the elements of the 8 elements
	(ordered 0th,1st,2nd,3rd,4th,5th,6th,7th) of the first row to the new order 0th,3rd,6th,1st,4th,7th,2nd,5th but also
	reorder the sedond row in the following order: 0th,3rd,1st,4th,2nd(,5th,6th,7th following - i.e. unchanged).

	In general this means that:
	The **input** realvec is assumed to be organized in numSets equal modulo sets -
	i.e. in manner of:
	\code
	n*numSets,n*numSets+1,...,n*numSets+(numSets-1)
	\endcode

	This marsystem ensures that the **output** will be ordered with the sets grouped together -
	i.e. in a manner of
	\code
	numSet[0][0],numSet[0][1],numSet[0][2],...,numSet[0][inObervations/numsets],
	numSet[1][0],numSet[1][1],numSet[1][2],...,numSet[1][inObervations/numsets],
	...,
	numSet[numSets-1][0],numSet[numSets-1][1],numSet[numSets-1][2],...,numSet[numSets-1][inObervations/numsets]
	\endcode

	Note that, if the number of observations is not divisible by the number of Sets, then the first "rest" groups
	will have one element more than the remaining groups.
	(With "rest" being the rest of the division of the number of observations by the number of Sets).

	Controls:
	- \b mrs_real/numSets [w] : deinterleave this number of observations.
	- \b mrs_realvec/sizes [w] : realvec vector(i.e. one dimensional realvec) -
	each element i corresponding to the i-th row for controlling this input-row's
	number of elements to be interleaved;
*/


class DeInterleaveSizecontrol: public MarSystem
{
private:
  //Add specific controls needed by this MarSystem.
  void addControls();
  MarControlPtr ctrl_numSets_;
  MarControlPtr ctrl_sizes_;
  void myUpdate(MarControlPtr sender);

public:
  DeInterleaveSizecontrol(std::string name);
  DeInterleaveSizecontrol(const DeInterleaveSizecontrol& a);
  ~DeInterleaveSizecontrol();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif
