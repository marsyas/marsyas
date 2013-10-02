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

#ifndef MARSYAS_SOM_H
#define MARSYAS_SOM_H

#include <marsyas/system/MarSystem.h>

#include <cstdlib>

namespace Marsyas
{
/**
	\ingroup MachineLearning
	\brief Self-organizing map

	The input should consist of the following observations: feature values followed
	by file index followed by the x, y coordinates for initialization.
	By default the x,y coordinates are set to -1.0, 1.0 and are ignored.
*/


class SOM: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_gridmap_;
  realvec grid_pos_;
  mrs_natural grid_width_;
  mrs_natural grid_height_;
  realvec adjustments_;
  mrs_real alpha_;
  mrs_real neigh_std_;

public:
  SOM(std::string name);
  SOM(const SOM& a);

  ~SOM();
  MarSystem* clone() const;


  void myProcess(realvec& in, realvec& out);

private:
  double randD(double max);
  double gaussian(double x, double mean, double std, bool scale);

  void init_grid_map();
  void find_grid_location(realvec& in, int t);

};

}//namespace Marsyas

#endif




