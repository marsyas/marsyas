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

#ifndef MARSYAS_PECLUSTERSELECT_H
#define MARSYAS_PECLUSTERSELECT_H

#include <marsyas/system/MarSystem.h>

#include <cstdlib>

namespace Marsyas
{
/**
\class PeakClusterSelect
\ingroup Special
\brief Downsample the number of clusters according to cluster density
         (most dense 'numClustersToKeep' clusters retained)

Controls:
- \b mrs_natural/numClustersToKeep [w] : sets the number of clusters to keep.
*/

class PeakClusterSelect: public MarSystem
{
private:
  MarControlPtr ctrl_numClustersToKeep_;

  // Driver method for quick sort
  void sort(realvec& rv, mrs_natural dimension, mrs_natural left, mrs_natural right, mrs_bool sortColumns=true);
  // Helper methods for quick sort
  int partition(realvec& rv, mrs_natural dimension, mrs_natural left, mrs_natural right, mrs_bool sortColumns=true);
  void swap(realvec& rv, mrs_natural sample1, mrs_natural sample2, mrs_bool swapColumns=true);

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  PeakClusterSelect(std::string name);
  PeakClusterSelect(const PeakClusterSelect& a);
  ~PeakClusterSelect();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif // MARSYAS_PECLUSTERSELECT_H



