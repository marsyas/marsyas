/**********************************************************
*  Reduced and modified version of basis.c by J-P Moreau  *
* ------------------------------------------------------- *
* Reference for original basis.c:                         *
*                                                         *
* "Numerical Algorithms with C, by Gisela Engeln-Muellges *
*  and Frank Uhlig, Springer-Verlag, 1996".               *
**********************************************************/
#include <marsyas/basis.h>

REAL sqr(REAL x)                    /* square a floating point number */
/***********************************************************************
* Compute the square of a floating point number.                       *
*                                                                      *
* global names used:                                                   *
* ==================                                                   *
* REAL                                                                 *
***********************************************************************/
{
  return x * x;
}

REAL norm_max      /* Find the maximum norm of a REAL vector .........*/
(
  REAL vektor[],               /* vector .................*/
  int  n                       /* length of vector .......*/
)                             /* Maximum norm ...........*/

/***********************************************************************
* Return the maximum norm of a [0..n-1] vector  v.                     *
*                                                                      *
* global names used:                                                   *
* ==================                                                   *
* REAL, FABS, ZERO                                                     *
***********************************************************************/
{
  REAL norm,                                             /* local max */
       betrag;                            /* magnitude of a component */

  for (n--, norm = ZERO; n >= 0; n--, vektor++)
    if ((betrag = FABS(*vektor)) > norm)
      norm = betrag;

  return norm;
}

void copy_vector        /* copy a REAL vector ........................*/
(
  REAL ziel[],            /* copied vector ............*/
  REAL quelle[],          /* original vector ..........*/
  int  n                  /* length of vector .........*/
)
/***********************************************************************
* copy the n elements of the vector quelle into the vector ziel.       *
*                                                                      *
* global names used:                                                   *
* ==================                                                   *
* REAL                                                                 *
***********************************************************************/
{
  for (n--; n >= 0; n--)
    *ziel++ = *quelle++;
}

/***********************************************************************
* read a vector x of size n from input file fp (index begins at zero)  *
*                                                                      *
* global names used:                                                   *
* ==================                                                   *
* REAL                                                                 *
***********************************************************************/
int ReadVec (FILE *fp, int n, REAL x[])
{
  int i;
  REAL tmp;

  for (i = 0; i < n; ++i)
  {
    if (fscanf (fp,FORMAT_IN, &tmp) <= 0) return (-1);
    x[i] = (REAL) tmp;
  }

  return (0);
}

/***********************************************************************
* read a vector x of size n from input file fp (index begins at one)   *
*                                                                      *
* global names used:                                                   *
* ==================                                                   *
* REAL                                                                 *
***********************************************************************/
int ReadVec1 (FILE *fp, int n, REAL x[])
{
  int i;
  REAL tmp;

  for (i = 1; i < n+1; ++i)
  {
    if (fscanf (fp,FORMAT_IN, &tmp) <= 0) return (-1);
    x[i] = (REAL) tmp;
  }

  return (0);
}

void SetVec (int n, REAL x[], REAL val)
{
  int i;

  for (i = 0; i < n; ++i)
    x[i] = val;
}

int WriteVec (FILE *fp, int n, REAL x[])
/*====================================================================*
 *                                                                    *
 *  Put out vector of length n to output file. Index begins at zero.  *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *  Input parameters:                                                 *
 *  ================                                                  *
 *                                                                    *
 *      n        int n;                                               *
 *               lenght of vector                                     *
 *      x        REAL x[];                                            *
 *               vector                                               *
 *                                                                    *
 *   Return value :                                                   *
 *   =============                                                    *
 *      =  0     All ok                                               *
 *      = -1     Error putting out to stdout                          *
 *                                                                    *
 *====================================================================*/
{
  int i;

  for (i = 0; i < n; ++i)
    if (fprintf (fp,FORMAT_126LF, x[i]) <= 0) return (-1);
  if (fprintf (fp,"\n") <= 0) return (-1);

  return 0;
}

int WriteVec1 (FILE *fp, int n, REAL x[])
/*====================================================================*
 *                                                                    *
 *  Put out vector of length n to output file. Index begins at one.   *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *  Input parameters:                                                 *
 *  ================                                                  *
 *                                                                    *
 *      n        int n;                                               *
 *               lenght of vector                                     *
 *      x        REAL x[];                                            *
 *               vector                                               *
 *                                                                    *
 *   Return value :                                                   *
 *   =============                                                    *
 *      =  0     All ok                                               *
 *      = -1     Error putting out to stdout                          *
 *                                                                    *
 *====================================================================*/
{
  int i;

  for (i = 1; i < n+1; ++i)
    if (fprintf (fp,FORMAT_126LF, x[i]) <= 0) return (-1);
  if (fprintf (fp,"\n") <= 0) return (-1);

  return 0;
}

REAL pi() {
  return 4.0*atan(1.0);
}

void CopyMat (int m, int n, REAL * source[], REAL * dest[])
/*====================================================================*
 *                                                                    *
 *  Copy the m x n matrix source to the  m x n matrix dest.           *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *  Input parameters:                                                 *
 *  ================                                                  *
 *      m        int m; ( m > 0 )                                     *
 *               numnber of rows of matrix                            *
 *      n        int n; ( n > 0 )                                     *
 *               number of columns of matrix                          *
 *      source   REAL * source[];                                     *
 *               matrix                                               *
 *      dest     REAL * dest[];                                       *
 *               matrix to be copied to                               *
 *                                                                    *
 *   Output parameter:                                                *
 *   ================                                                 *
 *      dest     same as above                                        *
 *                                                                    *
 *   ATTENTION : WE do not allocate storage for dest here.            *
 *                                                                    *
 *====================================================================*/
{
  int i, j;

  for (i = 0; i < m; ++i)
    for (j = 0; j < n; j++)
      dest[i][j] = source[i][j];
}

REAL maxroot(void)    /* Root of the largest representable number ....*/
/***********************************************************************
* Compute the square root of the largest machine number 2 ^ (MAX_EXP/2)*
* if not already done                                                  *
*                                                                      *
* global names used:                                                   *
* ==================                                                   *
* REAL, boolean, FALSE, TRUE, SQRT, POSMAX                             *
***********************************************************************/
{
  static REAL       save_maxroot;
  static boolean    schon_berechnet = FALSE;
  REAL              faktor;
  unsigned long int n;

  if (! schon_berechnet)
  {
    save_maxroot = ONE;
    faktor       = TWO;
    for (n = MAX_EXP / 2; n > 1; n /= 2, faktor *= faktor)
      if (n % 2 != 0)
        save_maxroot *= faktor;
    save_maxroot    *= faktor;
    schon_berechnet  = TRUE;
  }

  return save_maxroot;
}


void quadsolv           /* Complex quadratic equation ................*/
(
  REAL    ar,        /* second degree coefficient .......*/
  REAL    ai,
  REAL    br,        /* linear coefficient ..............*/
  REAL    bi,
  REAL    cr,        /* polynomial constant .............*/
  REAL    ci,
  REAL *  tr,        /* solution ........................*/
  REAL *  ti
)
/*====================================================================*
 *                                                                    *
 *  Compute the least magnitude solution of the quadratic equation    *
 *  a * t**2 + b * t + c = 0. Here a, b, c and t are complex.         *
 *                                         2                          *
 *  Formeula used: t = 2c / (-b +/- sqrt (b  - 4ac)).                 *
 *  This formula is valid for a=0 .                                   *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *  Input parameters:                                                 *
 *  ================                                                  *
 *      ar, ai   coefficient of t**2             REAL   ar, ai;       *
 *      br, bi   coefficient of t                REAL   br, bi;       *
 *      cr, ci   constant term                   REAL   cr, ci;       *
 *                                                                    *
 *  Output parameter:                                                 *
 *  ================                                                  *
 *      tr, ti   complex solution of minimal magnitude                *
 *                                               REAL   *tr, *ti;     *
 *                                                                    *
 *  Macro used :     SQRT                                             *
 *  ============                                                      *
 *                                                                    *
 *====================================================================*/
{
  REAL pr, pi, qr, qi, h;

  pr = br * br - bi * bi;
  pi = TWO * br * bi;                       /*  p = b * b             */

  qr = ar * cr - ai * ci;
  qi = ar * ci + ai * cr;                   /*  q = a * c             */

  pr = pr - (REAL)4.0 * qr;
  pi = pi - (REAL)4.0 * qi;                 /* p = b * b - 4 * a * c  */

  h  = SQRT (pr * pr + pi * pi);            /* q = sqrt (p)           */

  qr = h + pr;
  if (qr > ZERO)
    qr = SQRT (qr * HALF);
  else
    qr = ZERO;

  qi = h - pr;
  if (qi > ZERO)
    qi = SQRT (qi * HALF);
  else
    qi = ZERO;

  if (pi < ZERO) qi = -qi;

  h = qr * br + qi * bi;     /* p = -b +/- q, choose sign for large  */
  /* magnitude  p                         */
  if (h > ZERO)
  {
    qr = -qr;
    qi = -qi;
  }

  pr = qr - br;
  pi = qi - bi;
  h = pr * pr + pi * pi;                      /* t = (2 * c) / p      */

  if (h == ZERO)
  {
    *tr = ZERO;
    *ti = ZERO;
  }
  else
  {
    *tr = TWO * (cr * pr + ci * pi) / h;
    *ti = TWO * (ci * pr - cr * pi) / h;
  }
} //quadsolv


REAL comabs             /* Complex absolute value ....................*/
(
  REAL  ar,          /* Real part .......................*/
  REAL  ai           /* Imaginary part ..................*/
)
/*====================================================================*
 *                                                                    *
 *  Complex absolute value of   a                                     *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *   Input parameters:                                                *
 *   ================                                                 *
 *      ar,ai    REAL   ar, ai;                                       *
 *               Real, imaginary parts of  a                          *
 *                                                                    *
 *   Return value :                                                   *
 *   =============                                                    *
 *      Absolute value of a                                           *
 *                                                                    *
 *   Macros used :    SQRT, ABS, SWAP                                 *
 *   =============                                                    *
 *                                                                    *
 *====================================================================*/
{
  if (ar == ZERO && ai == ZERO) return (ZERO);

  ar = ABS (ar);
  ai = ABS (ai);

  if (ai > ar)                                  /* Switch  ai and ar */
    SWAP (REAL, ai, ar)

    return ((ai == ZERO) ? (ar) : (ar * SQRT (ONE + ai / ar * ai / ar)));
}


int comdiv              /* Complex division ..........................*/
(
  REAL   ar,            /* Real part of numerator ..........*/
  REAL   ai,            /* Imaginary part of numerator .....*/
  REAL   br,            /* Real part of denominator ........*/
  REAL   bi,            /* Imaginary part of denominator ...*/
  REAL * cr,            /* Real part of quotient ...........*/
  REAL * ci             /* Imaginary part of quotient ......*/
)
/*====================================================================*
 *                                                                    *
 *  Complex division  c = a / b                                       *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *   Input parameters:                                                *
 *   ================                                                 *
 *      ar,ai    REAL   ar, ai;                                       *
 *               Real, imaginary parts of numerator                   *
 *      br,bi    REAL   br, bi;                                       *
 *               Real, imaginary parts of denominator                 *
 *                                                                    *
 *   Output parameters:                                               *
 *   ==================                                               *
 *      cr,ci    REAL   *cr, *ci;                                     *
 *               Real , imaginary parts of the quotient               *
 *                                                                    *
 *   Return value :                                                   *
 *   =============                                                    *
 *      = 0      ok                                                   *
 *      = 1      division by 0                                        *
 *                                                                    *
 *   Macro used :     ABS                                             *
 *   ============                                                     *
 *                                                                    *
 *====================================================================*/
{
  REAL tmp;

  if (br == ZERO && bi == ZERO) return (1);

  if (ABS (br) > ABS (bi))
  {
    tmp  = bi / br;
    br   = tmp * bi + br;
    *cr  = (ar + tmp * ai) / br;
    *ci  = (ai - tmp * ar) / br;
  }
  else
  {
    tmp  = br / bi;
    bi   = tmp * br + bi;
    *cr  = (tmp * ar + ai) / bi;
    *ci  = (tmp * ai - ar) / bi;
  }

  return (0);
}

int ReadMat (FILE *fp, int m, int n, REAL * a[])
/*====================================================================*
 *                                                                    *
 *  Read an m x n matrix from input file. Index starts at zero.       *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *  Input parameters :                                                *
 *  ==================                                                *
 *      m        int m; ( m > 0 )                                     *
 *               number of rows of matrix                             *
 *      n        int n; ( n > 0 )                                     *
 *               column number of  matrix                             *
 *      a        REAL * a[];                                          *
 *               matrix                                               *
 *                                                                    *
 *   Output parameter:                                                *
 *   ================                                                 *
 *      a        matrix                                               *
 *                                                                    *
 *   ATTENTION : WE do not allocate storage for a here.               *
 *                                                                    *
 *====================================================================*/
{
  int i, j;
  double x;

  for (i = 0; i < m; ++i)
    for (j = 0; j < n; j++)
    {
      if (fscanf (fp,FORMAT_IN, &x) <= 0) return (-1);
      a[i][j] = (REAL) x;
    }

  return (0);
}

//same as ReadMat, but beginning at index 1
int ReadMat1 (FILE *fp, int m, int n, REAL * a[])
{
  int i, j;
  double x;

  for (i = 1; i < m+1; ++i)
    for (j = 1; j < n+1; j++)
    {
      if (fscanf (fp,FORMAT_IN, &x) <= 0) return (-1);
      a[i][j] = (REAL) x;
    }

  return (0);
}

int WriteMat (FILE *fp, int m, int n, REAL * a[])
/*====================================================================*
 *                                                                    *
 *  Put out an m x n matrix in output file. Index starts at zero.     *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *  Input parameters:                                                 *
 *  ================                                                  *
 *      m        int m; ( m > 0 )                                     *
 *               row number of matrix                                 *
 *      n        int n; ( n > 0 )                                     *
 *               column number of matrix                              *
 *      a        REAL * a[];                                          *
 *               matrix                                               *
 *                                                                    *
 *   Return value :                                                   *
 *   =============                                                    *
 *      =  0      all put out                                         *
 *      = -1      Error writing onto stdout                           *
 *                                                                    *
 *====================================================================*/
{
  int i, j;

  if (fprintf (fp,"\n") <= 0) return (-1);

  for (i = 0; i < m; ++i)
  {
    for (j = 0; j < n; j++)
      if (fprintf (fp,FORMAT_126LF, a[i][j]) <= 0) return (-1);

    if (fprintf (fp,"\n") <= 0) return (-1);
  }
  if (fprintf (fp,"\n") <= 0) return (-1);

  return (0);
}

//same as WriteMat, but beginning at index 1
int WriteMat1 (FILE *fp, int m, int n, REAL * a[])
{
  int i, j;

  if (fprintf (fp,"\n") <= 0) return (-1);

  for (i = 1; i < m+1; ++i)
  {
    for (j = 1; j < n+1; j++)
      if (fprintf (fp,FORMAT_126LF, a[i][j]) <= 0) return (-1);

    if (fprintf (fp,"\n") <= 0) return (-1);
  }
  if (fprintf (fp,"\n") <= 0) return (-1);

  return (0);
}


void SetMat (int m, int n, REAL * a[], REAL val)
/*====================================================================*
 *                                                                    *
 *  Initialize an m x n matrix with a constant value val .            *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *  Input parameters:                                                 *
 *  ================                                                  *
 *      m        int m; ( m > 0 )                                     *
 *               row number of matrix                                 *
 *      n        int n; ( n > 0 )                                     *
 *               column number of matrix                              *
 *      a        REAL * a[];                                          *
 *               matrix                                               *
 *      val      constant value                                       *
 *                                                                    *
 *   Output parameter:                                                *
 *   ================                                                 *
 *      a        matrix with constant value val in every position     *
 *                                                                    *
 *====================================================================*/
{
  int i, j;

  for (i = 0; i < m; ++i)
    for (j = 0; j < n; j++)
      a[i][j] = val;
}

static char Separator[] =
  "--------------------------------------------------------------------";

int WriteHead (FILE *fp, char * string)
/*====================================================================*
 *                                                                    *
 *  Put out header with text in string in file fp.                    *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *  Input parameters:                                                 *
 *  ================                                                  *
 *      string   char *string;                                        *
 *               text of headertext (last byte is a 0)                *
 *                                                                    *
 *   Return value :                                                   *
 *   =============                                                    *
 *      =  0      All ok                                              *
 *      = -1      Error writing onto stdout                           *
 *      = -2      Invalid text for header                             *
 *                                                                    *
 *====================================================================*/
{
  if (string == NULL) return (-2);

  if (fprintf (fp,"\n%s\n%s\n%s\n\n", Separator, string, Separator) <= 0)
    return (-1);

  return 0;
}

int WriteHead1( char * string)
/*====================================================================*
 *                                                                    *
 *  Put out header with text in string in stdout.                     *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *  Input parameters:                                                 *
 *  ================                                                  *
 *      string   char *string;                                        *
 *               text of headertext (last byte is a 0)                *
 *                                                                    *
 *   Return value :                                                   *
 *   =============                                                    *
 *      =  0      All ok                                              *
 *      = -1      Error writing onto stdout                           *
 *      = -2      Invalid text for header                             *
 *                                                                    *
 *====================================================================*/
{
  if (string == NULL) return (-2);

  if (printf ("\n%s\n%s\n%s\n\n", Separator, string, Separator) <= 0)
    return (-1);

  return 0;
}

int WriteEnd (FILE *fp)
/*====================================================================*
 *                                                                    *
 *  Put out end of writing onto file fp.                              *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *   Return value :                                                   *
 *   =============                                                    *
 *      =  0      All ok                                              *
 *      = -1      error writing onto stdout                           *
 *                                                                    *
 *====================================================================*/
{
  if (fprintf (fp,"\n%s\n\n", Separator) <= 0) return (-1);
  return 0;
}

int WriteEnd1(void)
/*====================================================================*
 *                                                                    *
 *  Put out end of writing onto  stdout.                              *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *   Return value :                                                   *
 *   =============                                                    *
 *      =  0      All ok                                              *
 *      = -1      error writing onto stdout                           *
 *                                                                    *
 *====================================================================*/
{
  if (printf ("\n%s\n\n", Separator) <= 0) return (-1);
  return 0;
}

void LogError (char * string, int rc, char * file, int line)
/*====================================================================*
 *                                                                    *
 *  Put out error message onto  stdout.                               *
 *                                                                    *
 *====================================================================*
 *                                                                    *
 *  Input parameters:                                                 *
 *  ================                                                  *
 *      string   char *string;                                        *
 *               text of error massage (final byte is 0)              *
 *      rc       int rc;                                              *
 *               error code                                           *
 *      file     char *file;                                          *
 *               name of C file in which error was encountered        *
 *      line     int line;                                            *
 *               line number of C file with error                     *
 *                                                                    *
 *====================================================================*/
{
  if (string == NULL)
  {
    printf ("Unknown ERROR in file %s at line %d\n", file, line);
    return;
  }

  if (rc == 0)
    printf ("ERROR: %s, File %s, Line %d\n", string, file, line);
  else
    printf ("ERROR: %s, rc = %d, File %s, Line %d\n",
            string, rc, file, line);
  return;
}


REAL epsroot(void)  /* Compute square root of the machine constant ...*/
/***********************************************************************
* Compute square root of the machine constant, if not already done.    *
*                                                                      *
* global names used:                                                   *
* ==================                                                   *
* REAL, boolean, FALSE, TRUE, SQRT, MACH_EPS                           *
***********************************************************************/

{
  static REAL    save_mach_eps_root;
  static boolean schon_berechnet     = FALSE;

  if (! schon_berechnet)
    schon_berechnet    = TRUE,
    save_mach_eps_root = SQRT(MACH_EPS);

  return save_mach_eps_root;
}


REAL epsquad(void)      /* Find the machine constant squared .........*/
/***********************************************************************
* Compute the square of the machine constant, if not already done.     *
*                                                                      *
* global names used:                                                   *
* ==================                                                   *
* REAL, boolean, FALSE, TRUE, MACH_EPS                                 *
***********************************************************************/

{
  static REAL    save_mach_eps_quad;
  static boolean schon_berechnet     = FALSE;

  if (! schon_berechnet)
    schon_berechnet    = TRUE,
    save_mach_eps_quad = MACH_EPS * MACH_EPS;

  return save_mach_eps_quad;
}

// End of file basis_r.cpp
