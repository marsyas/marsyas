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
\class NumericLib
\brief Assorted Numerical Routines

	Numerical routines ported and adapted from several sources (indicated where appropriated).
	Code adapted by <lmartins@inescporto.pt> - 16.06.2006 
*/

#include "NumericLib.h"

//*************************************************************
// Includes for determinant calculation (adapted from):
// http://perso.wanadoo.fr/jean-pierre.moreau/c_matrices.html
//*************************************************************
#include "basis.h"
#include "vmblock.h"
#include "lu.h"
//*************************************************************

#include <cfloat>
#include <limits>

using namespace std;
using namespace Marsyas;

//#define DBL_EPSILON 2.2204460492503131E-16 //=> already defined in float.h...
//#define DBL_MAX 1.7976931348623157E+308 //=> already defined in float.h...
//#define  PI  3.14159265358979323846 /* circular transcendental nb.  */ //already defined in common.h

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

	for (i=0; i<=n; i++)
		for (j=0; j<=n; j++)
			A[i][j] = 0.0;

	// define matrix A column by column
	for (i=1; i<=n; i++)
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
		for (i=1; i<=n; i++)
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
		Cmul(f1MULLER_,RCadd(1.,q2MULLER_))));
	B2   = Cadd(Csub(f2MULLER_,f1MULLER_),Cmul(q2MULLER_,Cadd(Cmul(q2MULLER_,
		Csub(f0MULLER_,f1MULLER_)),RCmul(2.,Csub(f2MULLER_,f1MULLER_)))));
	C2   = Cmul(f2MULLER_,RCadd(1.,q2MULLER_));
	/* discr = B2^2 - 4A2C2                   */
	discr = Csub(Cmul(B2,B2),RCmul(4.,Cmul(A2,C2)));
	/* denominators of q2MULLER_                     */
	N1 = Csub(B2,Csqrt(discr));  
	N2 = Cadd(B2,Csqrt(discr));  
	/* choose denominater with largest modulus    */
	if (Cabs(N1)>Cabs(N2) && Cabs(N1)>DBL_EPSILON)
		q2MULLER_ = Cdiv(RCmul(-2.,C2),N1);  
	else if (Cabs(N2)>DBL_EPSILON)
		q2MULLER_ = Cdiv(RCmul(-2.,C2),N2);  
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
				h2MULLER_=RCmul(.5,h2MULLER_); /* new and old x2MULLER_       */
				q2MULLER_=RCmul(.5,q2MULLER_); 
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
			q2MULLER_ = RCmul(.5,q2MULLER_); /* in case of overflow:            */
			h2MULLER_ = RCmul(.5,h2MULLER_); /* halve q2MULLER_ and h2MULLER_; compute new x2MULLER_ */
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
			q2MULLER_ = RCmul(2.,q2MULLER_);  /* mrs_real q2MULLER_ and h[2]                */
			h2MULLER_ = RCmul(2.,h2MULLER_);     
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
	for (iter=0;iter<ITERMAXNEWTON;iter++) { /* main loop        */
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

	for (i=0;i<=*n;i++)  pred[i]=p[i];  /* original polynomial    */
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

	for (j=0;j<=*n;j++) {      /* determines the "real" degree of       */
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
			i++; 
		else 
			notfound=NumLib_FALSE;
	} while (i<=*n && notfound);

	if (i==0) {            /* no root determined at 0              */
		*nred = *n;        /* original degree=deflated degree and  */
		return 0;          /* return no error                      */
	} else {               /* roots determined at 0:               */
		for (j=0;j<=i-1;j++) /* store roots at 0                   */ 
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
		RCmul(4.,Cmul(pred[2],pred[0])));
	/* Z1 = -p1+sqrt(discr)                 */
	Z1      = Cadd(RCmul(-1.,pred[1]),Csqrt(discr));
	/* Z2 = -p1-sqrt(discr)                 */
	Z2      = Csub(RCmul(-1.,pred[1]),Csqrt(discr));
	/* N  = 2*p2                            */
	N       = RCmul(2.,pred[2]);
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
		root[0] = Cdiv(RCmul(-1.,pred[0]),pred[1]);
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
		for (i=0;i<=*n;i++) 
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

