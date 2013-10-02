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

#include "PCA.h"
#include "../common_source.h"

#include "config.h"

using std::ostringstream;
using namespace Marsyas;

#define SIGN(a, b) ( (b) < 0 ? -fabs(a) : fabs(a) )


PCA::PCA(mrs_string name):MarSystem("PCA",name)
{
  //type_ = "PCA";
  //name_ = name;

  addControls();
}


PCA::PCA(const PCA& a): MarSystem(a)
{
  evals_ = NULL;
  interm_ = NULL;
}


PCA::~PCA()
{
  delete [] evals_;
  delete [] interm_;
}

MarSystem*
PCA::clone() const
{
  return new PCA(*this);
}

void
PCA::addControls()
{
  npcs_.create(3,3);

  addctrl("mrs_natural/npc",4);
  setctrlState("mrs_natural/npc",true);
  addctrl("mrs_realvec/pcs",npcs_);
  dims_ = 0;
  evals_ = NULL;
  interm_ = NULL;
}

void
PCA::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("PCA.cpp - PCA:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", getctrl("mrs_natural/npc"));
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  onObservations_ = getctrl("mrs_natural/onObservations")->to<mrs_natural>();

  npc_ = getctrl("mrs_natural/npc")->to<mrs_natural>();

  if( npcs_.getRows() != inObservations_-1 || npcs_.getCols() != npc_ )
    npcs_.create(inObservations_-1,npc_);

  if( npc_ != onObservations_-1 ) {

    updControl("mrs_natural/onObservations", npc_ +1);
    onObservations_ = npc_+1;
  }

  if( dims_ != inObservations_-1 )
  {
    dims_ = inObservations_-1;
    corr_matrix_.create(dims_,dims_);
    temp_matrix_.create(dims_, inSamples_);
    evals_ = new mrs_real[dims_];
    interm_ = new mrs_real[dims_];
  }

  ostringstream oss;
  for (int i=1 ; i <= npc_ ; ++i )
  {
    oss << "PC_" << i << ",";
  }
  setctrl("mrs_string/onObsNames", oss.str());
}

void
PCA::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  mrs_natural o1,o2;


  for (o=0; o < inObservations_-1; o++)
    for (t = 0; t < inSamples_; t++)
    {
      temp_matrix_(o, t) = in(o,t);
    }



  //in.meanSample(means_);//original code
  //in.stdSample(means_,stds_); //original code
  //means_.create(in.getRows());//not needed anymore if using new realvec::operator=() ;-)
  //stds_.create(in.getRows());//not needed anymore if using new realvec::operator=() ;-)
  temp_matrix_.meanObs(means_);
  temp_matrix_.stdObs(stds_);






  // Adjust data : ( X - means(X) ) / ( sqrt(n) * stds(X) )
  for (o=0; o < inObservations_-1; o++)
    for (t = 0; t < inSamples_; t++)
      temp_matrix_(o,t) = ( temp_matrix_(o,t) - means_(o) ) / ( sqrt((mrs_real)inSamples_) * stds_(o) ) ;


  // Calculate the correlation matrix
  for ( o1 = 0 ; o1 < inObservations_-1; o1++ )
  {
    corr_matrix_(o1,o1) = 1.0;
    for ( o2 = o1; o2 < inObservations_-1 ; o2++ )
    {
      corr_matrix_(o1,o2) = 0.0;
      for( t=0 ; t < inSamples_ ; t++ )
        corr_matrix_(o1,o2) += temp_matrix_(o1,t) * temp_matrix_(o2,t);
      corr_matrix_(o2,o1) = corr_matrix_(o1,o2);
    }
  }
  corr_matrix_(inObservations_-2, inObservations_-2) = 1.0;



  // Triangular decomposition
  tred2(corr_matrix_, inObservations_-1, evals_, interm_);


  // Reduction of symmetric tridiagonal matrix
  tqli( evals_, interm_, inObservations_-1, corr_matrix_);


  
  mrs_real percent_eig = 0.0;
  mrs_real sum_eig = 0.0;
  for (int m = inObservations_-2; m >= 0; m--)
    sum_eig += evals_[m];


  for (int m = inObservations_-2; m >= 0; m--)
    {
      percent_eig += evals_[m];
      std::cout << evals_[m] / sum_eig << "\t";
      std::cout << percent_eig / sum_eig << std::endl;
    }

  /* evals now contains the eigenvalues,
     corr_matrix_ now contains the associated eigenvectors. */

  /* Project row data onto the top "npc_" principal components. */
  for( t=0 ; t<inSamples_ ; t++ )
  {
    for( o=0 ; o<inObservations_ -1; o++ )
      {
	interm_[o] = in(o,t);
      }
    for( o=0 ; o<onObservations_ -1; o++ )
    {
      out(o,t) = 0.0;
      for(o2=0 ; o2 < inObservations_ -1; o2++)
	{
	  out(o,t) += interm_[o2] * corr_matrix_(o2,inObservations_-o-2);
	  npcs_(o2,o) = corr_matrix_(o2,inObservations_-o-2);
      }
    }
  }

  // copy the labels
  for( t=0 ; t<inSamples_ ; t++ )
    out(onObservations_-1, t) = in(inObservations_-1, t);
  setctrl("mrs_realvec/pcs",npcs_);

}

/*  Reduce a real, symmetric matrix to a symmetric, tridiag. matrix. */
void
PCA::tred2(realvec &a, mrs_natural m, mrs_real *d, mrs_real *e)
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
  mrs_natural l, k, j, i;
  mrs_real scale, hh, h, g, f;

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
  for (i = 0; i < m; ++i)
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
PCA::tqli(mrs_real d[], mrs_real e[], mrs_natural m, realvec &z)
/*
 Source code adapted from F. Murtagh, Munich, 6 June 1989
 http://astro.u-strasbg.fr/~fmurtagh/mda-sw/pca.c
*/
{
  mrs_natural n, l, i, k;
  mrs_real s, r, p, g, f, dd, c, b;

  for (i = 1; i < m; ++i)
    e[i-1] = e[i];
  e[m-1] = 0.0;
  for (l = 0; l < m; l++)
  {
#ifdef MARSYAS_ASSERTS
    mrs_natural iter = 0;
#endif
    do
    {
      for (n = l; n < m-1; n++)
      {
        dd = fabs(d[n]) + fabs(d[n+1]);
        if (fabs(e[n]) + dd == dd) break;
      }
      if (n != l)
      {
        MRSASSERT( iter++ != 30 ); // No convergence

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








