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

#include "lu.h"

/**************************************************************
* Given an N x N matrix A, this routine replaces it by the LU *
* decomposition of a rowwise permutation of itself. A and N   *
* are input. INDX is an output vector which records the row   *
* permutation effected by the partial pivoting; D is output   *
* as -1 or 1, depending on whether the number of row inter-   *
* changes was even or odd, respectively. This routine is used *
* in combination with LUBKSB to solve linear equations or to  *
* invert a matrix. Return code is 1, if matrix is singular.   *
**************************************************************/
int LUDCMP(REAL **A, int n, int *INDX, int *d)  {

  REAL AMAX,DUM, SUM;
  int  I,IMAX,J,K;
  IMAX = 0;

  REAL *VV;
  void *vmblock = NULL;

  vmblock = vminit();
  VV      = (REAL *) vmalloc(vmblock, VEKTOR,  NMAX, 0);

  if (! vmcomplete(vmblock))
  {
    // LogError ("No Memory", 0, __FILE__, __LINE__);
    return -1;
  }

  *d=1;

  for  (I=1; I<n+1; I++)  {
    AMAX=0.0;
    for (J=1; J<n+1; J++)
      if (ABS(A[I][J]) > AMAX)  AMAX=ABS(A[I][J]);

    if(AMAX < TINY)
      return 1; //singular Matrix!
    VV[I] = 1.0 / AMAX;
  } // i loop

  for (J=1; J<n+1; J++)  {

    for (I=1; I<J; I++)  {
      SUM = A[I][J];
      for (K=1; K<I; K++)
        SUM = SUM - A[I][K]*A[K][J];
      A[I][J] = SUM;
    } // i loop
    AMAX = 0.0;

    for (I=J; I<n+1; I++)  {
      SUM = A[I][J];
      for  (K=1; K<J; K++)
        SUM = SUM - A[I][K]*A[K][J];
      A[I][J] = SUM;
      DUM = VV[I]*ABS(SUM);
      if (DUM >= AMAX) {
        IMAX = I;
        AMAX = DUM;
      }
    } // i loop

    if (J != IMAX)  {
      for (K=1; K<n+1; K++)  {
        DUM = A[IMAX][K];
        A[IMAX][K] = A[J][K];
        A[J][K] = DUM;
      } // k loop
      *d = -*d;
      VV[IMAX] = VV[J];
    }

    INDX[J] = IMAX;

    if (ABS(A[J][J]) < TINY)   A[J][J] = TINY;

    if (J != n)  {
      DUM = 1.0 / A[J][J];
      for (I=J+1; I<n+1; I++)
        A[I][J] *= DUM;
    }
  } // j loop

  free(vmblock);
  return 0;

} // subroutine LUDCMP


/*****************************************************************
* Solves the set of N linear equations A . X = B.  Here A is     *
* input, not as the matrix A but rather as its LU decomposition, *
* determined by the routine LUDCMP. INDX is input as the permuta-*
* tion vector returned by LUDCMP. B is input as the right-hand   *
* side vector B, and returns with the solution vector X. A, N and*
* INDX are not modified by this routine and can be used for suc- *
* cessive calls with different right-hand sides. This routine is *
* also efficient for plain matrix inversion.                     *
*****************************************************************/
void LUBKSB(REAL **A, int n, int *INDX, REAL *B)  {
  REAL SUM;
  int  I,II,J,LL;

  II = 0;

  for (I=1; I<n+1; I++)  {
    LL = INDX[I];
    SUM = B[LL];
    B[LL] = B[I];
    if (II != 0)
      for (J=II; J<I; J++)
        SUM = SUM - A[I][J]*B[J];
    else if (SUM != 0.0)  II = I;
    B[I] = SUM;
  } // i loop

  for (I=n; I>0; I--)  {
    SUM = B[I];
    if (I < n)  {
      for (J=I+1; J<n+1; J++)
        SUM = SUM - A[I][J]*B[J];
    }
    B[I] = SUM / A[I][I];
  } // i loop


} // LUBKSB

// end of file lu.cpp


