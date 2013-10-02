/******************************************************
*    LU decomposition routines used by test_lu.cpp    *
*    with dynamic allocations                         *
*                                                     *
*                  C++ version by J-P Moreau, Paris   *
* --------------------------------------------------- *
* Reference:                                          *
*                                                     *
* "Numerical Recipes by W.H. Press, B. P. Flannery,   *
*  S.A. Teukolsky and W.T. Vetterling, Cambridge      *
*  University Press, 1986".                           *
* --------------------------------------------------- *
* Uses: basis_r.cpp and vmblock.cpp                   *
******************************************************/

#if !defined(_LU_CPP_)
#define _LU_CPP_

#include <marsyas/basis.h>
#include "vmblock.h"

#define NMAX 100
#define TINY 1.5e-16

int LUDCMP(REAL **A, int n, int *INDX, int *d);
void LUBKSB(REAL **A, int n, int *INDX, REAL *B);


#endif //_LU_CPP_


