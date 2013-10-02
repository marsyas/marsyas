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

#ifndef MARSYAS_NORMCUT_H
#define MARSYAS_NORMCUT_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\class NormCut
\ingroup MarSystem
\brief Computes the Normalized Cut for an input similarity matrix

*/


//typedef struct {
//   mrs_real offset;
//   mrs_natural    verbose; // not implemented yet
//   mrs_natural    maxiterations;
//   mrs_real eigsErrorTolerance;
//} dataNcutParams, *dataNcutParamsPtr;

class NormCut: public MarSystem
{

private:
  realvec nCutDiscrete_;
  realvec nCutEigVectors_;
  realvec nCutEigValues_;

  mrs_natural numClusters_;

  mrs_real       paramOffset_;
  mrs_natural    paramVerbose_; // not implemented yet
  mrs_natural    paramMaxiterations_;
  mrs_real       paramEigsErrorTolerance_;

  MarControlPtr ctrl_numClusters_;
// 		MarControlPtr ctrl_offset_;
// 		MarControlPtr ctrl_verbose_;
// 		MarControlPtr ctrl_maxIters_;
// 		MarControlPtr ctrl_eigsErrorTol_;

  /*
  * Function :    ncutW (driver function)
  * Description : Clusters data items using the normalized cut algorithm given an n x n similarity matrix
  *
  * Arguments: mrs_natural *n                    -- size of data set (input)
  *            realvec &W                 -- n x n symmetric similarity matrix (input)
  *            mrs_natural *nbcluster            -- desired number of clusters (input)
  *            dataNcutPtr params                   -- algorithm parameters (input)
  *            realvec &NcutDiscrete      -- nbcluster x n clustering results (output)
  *            realvec &NcutEigenvectors  -- resulting eigenvectors (output)
  *            realvec &NcutEigenvalues   -- resulting eigenvalues (output)
  */
  //void ncutW(mrs_natural *n, realvec &W, mrs_natural *nbcluster, realvec &NcutDiscrete, realvec &NcutEigenvectors, realvec &NcutEigenvalues, dataNcutParamsPtr params=0);

  void ncut(mrs_natural n, realvec &W, mrs_natural nbcluster, realvec &NcutEigenvectors, realvec &NcutEigenvalues);
  void discretisation(mrs_natural n,  mrs_natural nbcluster, realvec &NcutEigenvectors, realvec &NcutDiscrete);
  void discretisationEigenvectorData(mrs_natural n,  mrs_natural nbcluster, realvec &V, realvec &Vdiscrete);

  /*
  * Display column-oriented matrices
  * Note : Matrices are stored in column-wise order for compatibility with CLAPACK (translated from FORTRAN)
  * Note2: n=1 --> display column vector         n=2 --> display row vector
  */
  void prmrs_natural( realvec &A, mrs_natural m , mrs_natural n );
  void prmrs_natural( mrs_natural *A, mrs_natural m , mrs_natural n );

  void print( realvec &A, int m , int n );

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  NormCut(std::string name);
  NormCut(const NormCut& a);
  ~NormCut();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif
// MARSYAS_NormCut_H

