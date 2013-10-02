/*
** Copyright (C) 2009 Stefaan Lippens
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

#ifndef MARSYAS_MIXTOMONO_H
#define MARSYAS_MIXTOMONO_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class MixToMono
	\ingroup Processing
	\brief Mix the input channels (observations rows) into one output
	observation channel.

	This MarSystem averages its input channels (the different observation
	rows of a slice) to a single output channel. The weights for the
	averaging are determined automatically based on the number of input
	channels, so it works intuitively for mono, stereo or input with more
	channels.

	\see Sum.

	This MarSystem has no extra controls.
*/

class MixToMono: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  /// The weight factor to use during averaging.
  mrs_real weight_;

public:
  /// MixToMono constructor.
  MixToMono(std::string name);

  /// MixToMono copy constructor.
  MixToMono(const MixToMono& a);

  /// MixToMono destructor.
  ~MixToMono();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_MIXTOMONO_H

