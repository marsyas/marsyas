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

#ifndef MARSYAS_RBF_H
#define MARSYAS_RBF_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class RBF
	\ingroup Special
	\brief Radial Basis Functions

	Controls:
	- \b mrs_string/RBFtype [rw] : chose RBF function to use.
	- \b mrs_real/Beta [rw] : sets the Beta constant used in some of the RBF functions.
	- \b mrs_bool/symmetricIn [w]: if the input is a symmetric Matrix, setting this to true improves performance
*/
class RBF: public MarSystem
{
private:
  std::string RBFtype_;
  MarControlPtr ctrl_RBFtype_;
  MarControlPtr ctrl_Beta_;
  MarControlPtr ctrl_symmetricIn_;

  mrs_real (RBF::*RBFfunc_)(const mrs_real) const;
  mrs_real GaussianRBF(const mrs_real val) const;
  mrs_real MultiquadraticRBF(const mrs_real val) const;
  mrs_real ThinPlateSplineRBF(const mrs_real val) const;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  RBF(std::string name);
  RBF(const RBF& a);
  ~RBF();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif



