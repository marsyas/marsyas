// Header file of vmblock.cpp

#ifndef VMBLOCK_H_INCLUDED
#define VMBLOCK_H_INCLUDED

/***********************************************************************
* symbolic names that let the user choose the kind of dynamical data   *
* structure to be allocated upon calling vmalloc()                     *
***********************************************************************/

#define VEKTOR   0                /* for a REAL vector                */
/*.IX{VEKTOR}*/
#define VVEKTOR  1                /* for a vector with elements       */
/*.IX{VVEKTOR}*/
/* of given size                    */
#define MATRIX   2                /* for a REAL matrix                */
/*.IX{MATRIX}*/
#define IMATRIX  3                /* for an int matrix                */
/*.IX{IMATRIX}*/
#define MMATRIX  4                /* for a matrix of 4x4 matrices     */
/*.IX{PMATRIX}*/
/* (with elements of type `mat4x4') */
#define UMATRIX  5                /* for a lower triangular matrix    */
/*.IX{UMATRIX}*/
#define PMATRIX  6                /* for a matrix of points in R3     */
/*.IX{PMATRIX}*/


/***********************************************************************
* operations on a vector matrix list                                   *
***********************************************************************/

void *vminit         /* create an empty vector/matrix list ...........*/
(
  void
);                      /* address of list ...................*/


void *vmalloc        /* create a dynamic vector or matrix ............*/
(
  void   *vmblock,       /* address of a vector/matrix list ...*/
  int    typ,            /* kind of vector or matrix ..........*/
  size_t zeilen,         /* length (vector) or number of rows .*/
  size_t spalten         /* number of columns or element size .*/
);                      /* address of the created object .....*/


bool  vmcomplete     /* check vector/matrix list for lack of memory ..*/
(
  void *vmblock          /* address of list ...................*/
);                      /* no lack of memory? ................*/


void vmfree          /* free the memory for a vektor/matrix list .....*/
(
  void *vmblock          /* address of list ...................*/
);

#endif

/* ------------------------- END vmblock.h -------------------------- */
