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

#ifndef MARSYAS_PARALLEL_H
#define MARSYAS_PARALLEL_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class Parallel
    \ingroup Composites
    \brief Parallel composite of MarSystem objects

	Controls: none
*/


class Parallel: public MarSystem
{
private:
  void myUpdate(MarControlPtr sender);
  void deleteSlices();

  struct ChildInfo {
    mrs_natural inObservations;
    mrs_natural onObservations;
  };

  std::vector<ChildInfo> childInfos_;
  std::vector<realvec*> slices_;

public:
  Parallel(std::string name);

  ~Parallel();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif


