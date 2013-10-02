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

#ifndef MARSYAS_SELFSIMILARITYMATRIX_H
#define MARSYAS_SELFSIMILARITYMATRIX_H

#include <marsyas/system/MarSystem.h>
#include "Metric.h"

namespace Marsyas
{
/**
	\class SelfSimilarityMatrix
	\ingroup MarSystem
	\brief Calculate Similarity Matrix of a set of feature vectors at input

	Controls:
	- \b mrs_bool/calcCovMatrix [w] : enables/disables calculation of covariance matrix from input data
	- \b mrs_realvec/covMatrix  [r] : covariance matrix from input data (if disabled, will contain an empty realvec)
	- \b mrs_string/normalize  [w] : sets the type of input data normalization to be used: none, MinMax or MinStd.
*/

class SelfSimilarityMatrix: public MarSystem
{
private:
  realvec i_featVec_;
  realvec j_featVec_;
  realvec stackedFeatVecs_;
  realvec metricResult_;

  realvec vars_;
  realvec covMatrix_;

  MarControlPtr ctrl_stdDev_;
  MarControlPtr ctrl_covMatrix_;
  MarControlPtr ctrl_calcCovMatrix_;
  MarControlPtr ctrl_normalize_;

  MarControlPtr ctrl_mode_;
  MarControlPtr ctrl_instanceIndexes_;
  MarControlPtr ctrl_nInstances_;
  MarControlPtr ctrl_done_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  enum covMatrixType {
    noCovMatrix = 0,
    fixedStdDev = 1,
    diagCovMatrix = 2,
    fullCovMatrix = 3
  };

  enum mode {
    outputDistanceMatrix = 0,
    outputPairDistance = 1
  };

  SelfSimilarityMatrix(std::string name);
  SelfSimilarityMatrix(const SelfSimilarityMatrix& a);
  ~SelfSimilarityMatrix();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};
}

#endif



