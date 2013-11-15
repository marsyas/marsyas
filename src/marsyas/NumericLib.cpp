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

#include <marsyas/common_source.h>
#include <marsyas/NumericLib.h>

//*************************************************************
// Includes for determinant calculation (adapted from):
// http://perso.wanadoo.fr/jean-pierre.moreau/c_matrices.html
//*************************************************************
#include <marsyas/basis.h>
#include "vmblock.h"
#include "lu.h"
//*************************************************************

#include <cfloat>
#include <limits>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using std::ostringstream;
using std::numeric_limits;
using std::endl;
using std::cout;
using std::cerr;
using std::min;
using std::max;



using namespace Marsyas;

//#define DBL_EPSILON 2.2204460492503131E-16 //=> already defined in float.h...
//#define DBL_MAX 1.7976931348623157E+308 //=> already defined in float.h...
//#define  PI  3.14159265358979323846 /* circular transcendental nb.  */ //already defined in common_source.h

#define NumericLib_MAXCOEFF 5001     /* max. number of coefficients  */

#define NumLib_TRUE 1
#define NumLib_FALSE 0

//macros for complex operations (to avoid modifying the legacy code...)
//Use STL <complex> algorithms instead of proprietary (and now commented) methods
#define Cadd(a,b) (a+b)
#define Csub(a,b) (a-b)
#define Cmul(a,b) (a*b)
#define Cdiv(a,b) (a/b)
#define Ccomplex(a,b) mrs_complex(a,b)
#define Cabs(z) std::abs(z)
#define Carg(z) std::arg(z)
#define Conjg(z) std::conj(z)
#define Csqrt(z) std::sqrt(z)
#define RCmul(x,a) (x*a)
#define RCadd(x,a) (x+a)
#define CADD(x,a,b) {x = a + b;} //due to some legacy code...
#define CMUL(x,a,b) {x = a * b;} //due to some legacy code...
#define COMPLEXM(x,a,b) {x = mrs_complex(a,b);} //due to some legacy code...

//MULLER macros
#define ITERMAXMULLER   150   /* max. number of iteration steps                 */
#define CONVERGENCE 100/* halve q2, when |P(x2)/P(x1)|^2 > CONVERGENCE   */
#define MAXDIST   1e3  /* max. relative change of distance between       */
/* x-values allowed in one step                   */
#define FACTORMULLER    1e5  /* if |f2|<FACTOR*macc and (x2-x1)/x2<FACTOR*macc */
/* then root is determined; end routine           */
#define KITERMAX  1e3  /* halve distance between old and new x2 max.     */
/* KITERMAX times in case of possible overflow    */
#define FVALUEMULLER    1e36 /* initialisation of |P(x)|^2                     */
#define BOUND1    1.01 /* improve convergence in case of small changes   */
#define BOUND2    0.99 /* of |P(x)|^2                                    */
#define BOUND3    0.01
#define BOUND4    sqrt(DBL_MAX)/1e4 /* if |P(x2).real()|+|P(x2).i|>BOUND4 =>  */
/* suppress overflow of |P(x2)|^2                 */
#define BOUND6    log10(BOUND4)-4   /* if |x2|^nred>10^BOUND6 =>         */
/* suppress overflow of P(x2)                     */
#define BOUND7    1e-5 /* relative distance between determined root and  */
/* real root bigger than BOUND7 => 2. iteration   */
#define NOISESTART DBL_EPSILON*1e2 /* when noise starts counting         */
#define NOISEMAX  5     /* if noise>NOISEMAX: terminate iteration        */

//NEWTON macros
#define  ITERMAXNEWTON  20   /* max. number of iterations                      */
#define  FACTORNEWTON   5    /* calculate new dx, when change of x0 is smaller */
/* than FACTOR*(old change of x0)                 */
#define  FVALUENEWTON   1E36 /* initialisation of |P(xmin)|                    */
#define  BOUND    sqrt(DBL_EPSILON)
/* if the imaginary part of the root is smaller   */
/* than BOUND5 => real root                       */
#define  NOISEMAX 5    /* max. number of iterations with no better value */


NumericLib::NumericLib()
{

}

NumericLib::~NumericLib()
{

}

bool
NumericLib::polyRoots(std::vector<mrs_complex> coefs, bool complexCoefs, mrs_natural order, std::vector<mrs_complex> &roots)
{
  // complexCoefs == true  => complex coefficients (UNTESTED!!) [!]
  // complexCoefs == false => real    coefficients

  unsigned char error;
  mrs_real maxerr;
  mrs_complex* pred = new mrs_complex[NumericLib_MAXCOEFF];//coeff. vector for deflated polynom (null() just needs it...)

  //determine polynomial roots
  error = null(&coefs[0], pred, &order, &roots[0], &maxerr, complexCoefs);

  delete [] pred;

  if (error==0)
    return true;
  else
  {
    MRSERR("NumericLib::polyRoots() - numeric error in polynomial roots calculation!");
    return false;
  }
}

mrs_real
NumericLib::determinant(realvec matrix)
{
  //********************************************************************
  // Determinant calculation adapted by <lmartins@inescporto.pt> from:
  //	http://perso.orange.fr/jean-pierre.moreau/Cplus/deter1_cpp.txt
  //********************************************************************

  //input matrix must be square
  if(matrix.getCols() != matrix.getRows())
  {
    MRSERR("NumericLib::determinant() : input matrix should be square! Returning invalid determinant value...");
    return numeric_limits<mrs_real>::max();
  }

  /****************************************************
  * Calculate the determinant of a real square matrix *
  * by the LU decomposition method                    *
  * ------------------------------------------------- *
  * SAMPLE RUN:                                       *
  * (Calculate the determinant of real square matrix: *
  *        | 1  0.5  0  0    0  0    0  0    0 |      *
  *        |-1  0.5  1  0    0  0    0  0    0 |      *
  *        | 0 -1    0  2.5  0  0    0  0    0 |      *
  *        | 0  0   -1 -1.5  4  0    0  0    0 |      *
  *        | 0  0    0 -1   -3  5.5  0  0    0 |      *
  *        | 0  0    0  0   -1 -4.5  7  0    0 |      *
  *        | 0  0    0  0    0 -1   -6  8.5  0 |      *
  *        | 0  0    0  0    0  0   -1 -7.5 10 |      *
  *        | 0  0    0  0    0  0    0 -1   -9 |  )   *
  *                                                   *
  *                                                   *
  *  Determinant =   1.000000                         *
  *                                                   *
  *                                                   *
  *                 C++ version by J-P Moreau, Paris. *
  *****************************************************
  Exact value is: 1
  ---------------------------------------------------*/
  REAL **A;       // input matrix of size n x n
  int  *INDX;     // dummy integer vector
  void *vmblock = NULL;

  // NOTE: index zero not used here.

  int i, id, j, n, rc;
  REAL det;

  //n=9;
  n = matrix.getCols();

  vmblock = vminit();
  A       = (REAL **)vmalloc(vmblock, MATRIX,  n+1, n+1);
  INDX    = (int *)  vmalloc(vmblock, VEKTOR,  n+1, 0);

  if (! vmcomplete(vmblock))
  {
    MRSERR("NumericLib::determinant() : No memory! Returning invalid determinant value...");
    return numeric_limits<mrs_real>::max();
  }

  for (i=0; i<=n; ++i)
    for (j=0; j<=n; j++)
      A[i][j] = 0.0;

  // define matrix A column by column
  for (i=1; i<=n; ++i)
    for (j=1; j<=n; j++)
      A[i][j] = (REAL)matrix(i-1, j-1);

  //	A[1][1]=1.0; A[2][1]=-1.0;
  //	A[1][2]=0.5; A[2][2]=0.5; A[3][2]=-1.0;
  //	A[2][3]=1.0; A[4][3]=-1.0;
  //	A[3][4]=2.5; A[4][4]=-1.5; A[5][4]=-1.0;
  //	A[4][5]=4.0; A[5][5]=-3.0; A[6][5]=-1.0;
  //	A[5][6]=5.5; A[6][6]=-4.5; A[7][6]=-1.0;
  //	A[6][7]=7.0; A[7][7]=-6.0; A[8][7]=-1.0;
  //	A[7][8]=8.5; A[8][8]=-7.5; A[9][8]=-1.0;
  //	A[8][9]=10.0; A[9][9]=-9.0;
  //
  //call LU decomposition
  rc = LUDCMP(A,n,INDX,&id);

  //calculate determinant
  det = id;
  if (rc==0)  {
    for (i=1; i<=n; ++i)
      det *= A[i][i];
    return (mrs_real)det;
  }
  else {
    if(rc==-1)
    {
      MRSERR("NumericLib::determinant() : Memory Allocation error in LUDCMP()! Returning invalid determinant value...");
      return numeric_limits<mrs_real>::max();
    }
    else
    {
      MRSWARN("NumericLib::determinant() : Error in LU decomposition: singular input matrix. Determinant equals to zero.");
      return 0.0; // det(singular matrix) = 0
    }
  }
}

//************************************************************************************************
//				PRIVATE METHODS
//************************************************************************************************

// MULLER
/******************************************************************/
/*                                                                */
/* file:          muller.c                                        */
/*                                                                */
/* main function: muller()                                        */
/*                                                                */
/* version:       1.2                                             */
/*                                                                */
/* author:        B. Frenzel                                      */
/*                                                                */
/* date:          Jan 7 1993                                      */
/*                                                                */
/* input:         pred[]  coefficient vector of the deflated      */
/*                        polynomial                              */
/*                nred    the highest exponent of the deflated    */
/*                        polynomial                              */
/*                                                                */
/* return:        xb      determined root                         */
/*                                                                */
/* subroutines:   initialize(),root_of_parabola(),                */
/*                iteration_equation(), compute_function(),       */
/*                check_x_value(), root_check()                   */
/*                                                                */
/* description:                                                   */
/* muller() determines the roots of a polynomial with complex     */
/* coefficients with the Muller method; these roots are the       */
/* initial estimations for the following Newton method            */
/*                                                                */
/* Copyright:                                                     */
/* Lehrstuhl fuer Nachrichtentechnik Erlangen                     */
/* Cauerstr. 7, 91054 Erlangen, FRG, 1993                         */
/* e-mail: int@nt.e-technik.uni-erlangen.de                       */
/*                                                                */
/******************************************************************/

/***** main routine of Mueller's method *****/
mrs_complex
NumericLib::muller(mrs_complex *pred,mrs_natural nred)
/*mrs_complex *pred;    coefficient vector of the deflated polynomial    */
/*mrs_natural  nred;    the highest exponent of the deflated polynomial  */
{
  mrs_real   f1absq=FVALUEMULLER, /* f1absq=|f1MULLER_|^2                        */
             f2absq=FVALUEMULLER,		/* f2absq=|f2MULLER_|^2                        */
             f2absqb=FVALUEMULLER,		/* f2absqb=|P(xb)|^2                          */
             h2abs,						/* h2abs=|h2MULLER_|                           */
             epsilon;					/* bound for |q2MULLER_|                       */
  mrs_natural      seconditer=0,  /* second iteration, when root is too bad     */
                   noise=0,					/* noise counter                              */
                   rootd=NumLib_FALSE;				/* rootd = TRUE  => root determined           */
  /* rootd = FALSE => no root determined        */
  mrs_complex xb;					/* best x-value                               */

  /* initializing routine                       */
  initialize(pred,&xb,&epsilon);

  fdvalue(pred,nred,&f0MULLER_,&f0MULLER_,x0MULLER_,NumLib_FALSE);   /* compute exact function value */
  fdvalue(pred,nred,&f1MULLER_,&f1MULLER_,x1MULLER_,NumLib_FALSE);   /* oct-29-1993 ml               */
  fdvalue(pred,nred,&f2MULLER_,&f2MULLER_,x2MULLER_,NumLib_FALSE);

  do {                          /* loop for possible second iteration         */
    do {                      /* main iteration loop                        */
      /* calculate the roots of the parabola        */
      root_of_parabola();

      /* store values for the next iteration        */
      x0MULLER_ = x1MULLER_;
      x1MULLER_ = x2MULLER_;
      h2abs = Cabs(h2MULLER_);   /* distance between x2MULLER_ and x1MULLER_ */

      /* main iteration-equation                    */
      iteration_equation(&h2abs);

      /* store values for the next iteration        */
      f0MULLER_ = f1MULLER_;
      f1MULLER_ = f2MULLER_;
      f1absq = f2absq;

      /* compute P(x2MULLER_) and make some checks         */
      compute_function(pred,nred,f1absq,&f2absq,epsilon);

      /* printf("betrag %10.5e  %4.2d  %4.2d\n",f2absq,iterMULLER_,seconditer);*/

      /* is the new x-value the best approximation? */
      check_x_value(&xb,&f2absqb,&rootd,f1absq,f2absq,
                    epsilon,&noise);

      /* increase noise counter                     */
      if (fabs((Cabs(xb)-Cabs(x2MULLER_))/Cabs(xb))<NOISESTART)
        noise++;
    } while ((iterMULLER_<=ITERMAXMULLER) && (!rootd) && (noise<=NOISEMAX));

    seconditer++;	/* increase seconditer  */

    /* check, if determined root is good enough   */
    root_check(pred,nred,f2absqb,&seconditer,&rootd,&noise,xb);
  } while (seconditer==2);

  return xb;/* return best x value*/
}

/***** initializing routine *****/
void
NumericLib::initialize(mrs_complex *pred,mrs_complex *xb,mrs_real *epsilon)
/*mrs_complex *pred,     coefficient vector of the deflated polynomial */
/*            *xb;       best x-value                                  */
/*mrs_real    *epsilon;  bound for |q2MULLER_|                          */
{
  // FIXME Unused parameter;
  (void) pred;
  /* initial estimations for x0MULLER_,...,x2MULLER_ and its values */
  /* ml, 12-21-94 changed                                         */

  x0MULLER_ = Ccomplex(0.,0.);							/* x0MULLER_ = 0 + j*1           */
  x1MULLER_ = Ccomplex(-1./sqrt(2.0),-1./sqrt(2.0));		/* x1MULLER_ = 0 - j*1           */
  x2MULLER_ = Ccomplex(1./sqrt(2.0),1./sqrt(2.0));		/* x2MULLER_ = (1 + j*1)/sqrt(2) */

  h1MULLER_ = Csub(x1MULLER_,x0MULLER_); /* h1MULLER_ = x1MULLER_ - x0MULLER_      */
  h2MULLER_ = Csub(x2MULLER_,x1MULLER_); /* h2MULLER_ = x2MULLER_ - x1MULLER_      */
  q2MULLER_ = Cdiv(h2MULLER_,h1MULLER_); /* q2MULLER_ = h2MULLER_/h1MULLER_        */

  *xb      = x2MULLER_;				/* best initial x-value = zero   */
  *epsilon = FACTORMULLER*DBL_EPSILON;/* accuracy for determined root  */
  iterMULLER_ = 0;						/* reset iteration counter       */
}

/***** root_of_parabola() calculate smaller root of Muller's parabola *****/
void
NumericLib::root_of_parabola(void)
{
  mrs_complex A2,B2,C2,  /* variables to get q2MULLER_  */
              discr,     /* discriminante                      */
              N1,N2;     /* denominators of q2MULLER_           */

  /* A2 = q2MULLER_(f2MULLER_ - (1+q2MULLER_)f1MULLER_ + f0q2) */
  /* B2 = q2MULLER_[q2MULLER_(f0MULLER_-f1MULLER_) + 2(f2MULLER_-f1MULLER_)] + (f2MULLER_-f1MULLER_)*/
  /* C2 = (1+q2MULLER_)f[2]*/
  A2   = Cmul(q2MULLER_,Csub(Cadd(f2MULLER_,Cmul(q2MULLER_,f0MULLER_)),
                             Cmul(f1MULLER_,RCadd((mrs_real)1.,q2MULLER_))));
  B2   = Cadd(Csub(f2MULLER_,f1MULLER_),Cmul(q2MULLER_,Cadd(Cmul(q2MULLER_,
              Csub(f0MULLER_,f1MULLER_)),RCmul((mrs_real)2.,Csub(f2MULLER_,f1MULLER_)))));
  C2   = Cmul(f2MULLER_,RCadd((mrs_real)1.,q2MULLER_));
  /* discr = B2^2 - 4A2C2                   */
  discr = Csub(Cmul(B2,B2),RCmul((mrs_real)4.,Cmul(A2,C2)));
  /* denominators of q2MULLER_                     */
  N1 = Csub(B2,Csqrt(discr));
  N2 = Cadd(B2,Csqrt(discr));
  /* choose denominater with largest modulus    */
  if (Cabs(N1)>Cabs(N2) && Cabs(N1)>DBL_EPSILON)
    q2MULLER_ = Cdiv(RCmul((mrs_real)-2.,C2),N1);
  else if (Cabs(N2)>DBL_EPSILON)
    q2MULLER_ = Cdiv(RCmul((mrs_real)-2.,C2),N2);
  else
    q2MULLER_ = Ccomplex(cos((mrs_real)iterMULLER_),sin((mrs_real)iterMULLER_));
}

/***** main iteration equation: x2MULLER_ = h2MULLER_*q2MULLER_ + x2MULLER_ *****/
void
NumericLib::iteration_equation(mrs_real *h2abs)
/*mrs_real *h2abs;      Absolute value of the old distance*/
{
  mrs_real h2absnew,  /* Absolute value of the new h2MULLER_        */
           help;           /* help variable                             */

  h2MULLER_ = Cmul(h2MULLER_,q2MULLER_);
  h2absnew = Cabs(h2MULLER_);      /* distance between old and new x2MULLER_ */

  if (h2absnew > (*h2abs*MAXDIST)) { /* maximum relative change          */
    help = MAXDIST/h2absnew;
    h2MULLER_ = RCmul(help,h2MULLER_);
    q2MULLER_ = RCmul(help,q2MULLER_);
  }

  *h2abs = h2absnew; /* actualize old distance for next iteration*/

  x2MULLER_ = Cadd(x2MULLER_,h2MULLER_);
}

/**** suppress overflow *****/
void
NumericLib::suppress_overflow(mrs_natural nred)
/*mrs_natural nred;        the highest exponent of the deflated polynomial */
{
  mrs_natural		kiter;            /* internal iteration counter        */
  unsigned char	loop;             /* loop = FALSE => terminate loop    */
  mrs_real        help;             /* help variable                     */

  kiter = 0;                        /* reset iteration counter           */
  do {
    loop=NumLib_FALSE;                   /* initial estimation: no overflow   */
    help = Cabs(x2MULLER_);        /* help = |x2MULLER_|                 */
    if (help>1. && fabs(nred*log10(help))>BOUND6) {
      kiter++;      /* if |x2MULLER_|>1 and |x2MULLER_|^nred>10^BOUND6 */
      if (kiter<KITERMAX) { /* then halve the distance between   */
        h2MULLER_=RCmul((mrs_real).5,h2MULLER_); /* new and old x2MULLER_       */
        q2MULLER_=RCmul((mrs_real).5,q2MULLER_);
        x2MULLER_=Csub(x2MULLER_,h2MULLER_);
        loop=NumLib_TRUE;
      } else
        kiter=0;
    }
  } while(loop);
}

/***** check of too big function values *****/
void
NumericLib::too_big_functionvalues(mrs_real *f2absq)
/*mrs_real *f2absq;   f2absq=|f2MULLER_|^2          */
{
  if ((fabs(f2MULLER_.real())+fabs(f2MULLER_.imag()))>BOUND4)        /* limit |f2MULLER_|^2, when     */
    *f2absq = fabs(f2MULLER_.real())+fabs(f2MULLER_.imag());       /* |f2MULLER_.real()|+|f2MULLER_.imag()|>BOUND4   */
  else
    *f2absq = (f2MULLER_.real())*(f2MULLER_.real())+(f2MULLER_.imag())*(f2MULLER_.imag()); /* |f2MULLER_|^2 = f2MULLER_.real()^2+f2MULLER_.imag()^2 */
}

/***** Muller's modification to improve convergence *****/
void
NumericLib::convergence_check(mrs_natural *overflow,mrs_real f1absq,mrs_real f2absq,
                              mrs_real epsilon)
/*mrs_real f1absq,       f1absq = |f1MULLER_|^2                        */
/*       f2absq,       f2absq = |f2MULLER_|^2                          */
/*       epsilon;      bound for |q2MULLER_|                           */
/*mrs_natural    *overflow;    *overflow = TRUE  => overflow occures  */
/*                     *overflow = FALSE => no overflow occures		*/
{
  if ((f2absq>(CONVERGENCE*f1absq)) && (Cabs(q2MULLER_)>epsilon) &&
      (iterMULLER_<ITERMAXMULLER)) {
    q2MULLER_ = RCmul((mrs_real).5,q2MULLER_); /* in case of overflow:            */
    h2MULLER_ = RCmul((mrs_real).5,h2MULLER_); /* halve q2MULLER_ and h2MULLER_; compute new x2MULLER_ */
    x2MULLER_ = Csub(x2MULLER_,h2MULLER_);
    *overflow = NumLib_TRUE;
  }
}

/***** compute P(x2MULLER_) and make some checks *****/
void
NumericLib::compute_function(mrs_complex *pred,mrs_natural nred,mrs_real f1absq,
                             mrs_real *f2absq,mrs_real epsilon)
/*mrs_complex *pred;      coefficient vector of the deflated polynomial		*/
/*mrs_natural      nred;       the highest exponent of the deflated polynomial */
/*mrs_real   f1absq,     f1absq = |f1MULLER_|^2                                 */
/*         *f2absq,    f2absq = |f2MULLER_|^2									*/
/*         epsilon;    bound for |q2MULLER_|									*/
{
  mrs_natural    overflow;    /* overflow = TRUE  => overflow occures       */
  /* overflow = FALSE => no overflow occures    */

  do {
    overflow = NumLib_FALSE; /* initial estimation: no overflow          */

    /* suppress overflow                                */
    suppress_overflow(nred);

    /* calculate new value => result in f2MULLER_              */
    fdvalue(pred,nred,&f2MULLER_,&f2MULLER_,x2MULLER_,NumLib_FALSE);

    /* check of too big function values                 */
    too_big_functionvalues(f2absq);

    /* increase iterationcounter                        */
    iterMULLER_++;

    /* Muller's modification to improve convergence     */
    convergence_check(&overflow,f1absq,*f2absq,epsilon);
  } while (overflow);
}

/***** is the new x2MULLER_ the best approximation? *****/
void
NumericLib::check_x_value(mrs_complex *xb,mrs_real *f2absqb,mrs_natural *rootd,
                          mrs_real f1absq,mrs_real f2absq,mrs_real epsilon,
                          mrs_natural *noise)
/*mrs_complex *xb;        best x-value                                    */
/*mrs_real   *f2absqb,   f2absqb |P(xb)|^2								*/
/*         f1absq,     f1absq = |f1MULLER_|^2								*/
/*         f2absq,     f2absq = |f2MULLER_|^2                              */
/*         epsilon;    bound for |q2MULLER_|                               */
/*mrs_natural      *rootd,     *rootd = TRUE  => root determined          */
/*                     *rootd = FALSE => no root determined				*/
/*         *noise;     noisecounter										*/
{
  if ((f2absq<=(BOUND1*f1absq)) && (f2absq>=(BOUND2*f1absq))) {
    /* function-value changes slowly     */
    if (Cabs(h2MULLER_)<BOUND3) {  /* if |h[2]| is small enough =>      */
      q2MULLER_ = RCmul((mrs_real)2.,q2MULLER_);  /* mrs_real q2MULLER_ and h[2]                */
      h2MULLER_ = RCmul((mrs_real)2.,h2MULLER_);
    } else {                /* otherwise: |q2MULLER_| = 1 and           */
      /*            h[2] = h[2]*q2MULLER_         */
      q2MULLER_ = Ccomplex(cos((mrs_real)iterMULLER_),sin((mrs_real)iterMULLER_));
      h2MULLER_ = Cmul(h2MULLER_,q2MULLER_);
    }
  } else if (f2absq<*f2absqb) {
    *f2absqb = f2absq;      /* the new function value is the     */
    *xb      = x2MULLER_;          /* best approximation                */
    *noise   = 0;           /* reset noise counter               */
    if ((sqrt(f2absq)<epsilon) &&
        (Cabs(Cdiv(Csub(x2MULLER_,x1MULLER_),x2MULLER_))<epsilon))
      *rootd = NumLib_TRUE;     /* root determined                   */
  }
}

/***** check, if determined root is good enough. *****/
void
NumericLib::root_check(mrs_complex *pred,mrs_natural nred,mrs_real f2absqb,mrs_natural *seconditer,
                       mrs_natural *rootd,mrs_natural *noise,mrs_complex xb)
/*mrs_complex *pred,        coefficient vector of the deflated polynomial		*/
/*         xb;           best x-value											*/
/*mrs_natural      nred,         the highest exponent of the deflated polynomial */
/*         *noise,       noisecounter											*/
/*         *rootd,       *rootd = TRUE  => root determined						*/
/*                       *rootd = FALSE => no root determined					*/
/*         *seconditer;  *seconditer = TRUE  => start second iteration with		*/
/*                                              new initial estimations			*/
/*                       *seconditer = FALSE => end routine						*/
/*mrs_real   f2absqb;      f2absqb |P(xb)|^2										*/
{

  mrs_complex df;     /* df=P'(x0MULLER_)	*/

  if ((*seconditer==1) && (f2absqb>0)) {
    fdvalue(pred,nred,&f2MULLER_,&df,xb,NumLib_TRUE); /* f2MULLER_=P(x0MULLER_), df=P'(x0MULLER_)        */
    if (Cabs(f2MULLER_)/(Cabs(df)*Cabs(xb))>BOUND7) {
      /* start second iteration with new initial estimations        */
      /*	x0MULLER_ = Ccomplex(-1./sqrt(2),1./sqrt(2));
      	x1MULLER_ = Ccomplex(1./sqrt(2),-1./sqrt(2));
      	x2MULLER_ = Ccomplex(-1./sqrt(2),-1./sqrt(2)); */
      /*ml, 12-21-94: former initial values: */
      x0MULLER_ = Ccomplex(1.,0.);
      x1MULLER_ = Ccomplex(-1.,0.);
      x2MULLER_ = Ccomplex(0.,0.);       /*   */
      fdvalue(pred,nred,&f0MULLER_,&df,x0MULLER_,NumLib_FALSE); /* f0MULLER_ =  P(x0MULLER_)           */
      fdvalue(pred,nred,&f1MULLER_,&df,x1MULLER_,NumLib_FALSE); /* f1MULLER_ =  P(x1MULLER_)           */
      fdvalue(pred,nred,&f2MULLER_,&df,x2MULLER_,NumLib_FALSE); /* f2MULLER_ =  P(x2MULLER_)           */
      iterMULLER_ = 0;          /* reset iteration counter           */
      (*seconditer)++;         /* increase seconditer               */
      *rootd = NumLib_FALSE;          /* no root determined                */
      *noise = 0;              /* reset noise counter               */
    }
  }
}

//NEWTON
/******************************************************************/
/*                                                                */
/* file:          newton.c                                        */
/*                                                                */
/* main function: newton()                                        */
/*                                                                */
/* version:       1.2                                             */
/*                                                                */
/* author:        B. Frenzel                                      */
/*                                                                */
/* date:          Jan 7 1993                                      */
/*                                                                */
/* input:         p[]     coefficient vector of the original      */
/*                        polynomial                              */
/*                n       the highest exponent of the original    */
/*                        polynomial                              */
/*                ns      determined root with Muller method      */
/*                                                                */
/* output:        *dxabs error of determined root                 */
/*                                                                */
/* return:        xmin    determined root                         */
/* subroutines:   fdvalue()                                       */
/*                                                                */
/* description:                                                   */
/* newton() determines the root of a polynomial with complex      */
/* coefficients; the initial estimation is the root determined    */
/* by Muller's method                                             */
/*                                                                */
/* Copyright:                                                     */
/* Lehrstuhl fuer Nachrichtentechnik Erlangen                     */
/* Cauerstr. 7, 8520 Erlangen, FRG, 1993                          */
/* e-mail: int@nt.e-technik.uni-erlangen.de                       */
/*                                                                */
/******************************************************************/
mrs_complex
NumericLib::newton(mrs_complex *p,mrs_natural n,mrs_complex ns,mrs_real *dxabs,
                   unsigned char flag)
/*mrs_complex *p,         coefficient vector of the original polynomial		*/
/*			  ns;         determined root with Muller method				*/
/*mrs_natural n;          highest exponent of the original polynomial		*/
/*mrs_real    *dxabs;     dxabs  = |P(x0)/P'(x0)|							*/
/*unsigned char flag;  flag = TRUE  => complex coefficients					*/
/*                     flag = FALSE => real    coefficients					*/
{
  mrs_complex x0,        /* iteration variable for x-value     */
              xmin,              /* best x determined in newton()      */
              f,                 /* f       = P(x0)                    */
              df,                /* df      = P'(x0)                   */
              dx,                /* dx      = P(x0)/P'(x0)             */
              dxh;               /* help variable dxh = P(x0)/P'(x0)   */
  mrs_real   fabsmin=FVALUENEWTON,    /* fabsmin = |P(xmin)|   */
             eps=DBL_EPSILON;   /* routine ends, when estimated dist. */
  /* between x0 and root is less or     */
  /* equal eps                          */
  mrs_natural      iter   =0,	/* counter                       */
                   noise  =0;				/* noisecounter                  */

  x0   = ns; /* initial estimation = root determined			 */
  /* with Muller method										 */
  xmin = x0;        /* initial estimation for the best x-value */
  dx   = Ccomplex(1.,0.); /* initial value: P(x0)/P'(x0)=1+j*0 */
  *dxabs = Cabs(dx);     /* initial value: |P(x0)/P'(x0)|=1    */
  /* printf("%8.4e %8.4e\n",xmin.real(),xmin.imag()); */
  for (iter=0; iter<ITERMAXNEWTON; iter++) { /* main loop        */
    fdvalue(p,n,&f,&df,x0,NumLib_TRUE);  /* f=P(x0), df=P'(x0)      */

    if (Cabs(f)<fabsmin) {  /* the new x0 is a better           */
      xmin = x0;         /* approximation than the old xmin   */
      fabsmin = Cabs(f); /* store new xmin and fabsmin        */
      noise = 0;         /* reset noise counter               */
    }

    if (Cabs(df)!=0.) {     /* calculate new dx					*/
      dxh=Cdiv(f,df);
      if (Cabs(dxh)<*dxabs*FACTORNEWTON) { /* new dx small enough?  */
        dx = dxh;          /* store new dx for next				  */
        *dxabs = Cabs(dx); /* iteration                           */
      }
    }

    if (Cabs(xmin)!=0.) {
      if (*dxabs/Cabs(xmin)<eps || noise == NOISEMAX) {
        /* routine ends      */
        if (fabs(xmin.imag())<BOUND && flag==0) {
          /* define determined root as real,*/
          xmin = mrs_complex(xmin.real(),0.);//xmin.imag()=0.;  /* if imag. part<BOUND            */
        }
        *dxabs=*dxabs/Cabs(xmin); /* return relative error */
        return xmin;     /* return best approximation      */
      }
    }

    x0 = Csub(x0,dx); /* main iteration: x0 = x0 - P(x0)/P'(x0)  */

    noise++;  /* increase noise counter                          */
  }


  if (fabs(xmin.imag())<BOUND && flag==0) /* define determined root      */
    xmin = mrs_complex(xmin.real(),0.);//xmin.imag()=0.;/* as real, if imag. part<BOUND */
  if (Cabs(xmin)!=0.)
    *dxabs=*dxabs/Cabs(xmin); /* return relative error           */
  /* maximum number of iterations exceeded: */
  return xmin;            /* return best xmin until now             */
}

//NULL
/******************************************************************/
/*                                                                */
/* file:          null.cpp                                        */
/*                                                                */
/* main function: null()                                          */
/*                                                                */
/* version:       1.3 (adapted by lmartins@inescporto.pt 15.06.06)*/
/*                                                                */
/* author:        B. Frenzel                                      */
/*                                                                */
/* date:          Jan 19 1993                                     */
/*                                                                */
/* input:         p[]     coefficient vector of the original      */
/*                        polynomial                              */
/*                pred[]  coefficient vector of the deflated      */
/*                        polynomial                              */
/*                *n      the highest exponent of the original    */
/*                        polynomial                              */
/*                flag    flag = TRUE  => complex coefficients    */
/*                        flag = FALSE => real    coefficients    */
/*                                                                */
/* output:        root[]  vector of determined roots              */
/*                *maxerr estimation of max. error of all         */
/*                        determined roots                        */
/*                                                                */
/* subroutines:   poly_check(),quadratic(),lin_or_quad(),monic(), */
/*                muller(),newton()                               */
/*                                                                */
/* description:                                                   */
/* main rootfinding routine for polynomials with complex or real  */
/* coefficients using Muller's method combined with Newton's m.   */
/*                                                                */
/* Copyright:                                                     */
/* Lehrstuhl fuer Nachrichtentechnik Erlangen                     */
/* Cauerstr. 7, 8520 Erlangen, FRG, 1993                          */
/* e-mail: int@nt.e-technik.uni-erlangen.de                       */
/*                                                                */
/******************************************************************/
unsigned char
NumericLib::null(mrs_complex *p,mrs_complex *pred,mrs_natural *n,mrs_complex *root,
                 mrs_real *maxerr,unsigned char flag)
/*mrs_complex *p,     coefficient vector of the original polynomial   */
/*            *pred,  coefficient vector of the deflated polynomial   */
/*            *root;  determined roots                                */
/*mrs_natural *n;     the highest exponent of the original polynomial */
/*mrs_real    *maxerr; max. error of all determined roots             */
/*unsigned char flag;  flag = TRUE  => complex coefficients           */
/*                     flag = FALSE => real    coefficients           */
{
  mrs_real    newerr;  /* error of actual root                      */
  mrs_complex ns;      /* root determined by Muller's method        */
  mrs_natural nred,    /* highest exponent of deflated polynomial   */
              i;       /* counter                                   */
  unsigned char error; /* indicates an error in poly_check          */
  mrs_natural   red,
                diff;  /* number of roots at 0                      */

  *maxerr = 0.;    /* initialize max. error of determined roots */
  nred = *n;       /* At the beginning: degree defl. polyn. =   */
  /* degree of original polyn.                 */

  /* check input of the polynomial             */
  error = poly_check(p,&nred,n,root);
  diff  = (*n-nred); /* reduce polynomial, if roots at 0        */
  p    += diff;
  *n   =  nred;

  if (error)
    return error; /* error in poly_check(); return error     */

  /* polynomial is linear or quadratic       */
  if (lin_or_quad(p,nred,root)==0) {
    *n += diff;     /* remember roots at 0                   */
    *maxerr = DBL_EPSILON;
    return 0;       /* return no error                       */
  }

  monic(p,n);          /* get monic polynom                     */

  for (i=0; i<=*n; ++i)  pred[i]=p[i]; /* original polynomial    */
  /* = deflated polynomial at beginning   */
  /* of Muller                            */

  do {                  /* main loop of null()                  */
    /* Muller method                        */
    ns = muller(pred,nred);



    /* Newton method                        */
    root[nred-1] = newton(p,*n,ns,&newerr,flag);

    /* stores max. error of all roots       */
    if (newerr>*maxerr)
      *maxerr=newerr;
    /* deflate polynomial                   */
    red = poldef(pred,nred,root,flag);
    pred += red;        /* forget lowest coefficients        */
    nred -= red;        /* reduce degree of polynomial       */
  } while (nred>2);
  /* last one or two roots             */
  (void) lin_or_quad(pred,nred,root);
  if (nred==2) {
    root[1] = newton(p,*n,root[1],&newerr,flag);
    if (newerr>*maxerr)
      *maxerr=newerr;
  }
  root[0] = newton(p,*n,root[0],&newerr,flag);
  if (newerr>*maxerr)
    *maxerr=newerr;

  *n += diff;              /* remember roots at 0               */
  return 0;                /* return no error                   */
}

/***** poly_check() check the formal correctness of input *****/
unsigned char
NumericLib::poly_check(mrs_complex *pred,mrs_natural *nred,mrs_natural *n,mrs_complex *root)
/*mrs_complex *pred,  coefficient vector of the original polynomial   */
/*         *root;  determined roots                                */
/*mrs_natural      *nred,  highest exponent of the deflated polynomial     */
/*         *n;     highest exponent of the original polynomial     */
{
  mrs_natural  i = -1, /* i stores the (reduced) real degree            */
               j;      /* counter variable                              */
  unsigned char
  notfound=NumLib_TRUE; /* indicates, whether a coefficient      */
  /* unequal zero was found                */

  if (*n<0) return 1;  /* degree of polynomial less than zero   */
  /* return error                          */

  for (j=0; j<=*n; j++) {    /* determines the "real" degree of       */
    if(Cabs(pred[j])!=0.) /* polynomial, cancel leading roots      */
      i=j;
  }
  if (i==-1) return 2;   /* polynomial is a null vector; return error */
  if (i==0) return 3;    /* polynomial is constant unequal null;      */
  /* return error                              */

  *n=i;                  /* set new exponent of polynomial            */
  i=0;                   /* reset variable for exponent               */
  do {                   /* find roots at 0                           */
    if (Cabs(pred[i])==0.)
      ++i;
    else
      notfound=NumLib_FALSE;
  } while (i<=*n && notfound);

  if (i==0) {            /* no root determined at 0              */
    *nred = *n;        /* original degree=deflated degree and  */
    return 0;          /* return no error                      */
  } else {               /* roots determined at 0:               */
    for (j=0; j<=i-1; j++) /* store roots at 0                   */
      root[*n-j-1] = Ccomplex(0.,0.);
    *nred = *n-i;  /* reduce degree of deflated polynomial    */
    return 0;      /* and return no error                     */
  }
}

/***** quadratic() calculates the roots of a quadratic polynomial *****/
void
NumericLib::quadratic(mrs_complex *pred,mrs_complex *root)
/*mrs_complex *pred,  coefficient vector of the deflated polynomial   */
/*            *root;  determined roots                                */

{
  mrs_complex discr,       /* discriminate                         */
              Z1,Z2,				 /* numerators of the quadratic formula  */
              N;					 /* denominator of the quadratic formula */

  /* discr = p1^2-4*p2*p0                 */
  discr   = Csub(Cmul(pred[1],pred[1]),
                 RCmul((mrs_real)4.,Cmul(pred[2],pred[0])));
  /* Z1 = -p1+sqrt(discr)                 */
  Z1      = Cadd(RCmul((mrs_real)-1.,pred[1]),Csqrt(discr));
  /* Z2 = -p1-sqrt(discr)                 */
  Z2      = Csub(RCmul((mrs_real)-1.,pred[1]),Csqrt(discr));
  /* N  = 2*p2                            */
  N       = RCmul((mrs_real)2.,pred[2]);
  root[0] = Cdiv(Z1,N); /* first root  = Z1/N                   */
  root[1] = Cdiv(Z2,N); /* second root = Z2/N                   */
}

/***** lin_or_quad() calculates roots of lin. or quadratic equation *****/
unsigned char
NumericLib::lin_or_quad(mrs_complex *pred,mrs_natural nred,mrs_complex *root)
/*mrs_complex *pred,  coefficient vector of the deflated polynomial   */
/*         *root;  determined roots                                */
/*mrs_natural      nred;   highest exponent of the deflated polynomial     */
{
  if (nred==1) {     /* root = -p0/p1                           */
    root[0] = Cdiv(RCmul((mrs_real)-1.,pred[0]),pred[1]);
    return 0;      /* and return no error                     */
  } else if (nred==2) { /* quadratic polynomial                 */
    quadratic(pred,root);
    return 0;         /* return no error                      */
  }

  return 1; /* nred>2 => no roots were calculated               */
}

/***** monic() computes monic polynomial for original polynomial *****/
void
NumericLib::monic(mrs_complex *p,mrs_natural *n)
/*mrs_complex *p;     coefficient vector of the original polynomial   */
/*mrs_natural *n;     the highest exponent of the original polynomial */
{
  mrs_real factor;     /* stores absolute value of the coefficient  */
  /* with highest exponent                     */
  mrs_natural    i;    /* counter variable                          */

  factor=1./Cabs(p[*n]);     /* factor = |1/pn|                 */
  if ( factor!=1.)           /* get monic pol., when |pn| != 1  */
    for (i=0; i<=*n; ++i)
      p[i]=RCmul(factor,p[i]);
}

//TOOLS
/******************************************************************/
/*                                                                */
/* file:          tools.c                                         */
/*                                                                */
/* version:       1.2                                             */
/*                                                                */
/* author:        B. Frenzel                                      */
/*                                                                */
/* date:          Jan 7 1993                                      */
/*                                                                */
/* function:      description:                                    */
/*                                                                */
/* hornc()        hornc() deflates the polynomial with coeff.     */
/*                stored in pred[0],...,pred[n] by Horner's       */
/*                method (one root)                               */
/*                                                                */
/* horncd()       horncd() deflates the polynomial with coeff.    */
/*                stored in pred[0],...,pred[n] by Horner's       */
/*                method (two roots)                              */
/*                                                                */
/* poldef()       decides whether to call hornc() or horncd()     */
/*                                                                */
/* fdvalue()      fdvalue() computes f=P(x0) by Horner's method;  */
/*                if flag=TRUE, it additionally computes the      */
/*                derivative df=P'(x0)                            */
/*                                                                */
/* Copyright:                                                     */
/* Lehrstuhl fuer Nachrichtentechnik Erlangen                     */
/* Cauerstr. 7, 8520 Erlangen, FRG, 1993                          */
/* e-mail: int@nt.e-technik.uni-erlangen.de                       */
/*                                                                */
/******************************************************************/

/***** Horner method to deflate one root *****/
void
NumericLib::hornc(mrs_complex *pred,mrs_natural nred,mrs_complex x0,unsigned char flag)
/*mrs_complex   *pred,  coefficient vector of the polynomial           */
/*              x0;     root to be deflated                            */
/*mrs_natural   nred;   the highest exponent of (deflated) polynomial  */
/*unsigned char flag;   indicates how to reduce polynomial             */
{
  mrs_natural      i;     /* counter                        */
  mrs_complex help1;      /* help variable                  */

  if ((flag&1)==0)        /* real coefficients              */
    for(i=nred-1; i>0; i--)
      pred[i] = mrs_complex(pred[i].real() + (x0.real()*pred[i+1].real()), pred[i].imag()); //pred[i].real() += (x0.real()*pred[i+1].real());
  else                    /* complex coefficients           */
    for (i=nred-1; i>0; i--) {
      CMUL(help1,pred[i+1],x0);
      CADD(pred[i],help1,pred[i]);
    }
}

/***** Horner method to deflate two roots *****/
void
NumericLib::horncd(mrs_complex *pred,mrs_natural nred,mrs_real a,mrs_real b)
/*mrs_complex *pred;     coefficient vector of the polynomial           */
/*mrs_real    a,         coefficients of the quadratic polynomial       */
/*            b;         x^2+ax+b                                       */
/*mrs_natural nred;      the highest exponent of (deflated) polynomial  */
{
  mrs_natural i;        /* counter */

  //lmartins: pred[nred-1].real() += pred[nred].real()*a;
  pred[nred-1] = mrs_complex(pred[nred-1].real() + pred[nred].real()*a, pred[nred-1].imag());

  for (i=nred-2; i>1; i--)
    //lmartins pred[i].real() += (a*pred[i+1].real()+b*pred[i+2].real());
    pred[i] = mrs_complex(pred[i].real() + (a*pred[i+1].real()+b*pred[i+2].real()), pred[i].imag());
}

/***** main routine to deflate polynomial *****/
mrs_natural
NumericLib::poldef(mrs_complex *pred,mrs_natural nred,mrs_complex *root,unsigned char flag)
/*mrs_complex *pred,     coefficient vector of the polynomial           */
/*            *root;     vector of determined roots                     */
/*mrs_natural nred;     the highest exponent of (deflated) polynomial   */
/*unsigned char flag;  indicates how to reduce polynomial               */
{
  mrs_real   a,   /* coefficients of the quadratic polynomial       */
             b;          /* x^2+ax+b                                       */
  mrs_complex x0; /* root to be deflated                            */


  x0 = root[nred-1];
  if (x0.imag()!=0.)          /* x0 is complex                      */
    flag |=2;

  if (flag==2) {              /* real coefficients and complex root */
    a = 2*x0.real();        /* => deflate x0 and Conjg(x0)        */
    b = -(x0.real()*x0.real()+x0.imag()*x0.imag());
    root[nred-2]=Conjg(x0); /* store second root = Conjg(x0)   */
    horncd(pred,nred,a,b);
    return 2;            /* two roots deflated                 */
  } else {
    hornc(pred,nred,x0,flag); /* deflate only one root         */
    return 1;            /* one root deflated                  */
  }
}

/***** fdvalue computes P(x0) and optional P'(x0) *****/
void
NumericLib::fdvalue(mrs_complex *p,mrs_natural n,mrs_complex *f,mrs_complex *df,mrs_complex x0,
                    unsigned char flag)
/*mrs_complex   *p,     coefficient vector of the polynomial P(x)   */
/*              *f,     the result f=P(x0)                          */
/*              *df,    the result df=P'(x0), if flag=TRUE          */
/*              x0;     polynomial will be computed at x0           */
/*mrs_natural   n;      the highest exponent of p                   */
/*unsigned char flag;   flag==TRUE => compute P'(x0)                */
{
  mrs_natural i;     /* counter                                     */
  mrs_complex help1; /* help variable                               */

  *f  = p[n];
  if (flag==NumLib_TRUE) {             /* if flag=TRUE, compute P(x0)  */
    COMPLEXM(*df,0.,0.);      /* and P'(x0)                   */
    for (i=n-1; i>=0; i--) {
      CMUL(help1,*df,x0);   /* *df = *f   + *df * x0        */
      CADD(*df,help1,*f);
      CMUL(help1,*f,x0);    /* *f  = p[i] + *f * x0         */
      CADD(*f,help1,p[i]);
    }
  } else                        /* otherwise: compute only P(x0)*/
    for (i=n-1; i>=0; i--) {
      CMUL(help1,*f,x0);    /* *f = p[i] + *f * x0          */
      CADD(*f,help1,p[i]);
    }
}


//**********************************************************************************************************


// Used to force A and B to be stored prior to
// doing the addition of A and B, for use in
// situations where optimizers might hold one
// of these in a register
mrs_real NumericLib::add(mrs_real *a, mrs_real *b)
{
  mrs_real ret;
  ret = *a + *b;
  return ret;
}

mrs_real NumericLib::pow_di(mrs_real *ap, mrs_natural *bp)
{
  mrs_real pow, x;
  mrs_natural n;
  unsigned long u;

  pow = 1;
  x = *ap;
  n = *bp;

  if(n != 0)
  {
    if(n < 0)
    {
      n = -n;
      x = 1/x;
    }
    for(u = n; ; )
    {
      if(u & 01)
        pow *= x;
      if(u >>= 1)
        x *= x;
      else
        break;
    }
  }
  return(pow);
}

// Machine parameters
// cmach :
//    'B' | 'b' --> base
//    'M' | 'm' --> digits in the mantissa
//    'R' | 'r' --> approximation method : 1=rounding 0=chopping
//    'E' | 'e' --> eps
mrs_real NumericLib::machp(const char *cmach)
{
  mrs_real zero, one, two, half, sixth, third, a, b, c, f, d__1, d__2, d__3, d__4, d__5, qtr, eps;
  mrs_real base;
  mrs_natural lt, rnd, i__1;
  lt = 0;
  rnd = 0;
  eps = 0.0;

  one = 1.;
  a = 1.;
  c = 1.;


  while( c == one ) {
    a *= 2;
    c = add(&a, &one);
    d__1 = -a;
    c = add(&c, &d__1);
  }

  b = 1.;
  c = add( &a, &b );

  while( c == a )
  {
    b *= 2;
    c = add( &a, &b );
  }

  qtr = one / 4;
  d__1 = -a;
  c = add(&c, &d__1);
  base = (mrs_natural)(c+qtr);


  if( *cmach == 'M' || *cmach == 'm' || *cmach == 'E' || *cmach == 'e' )
  {
    lt = 0;
    a = 1.;
    c = 1.;
    printf("%g %g %g %g\n", c, one, a, d__1);
    while( c == one ) {
      ++lt;
      a *= base;
      c = add( &a, &one );
      d__1 = -a;
      c = add( &c, &d__1 );
    }
  }

  if( *cmach == 'R' || *cmach == 'r' || *cmach == 'E' || *cmach == 'e' )
  {
    b = (mrs_real) base;
    d__1 = b / 2;
    d__2 = -b / 100;
    f = add( &d__1, &d__2 );
    c = add( &f, &a );
    if( c == a ) {
      rnd = 1; // true
    } else {
      rnd = 0; // false
    }
    d__1 = b / 2;
    d__2 = b / 100;
    f = add( &d__1 , &d__2 );
    c = add( &f, &a );
    if( rnd && c == a ) {
      rnd = 0; // false
    }
  }

  if( *cmach == 'E' || *cmach == 'e' )
  {
    zero = 0.;
    two = 2.;

    i__1 = -lt;
    a = pow_di( &base, &i__1);
    eps = a;

    b = two / 3;
    half = one / 2;
    d__1 = -half;
    sixth = add( &b, &d__1 );
    third = add( &sixth, &sixth );
    d__1 = -half;
    b = add( &third, &d__1 );
    b = add( &b, &sixth );
    b = fabs(b);
    if( b < eps )
      b = eps;

    eps = 1.;

    while( eps > b && b > zero )
    {
      eps = b;
      d__1 = half * eps;
      /* Computing 5th power */
      d__3 = two, d__4 = d__3, d__3 *= d__3;
      /* Computing 2nd power */
      d__5 = eps;
      d__2 = d__4 * (d__3 * d__3) * (d__5 * d__5);
      c = add(&d__1, &d__2);
      d__1 = -c;
      c = add(&half, &d__1);
      b = add(&half, &c);
      d__1 = -b;
      c = add(&half, &d__1);
      b = add(&half, &c);
    }

    if( a < eps )
      eps = a;

    if( rnd == 1 ) {
      i__1 = 1 - lt;
      eps = pow_di(&base, &i__1) / 2;
    } else {
      i__1 = 1 - lt;
      eps = pow_di( &base, &i__1 );
    }

  }

  switch(*cmach) {
  case 'B' :
  case 'b' : return base; break;

  case 'M' :
  case 'm' : return lt; break;

  case 'R' :
  case 'r' : return rnd; break;

  case 'E' :
  case 'e' : return eps; break;

  default  : return -1;
  }
}


/*  Reduce a real, symmetric matrix to a symmetric, tridiag. matrix. */
void
NumericLib::tred2(realvec &a, mrs_natural m, realvec &d, realvec &e)
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
        scale += fabs(a(k*m+i));
      if (scale == 0.0)
        e(i) = a(l*m+i);
      else
      {
        for (k = 0; k <= l; k++)
        {
          a(k*m+i) /= scale;
          h += a(k*m+i) * a(k*m+i);
        }
        f = a(l*m+i);
        g = f>0 ? -sqrt(h) : sqrt(h);
        e(i) = scale * g;

        h -= f * g;
        a(l*m+i) = f - g ;
        f = 0.0;
        for (j = 0; j <= l; j++)
        {
          a(i*m+j) = a(j*m+i)/h;
          g = 0.0;
          for (k = 0; k <= j; k++)
            g += a(k*m+j) * a(k*m+i) ;
          for (k = j+1; k <= l; k++)
            g += a(j*m+k) * a(k*m+i);
          e(j) = g / h;
          f += e(j) * a(j*m+i);
        }
        hh = f / (h + h);
        for (j = 0; j <= l; j++)
        {
          f = a(j*m+i);
          e(j) = g = e(j) - hh * f;
          for (k = 0; k <= j; k++)
            a(k*m+j) -= (f * e(k) + g * a(k*m+i));
        }
      }
    }
    else
      e(i) = a(l*m+i);
    d(i) = h;
  }
  d(0) = 0.0;
  e(0) = 0.0;
  for (i = 0; i < m; ++i)
  {
    l = i - 1;
    if (d(i))
    {
      for (j = 0; j <= l; j++)
      {
        g = 0.0;
        for (k = 0; k <= l; k++)
          g += a(k*m+i) * a(j*m+k);
        for (k = 0; k <= l; k++)
          a(j*m+k) -= g * a(i*m+k);
      }
    }
    d(i) = a(i*m+i);
    a(i*m+i) = 1.0 ;

    for (j = 0; j <= l; j++)
      a(i*m+j) = a(j*m+i) = 0.0;
  }
}

/*  Tridiagonal QL algorithm -- Implicit  */
void
NumericLib::tqli(realvec &d, realvec &e, mrs_natural m, realvec &z)
/*
 Source code adapted from F. Murtagh, Munich, 6 June 1989
 http://astro.u-strasbg.fr/~fmurtagh/mda-sw/pca.c
 */
{
  mrs_natural n, l, iter, i, j, k;
  mrs_real s, r, p, g, f, dd, c, b, tmp;

  for (i = 1; i < m; ++i)
    e(i-1) = e(i);
  e(m-1) = 0.0;
  for (l = 0; l < m; l++)
  {
    iter = 0;
    do
    {
      for (n = l; n < m-1; n++)
      {
        dd = fabs(d(n)) + fabs(d(n+1));
        if (fabs(e(n)) + dd == dd) break;
      }
      if (n != l)
      {
        //MRSASSERT( iter++ != 30 ); // No convergence
        if( iter++ == 30 )
        {
          cerr << "tqli did not converge!" << endl;
          MRSERR("NumericLib.cpp: tqli did not converge!");
          MRSASSERT(0);
          return;
          //exit(EXIT_SUCCESS);
        }

        g = (d(l+1) - d(l)) / (2.0 * e(l));
        r = sqrt((g * g) + 1.0);
        g = d(n) - d(l) + e(l) / (g + SIGN(r, g));
        s = c = 1.0;
        p = 0.0;
        for (i = n-1; i >= l; i--)
        {
          f = s * e(i);
          b = c * e(i);
          if (fabs(f) >= fabs(g))
          {
            c = g / f;
            r = sqrt((c * c) + 1.0);
            e(i+1) = f * r;
            c *= (s = 1.0/r);
          }
          else
          {
            s = f / g;
            r = sqrt((s * s) + 1.0);
            e(i+1) = g * r;
            s *= (c = 1.0/r);
          }
          g = d(i+1) - p;
          r = (d(i) - g) * s + 2.0 * c * b;
          p = s * r;
          d(i+1) = g + p;
          g = c * r - b;
          for (k = 0; k < m; k++)
          {
            f = z((i+1)*m+k);
            z((i+1)*m+k) = s * z(i*m+k) + c * f;
            z(i*m+k) = c * z(i*m+k) - s * f;
          }
        }
        d(l) = d(l) - p;
        e(l) = g;
        e(n) = 0.0;
      }
    }  while (n != l);

  }

  for (i = 0; i < m-1; ++i) {
    k = i;
    tmp = d(i);
    for (j = i+1; j < m; j++) {
      if (d(j) < tmp) {
        k = j;
        tmp = d(j);
      }
    }
    if (k != i) {
      d(k) = d(i);
      d(i) = tmp;
      for (j = 0; j < m; j++) {
        tmp = z(i*m+j);
        z(i*m+j) = z(k*m+j);
        z(k*m+j) = tmp;
      }
    }
  }

}

// A is m x n
// U is m x m
// V is n x n
// s is max(m,n)+1 x 1
void NumericLib::svd(mrs_natural m, mrs_natural n, realvec &A, realvec &U, realvec &V, realvec &s) {

  mrs_natural nu = min(m,n);
  realvec e(n);
  realvec work(m);
  mrs_natural wantu = 1;  					/* boolean */
  mrs_natural wantv = 1;  					/* boolean */
  mrs_natural i=0, j=0, k=0;

  // Reduce A to bidiagonal form, storing the diagonal elements
  // in s and the super-diagonal elements in e.
  mrs_natural nct = min(m-1,n);
  mrs_natural nrt = max((mrs_natural) 0,min(n-2,m));
  for (k = 0; k < max(nct,nrt); k++) {
    if (k < nct) {

      // Compute the transformation for the k-th column and
      // place the k-th diagonal in s(k).
      // Compute 2-norm of k-th column without under/overflow.
      s(k) = 0;
      for (i = k; i < m; ++i) {
#ifdef MARSYAS_WIN32
        s(k) = _hypot(s(k),A(k*m+i));
#else
        s(k) = hypot(s(k),A(k*m+i));
#endif
      }
      if (s(k) != 0.0) {
        if (A(k*m+k) < 0.0) {
          s(k) = -s(k);
        }
        for (i = k; i < m; ++i) {
          A(k*m+i) /= s(k);
        }
        A(k*m+k) += 1.0;
      }
      s(k) = -s(k);
    }
    for (j = k+1; j < n; j++) {
      if ((k < nct) && (s(k) != 0.0))  {

        // Apply the transformation.

        mrs_real t = 0;
        for (i = k; i < m; ++i) {
          t += A(k*m+i)*A(j*m+i);
        }
        t = -t/A(k*m+k);
        for (i = k; i < m; ++i) {
          A(j*m+i) += t*A(k*m+i);
        }
      }

      // Place the k-th row of A into e for the
      // subsequent calculation of the row transformation.

      e(j) = A(j*m+k);
    }
    if (wantu & (k < nct)) {

      // Place the transformation in U for subsequent back
      // multiplication.

      for (i = k; i < m; ++i) {
        U(k*m+i) = A(k*m+i);
      }
    }
    if (k < nrt) {

      // Compute the k-th row transformation and place the
      // k-th super-diagonal in e(k).
      // Compute 2-norm without under/overflow.
      e(k) = 0;
      for (i = k+1; i < n; ++i) {
#ifdef MARSYAS_WIN32
        e(k) = _hypot(e(k),e(i));
#else
        e(k) = hypot(e(k),e(i));
#endif

      }
      if (e(k) != 0.0) {
        if (e(k+1) < 0.0) {
          e(k) = -e(k);
        }
        for (i = k+1; i < n; ++i) {
          e(i) /= e(k);
        }
        e(k+1) += 1.0;
      }
      e(k) = -e(k);
      if ((k+1 < m) & (e(k) != 0.0)) {

        // Apply the transformation.

        for (i = k+1; i < m; ++i) {
          work(i) = 0.0;
        }
        for (j = k+1; j < n; j++) {
          for (i = k+1; i < m; ++i) {
            work(i) += e(j)*A(j*m+i);
          }
        }
        for (j = k+1; j < n; j++) {
          mrs_real t = -e(j)/e(k+1);
          for (i = k+1; i < m; ++i) {
            A(j*m+i) += t*work(i);
          }
        }
      }
      if (wantv) {

        // Place the transformation in V for subsequent
        // back multiplication.

        for (i = k+1; i < n; ++i) {
          V(k*n+i) = e(i);
        }
      }
    }
  }

  // Set up the final bidiagonal matrix or order p.

  mrs_natural p = min(n,m+1);
  if (nct < n) {
    s(nct) = A(nct*m+nct);
  }
  if (m < p) {
    s(p-1) = 0.0;
  }
  if (nrt+1 < p) {
    e(nrt) = A((p-1)*m+nrt);
  }
  e(p-1) = 0.0;

  // If required, generate U.

  if (wantu) {
    for (j = nct; j < nu; j++) {
      for (i = 0; i < m; ++i) {
        U(j*m+i) = 0.0;
      }
      U(j*m+j) = 1.0;
    }
    for (k = nct-1; k >= 0; k--) {
      if (s(k) != 0.0) {
        for (j = k+1; j < nu; j++) {
          mrs_real t = 0;
          for (i = k; i < m; ++i) {
            t += U(k*m+i)*U(j*m+i);
          }
          t = -t/U(k*m+k);
          for (i = k; i < m; ++i) {
            U(j*m+i) += t*U(k*m+i);
          }
        }
        for (i = k; i < m; ++i ) {
          U(k*m+i) = -U(k*m+i);
        }
        U(k*m+k) = 1.0 + U(k*m+k);
        for (i = 0; i < k-1; ++i) {
          U(k*m+i) = 0.0;
        }
      } else {
        for (i = 0; i < m; ++i) {
          U(k*m+i) = 0.0;
        }
        U(k*m+k) = 1.0;
      }
    }
  }

  // If required, generate V.

  if (wantv) {
    for (k = n-1; k >= 0; k--) {
      if ((k < nrt) & (e(k) != 0.0)) {
        for (j = k+1; j < nu; j++) {
          mrs_real t = 0;
          for (i = k+1; i < n; ++i) {
            t += V(k*n+i)*V(j*n+i);
          }
          t = -t/V(k*n+(k+1));
          for (i = k+1; i < n; ++i) {
            V(j*n+i) += t*V(k*n+i);
          }
        }
      }
      for (i = 0; i < n; ++i) {
        V(k*n+i) = 0.0;
      }
      V(k*n+k) = 1.0;
    }
  }

  // Main iteration loop for the singular values.

  mrs_natural pp = p-1;
  mrs_natural iter = 0;
  // mrs_real eps = machp("E"); //pow(2.0,-52.0);
  mrs_real eps = std::numeric_limits<double>::epsilon();



  while (p > 0) {
    mrs_natural k=0;
    mrs_natural kase=0;

    // Here is where a test for too many iterations would go.

    // This section of the program inspects for
    // negligible elements in the s and e arrays.  On
    // completion the variables kase and k are set as follows.

    // kase = 1     if s(p) and e(k-1) are negligible and k<p
    // kase = 2     if s(k) is negligible and k<p
    // kase = 3     if e(k-1) is negligible, k<p, and
    //              s(k), ..., s(p) are not negligible (qr step).
    // kase = 4     if e(p-1) is negligible (convergence).

    for (k = p-2; k >= -1; k--) {
      if (k == -1) {
        break;
      }
      if (fabs(e(k)) <= eps*(fabs(s(k)) + fabs(s(k+1)))) {
        e(k) = 0.0;
        break;
      }
    }
    if (k == p-2) {
      kase = 4;
    } else {
      mrs_natural ks;
      for (ks = p-1; ks >= k; ks--) {
        if (ks == k) {
          break;
        }
        mrs_real t = (ks != p ? fabs(e(ks)) : 0.) +
                     (ks != k+1 ? fabs(e(ks-1)) : 0.);
        if (fabs(s(ks)) <= eps*t)  {
          s(ks) = 0.0;
          break;
        }
      }
      if (ks == k) {
        kase = 3;
      } else if (ks == p-1) {
        kase = 1;
      } else {
        kase = 2;
        k = ks;
      }
    }
    k++;

    // Perform the task indicated by kase.

    switch (kase) {

      // Deflate negligible s(p).

    case 1: {
      mrs_real f = e(p-2);
      e(p-2) = 0.0;
      for (j = p-2; j >= k; j--) {
#ifdef MARSYAS_WIN32
        mrs_real t = _hypot(s(j),f);
#else
        mrs_real t = hypot(s(j),f);
#endif

        mrs_real cs = s(j)/t;
        mrs_real sn = f/t;
        s(j) = t;
        if (j != k) {
          f = -sn*e(j-1);
          e(j-1) = cs*e(j-1);
        }
        if (wantv) {
          for (i = 0; i < n; ++i) {
            t = cs*V(j*n+i) + sn*V((p-1)*n+i);
            V((p-1)*n+i) = -sn*V(j*n+i) + cs*V((p-1)*n+i);
            V(j*n+i) = t;
          }
        }
      }
    }
    break;

    // Split at negligible s(k).

    case 2: {
      mrs_real f = e(k-1);
      e(k-1) = 0.0;
      for (j = k; j < p; j++) {
#ifdef MARSYAS_WIN32
        mrs_real t = _hypot(s(j),f);
#else
        mrs_real t = hypot(s(j),f);
#endif
        mrs_real cs = s(j)/t;
        mrs_real sn = f/t;
        s(j) = t;
        f = -sn*e(j);
        e(j) = cs*e(j);
        if (wantu) {
          for (i = 0; i < m; ++i) {
            t = cs*U(j*m+i) + sn*U((k-1)*m+i);
            U((k-1)*m+i) = -sn*U(j*m+i) + cs*U((k-1)*m+i);
            U(j*m+i) = t;
          }
        }
      }
    }
    break;

    // Perform one qr step.

    case 3: {

      // Calculate the shift.

      mrs_real scale = max(max(max(max(
                                     fabs(s(p-1)),fabs(s(p-2))),fabs(e(p-2))),
                               fabs(s(k))),fabs(e(k)));
      mrs_real sp = s(p-1)/scale;
      mrs_real spm1 = s(p-2)/scale;
      mrs_real epm1 = e(p-2)/scale;
      mrs_real sk = s(k)/scale;
      mrs_real ek = e(k)/scale;
      mrs_real b = ((spm1 + sp)*(spm1 - sp) + epm1*epm1)/2.0;
      mrs_real c = (sp*epm1)*(sp*epm1);
      mrs_real shift = 0.0;
      if ((b != 0.0) || (c != 0.0)) {
        shift = sqrt(b*b + c);
        if (b < 0.0) {
          shift = -shift;
        }
        shift = c/(b + shift);
      }
      mrs_real f = (sk + sp)*(sk - sp) + shift;
      mrs_real g = sk*ek;

      // Chase zeros.

      for (j = k; j < p-1; j++) {
#ifdef MARSYAS_WIN32
        mrs_real t = _hypot(f,g);
#else
        mrs_real t = hypot(f,g);
#endif

        mrs_real cs = f/t;
        mrs_real sn = g/t;
        if (j != k) {
          e(j-1) = t;
        }
        f = cs*s(j) + sn*e(j);
        e(j) = cs*e(j) - sn*s(j);
        g = sn*s(j+1);
        s(j+1) = cs*s(j+1);
        if (wantv) {
          for (i = 0; i < n; ++i) {
            t = cs*V(j*n+i) + sn*V((j+1)*n+i);
            V((j+1)*n+i) = -sn*V(j*n+i) + cs*V((j+1)*n+i);
            V(j*n+i) = t;
          }
        }
#ifdef MARSYAS_WIN32
        t = _hypot(f,g);
#else
        t = hypot(f,g);
#endif
        cs = f/t;
        sn = g/t;
        s(j) = t;
        f = cs*e(j) + sn*s(j+1);
        s(j+1) = -sn*e(j) + cs*s(j+1);
        g = sn*e(j+1);
        e(j+1) = cs*e(j+1);
        if (wantu && (j < m-1)) {
          for (i = 0; i < m; ++i) {
            t = cs*U(j*m+i) + sn*U((j+1)*m+i);
            U((j+1)*m+i) = -sn*U(j*m+i) + cs*U((j+1)*m+i);
            U(j*m+i) = t;
          }
        }
      }
      e(p-2) = f;
      iter = iter + 1;
    }
    break;

    // Convergence.

    case 4: {

      // Make the singular values positive.

      if (s(k) <= 0.0) {
        s(k) = (s(k) < 0.0 ? -s(k) : 0.0);
        if (wantv) {
          for (i = 0; i <= pp; ++i) {
            V(k*n+i) = -V(k*n+i);
          }
        }
      }

      // Order the singular values.

      while (k < pp) {
        if (s(k) >= s(k+1)) {
          break;
        }
        mrs_real t = s(k);
        s(k) = s(k+1);
        s(k+1) = t;
        if (wantv && (k < n-1)) {
          for (i = 0; i < n; ++i) {
            t = V((k+1)*n+i); V((k+1)*n+i) = V(k*n+i); V(k*n+i) = t;
          }
        }
        if (wantu && (k < m-1)) {
          for (i = 0; i < m; ++i) {
            t = U((k+1)*m+i); U((k+1)*m+i) = U(k*m+i); U(k*m+i) = t;
          }
        }
        k++;
      }
      iter = 0;
      p--;
    }
    break;
    }
  }
}

/////////////////////////////////////////////////////////////
//  METRICS
/////////////////////////////////////////////////////////////
mrs_real
NumericLib::euclideanDistance(const realvec& Vi, const realvec& Vj, const realvec& covMatrix)
{
  mrs_real res1;
  mrs_real res = 0;

  //if no variances are passed as argument (i.e. empty covMatrix),
  //just do a plain euclidean computation
  if(covMatrix.getSize() == 0)
  {
    for (mrs_natural r=0 ; r < Vi.getSize()  ; ++r)
    {
      res1 = Vi(r)-Vj(r);
      res1 *= res1; //square
      res += res1; //summation
    }
    res = sqrt(res);
  }
  else if (covMatrix.sum () > 0)
  {
    // do a standardized L2 euclidean distance
    //(i.e. just use the diagonal elements of covMatrix)
    for (mrs_natural r=0 ; r < Vi.getSize()  ; ++r)
    {
      res1 = Vi(r)-Vj(r);
      res1 *= res1; //square
      res1 /= covMatrix(r,r); //divide by var of each feature
      res += res1; //summation
    }
    res = sqrt(res);
  }
  return res;
}

mrs_real
NumericLib::mahalanobisDistance(const realvec& Vi, const realvec& Vj, const realvec& covMatrix)
{
  // These remove warnings about unused parameters.
  (void) Vi;
  (void) Vj;
  (void) covMatrix;
  //realvec invCovMatrix;
  //covMatrix.invert(invCovMatrix);

  //NOT IMPLEMENTED YET!!!! [!]
  return MINREAL;
}

mrs_real
NumericLib::cosineDistance(const realvec& Vi, const realvec& Vj, const realvec& dummy)
{
  (void) dummy;
  //as defined in:
  //http://www.mathworks.com/access/helpdesk/help/toolbox/stats/index.html?/access/helpdesk/help/toolbox/stats/pdist.html
  mrs_real res1 = 0;
  mrs_real res2 = 0;
  mrs_real res3 = 0;
  mrs_real res = 0;
  for (mrs_natural r=0 ; r < Vi.getSize()  ; ++r)
  {
    res1 += Vi(r)*Vj(r);
    res2 += Vi(r)*Vi(r);
    res3 += Vj(r)*Vj(r);
  }
  if (res2!=0 && res3!=0)
  {
    res = res1/sqrt(res2 * res3);
    if(res > 1.0) //cosine similarity should never be bigger than 1.0!!
    {
      if (res-1.0 > 1e-6) {
        MRSWARN("NumericLib::cosineDistance() - cosine similarity value is > 1.0 by " << res-1.0 << " -> setting value to 1.0!");
      }
      res = 1.0;
    }
    return (1.0 - res); //return DISTANCE (and not the cosine similarity)
  }
  else
  {
    MRSERR("NumericLib::cosineDistance() - at least one of the input points have small relative magnitudes, making it effectively zero... returning invalid value of -1.0!");
    return -1.0;
  }
}

mrs_real
NumericLib::cityblockDistance(const realvec& Vi, const realvec& Vj, const realvec& dummy)
{
  (void) Vi; (void) Vj; (void) dummy; // Remove warnings about unused parameters
  return MINREAL; //NOT IMPLEMENTED YET!!!! [!]
}

mrs_real
NumericLib::correlationDistance(const realvec& Vi, const realvec& Vj, const realvec& dummy)
{
  (void) Vi; (void) Vj; (void) dummy; // Remove warnings about unused parameters
  return MINREAL; //NOT IMPLEMENTED YET!!!! [!]
}

mrs_real
NumericLib::divergenceShape(const realvec& Ci, const realvec& Cj, const realvec& dummy)
{
  (void) dummy;
  ///matrices should be square and equal sized
  if(Ci.getCols() != Cj.getCols() && Ci.getRows() != Cj.getRows() &&
      Ci.getCols()!= Ci.getRows())
  {
    MRSERR("realvec::divergenceShape() : input matrices should be square and equal sized. Returning invalid value (-1.0)");
    return -1.0;
  }

  realvec Cii = Ci;
  realvec Cjj = Cj;

  mrs_real res;

  realvec Citemp(Cii.getRows(), Cii.getCols());
  realvec invCi(Cii);

  realvec Cjtemp(Cjj.getRows(),Cjj.getCols());
  realvec invCj(Cjj);

#ifdef _MATLAB_REALVEC_
  MATLAB_PUT(Cii, "Ci");
  MATLAB_PUT(Cjj, "Cj");
  MATLAB_PUT(Citemp, "Citemp");
  MATLAB_PUT(Cjtemp, "Cjtemp");
#endif


  invCi.invert(Citemp);
  invCj.invert(Cjtemp);

#ifdef _MATLAB_REALVEC_
  MATLAB_PUT(Citemp, "Citemp2");
  MATLAB_PUT(Cjtemp, "Cjtemp2");
  MATLAB_PUT(invCi, "invCi");
  MATLAB_PUT(invCj, "invCj");
#endif

  Cjj *= (-1.0);
  Cii += Cjj;

#ifdef _MATLAB_REALVEC_
  MATLAB_PUT(Cii, "Ci_minus_Cj");
#endif

  invCi *= (-1.0);
  invCj += invCi;

#ifdef _MATLAB_REALVEC_
  MATLAB_PUT(invCj, "invCj_minus_invCi");
#endif

  Cii *= invCj;

  res = Cii.trace() / 2.0;

#ifdef _MATLAB_REALVEC_
  MATLAB_PUT(Cii, "divergenceMatrix");
  MATLAB_PUT(res, "divergence");
#endif

  return res;
}

mrs_real
NumericLib::bhattacharyyaShape(const realvec& Ci, const realvec& Cj, const realvec& dummy)
{
  (void) dummy;
  ///matrices should be square and equal sized
  if(Ci.getCols() != Cj.getCols() && Ci.getRows() != Cj.getRows() &&
      Ci.getCols()!= Ci.getRows())
  {
    MRSERR("realvec::bhattacharyyaShape() : input matrices should be square and equal sized. Returning invalid value (-1.0)");
    return -1.0;
  }

  realvec Cii = Ci;
  realvec Cjj = Cj;

  //denominator
  mrs_real sqrtdetCi = sqrt(Cii.det());
  mrs_real sqrtdetCj = sqrt(Cjj.det());
  mrs_real den = sqrtdetCi * sqrtdetCj;
#ifdef _MATLAB_REALVEC_
  MATLAB_PUT(Cii, "Ci");
  MATLAB_PUT(Cjj, "Cj");
  MATLAB_PUT(sqrtdetCi, "sqrtdetCi");
  MATLAB_PUT(sqrtdetCj, "sqrtdetCj");
  MATLAB_PUT(den, "den");
#endif

  //numerator
  Cii += Cjj;
  Cii /= 2.0;
  mrs_real num = Cii.det();

  //bhattacharyyaShape
  return log(num/den);
}

///////////////////////////////////////////////////////////////
// hungarian assignement methods
///////////////////////////////////////////////////////////////
mrs_real
NumericLib::hungarianAssignment(realvec& matrixdist, realvec& matrixAssign)
{
  mrs_real cost;

  mrs_natural rows = matrixdist.getRows();
  mrs_natural cols = matrixdist.getCols();

  if(matrixAssign.getCols() != cols || matrixAssign.getRows() != 1)
  {
    MRSERR("NumericLib::hungarianAssignemnt - wrong size for matrix Assign!");
    return -1.0;
  }

  //copy input data [!]
  mrs_real* distMatrix = new mrs_real[rows * cols];
  for(mrs_natural r=0; r < rows; ++r)
    for(mrs_natural c = 0; c < cols; ++c)
      distMatrix[r*cols + c] = matrixdist(r,c);

  mrs_natural* assignement = new mrs_natural[cols];

  assignmentoptimal(assignement, &cost, distMatrix, rows, cols);

  //copy resulting assignment to matrixAssign [!]
  for(mrs_natural i = 0; i < cols; ++i)
    matrixAssign(i) = assignement[i];

  delete [] distMatrix;
  delete [] assignement;

  return cost;
}

mrs_real
NumericLib::mxGetInf()
{
  return numeric_limits<double>::infinity();
}
bool
NumericLib::mxIsInf(mrs_real s)
{
  return s == numeric_limits<double>::infinity() || s == -numeric_limits<double>::infinity();
}

void
NumericLib::mxFree( void * s )
{
  free ( s );
}

void
NumericLib::mexErrMsgTxt(const char * s)
{
  cout << s << endl;
}

void
NumericLib::assignmentoptimal(mrs_natural *assignment, mrs_real *cost, mrs_real *distMatrixIn, mrs_natural nOfRows, mrs_natural nOfColumns)
{

  mrs_real *distMatrix, *distMatrixTemp, value, minValue; //*columnEnd,
  bool *coveredColumns, *coveredRows, *starMatrix, *newStarMatrix, *primeMatrix;
  mrs_natural nOfElements, minDim, row, col;
#ifdef CHECK_FOR_INF
  bool infiniteValueFound;
  mrs_real maxFiniteValue, infValue;
#endif

  /* initialization */
  *cost = 0;
  for(row=0; row<nOfRows; row++)
    //#ifdef ONE_INDEXING
    //		assignment[row] =  0.0;
    //#else
    assignment[row] = -1;//.0;
  //#endif

  /* generate working copy of distance Matrix */
  /* check if all matrix elements are positive */
  nOfElements   = nOfRows * nOfColumns;
  distMatrix    = (mrs_real *)malloc(nOfElements * sizeof(mrs_real));
  //distMatrix aponta para o primeiro elemento por causa do malloc;
  //quando soma mais nOfElements aponta para o final
  for(row=0; row<nOfElements; row++)
  {
    value = distMatrixIn[row];
    //if(mxIsFinite(value) && (value < 0))
    if((( value > -numeric_limits<double>::infinity() && value < numeric_limits<double>::infinity())) && (value < 0))
      mexErrMsgTxt("All matrix elements have to be non-negative.");
    distMatrix[row] = value;
  }



#ifdef CHECK_FOR_INF
  mrs_real *distMatrixEnd = distMatrix + nOfElements;
  /* check for infinite values */
  maxFiniteValue     = -1;
  infiniteValueFound = false;

  distMatrixTemp = distMatrix;
  while(distMatrixTemp < distMatrixEnd)
  {
    value = *distMatrixTemp++;
    //if(mxIsFinite(value))
    if ( value > -numeric_limits<double>::infinity() && value < numeric_limits<double>::infinity())
    {
      if(value > maxFiniteValue)
        maxFiniteValue = value;
    }
    else
      infiniteValueFound = true;
  }
  if(infiniteValueFound)
  {
    if(maxFiniteValue == -1) /* all elements are infinite */
      return;

    /* set all infinite elements to big finite value */
    if(maxFiniteValue > 0)
      infValue = 10 * maxFiniteValue * nOfElements;
    else
      infValue = 10;
    distMatrixTemp = distMatrix;
    while(distMatrixTemp < distMatrixEnd)
      if(mxIsInf(*distMatrixTemp++))
        *(distMatrixTemp-1) = infValue;
  }
#endif

  /* memory allocation */
  /*
  coveredColumns = (bool *)mxCalloc(nOfColumns,  sizeof(bool));
  coveredRows    = (bool *)mxCalloc(nOfRows,     sizeof(bool));
  starMatrix     = (bool *)mxCalloc(nOfElements, sizeof(bool));
  primeMatrix    = (bool *)mxCalloc(nOfElements, sizeof(bool));
  newStarMatrix  = (bool *)mxCalloc(nOfElements, sizeof(bool)); *//* used in step4 */

  coveredColumns = (bool *)calloc(nOfColumns,  sizeof(bool));
  coveredRows    = (bool *)calloc(nOfRows,     sizeof(bool));
  starMatrix     = (bool *)calloc(nOfElements, sizeof(bool));
  primeMatrix    = (bool *)calloc(nOfElements, sizeof(bool));
  newStarMatrix  = (bool *)calloc(nOfElements, sizeof(bool));

  /* preliminary steps */
  if(nOfRows <= nOfColumns)
  {
    minDim = nOfRows;

    for(row=0; row<nOfRows; row++)
    {
      /* find the smallest element in the row */
      //distMatrixTemp = distMatrix + row;
      //minValue = *distMatrixTemp;
      //distMatrixTemp += nOfRows;
      distMatrixTemp = distMatrix + row*nOfColumns;
      minValue = *distMatrixTemp;
      //while(distMatrixTemp < distMatrixEnd)
      //{
      //	value = *distMatrixTemp;
      //	if(value < minValue)
      //		minValue = value;
      //	distMatrixTemp += nOfRows;
      //}
      for (mrs_natural i=1; i< nOfColumns; ++i) {
        value = *(distMatrixTemp++);
        if (value < minValue)
          minValue = value;
      }

      /* subtract the smallest element from each element of the row */
      //distMatrixTemp = distMatrix + row;
      //while(distMatrixTemp < distMatrixEnd)
      //{
      //	*distMatrixTemp -= minValue;
      //	distMatrixTemp += nOfRows;
      //}
      distMatrixTemp = distMatrix + row*nOfColumns;
      for (mrs_natural i=0; i< nOfColumns; ++i)
        *(distMatrixTemp++) -= minValue;

    }

    /* Steps 1 and 2a */
    //for(row=0; row<nOfRows; row++)
    //	for(col=0; col<nOfColumns; col++)
    //		if(distMatrix[row + nOfRows*col] == 0)
    //			if(!coveredColumns[col])
    //			{
    //				starMatrix[row + nOfRows*col] = true;
    //				coveredColumns[col]           = true;
    //				break;
    //			}
    for(row=0; row<nOfRows; row++)
      for(col=0; col<nOfColumns; col++)
        if(distMatrix[row*nOfColumns + col] == 0)
          if(!coveredColumns[col])
          {
            starMatrix[row*nOfColumns + col] = true;
            coveredColumns[col]           = true;
            break;
          }

  }
  else /* if(nOfRows > nOfColumns) */
  {
    minDim = nOfColumns;

    for(col=0; col<nOfColumns; col++)
    {
      /* find the smallest element in the column */
      //distMatrixTemp = distMatrix     + nOfRows*col;
      //columnEnd      = distMatrixTemp + nOfRows;
      distMatrixTemp = distMatrix + col;

      //minValue = *distMatrixTemp++;
      minValue = *distMatrixTemp;
      //while(distMatrixTemp < columnEnd)
      //{
      //	value = *distMatrixTemp++;
      //	if(value < minValue)
      //		minValue = value;
      //}
      for (mrs_natural i=1; i<nOfRows; ++i) {
        value = *(distMatrixTemp + nOfColumns*i);
        if (value < minValue)
          minValue = value;
      }

      /* subtract the smallest element from each element of the column */
      //distMatrixTemp = distMatrix + nOfRows*col;
      //while(distMatrixTemp < columnEnd)
      //	*distMatrixTemp++ -= minValue;
      distMatrixTemp = distMatrix + col;
      for (mrs_natural i=0; i<nOfRows; ++i)
        *(distMatrixTemp + nOfColumns*i) -= minValue;

    }

    /* Steps 1 and 2a */
    for(col=0; col<nOfColumns; col++)
      for(row=0; row<nOfRows; row++)
        if(distMatrix[row*nOfColumns + col] == 0)
          if(!coveredRows[row])
          {
            starMatrix[row*nOfColumns + col] = true;
            coveredColumns[col]           = true;
            coveredRows[row]              = true;
            break;
          }
    for(row=0; row<nOfRows; row++)
      coveredRows[row] = false;

  }


  /* move to step 2b */
  step2b(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);

  /* compute cost and remove invalid assignments */
  computeassignmentcost(assignment, cost, distMatrixIn, nOfRows, nOfColumns);

  /* free allocated memory */
  mxFree(distMatrix);
  mxFree(coveredColumns);
  mxFree(coveredRows);
  mxFree(starMatrix);
  mxFree(primeMatrix);
  mxFree(newStarMatrix);

  return;
}

void
NumericLib::buildassignmentvector(mrs_natural *assignment, bool *starMatrix, mrs_natural nOfRows, mrs_natural nOfColumns)
{
  mrs_natural row, col;

  for(row=0; row<nOfRows; row++)
    for(col=0; col<nOfColumns; col++)
      //if(starMatrix[row + nOfRows*col])
      if(starMatrix[row*nOfColumns + col])
      {
        //#ifdef ONE_INDEXING
        //				assignment[row] = col + 1; /* MATLAB-Indexing */
        //#else
        assignment[row] = col;
        //#endif
        break;
      }
}

void
NumericLib::computeassignmentcost(mrs_natural *assignment, mrs_real *cost, mrs_real *distMatrix, mrs_natural nOfRows, mrs_natural nOfColumns)
{
  mrs_natural row, col;
#ifdef CHECK_FOR_INF
  mrs_real value;
#endif

  for(row=0; row<nOfRows; row++)
  {
    //#ifdef ONE_INDEXING
    //		col = assignment[row]-1; /* MATLAB-Indexing */
    //#else
    col = assignment[row];
    //#endif

    if(col >= 0)
    {
#ifdef CHECK_FOR_INF
      value = distMatrix[row + nOfRows*col];
      //if(mxIsFinite(value))
      if( value > -numeric_limits<double>::infinity() && value < numeric_limits<double>::infinity())
        *cost += value;
      else
        //#ifdef ONE_INDEXING
        //				assignment[row] =  0.0;
        //#else
        assignment[row] = -1.0;
      //#endif

#else
      //*cost += distMatrix[row + nOfRows*col];
      *cost += distMatrix[row*nOfColumns + col];
#endif
    }
  }
}

void
NumericLib::step2a(mrs_natural *assignment, mrs_real *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, mrs_natural nOfRows, mrs_natural nOfColumns, mrs_natural minDim)
{
  bool *starMatrixTemp; //*columnEnd;
  mrs_natural col;

  /* cover every column containing a starred zero */
  for(col=0; col<nOfColumns; col++)
  {
    //starMatrixTemp = starMatrix     + nOfRows*col;
    //columnEnd      = starMatrixTemp + nOfRows;
    //while(starMatrixTemp < columnEnd){
    //	if(*starMatrixTemp++)
    //	{
    //		coveredColumns[col] = true;
    //		break;
    //	}
    //}
    starMatrixTemp = starMatrix + col;
    for (mrs_natural i=0; i< nOfRows; ++i) {
      if (*(starMatrixTemp + nOfColumns*i)) {
        coveredColumns[col] = true;
        break;
      }
    }
  }

  /* move to step 3 */
  step2b(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}

void
NumericLib::step2b(mrs_natural *assignment, mrs_real *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, mrs_natural nOfRows, mrs_natural nOfColumns, mrs_natural minDim)
{
  mrs_natural col, nOfCoveredColumns;

  /* count covered columns */
  nOfCoveredColumns = 0;
  for(col=0; col<nOfColumns; col++)
    if(coveredColumns[col])
      nOfCoveredColumns++;

  if(nOfCoveredColumns == minDim)
  {
    /* algorithm finished */
    buildassignmentvector(assignment, starMatrix, nOfRows, nOfColumns);
  }
  else
  {
    /* move to step 3 */
    step3(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
  }

}

void
NumericLib::step3(mrs_natural *assignment, mrs_real *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, mrs_natural nOfRows, mrs_natural nOfColumns, mrs_natural minDim)
{
  bool zerosFound;
  mrs_natural row, col, starCol;

  zerosFound = true;
  while(zerosFound)
  {
    zerosFound = false;
    for(col=0; col<nOfColumns; col++)
      if(!coveredColumns[col])
        for(row=0; row<nOfRows; row++)
          //if((!coveredRows[row]) && (distMatrix[row + nOfRows*col] == 0))
          if((!coveredRows[row]) && (distMatrix[row*nOfColumns + col] == 0))
          {
            /* prime zero */
            //primeMatrix[row + nOfRows*col] = true;
            primeMatrix[row*nOfColumns + col] = true;

            /* find starred zero in current row */
            for(starCol=0; starCol<nOfColumns; starCol++)
              //if(starMatrix[row + nOfRows*starCol])
              if(starMatrix[row*nOfColumns + starCol])
                break;


            if(starCol == nOfColumns) /* no starred zero found */
            {
              /* move to step 4 */
              step4(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim, row, col);
              return;
            }
            else
            {
              coveredRows[row]        = true;
              coveredColumns[starCol] = false;
              zerosFound              = true;
              break;
            }
          }
  }

  /* move to step 5 */
  step5(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}

void
NumericLib::step4(mrs_natural *assignment, mrs_real *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, mrs_natural nOfRows, mrs_natural nOfColumns, mrs_natural minDim, mrs_natural row, mrs_natural col)
{
  mrs_natural n, starRow, starCol, primeRow, primeCol;
  mrs_natural nOfElements = nOfRows*nOfColumns;

  /* generate temporary copy of starMatrix */
  for(n=0; n<nOfElements; n++)
    newStarMatrix[n] = starMatrix[n];

  /* star current zero */
  //newStarMatrix[row + nOfRows*col] = true;
  newStarMatrix[row*nOfColumns + col] = true;

  /* find starred zero in current column */
  starCol = col;
  for(starRow=0; starRow<nOfRows; starRow++)
    //if(starMatrix[starRow + nOfRows*starCol])
    if(starMatrix[starRow*nOfColumns + starCol])
      break;

  while(starRow<nOfRows)
  {
    /* unstar the starred zero */
    //newStarMatrix[starRow + nOfRows*starCol] = false;
    newStarMatrix[starRow*nOfColumns + starCol] = false;

    /* find primed zero in current row */
    primeRow = starRow;
    for(primeCol=0; primeCol<nOfColumns; primeCol++)
      //if(primeMatrix[primeRow + nOfRows*primeCol])
      if(primeMatrix[primeRow*nOfColumns + primeCol])
        break;

    /* star the primed zero */
    //newStarMatrix[primeRow + nOfRows*primeCol] = true;
    newStarMatrix[primeRow*nOfColumns + primeCol] = true;

    /* find starred zero in current column */
    starCol = primeCol;
    for(starRow=0; starRow<nOfRows; starRow++)
      /*if(starMatrix[starRow + nOfRows*starCol])*/
      if(starMatrix[starRow*nOfColumns + starCol])
        break;
  }

  /* use temporary copy as new starMatrix */
  /* delete all primes, uncover all rows */
  for(n=0; n<nOfElements; n++)
  {
    primeMatrix[n] = false;
    starMatrix[n]  = newStarMatrix[n];
  }
  for(n=0; n<nOfRows; n++)
    coveredRows[n] = false;

  /* move to step 2a */
  step2a(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}

void
NumericLib::step5(mrs_natural *assignment, mrs_real *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, mrs_natural nOfRows, mrs_natural nOfColumns, mrs_natural minDim)
{
  mrs_real h, value;
  mrs_natural row, col;

  /* find smallest uncovered element h */
  h = mxGetInf();
  for(row=0; row<nOfRows; row++)
    if(!coveredRows[row])
      for(col=0; col<nOfColumns; col++)
        if(!coveredColumns[col])
        {
          //value = distMatrix[row + nOfRows*col];
          value = distMatrix[row*nOfColumns + col];
          if(value < h)
            h = value;
        }

  /* add h to each covered row */
  for(row=0; row<nOfRows; row++)
    if(coveredRows[row])
      for(col=0; col<nOfColumns; col++)
        //distMatrix[row + nOfRows*col] += h;
        distMatrix[row*nOfColumns + col] += h;

  /* subtract h from each uncovered column */
  for(col=0; col<nOfColumns; col++)
    if(!coveredColumns[col])
      for(row=0; row<nOfRows; row++)
        /*distMatrix[row + nOfRows*col] -= h;*/
        distMatrix[row*nOfColumns + col] -= h;

  /* move to step 3 */
  step3(assignment, distMatrix, starMatrix, newStarMatrix, primeMatrix, coveredColumns, coveredRows, nOfRows, nOfColumns, minDim);
}
