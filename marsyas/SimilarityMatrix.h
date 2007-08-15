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

#ifndef MARSYAS_SIMILARITYMATRIX_H
#define MARSYAS_SIMILARITYMATRIX_H

#include "MarSystem.h"
#include "Metric.h"

namespace Marsyas
{
/** 
	\class SimilarityMatrix
	\ingroup MarSystem
	\brief Calculate Similarity Matrix of a set of feature vectors at input

	Controls:
	- \b mrs_bool/calcCovMatrix [w] : enables/disables calculation of covariance matrix from input data
	- \b mrs_realvec/covMatrix  [r] : covariance matrix from input data (if disabled, will contain an empty realvec)
*/

  class SimilarityMatrix: public MarSystem
  {
  private:
		realvec i_featVec_;
		realvec j_featVec_;
		realvec stackedFeatVecs_;
		realvec metricResult_;

		realvec vars_;
		realvec covMatrix_;

		MarControlPtr ctrl_covMatrix_;
		MarControlPtr ctrl_calcCovMatrix_;
		MarControlPtr ctrl_normalize_;

    void addControls();
    void myUpdate(MarControlPtr sender);
    
  public:
		enum covMatrixType {
			noCovMatrix = 0,
			diagCovMatrix = 1,
			fullCovMatrix = 2
		};
    
		SimilarityMatrix(std::string name);
		SimilarityMatrix(const SimilarityMatrix& a);
    ~SimilarityMatrix();

    MarSystem* clone() const; 

    void myProcess(realvec& in, realvec& out);
  };
}

#endif



