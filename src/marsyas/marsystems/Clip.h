/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_CLIP_H
#define MARSYAS_CLIP_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class Clip
	\ingroup Processing
    \brief Clip

    Clip any values greater than 1 or -1 to 1 and -1 by
	default or by range if the corresponding control is set.

	Controls:
	- \b mrs_real/range [w] : adjust the clipping range.

*/


class Clip: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);
  MarControlPtr ctrl_range_;
public:
  Clip(std::string name);
  Clip(const Clip& a);
  ~Clip();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


