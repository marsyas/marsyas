/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "../common_source.h"
#include "NormCut.h"
#include <marsyas/NumericLib.h>

#include <iostream>
#include <cmath>
#include <algorithm>

//#define MTLB_DBG_LOG

using std::ostringstream;
using std::cout;
using std::min;

using std::endl;

using namespace Marsyas;

NormCut::NormCut(mrs_string name):MarSystem("NormCut", name)
{
  numClusters_ = -1;
  paramOffset_ = 0.5;
  paramVerbose_ = 3;
  paramMaxiterations_ = 20;
  paramEigsErrorTolerance_ = 0.000001;

  addControls();
}

NormCut::NormCut(const NormCut& a) : MarSystem(a)
{
  numClusters_ = a.numClusters_;
  paramOffset_ = a.paramOffset_;
  paramVerbose_ = a.paramVerbose_;
  paramMaxiterations_ = a.paramMaxiterations_;
  paramEigsErrorTolerance_ = a.paramEigsErrorTolerance_;

  ctrl_numClusters_ = getctrl("mrs_natural/numClusters");
}

NormCut::~NormCut()
{

}

MarSystem*
NormCut::clone() const
{
  return new NormCut(*this);
}

void
NormCut::addControls()
{
  addctrl("mrs_natural/numClusters", 2, ctrl_numClusters_);
  setctrlState("mrs_natural/numClusters", true);

  addctrl("mrs_real/offset", 0.5);
  setctrlState("mrs_real/offset", true);

  addctrl("mrs_natural/verbose", 3);
  setctrlState("mrs_natural/verbose", true);

  addctrl("mrs_natural/maxIters", 20);
  setctrlState("mrs_natural/maxIters", true);

  addctrl("mrs_real/eigsErrorTol", 0.000001);
  setctrlState("mrs_real/eigsErrorTol", true);
}

void
NormCut::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
  ctrl_onObservations_->setValue(1, NOUPDATE);
  ctrl_osrate_->setValue(ctrl_israte_, NOUPDATE);
  ctrl_onObsNames_->setValue("PeakLabels", NOUPDATE);

  //check if there was a change in the input similarity matrix (i.e. in the nr of elements)
  //or if the number of clusters to detect was changed
  // [AL]: [!] this means that the number of inObservations has to be set *before* the number of clusters,
  // [AL] otherwise nCutDiscrete_	and nCutEigVectors_ have the wrong dimension !!!
  if(numClusters_ != ctrl_numClusters_->to<mrs_natural>() || onSamples_ != inSamples_)
  {
    numClusters_ = ctrl_numClusters_->to<mrs_natural>();

    nCutDiscrete_.stretch(numClusters_ * ctrl_inObservations_->to<mrs_natural>());
    nCutEigVectors_.stretch(numClusters_ * ctrl_inObservations_->to<mrs_natural>());
    nCutEigValues_.stretch(numClusters_);
  }
}

void
NormCut::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;

#ifdef MARSYAS_MATLAB
#ifdef MTLB_DBG_LOG
  MATLAB_PUT(in, "in");
  MATLAB_EVAL("figure(71),imagesc(in',[0 1]),colorbar");
#endif
#endif
  //check if there is any data at the input, otherwise do nothing
  if(in.getSize() == 0 || numClusters_ == 0)
  {
    //MRSWARN("NormCut::myProcess - empty input!");
    out.setval(-1.0);
    return;
  }
  if(in.getSize() == 1 || numClusters_ == 0)
  {
    //MRSWARN("NormCut::myProcess - empty input!");
    out.setval(0);
    return;
  }

  out.setval(0); //should it be -1.0 instead? [!] FIXME

  //ncut( n, W, nbcluster, NcutEigenvectors, NcutEigenvalues, params );
  //discretisation( n, nbcluster, NcutEigenvectors, NcutDiscrete, params );

  ncut(inObservations_, in, numClusters_, nCutEigVectors_, nCutDiscrete_ );
  discretisation(inObservations_, numClusters_, nCutEigVectors_, nCutDiscrete_ );

  for( o=0 ; o<inObservations_ ; o++ )
  {
    for( t=0 ; t<numClusters_ ; t++ )
    {
      // Initialize NcutDiscrete as we go
      if( nCutDiscrete_(o*(numClusters_)+t) == 1.0 )
        out(0,o) = t;
      //cout << "Peak " << o << " -> cluster = " << out(0,o) << endl;
    }
  }

  //cout << out << endl;

  //   //get a local copy of the current gain control value
  //   //(it will be used for this entire processing, even if it's
  //   //changed by someone else, e.g. by a different thread)
  //   mrs_real gainValue = ctrl_gain_->to<mrs_real>();
  //
  //   // It is important to loop over both observations
  //   // and channels so that for example a gain can be
  //   // applied to multi-channel signals
  //   for (o=0; o < inObservations_; o++)
  //   {
  //      for (t = 0; t < inSamples_; t++)
  //      {
  //         //apply gain to all channels
  //         out(o,t) = gainValue * in(o,t);
  //      }
  //   }
}


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
//void
//NormCut::ncutW(mrs_natural *n, realvec &W, mrs_natural *nbcluster, realvec &NcutDiscrete, realvec &NcutEigenvectors, realvec &NcutEigenvalues, dataNcutParamsPtr params)
//{
//   ncut( n, W, nbcluster, NcutEigenvectors, NcutEigenvalues, params );
//	/* prmrs_natural(NcutEigenvectors, *n, *nbcluster);
//	 prmrs_natural(NcutEigenvalues, *nbcluster, 1);*/
//   discretisation( n, nbcluster, NcutEigenvectors, NcutDiscrete, params );
//	// prmrs_natural(NcutDiscrete, *n, *nbcluster);
//}

void
NormCut::ncut(mrs_natural n, realvec &W, mrs_natural nbcluster, realvec &NcutEigenvectors, realvec &NcutEigenvalues)
{
  //dataNcutParams param;
  realvec dinvsqrt(n);
  mrs_real ulp;
  realvec P(n*(n));

  mrs_real norm;
  mrs_real sqrtn = sqrt((mrs_real) n);

  nbcluster = min(nbcluster,n);

  // params required by dsyevr_ but are not initialized or used
  realvec evals(n);
  realvec mrs_naturalerm(n);

  mrs_natural i,j;

  //   if( params != NULL )
  //      param = *params;
  //   else
  //   {
  //      param.offset = 0.5;
  //      param.verbose = 3;
  //      param.maxiterations = 20;
  //      param.eigsErrorTolerance = 0.000001;
  //   }
  // SHOULD BE USED IN CONSTRUCTOR !!! JEN

  // Check for matrix symmetry
  for( i=0 ; i<n ; ++i )
    for( j=0 ; j<n ; j++ ) {
      if( W(i*(n)+j) > 1.0 )
      {
        //cout << W;
        //ERROR("W(i, j) values should be <= 1");
        MRSWARN("NormCut::ncut() - input values should be <= 1 : delta @(" << i << "," << j << ") = " << W(i*(n)+j)-1.0);
      }
      if( W(i*(n)+j) != W(j*(n)+i) )
      {
        //cout << W;
        //ERROR("W is not symmetric");
        MRSWARN("NormCut::ncut - input matrix is not symmetric!");
      }
      P(i*(n)+j)=0.;
    }

  //ulp = NumericLib::machp("Epsilon") * NumericLib::machp("Base"); // unit in last place
  ulp = std::numeric_limits<double>::epsilon() * std::numeric_limits<double>::radix;

  //sum each row
  for( i=0 ; i<n ; ++i )
  {
    // can sum the columns since it's symmetric... and its faster
    dinvsqrt(i) = 2.*paramOffset_;
    for( j=0 ; j<n ; j++ )
      dinvsqrt(i) += W(i*(n)+j);
    dinvsqrt(i) = 1./sqrt(dinvsqrt(i)+ulp);
    MRSASSERT(dinvsqrt(i) == dinvsqrt(i));
  }

  // P <- dinvsqrt*dinvsqrt'
  for( i=0 ; i<n ; ++i )
    for( j=i ; j<n ; j++ )
      P(i*(n)+j) = dinvsqrt(i)*dinvsqrt(j);

  // 1. Add offset to diagonal of W
  // 2. P <- P .* W (only lower triangle of P computed)
  for( j=0 ; j<n ; j++ ) {
    P(j*(n)+j) = P(j*(n)+j)*( W(j*(n)+j) + paramOffset_ );
    for( i=j+1 ; i<n ; ++i ) {
      P(j*(n)+i) = P(j*(n)+i)*W(j*(n)+i);
    }
  }

  NumericLib::tred2(P, n, evals, mrs_naturalerm );
  NumericLib::tqli( evals, mrs_naturalerm, n, P );
  /* evals now contains the eigenvalues,
     P now contains the associated eigenvectors. */

  // Must reverse eigenvectors and eigenvalues because
  // tqli returns them in ascending order, rather than
  // descending
  for( j=0 ; j<nbcluster ; j++ )
  {
    for( i=0 ; i<n ; ++i )
    {
      NcutEigenvectors(j*(n)+i) = P((n-j-1)*(n)+i);
      MRSASSERT(NcutEigenvectors(j*n + i) == NcutEigenvectors(j*n + i));
    }
    NcutEigenvalues(j) = evals(n-j-1);
  }

  for( j=0 ; j<nbcluster ; j++ )
    for( i=0 ; i<n ; ++i )
    {
      NcutEigenvectors(j*(n)+i) = NcutEigenvectors(j*(n)+i)*dinvsqrt(i);
      MRSASSERT(NcutEigenvectors(j*n + i) == NcutEigenvectors(j*n + i));
    }


  for( j=0 ; j<nbcluster ; j++ ) {
    norm=0.;
    for( i=0 ; i<n ; ++i )
      norm += NcutEigenvectors(j*(n) + i)*NcutEigenvectors(j*(n) + i);
    norm = sqrt(norm);
    norm = sqrtn / norm;
    if( NcutEigenvectors(j*(n)) >= 0. )
      for( i=0 ; i<n ; ++i )
      {
        NcutEigenvectors(j*(n) + i) *= -norm;
        MRSASSERT(NcutEigenvectors(j*n + i) == NcutEigenvectors(j*n + i));
      }

    else
      for( i=0 ; i<n ; ++i )
      {
        NcutEigenvectors(j*(n) + i) *= norm;
        MRSASSERT(NcutEigenvectors(j*n + i) == NcutEigenvectors(j*n + i));
      }

  }

}

void NormCut::discretisation(mrs_natural n,  mrs_natural nbcluster, realvec &NcutEigenvectors, realvec &NcutDiscrete)
{
  realvec vm(n);
  realvec R(nbcluster*(nbcluster));
  realvec EVtimesR(n*(nbcluster));
  realvec c(n);
  realvec tmp(n);
  double minval;
  int mini	= 0; //AL: I don't really know what implications this has - but otherwise the variable might be used uninitialized...
  realvec EVDtimesEV(nbcluster*(nbcluster));
  double NcutValue;
  double lastObjectiveValue=0;

  // output params for dgesdd_
  realvec s(nbcluster+1); // array of singluar values in descending order
  realvec U(nbcluster*(nbcluster)); // U
  realvec V(nbcluster*(nbcluster)); // V

  //double ulp = NumericLib::machp("Epsilon") * NumericLib::machp("Base"); // unit in last place
  double ulp = std::numeric_limits<double>::epsilon() * std::numeric_limits<double>::radix;

  int nbIterDiscrete = 0;
  bool exitLoop = false;

  int randn;
  int i,j,k;

  //dataNcutParams param;
  //
  //   if( params != NULL )
  //      param = *params;
  //   else
  //   {
  //      param.offset = 0.5;
  //      param.verbose = 3;
  //      param.maxiterations = 20;
  //      param.eigsErrorTolerance = 0.000001;
  //   }

  for( i=0 ; i<n ; ++i ) {
    vm(i) = 0;
    for( j=0 ; j<nbcluster ; j++ )
    {
      vm(i) += NcutEigenvectors(j*(n)+i)*NcutEigenvectors(j*(n)+i);
      MRSASSERT(vm(i) == vm(i));
    }
    vm(i) = sqrt(vm(i));
    for( j=0 ; j<nbcluster ; j++ ) {
      if (vm(i) > 0)
        NcutEigenvectors(j*(n)+i) /= vm(i);
      else
        NcutEigenvectors(j*(n)+i)  = 0.;
      MRSASSERT(NcutEigenvectors(j*(n)+i) == NcutEigenvectors(j*(n)+i));
    }
    c(i) = 0;
  }

  // randn = (int)floor(0.5+(n-1)*(0.42863169099606)); // rand()/RAND_MAX for large decimal -- temporary while debugging
  randn	= 0; // although I have absolutely no clue what this means, everything seems to work better with this being 0 (AL)

  for( i=0 ; i<nbcluster ; ++i ) {
    R(i) = NcutEigenvectors(i*(n)+randn);
    MRSASSERT(R(i) == R(i));
    for( j=0 ; j<nbcluster ; j++ )
      U(i*(nbcluster)+j) = 0.0;
  }

  for( j=1 ; j<nbcluster ; j++ )
  {
    minval = MAXREAL;

    for( i=0 ; i<n ; ++i )
    {
      tmp(i) = 0.;
      for( k=0 ; k<nbcluster ; k++ )
        tmp(i) += NcutEigenvectors(k*(n)+i)*R((j-1)*(nbcluster)+k);
    }

    for( i=0 ; i<n ; ++i ) {
      c(i) += fabs(tmp(i));
      if( c(i) < minval )
      {
        minval = c(i);
        mini = i;
      }
    }

    for( i=0 ; i<nbcluster ; ++i ) {
      R(j*(nbcluster)+i) = NcutEigenvectors(i*(n) + mini);
      MRSASSERT(R(j*(nbcluster)+i) == R(j*(nbcluster)+i));
    }
  }

  //	 cout << endl;
  //  print(R, *nbcluster, *nbcluster);
  while( !exitLoop )
  {
    nbIterDiscrete += 1;

    for( i=0 ; i<n ; ++i ) {
      for( j=0 ; j<nbcluster ; j++ ) {
        EVtimesR(j*(n)+i) = 0.;
        for( k=0 ; k<nbcluster ; k++)
        {
          EVtimesR(j*(n)+i) += NcutEigenvectors(k*(n)+i)*R(j*(nbcluster)+k);
          MRSASSERT(EVtimesR(j*(n)+i) == EVtimesR(j*(n)+i));
        }
      }
    }

    discretisationEigenvectorData(n, nbcluster, EVtimesR,NcutDiscrete);

    for( i=0 ; i<nbcluster ; ++i ) {
      for( j=0 ; j<nbcluster ; j++ ) {
        EVDtimesEV(j*(nbcluster)+i) = 0.;
        for( k=0 ; k<n ; k++)
        {
          EVDtimesEV(j*(nbcluster)+i) += NcutDiscrete(k*(nbcluster)+i)*NcutEigenvectors(j*(n)+k);
          MRSASSERT(EVDtimesEV(j*(nbcluster)+i) == EVDtimesEV(j*(nbcluster)+i));
        }
      }
    }
    //  cout << endl;
    //  print(EVtimesR, *n, *nbcluster);
    // cout << endl;
    // print(EVDtimesEV, *nbcluster, *nbcluster);
    // Do SVD : store U,S,V
    NumericLib::svd( nbcluster, nbcluster, EVDtimesEV, U, V, s );

    // 2*( n-trace(S) )
    NcutValue = 0.;
    for( i=0 ; i<nbcluster ; ++i )
      NcutValue += s(i);
    NcutValue = 2*( n - NcutValue );

    if( fabs(NcutValue - lastObjectiveValue) < ulp || nbIterDiscrete > paramMaxiterations_ )
    {
      exitLoop = 1;
    }
    else
    {
      lastObjectiveValue = NcutValue;
      // R= V * U';
      for( i=0 ; i<nbcluster ; ++i ) {
        for( j=0 ; j<nbcluster ; j++ ) {
          R(j*(nbcluster)+i) = 0.;
          for( k=0 ; k<nbcluster ; k++)
            R(j*(nbcluster)+i) += V(k*(nbcluster)+i)*U(k*(nbcluster)+j);
        }
      }

    }
  }
}

void
NormCut::discretisationEigenvectorData(mrs_natural n,  mrs_natural nbcluster, realvec &V, realvec &Vdiscrete)
{
  mrs_real maxval;
  mrs_natural maxi = 0; //AL: I don't really know what implications this has - but otherwise the variable might be used uninitialized...;

  mrs_natural i,j;

  for( i=0 ; i<n ; ++i ) {
    maxval = -MAXREAL; // (Mathieu ??)

    // Find largest value in the ith row of the eigenvectors
    for( j=0 ; j<nbcluster ; j++ )
    {
      // Initialize NcutDiscrete as we go
      Vdiscrete(i*(nbcluster)+j) = 0.0;

      if( V(j*(n)+i) > maxval )
      {
        maxval = V(j*(n)+i);
        maxi = j;
      }
    }
    // Data i belongs to cluster maxi
    Vdiscrete(i*(nbcluster)+maxi) = 1.0;
    // Vdiscrete(i) = maxi;  // (Mathieu !!)
  }
}


/*
 * Display column-oriented matrices
 * Note : Matrices are stored in column-wise order for compatibility with CLAPACK (translated from FORTRAN)
 * Note2: n=1 --> display column vector         n=-1 --> display row vector
 */
void
NormCut::prmrs_natural( realvec &A, mrs_natural m , mrs_natural n )
{
  mrs_natural i,j;

  if( n > 0 )
  {
    for(i=0; i < m; ++i)
    {
      for (j=0; j < n; j++)
        cout << A(j*m+i) << "\t";
      cout << endl;
    }
  }
  else if( n == -1 )
  {
    for( i=0 ; i<m ; ++i )
      cout << A(i) << "\t";
    cout << endl;
  }
}

/*
 * Display column-oriented matrices
 * Note : Matrices are stored in column-wise order for compatibility with CLAPACK (translated from FORTRAN)
 * Note2: n=1 --> display column vector         n=-1 --> display row vector
 */

void
NormCut::print( realvec &A, int m , int n )
{
  int i,j;

  if( n > 0 )
  {
    for(i=0; i < m; ++i)
    {
      for (j=0; j < n; j++)
        cout << A(j*m+i) << "\t";
      cout << endl;
    }
  }
  else if( n == -1 )
  {
    for( i=0 ; i<m ; ++i )
      cout << A(i) << "\t";
    cout << endl;
  }
}

//
//
//void
//Marsyas::discretisation(mrs_natural *n,  mrs_natural *nbcluster, realvec &NcutEigenvectors, realvec &NcutDiscrete, dataNcutParamsPtr params)
//{
//   realvec vm(*n);
//   realvec R(*nbcluster*(*nbcluster));
//   realvec EVtimesR(*n*(*nbcluster));
//   realvec c(*n);
//   realvec tmp(*n);
//   mrs_real minval;
//   mrs_natural mini;
//   realvec EVDtimesEV(*nbcluster*(*nbcluster)); // (Mathieu ??)
//   mrs_real NcutValue;
//   mrs_real lastObjectiveValue=0;
//
//   // output params for dgesdd_
//   realvec s(*nbcluster+1); // array of singluar values in descending order
//   realvec U(*nbcluster*(*nbcluster)); // U
//   realvec V(*nbcluster*(*nbcluster)); // V
//
//	 mrs_real ulp = NumericLib::machp("Epsilon") * NumericLib::machp("Base"); // unit in last place
//
//   mrs_natural nbIterDiscrete = 0;
//   bool exitLoop = false;
//
//   mrs_natural randn;
//   mrs_natural i,j,k;
//
//   dataNcutParams param;
//
//   if( params != NULL )
//      param = *params;
//   else
//   {
//      param.offset = 0.5;
//      param.verbose = 3;
//      param.maxiterations = 20;
//      param.eigsErrorTolerance = 0.000001;
//   }
//
//   for( i=0 ; i<*n ; ++i ){
//      vm(i) = 0;
//      for( j=0 ; j<*nbcluster ; j++ )
//         vm(i) += NcutEigenvectors(j*(*n)+i)*NcutEigenvectors(j*(*n)+i);
//      vm(i) = sqrt(vm(i));
//      for( j=0 ; j<*nbcluster ; j++ ){
//         NcutEigenvectors(j*(*n)+i) /= vm(i);
//      }
//      c(i) = 0;
//   }
//
//
//   randn = (mrs_natural) floor(0.5+(*n-1)*(0.42863169099606)); // rand()/RAND_MAX for large decimal -- temporary while debugging
////   randn = (mrs_natural)round((*n-1)*(mrs_real)rand()/RAND_MAX);
//
//
//   for( i=0 ; i<*nbcluster ; ++i ){
//      R(i) = NcutEigenvectors(i*(*n)+randn);
//      for( j=0 ; j<*nbcluster ; j++ )
//         U(i*(*nbcluster)+j) = 0.0;
//   }
//
//   for( j=1 ; j<*nbcluster ; j++ )
//   {
//      minval = MAXREAL;
//
//      for( i=0 ; i<*n ; ++i )
//      {
//         tmp(i) = 0.;
//         for( k=0 ; k<*nbcluster ; k++ )
//            tmp(i) += NcutEigenvectors(k*(*n)+i)*R((j-1)*(*nbcluster)+k);
//      }
//
//      for( i=0 ; i<*n ; ++i ){
//         c(i) += fabs(tmp(i));
//         if( c(i) < minval )
//         {
//            minval = c(i);
//            mini = i;
//         }
//      }
//
//      for( i=0 ; i<*nbcluster ; ++i ){
//         R(j*(*nbcluster)+i) = NcutEigenvectors(i*(*n) + mini);
//      }
//   }
//	 cout << endl;
//   prmrs_natural(R, *nbcluster, *nbcluster);
//
//
//   while( !exitLoop )
//   {
//      nbIterDiscrete += 1;
//
//      for( i=0 ; i<*n ; ++i ){
//         for( j=0 ; j<*nbcluster ; j++ ){
//            EVtimesR(j*(*n)+i) = 0.;
//            for( k=0 ; k<*nbcluster ; k++)
//               EVtimesR(j*(*n)+i) += NcutEigenvectors(k*(*n)+i)*R(j*(*nbcluster)+k);
//         }
//      }
//			cout << endl;
//      prmrs_natural(EVtimesR, *n, *nbcluster);
//
//      discretisationEigenvectorData(n, nbcluster, EVtimesR,NcutDiscrete);
//
//      for( i=0 ; i<*nbcluster ; ++i ){
//         for( j=0 ; j<*nbcluster ; j++ ){ // (Mathieu n replaced by nbcluster)
//            EVDtimesEV(j*(*nbcluster)+i) = 0.;
//            for( k=0 ; k<*n ; k++)
//               EVDtimesEV(j*(*nbcluster)+i) += NcutDiscrete(k*(*nbcluster)+i)*NcutEigenvectors(j*(*n)+k);
//         }
//      }
//			cout << endl;
//      prmrs_natural(EVDtimesEV, *nbcluster, *nbcluster);
//      // Do SVD : store U,S,V
//			NumericLib::svd( *nbcluster, *nbcluster, EVDtimesEV, U, V, s );
//
//      // 2*( n-trace(S) )
//      NcutValue = 0.;
//      for( i=0 ; i<*nbcluster ; ++i )
//         NcutValue += s(i);
//      NcutValue = 2*( *n - NcutValue );
//
//      if( fabs(NcutValue - lastObjectiveValue) < ulp || nbIterDiscrete > param.maxiterations )
//      {
//         exitLoop = 1;
//      }
//      else
//      {
//         lastObjectiveValue = NcutValue;
//         // R= V * U';
//         for( i=0 ; i<*nbcluster ; ++i ){
//            for( j=0 ; j<*nbcluster ; j++ ){
//               R(j*(*nbcluster)+i) = 0.;
//               for( k=0 ; k<*nbcluster ; k++)
//                  R(j*(*nbcluster)+i) += V(k*(*nbcluster)+i)*U(k*(*nbcluster)+j);
//            }
//         }
//
//      }
//   }
//}
