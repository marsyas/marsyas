/*
 *  NormCut.cpp
 *  normalizedCut
 *
 *  Created by Jennifer Murdoch on 17/11/06.
 *  Copyright 2006. All rights reserved.
 *
 */

#include "NormCut.h"
#include "NumericLib.h"

#include <iostream>
#include <math.h>

#define ERROR(x) {cout << "Error : " << x << endl; exit(EXIT_FAILURE); }

using namespace std;
using namespace Marsyas;

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
void 
Marsyas::ncutW(mrs_natural *n, realvec &W, mrs_natural *nbcluster, realvec &NcutDiscrete, realvec &NcutEigenvectors, realvec &NcutEigenvalues, dataNcutParamsPtr params)
{
   ncut( n, W, nbcluster, NcutEigenvectors, NcutEigenvalues, params );
	/* prmrs_natural(NcutEigenvectors, *n, *nbcluster);
	 prmrs_natural(NcutEigenvalues, *nbcluster, 1);*/
   discretisation( n, nbcluster, NcutEigenvectors, NcutDiscrete, params );
	// prmrs_natural(NcutDiscrete, *n, *nbcluster);
}

void 
Marsyas::ncut(mrs_natural *n, realvec &W, mrs_natural *nbcluster, realvec &NcutEigenvectors, realvec &NcutEigenvalues, dataNcutParamsPtr params)
{
   dataNcutParams param; 
   realvec dinvsqrt(*n);
   mrs_real ulp;
   realvec P(*n*(*n));
   
   mrs_real norm;
   mrs_real sqrtn = sqrt((mrs_real) *n);
   
   *nbcluster = min(*nbcluster,*n); 
   
   // params required by dsyevr_ but are not initialized or used 
   realvec evals(*n);
   realvec mrs_naturalerm(*n);
   
   mrs_natural i,j;
   
   if( params != NULL )
      param = *params;
   else
   {
      param.offset = 0.5;
      param.verbose = 3;
      param.maxiterations = 20;
      param.eigsErrorTolerance = 0.000001;
   }
   
   // Check for matrix symmetry
   for( i=0 ; i<*n ; i++ )
      for( j=0 ; j<*n ; j++ ){
         if( W(i*(*n)+j) != W(j*(*n)+i) )
            ERROR("W is not symmetric");
         P(i*(*n)+j)=0.; 
      }           
         
			ulp = NumericLib::machp("Epsilon") * NumericLib::machp("Base"); // unit in last place            
   
   //sum each row 
   for( i=0 ; i<*n ; i++ )
   {
      // can sum the columns since it's symmetric... and its faster
      dinvsqrt(i) = 2.*param.offset;
      for( j=0 ; j<*n ; j++ )
         dinvsqrt(i) += W(i*(*n)+j);
      dinvsqrt(i) = 1./sqrt(dinvsqrt(i)+ulp);
   }
   
   // P <- dinvsqrt*dinvsqrt'
   for( i=0 ; i<*n ; i++ )
      for( j=i ; j<*n ; j++ )
         P(i*(*n)+j) = dinvsqrt(i)*dinvsqrt(j);
   
   // 1. Add offset to diagonal of W
   // 2. P <- P .* W (only lower triangle of P computed)
   for( j=0 ; j<*n ; j++ ){
      P(j*(*n)+j) = P(j*(*n)+j)*( W(j*(*n)+j) + param.offset );       
      for( i=j+1 ; i<*n ; i++ ){
         P(j*(*n)+i) = P(j*(*n)+i)*W(j*(*n)+i);
      }  
   }         

   NumericLib::tred2(P, *n, evals, mrs_naturalerm );
   NumericLib::tqli( evals, mrs_naturalerm, *n, P );
   /* evals now contains the eigenvalues,
      P now contains the associated eigenvectors. */        
   
   // Must reverse eigenvectors and eigenvalues because
   // tqli returns them in ascending order, rather than
   // descending
   for( j=0 ; j<*nbcluster ; j++ )
   {
      for( i=0 ; i<*n ; i++ )
         NcutEigenvectors(j*(*n)+i) = P((*n-j-1)*(*n)+i);
      NcutEigenvalues(j) = evals(*n-j-1);
   }
   
   for( j=0 ; j<*nbcluster ; j++ )   
      for( i=0 ; i<*n ; i++ )
         NcutEigenvectors(j*(*n)+i) = NcutEigenvectors(j*(*n)+i)*dinvsqrt(i); 
   
   for( j=0 ; j<*nbcluster ; j++ ){
      norm=0.;
      for( i=0 ; i<*n ; i++ )
         norm += NcutEigenvectors(j*(*n) + i)*NcutEigenvectors(j*(*n) + i);
      norm = sqrt(norm);
      norm = sqrtn / norm;
      if( NcutEigenvectors(j*(*n)) >= 0. )
         for( i=0 ; i<*n ; i++ )
            NcutEigenvectors(j*(*n) + i) *= -norm;
      else
         for( i=0 ; i<*n ; i++ )
            NcutEigenvectors(j*(*n) + i) *= norm;         
   }
   
}

void 
print( realvec &A, int m , int n )
{
   int i,j;
   
   if( n > 0 )
   {         
      for(i=0; i < m; i++)
      {
         for (j=0; j < n; j++) 
            cout << A(j*m+i) << "\t";      
         cout << endl;
      }   
   }      
   else if( n == -1 )
   {
      for( i=0 ; i<m ; i++ )
         cout << A(i) << "\t";
      cout << endl;   }
}

void Marsyas::discretisation(mrs_natural *n,  mrs_natural *nbcluster, realvec &NcutEigenvectors, realvec &NcutDiscrete, dataNcutParamsPtr params)
{
   realvec vm(*n);
   realvec R(*nbcluster*(*nbcluster)); 
   realvec EVtimesR(*n*(*nbcluster));
   realvec c(*n);
   realvec tmp(*n);
   double minval;
   int mini;
   realvec EVDtimesEV(*nbcluster*(*nbcluster));
   double NcutValue;
   double lastObjectiveValue=0;
   
   // output params for dgesdd_
   realvec s(*nbcluster+1); // array of singluar values in descending order
   realvec U(*nbcluster*(*nbcluster)); // U
   realvec V(*nbcluster*(*nbcluster)); // V     
   
	 double ulp = NumericLib::machp("Epsilon") * NumericLib::machp("Base"); // unit in last place  
   
   int nbIterDiscrete = 0;   
   bool exitLoop = false;
   
   int randn;
   int i,j,k;   
   
   dataNcutParams param;
   
   if( params != NULL )
      param = *params;
   else
   {
      param.offset = 0.5;
      param.verbose = 3;
      param.maxiterations = 20;
      param.eigsErrorTolerance = 0.000001;
   }   
   
   for( i=0 ; i<*n ; i++ ){
      vm(i) = 0;
      for( j=0 ; j<*nbcluster ; j++ )
         vm(i) += NcutEigenvectors(j*(*n)+i)*NcutEigenvectors(j*(*n)+i);
      vm(i) = sqrt(vm(i));
      for( j=0 ; j<*nbcluster ; j++ ){
         NcutEigenvectors(j*(*n)+i) /= vm(i);
      }
      c(i) = 0;
   }
   
//print(NcutEigenvectors, *n, *nbcluster);
   randn = (int)floor(0.5+(*n-1)*(0.42863169099606)); // rand()/RAND_MAX for large decimal -- temporary while debugging
//   randn = (int)round((*n-1)*(double)rand()/RAND_MAX);
//cout << randn << endl;
      
   for( i=0 ; i<*nbcluster ; i++ ){
      R(i) = NcutEigenvectors(i*(*n)+randn);     
      for( j=0 ; j<*nbcluster ; j++ )
         U(i*(*nbcluster)+j) = 0.0;
   }
   
   for( j=1 ; j<*nbcluster ; j++ )
   {
      minval = DBL_MAX;
   
      for( i=0 ; i<*n ; i++ )
      {
         tmp(i) = 0.;
         for( k=0 ; k<*nbcluster ; k++ )
            tmp(i) += NcutEigenvectors(k*(*n)+i)*R((j-1)*(*nbcluster)+k);
      }
      
      for( i=0 ; i<*n ; i++ ){
         c(i) += fabs(tmp(i));
         if( c(i) < minval )
         {
            minval = c(i);
            mini = i;
         }         
      }
      
      for( i=0 ; i<*nbcluster ; i++ ){
         R(j*(*nbcluster)+i) = NcutEigenvectors(i*(*n) + mini);
      }
   }
   
//	 cout << endl;
 //  print(R, *nbcluster, *nbcluster);
   while( !exitLoop )
   {
      nbIterDiscrete += 1;
            
      for( i=0 ; i<*n ; i++ ){
         for( j=0 ; j<*nbcluster ; j++ ){
            EVtimesR(j*(*n)+i) = 0.;
            for( k=0 ; k<*nbcluster ; k++)
               EVtimesR(j*(*n)+i) += NcutEigenvectors(k*(*n)+i)*R(j*(*nbcluster)+k);
         }
      }            
      
      discretisationEigenvectorData(n, nbcluster, EVtimesR,NcutDiscrete);   
      
      for( i=0 ; i<*nbcluster ; i++ ){
         for( j=0 ; j<*nbcluster ; j++ ){
            EVDtimesEV(j*(*nbcluster)+i) = 0.;
            for( k=0 ; k<*n ; k++)
               EVDtimesEV(j*(*nbcluster)+i) += NcutDiscrete(k*(*nbcluster)+i)*NcutEigenvectors(j*(*n)+k);
         }
      }                      
    //  cout << endl;
 //  print(EVtimesR, *n, *nbcluster);
	// cout << endl;
  // print(EVDtimesEV, *nbcluster, *nbcluster);
      // Do SVD : store U,S,V
			NumericLib::svd( *nbcluster, *nbcluster, EVDtimesEV, U, V, s );  
      
      // 2*( n-trace(S) )
      NcutValue = 0.;
      for( i=0 ; i<*nbcluster ; i++ )
         NcutValue += s(i);
      NcutValue = 2*( *n - NcutValue );
      
      if( fabs(NcutValue - lastObjectiveValue) < ulp || nbIterDiscrete > param.maxiterations )
      {
         exitLoop = 1;
      }
      else
      {
         lastObjectiveValue = NcutValue;
         // R= V * U';
         for( i=0 ; i<*nbcluster ; i++ ){
            for( j=0 ; j<*nbcluster ; j++ ){
               R(j*(*nbcluster)+i) = 0.;
               for( k=0 ; k<*nbcluster ; k++)
                  R(j*(*nbcluster)+i) += V(k*(*nbcluster)+i)*U(k*(*nbcluster)+j);
            }
         }
         
      }    
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
//   for( i=0 ; i<*n ; i++ ){
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
//   for( i=0 ; i<*nbcluster ; i++ ){
//      R(i) = NcutEigenvectors(i*(*n)+randn);     
//      for( j=0 ; j<*nbcluster ; j++ )
//         U(i*(*nbcluster)+j) = 0.0;
//   }
//   
//   for( j=1 ; j<*nbcluster ; j++ )
//   {
//      minval = DBL_MAX;
//   
//      for( i=0 ; i<*n ; i++ )
//      {
//         tmp(i) = 0.;
//         for( k=0 ; k<*nbcluster ; k++ )
//            tmp(i) += NcutEigenvectors(k*(*n)+i)*R((j-1)*(*nbcluster)+k);
//      }
//      
//      for( i=0 ; i<*n ; i++ ){
//         c(i) += fabs(tmp(i));
//         if( c(i) < minval )
//         {
//            minval = c(i);
//            mini = i;
//         }         
//      }
//      
//      for( i=0 ; i<*nbcluster ; i++ ){
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
//      for( i=0 ; i<*n ; i++ ){
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
//      for( i=0 ; i<*nbcluster ; i++ ){
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
//      for( i=0 ; i<*nbcluster ; i++ )
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
//         for( i=0 ; i<*nbcluster ; i++ ){
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

void
Marsyas::discretisationEigenvectorData(mrs_natural *n,  mrs_natural *nbcluster, realvec &V, realvec &Vdiscrete)
{
   mrs_real maxval;
   mrs_natural maxi;   
   
   mrs_natural i,j;
   
   for( i=0 ; i<*n ; i++ ){
      maxval = -DBL_MAX; // (Mathieu ??)      
      
      // Find largest value in the ith row of the eigenvectors
      for( j=0 ; j<*nbcluster ; j++ )
      {
         // Initialize NcutDiscrete as we go
         Vdiscrete(i*(*nbcluster)+j) = 0.0;
         
         if( V(j*(*n)+i) > maxval )
         {
            maxval = V(j*(*n)+i);
            maxi = j;
         }
      }
      // Data i belongs to cluster maxi
      Vdiscrete(i*(*nbcluster)+maxi) = 1.0;
  // Vdiscrete(i) = maxi;  // (Mathieu !!)
   }   
}


/* 
 * Display column-oriented matrices 
 * Note : Matrices are stored in column-wise order for compatibility with CLAPACK (translated from FORTRAN)
 * Note2: n=1 --> display column vector         n=-1 --> display row vector
 */
void 
Marsyas::prmrs_natural( realvec &A, mrs_natural m , mrs_natural n )
{
   mrs_natural i,j;
   
   if( n > 0 )
   {         
      for(i=0; i < m; i++)
      {
         for (j=0; j < n; j++) 
            cout << A(j*m+i) << "\t";      
         cout << endl;
      }   
   }      
   else if( n == -1 )
   {
      for( i=0 ; i<m ; i++ )
         cout << A(i) << "\t";
      cout << endl;   }
}

/* 
 * Display column-oriented matrices 
 * Note : Matrices are stored in column-wise order for compatibility with CLAPACK (translated from FORTRAN)
 * Note2: n=1 --> display column vector         n=-1 --> display row vector 
 */



