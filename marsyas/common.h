#ifndef MARSYAS_COMMON_H
#define MARSYAS_COMMON_H

//*****************************************************************
//						common includes
//*****************************************************************
#if HAVE_CONFIG_H 
#include <marsyas/config.h> 
#endif 

#include <cstdio> 
#include <sys/types.h> 
#include <complex>
//#include <limits> //lmartins: why not use this STL include for defining numerical bounds? [!]

#include "MrsLog.h"


//*****************************************************************
//							common defines
//*****************************************************************
#define MRS_DEFAULT_SLICE_NSAMPLES 512
#define MRS_DEFAULT_SLICE_NOBSERVATIONS 1
#define MRS_DEFAULT_SLICE_SRATE   22050.0

#define MAXSHRT 32767 //lmartins: std::numeric_limits<short int>::max() //[!]
#define FMAXSHRT 32767.0f //lmartins: (float)(MAXSHRT) //[!]
#define MAXREAL 10000000.0 //lmartins: std::numeric_limits<mrs_real>::max() //[!]

#define MRSERR(x) {ostringstream oss; MrsLog::mrsErr((ostringstream&)(oss << x));}

#if MRSDIAGNOSTIC
#define MRSDIAG(x) MrsLog::mrsDiagnostic(x)
#else
#define MRSDIAG(x)
#endif

#if MRSWARNING
#define MRSWARN(x) {ostringstream oss; MrsLog::mrsWarning((ostringstream&)(oss << x));}
#else 
#define MRSWARN(x) 
#endif 

#define MRSDEBUG(x) MrsLog::mrsDebug(x)

#if MRSASSERTE
#define MRSASSERT(f) \
        if (f)       \
             {}      \
        else         \
           MrsLog::mrsAssert(__FILE__, __LINE__)
#else 
  #define MRSASSERT(x) 
#endif 

#define PI 3.14159265358979323846 //double precision instead of 3.14159265359f
#define TWOPI 6.28318530717958647692 //double precision instead of 6.28318530718f

#define FFT_FORWARD 1
#define FFT_INVERSE 0


//****************************************************************
//						common typedefs
//****************************************************************
namespace Marsyas
{
// #define real double   
// #define natural long
typedef double mrs_real;
typedef long mrs_natural;
typedef std::complex<mrs_real> mrs_complex;

} //namespace Marsyas

#endif /* !MARSYAS_COMMON_H */ 
	
