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

#ifndef MARSYAS_PCA_H
#define MARSYAS_PCA_H

#include <marsyas/system/MarSystem.h>

#include <string>

namespace Marsyas
{
/**
	\ingroup MachineLearning
	\brief Principal Component Analysis

	Perform Principal Component Analysis (PCA) on all samples of the
	incoming realvec of data. The correlation method is taken, with a
	correlation matrix computed over all samples within the single input
	realvec.  Eigenvalue/Eigenvector calculation is by the QL algorithm (ie.
	not suitable when the correlation matrix is singular or near singular).
	Output is the set of input samples projected onto the top "npc" (a
	MarSystem control) principal components.
*/


class PCA: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  realvec in_data_;
  realvec corr_matrix_;
  realvec temp_matrix_;
  mrs_real* evals_;
  mrs_real* interm_;
  mrs_natural dims_;
  mrs_natural npc_;
  realvec npcs_;

  realvec means_;
  realvec stds_;

public:
  PCA(std::string name);
  PCA(const PCA& a);
  ~PCA();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
  void tred2(realvec &a, mrs_natural m, mrs_real *d, mrs_real *e);
  void tqli(mrs_real d[], mrs_real e[], mrs_natural m, realvec &z);

};

}//namespace Marsyas

#endif
