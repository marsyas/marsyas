#ifndef MARSYAS_COMMON_H
#define MARSYAS_COMMON_H

/************************************************************************/
/*			Platform defines  ?                                             */
/************************************************************************/
//__OS_LINUX__
//__OS_MACOSX__
//WIN32
//__CYGWIN__
//__OS_IRIX__

/************************************************************************/
/*      common includes			                                            */
/************************************************************************/
#if HAVE_CONFIG_H 
#include <marsyas/config.h> 
#endif 

#include <cstdio> 
#include <sys/types.h> 
#include <complex>
//#include <limits> //lmartins: why not use this STL include for defining numerical bounds? [!]

#include "MrsLog.h"

/************************************************************************/
/*          common typedefs                                             */
/************************************************************************/
namespace Marsyas
{
	// #define real double   
	// #define natural long
	typedef double mrs_real;
	typedef long mrs_natural;
        typedef bool mrs_bool;
	typedef std::complex<mrs_real> mrs_complex;

} //namespace Marsyas

/************************************************************************/
/*              common defines                                          */
/************************************************************************/
#define MRS_DEFAULT_SLICE_NSAMPLES 512
#define MRS_DEFAULT_SLICE_NOBSERVATIONS 1
#define MRS_DEFAULT_SLICE_SRATE   22050.0

#define MAXSHRT 32767 //lmartins: std::numeric_limits<short int>::max() //[!]
#define FMAXSHRT 32767.0f //lmartins: (float)(MAXSHRT) //[!]
#define MAXREAL 10000000.0 //lmartins: std::numeric_limits<mrs_real>::max() //[!]

#define MRSERR(x) {std::ostringstream oss; MrsLog::mrsErr((std::ostringstream&)(oss << x));}

#if MRSDIAGNOSTIC
#define MRSDIAG(x) MrsLog::mrsDiagnostic(x)
#else
#define MRSDIAG(x)
#endif

#if MRSWARNING
#define MRSWARN(x) {std::ostringstream oss; MrsLog::mrsWarning((std::ostringstream&)(oss << x));}
#else 
#define MRSWARN(x) 
#endif 

#ifdef MRSDEBUGG
	#define MRSDEBUG(x) MrsLog::mrsDebug(x)
#elseif
	#define MRSDEBUG(x)
#endif 

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

/************************************************************************/
/*  MATLAB engine macros                                                */
/************************************************************************/
#ifdef MARSYAS_MATLAB
	#include "MATLABengine.h"
	#define MATLAB_PUT(var, name) {MATLABengine::getMatlabEng()->putVariable(var, name);}
	#define MATLAB_GET(name, var){MATLABengine::getMatlabEng()->getVariable(name, var);}
	#define MATLAB_EVAL(s) {MATLABengine::getMatlabEng()->evalString(s);} 
#else
	#define MATLAB_PUT(var, name)
	#define MATLAB_GET(name, var)
	#define MATLAB_EVAL(s)
#endif

/************************************************************************/
/*		WIN32 specific                                                    */
/************************************************************************/
//only  relevant for WIN32 MSVC (and ignored by all other platforms)
//For more info about the reason for this #pragma consult:
//http://msdn2.microsoft.com/en-us/library/ttcz0bys.aspx
#ifdef WIN32
#pragma warning(disable : 4996)
#endif

#endif /* !MARSYAS_COMMON_H */ 
	
