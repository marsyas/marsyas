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

#ifndef MARSYAS_SERIES_H
#define MARSYAS_SERIES_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class Series
    \ingroup Composites
    \brief Series of MarSystem objects

    Combines a series of MarSystem objects to a single MarSystem
    corresponding to executing the System objects one after the other
    in sequence.

	Controls: none
*/


class Series: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  Series(std::string name);
  ~Series();
  MarSystem* clone() const;


  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


