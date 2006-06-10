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

/** 
   \class PCA
   \brief Perform Principal Component Analysis

   Perform Principal Component Analysis (PCA) on all samples of the 
   incoming realvec of data. The correlation method is taken, with a
   correlation matrix computed over all samples within the single
   input realvec.  Eigenvalue/Eigenvector calculation is by the QL
   algorithm (ie. not suitable when the correlation matrix is 
              singular or near singular).  Output is the set of input samples 
   projected onto the top "npc" (a MarSystem control) principal
   components.
*/


#include "PCA.h"
using namespace std;

#define SIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )


PCA::PCA(string name)
{
  type_ = "PCA";
  name_ = name;
  addControls();
}

PCA::~PCA()
{
}

MarSystem* 
PCA::clone() const 
{
  return new PCA(*this);
}

void 
PCA::addControls()
{
  addDefaultControls();
   
   addctrl("natural/npc",3);
   setctrlState("natural/npc",true);
   dims_ = 0;
}

void
PCA::update()
{
  MRSDIAG("PCA.cpp - PCA:update");
  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/npc"));
  setctrl("real/osrate", getctrl("real/israte"));

  inObservations_ = getctrl("natural/inObservations").toNatural();
  onObservations_ = getctrl("natural/onObservations").toNatural();
  
  npc_ = getctrl("natural/npc").toNatural();
  
  if( npc_ != onObservations_ ){
     updctrl("natural/onObservations", npc_ );
     onObservations_ = npc_;
  }
    
  if( dims_ != inObservations_ ) 
  {
     dims_ = inObservations_;
     corr_matrix_.create(dims_,dims_);
  }
  
  ostringstream oss;
  for (int i=1 ; i <= npc_ ; i++ )
  {
     oss << "PC_" << i << ",";
  }
  setctrl("string/onObsNames", oss.str());
  
  defaultUpdate();  
}

void 
PCA::process(realvec& in, realvec& out)
{
  checkFlow(in,out);
      
  natural o1,o2;
   
  realvec in_data_( in );

  real evals [inObservations_];
  real interm[inObservations_];  
  
  in.meanSample(means_);
  in.stdSample(means_,stds_);  

  // Adjust data : ( X - means(X) ) / ( sqrt(n) * stds(X) )
  for (o=0; o < inObservations_; o++)
     for (t = 0; t < inSamples_; t++)
        in(o,t) = ( in(o,t) - means_(o) ) / ( sqrt(inSamples_) * stds_(o) ) ;
  
  // Calculate the correlation matrix
  for ( o1 = 0 ; o1 < inObservations_-1 ; o1++ )
  {
     corr_matrix_(o1,o1) = 1.0;
     for ( o2 = o1+1 ; o2 < inObservations_ ; o2++ )
     {
        corr_matrix_(o1,o2) = 0.0;
        for( t=0 ; t < inSamples_ ; t++ )
           corr_matrix_(o1,o2) += in(o1,t) * in(o2,t);
        corr_matrix_(o2,o1) = corr_matrix_(o1,o2);
     }
  }
  corr_matrix_(inObservations_-1, inObservations_-1) = 1.0;
  
  // Triangular decomposition
  tred2(corr_matrix_, inObservations_, evals, interm);
  // Reduction of symmetric tridiagonal matrix
  tqli( evals, interm, inObservations_, corr_matrix_);
       
  /* evals now contains the eigenvalues,
     corr_matrix_ now contain the associated eigenvectors. */
  
  /* Project row data onto the top "npc_" principal components. */  
  for( t=0 ; t<inSamples_ ; t++ )
  {
     for( o=0 ; o<inObservations_ ; o++ )
        interm[o] = in(o,t); 
     
     for( o=0 ; o<onObservations_ ; o++ )
     {
        out(o,t) = 0.0;
        for(o2=0 ; o2 < inObservations_ ; o2++)
           out(o,t) += interm[o2] * corr_matrix_(o2,inObservations_-o-1);
     }
  }    
}

/*  Reduce a real, symmetric matrix to a symmetric, tridiag. matrix. */
void 
PCA::tred2(realvec &a, natural m, real *d, real *e)
/* Householder reductiom of matrix a to tridiagomal form.
Algorithm: Martim et al., Num. Math. 11, 181-195, 1968.
Ref: Smith et al., Matrix Eigemsystem Routimes -- EISPACK Guide
Sprimger-Verlag, 1976, pp. 489-494.
W H Press et al., Numerical Recipes im C, Cambridge U P,
1988, pp. 373-374.  

Source code adapted from F. Murtagh, Munich, 6 June 1989
http://astro.u-strasbg.fr/~fmurtagh/mda-sw/pca.c
*/
{
   natural l, k, j, i;
   real scale, hh, h, g, f;
   
   for (i = m-1; i > 0; i--)
   {
      l = i - 1;
      h = scale = 0.0;
      if (l > 0)
      {
         for (k = 0; k <= l; k++)
            scale += fabs(a(i,k));
         if (scale == 0.0)
            e[i] = a(i,l);
         else
         {
            for (k = 0; k <= l; k++)
            {
               a(i,k) /= scale;
               h += a(i,k) * a(i,k);
            }
            f = a(i,l);
            g = f>0 ? -sqrt(h) : sqrt(h);
            e[i] = scale * g;
            
            h -= f * g;
            a(i,l) = f - g;
            f = 0.0;
            for (j = 0; j <= l; j++)
            {
               a(j,i) = a(i,j)/h;
               g = 0.0;
               for (k = 0; k <= j; k++)
                  g += a(j,k) * a(i,k);
               for (k = j+1; k <= l; k++)
                  g += a(k,j) * a(i,k);
               e[j] = g / h;
               f += e[j] * a(i,j);
            }
            hh = f / (h + h);
            for (j = 0; j <= l; j++)
            {
               f = a(i,j);
               e[j] = g = e[j] - hh * f;
               for (k = 0; k <= j; k++)
                  a(j,k) -= (f * e[k] + g * a(i,k));
            }
         }
      }
      else
         e[i] = a(i,l);
      d[i] = h;
   }
   d[0] = 0.0;
   e[0] = 0.0;
   for (i = 0; i < m; i++)
   {
      l = i - 1;
      if (d[i])
      {
         for (j = 0; j <= l; j++)
         {
            g = 0.0;
            for (k = 0; k <= l; k++)
               g += a(i,k) * a(k,j);
            for (k = 0; k <= l; k++)
               a(k,j) -= g * a(k,i);
         }
      }
      d[i] = a(i,i);
      a(i,i) = 1.0;

      for (j = 0; j <= l; j++)
         a(j,i) = a(i,j) = 0.0;
   }
}

/*  Tridiagonal QL algorithm -- Implicit  */
void 
PCA::tqli(real d[], real e[], natural m, realvec &z)
/*
 Source code adapted from F. Murtagh, Munich, 6 June 1989
 http://astro.u-strasbg.fr/~fmurtagh/mda-sw/pca.c
*/
{
   natural n, l, iter, i, k;
   real s, r, p, g, f, dd, c, b;
   
   for (i = 1; i < m; i++)
      e[i-1] = e[i];
   e[m] = 0.0;
   for (l = 0; l < m; l++)
   {
      iter = 0;
      do
      {
         for (n = l; n < m-1; n++)
         {
            dd = fabs(d[n]) + fabs(d[n+1]);
            if (fabs(e[n]) + dd == dd) break;
         }
         if (n != l)
         {
            assert( iter++ != 30 ); // No convergence

            g = (d[l+1] - d[l]) / (2.0 * e[l]);
            r = sqrt((g * g) + 1.0);
            g = d[n] - d[l] + e[l] / (g + SIGN(r, g));
            s = c = 1.0;
            p = 0.0;
            for (i = n-1; i >= l; i--) 
            {
               f = s * e[i];
               b = c * e[i];
               if (fabs(f) >= fabs(g))
               {
                  c = g / f;
                  r = sqrt((c * c) + 1.0);
                  e[i+1] = f * r;
                  c *= (s = 1.0/r);
               }
               else
               {
                  s = f / g;
                  r = sqrt((s * s) + 1.0);
                  e[i+1] = g * r;
                  s *= (c = 1.0/r);
               }
               g = d[i+1] - p;
               r = (d[i] - g) * s + 2.0 * c * b;
               p = s * r;
               d[i+1] = g + p;
               g = c * r - b;
               for (k = 0; k < m; k++)
               {
                  f = z(k,i+1);
                  z(k,i+1) = s * z(k,i) + c * f;
                  z(k,i) = c * z(k,i) - s * f;
               }
            }
            d[l] = d[l] - p;
            e[l] = g;
            e[n] = 0.0;
         }
      }  while (n != l);
   }
}







	
