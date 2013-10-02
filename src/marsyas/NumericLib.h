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

#if !defined(__NumericLib_h)
#define __NumericLib_h

/******************************************************************/
/*                    Numerical Routines                          */
/*                                                                */
/* author:        B. Frenzel                                      */
/*                                                                */
/* date:          Jan 7 1993                                      */
/*                                                                */
/* description:                                                   */
/* test.c    test environment for testing null()                  */
/* null.c    main rootfinding routine                             */
/* muller.c  rootfinding routine using muller method              */
/* newton.c  rootfinding routine using Newton method              */
/*                                                                */
/* version:       1.2 (edited by LGM - INESC Porto - 06.2006)     */
/*                                                                */
/* further necessary files:                                       */
/* complex.c contains complex operations                          */
/* tools.c   contains tools:                                      */
/*           fdvalue()   computes P(x) and optional P'(x)         */
/*           poldef()    deflates polynomial                      */
/*                                                                */
/* Copyright:                                                     */
/* Lehrstuhl fuer Nachrichtentechnik Erlangen                     */
/* Cauerstr. 7, 8520 Erlangen, FRG, 1993                          */
/* e-mail: mrs_natural@nt.e-technik.uni-erlangen.de                       */
/*                                                                */
/******************************************************************/

#include <cmath>
#include <cstdio>
#include <vector>
#include <limits>
#include <marsyas/common_header.h>
#include <marsyas/realvec.h>

namespace Marsyas
{
/**
\class NumericLib
	\ingroup NotmarCore
\brief Assorted Numerical Routines

	Numerical routines ported and adapted from several sources (indicated where appropriated).
	Code adapted by <lmartins@inescporto.pt> - 16.06.2006
*/


class marsyas_EXPORT NumericLib
{
private:

  /******************************************************************/
  /*                                                                */
  /* file:          null.cpp                                        */
  /*                                                                */
  /* main function: null()                                          */
  /*                                                                */
  /* version:       1.3 (edited by LGM - INESC Porto - 29.04.02)    */
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
  /* e-mail: mrs_natural@nt.e-technik.uni-erlangen.de                       */
  /*                                                                */
  /******************************************************************/
  unsigned char null(mrs_complex *p,mrs_complex *pred,mrs_natural *n,mrs_complex *root,
                     mrs_real *maxerr,unsigned char flag);

  unsigned char poly_check(mrs_complex *pred,mrs_natural *nred,mrs_natural *n,mrs_complex *root);
  void quadratic(mrs_complex *pred,mrs_complex *root);
  unsigned char lin_or_quad(mrs_complex *pred,mrs_natural nred,mrs_complex *root);
  void monic(mrs_complex *p,mrs_natural *n);

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
  /* e-mail: mrs_natural@nt.e-technik.uni-erlangen.de                       */
  /*                                                                */
  /******************************************************************/
  mrs_complex muller(mrs_complex *pred,mrs_natural nred);

  mrs_complex x0MULLER_,x1MULLER_,x2MULLER_,	/* common pomrs_naturals [x0,f(x0)=P(x0)], ... [x2,f(x2)]  */
              f0MULLER_,f1MULLER_,f2MULLER_,			/* of parabola and polynomial                      */
              h1MULLER_,h2MULLER_,					/* distance between x2 and x1                      */
              q2MULLER_;							/* smaller root of parabola                        */
  mrs_natural iterMULLER_;					/* iteration counter                               */

  void initialize(mrs_complex *pred,mrs_complex *xb,mrs_real *epsilon);
  void root_of_parabola(void);
  void iteration_equation(mrs_real *h2abs);
  void suppress_overflow(mrs_natural nred);
  void too_big_functionvalues(mrs_real *f2absq);
  void convergence_check(mrs_natural *overflow,mrs_real f1absq,mrs_real f2absq,
                         mrs_real epsilon);
  void compute_function(mrs_complex *pred,mrs_natural nred,mrs_real f1absq,
                        mrs_real *f2absq,mrs_real epsilon);
  void check_x_value(mrs_complex *xb,mrs_real *f2absqb,mrs_natural *rootd,
                     mrs_real f1absq,mrs_real f2absq,mrs_real epsilon,
                     mrs_natural *noise);
  void root_check(mrs_complex *pred,mrs_natural nred,mrs_real f2absqb,mrs_natural *seconditer,
                  mrs_natural *rootd,mrs_natural *noise,mrs_complex xb);

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
  /* e-mail: mrs_natural@nt.e-technik.uni-erlangen.de                       */
  /*                                                                */
  /******************************************************************/
  mrs_complex newton(mrs_complex *p,mrs_natural n,mrs_complex ns,mrs_real *dxabs,
                     unsigned char flag);

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
  /* e-mail: mrs_natural@nt.e-technik.uni-erlangen.de                       */
  /*                                                                */
  /******************************************************************/
  void hornc(mrs_complex *pred,mrs_natural nred,mrs_complex x0,unsigned char flag);
  void horncd(mrs_complex *pred,mrs_natural nred,mrs_real a,mrs_real b);
  mrs_natural poldef(mrs_complex *pred,mrs_natural nred,mrs_complex *root,unsigned char flag);
  void fdvalue(mrs_complex *p,mrs_natural n,mrs_complex *f,mrs_complex *df,mrs_complex x0,
               unsigned char flag) ;

  static mrs_real add(mrs_real *a, mrs_real *b);
  static mrs_real pow_di(mrs_real *ap, mrs_natural *bp);

  ////////////////////////////////////////////////
  //hungarianAssignemt private methods
  ////////////////////////////////////////////////
  static mrs_real mxGetInf();
  static bool mxIsInf(mrs_real s);
  static void mxFree( void * s );
  static void mexErrMsgTxt(const char * s);
  static void assignmentoptimal(mrs_natural *assignment, mrs_real *cost, mrs_real *distMatrix, mrs_natural nOfRows, mrs_natural nOfColumns);
  static void buildassignmentvector(mrs_natural *assignment, bool *starMatrix, mrs_natural nOfRows, mrs_natural nOfColumns);
  static void computeassignmentcost(mrs_natural *assignment, mrs_real *cost, mrs_real *distMatrix, mrs_natural nOfRows, mrs_natural nOfColumns);
  static void step2a(mrs_natural *assignment, mrs_real *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, mrs_natural nOfRows, mrs_natural nOfColumns, mrs_natural minDim);
  static void step2b(mrs_natural *assignment, mrs_real *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, mrs_natural nOfRows, mrs_natural nOfColumns, mrs_natural minDim);
  static void step3 (mrs_natural *assignment, mrs_real *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, mrs_natural nOfRows, mrs_natural nOfColumns, mrs_natural minDim);
  static void step4 (mrs_natural *assignment, mrs_real *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, mrs_natural nOfRows, mrs_natural nOfColumns, mrs_natural minDim, mrs_natural row, mrs_natural col);
  static void step5 (mrs_natural *assignment, mrs_real *distMatrix, bool *starMatrix, bool *newStarMatrix, bool *primeMatrix, bool *coveredColumns, bool *coveredRows, mrs_natural nOfRows, mrs_natural nOfColumns, mrs_natural minDim);

public:

  NumericLib();
  ~NumericLib();

  bool polyRoots(std::vector<mrs_complex> coefs, bool complexCoefs, mrs_natural order, std::vector<mrs_complex> &roots);
  mrs_real determinant(realvec matrix);

  static mrs_real gaussian(mrs_real x, mrs_real var, mrs_real mean)
  {
    return exp(-(x-mean)*(x-mean)/(2*var*var))/(var*sqrt(2*PI));
  }

  ///////////////////////////////////////////////////////////////////////////
  //						metrics
  ///////////////////////////////////////////////////////////////////////////
  static mrs_real euclideanDistance(const realvec& Vi, const realvec& Vj, const realvec& covMatrix);
  static mrs_real mahalanobisDistance(const realvec& Vi, const realvec& Vj, const realvec& covMatrix);
  static mrs_real cosineDistance(const realvec& Vi, const realvec& Vj, const realvec& dummy = realvec());
  static mrs_real cityblockDistance(const realvec& Vi, const realvec& Vj, const realvec& dummy = realvec());
  static mrs_real correlationDistance(const realvec& Vi, const realvec& Vj, const realvec& dummy = realvec());
  static mrs_real divergenceShape(const realvec& Ci, const realvec& Cj, const realvec& dummy = realvec());
  static mrs_real bhattacharyyaShape(const realvec& Ci, const realvec& Cj, const realvec& dummy = realvec());
  ///////////////////////////////////////////////////////////////////////////

  // A is m x n
  // U is m x m
  // V is n x n
  // s is max(m,n)+1 x 1
  static void svd(mrs_natural m, mrs_natural n, realvec &A, realvec &U, realvec &V, realvec &s);

  /*
  Householder reduction of matrix a to tridiagonal form.
  Algorithm: Martin et al., Num. Math. 11, 181-195, 1968.
  Ref: Smith et al., Matrix Eigensystem Routines -- EISPACK Guide
  Springer-Verlag, 1976, pp. 489-494.
  W H Press et al., Numerical Recipes in C, Cambridge U P,
  1988, pp. 373-374.

  Source code adapted from F. Murtagh, Munich, 6 June 1989
  http://astro.u-strasbg.fr/~fmurtagh/mda-sw/pca.c
  */
  //void tred2(realvec &**a, mrs_natural n, realvec &*d, realvec &*e);
  static void tred2(realvec &a, mrs_natural n, realvec &d, realvec &e);

  /*
  Tridiagonal QL algorithm -- Implicit

  Source code adapted from F. Murtagh, Munich, 6 June 1989
  http://astro.u-strasbg.fr/~fmurtagh/mda-sw/pca.c
  */
  //void tqli(double d[], double e[], mrs_natural n, realvec &*z);
  static void tqli(realvec &d, realvec &e, mrs_natural n, realvec &z);

  // Machine parameters
  // cmach :
  //    'B' | 'b' --> base
  //    'M' | 'm' --> digits in the mantissa
  //    'R' | 'r' --> approximation method : 1=rounding 0=chopping
  //    'E' | 'e' --> eps
  static mrs_real machp(const char *cmach);

  //////////////////////////////////////////////////
  //Hungarian assignment routine
  //////////////////////////////////////////////////
  static mrs_real hungarianAssignment(realvec& matrixdist, realvec& matrixAssign);

};

}//namespace Marsyas

#endif //__NumericLib_h


