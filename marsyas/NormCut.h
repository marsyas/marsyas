/*
 *  NormCut.h
 *  normalizedCut
 *
 *  Created by Jennifer Murdoch on 17/11/06.
 *  Copyright 2006. All rights reserved.
 *
 */

#ifndef __NORM_CUT__
#define __NORM_CUT__


#include "common.h"
#include "realvec.h"


namespace Marsyas
{

typedef struct {
   mrs_real offset;
   mrs_natural    verbose; // not implemented yet
   mrs_natural    maxiterations;
   mrs_real eigsErrorTolerance;
} dataNcutParams, *dataNcutParamsPtr;

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
void ncutW(mrs_natural *n, realvec &W, mrs_natural *nbcluster, realvec &NcutDiscrete, realvec &NcutEigenvectors, realvec &NcutEigenvalues, dataNcutParamsPtr params=0);

void ncut(mrs_natural *n, realvec &W, mrs_natural *nbcluster, realvec &NcutEigenvectors, realvec &NcutEigenvalues, dataNcutParamsPtr params);
void discretisation(mrs_natural *n,  mrs_natural *nbcluster, realvec &NcutEigenvectors, realvec &NcutDiscrete, dataNcutParamsPtr params);
void discretisationEigenvectorData(mrs_natural *n,  mrs_natural *nbcluster, realvec &V, realvec &Vdiscrete);

/* 
 * Display column-oriented matrices 
 * Note : Matrices are stored in column-wise order for compatibility with CLAPACK (translated from FORTRAN)
 * Note2: n=1 --> display column vector         n=2 --> display row vector
 */
void prmrs_natural( realvec &A, mrs_natural m , mrs_natural n );
void prmrs_natural( mrs_natural *A, mrs_natural m , mrs_natural n );

}

#endif

