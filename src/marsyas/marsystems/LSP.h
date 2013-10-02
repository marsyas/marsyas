/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_LSP_H
#define MARSYAS_LSP_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Compute Linear Spectral Pair (LSP) coefficients

	Takes the output of LPC and calculates the corresponding LSP values.
	See the LPC class implementation.

	\author Luís Gustavo Martins - lmartins@inescporto.pt
	\date May 2006
*/


class LSP: public MarSystem
{
private:
  mrs_natural order_;

  ///Z-Plane pole-shifting factor
  //mrs_real gamma_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  LSP(std::string name);
  ~LSP();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif



