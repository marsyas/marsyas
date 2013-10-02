/*
** Copyright (C) 2010 Stefaan Lippens
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

#ifndef MARSYAS_SUBTRACTMEAN_H
#define MARSYAS_SUBTRACTMEAN_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class SubtractMean
	\ingroup Processing
	\brief Subtract the mean of each observation channel.


	Simple MarSystem that subtracts the mean (per slice) of each
	observation channel.

	This MarSystem has no extra controls.

*/

class marsyas_EXPORT SubtractMean: public MarSystem
{
private:

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

public:
  /// SubtractMean constructor.
  SubtractMean(std::string name);

  /// SubtractMean copy constructor.
  SubtractMean(const SubtractMean& a);

  /// SubtractMean destructor.
  ~SubtractMean();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_SUBTRACTMEAN_H

