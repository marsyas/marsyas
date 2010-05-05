#ifndef MARSYAS_COMMONHEADER_H
#define MARSYAS_COMMONHEADER_H

/**
  \file common_header.h
  \ingroup NotmarCore
*/

/************************************************************************/
/*      common includes			                                            */
/************************************************************************/

#include <cstdio> 
#include <sys/types.h> 
#include <complex>
#include <limits>
#include "MrsLog.h"

/************************************************************************/
/*          common typedefs                                             */
/************************************************************************/
namespace Marsyas
{
	typedef double mrs_real;
	typedef long mrs_natural;
  typedef bool mrs_bool;
	typedef std::complex<mrs_real> mrs_complex;
	typedef std::string mrs_string;
	
	class realvec;
	typedef realvec mrs_realvec; //done at realvec.h

} //namespace Marsyas

/************************************************************************/
/*  common defines                                                      */
/************************************************************************/
#define MRS_DEFAULT_SLICE_NSAMPLES 512
#define MRS_DEFAULT_SLICE_NOBSERVATIONS 1
#define MRS_DEFAULT_SLICE_SRATE   22050.0

#define FFT_FORWARD 1
#define FFT_INVERSE 0

#define EMPTYSTRING "MARSYAS_EMPTY"

#define MAXREAL std::numeric_limits<mrs_real>::max()
#define MINREAL std::numeric_limits<mrs_real>::min()
#define MAXNATURAL std::numeric_limits<mrs_natural>::max()
#define MINNATURAL std::numeric_limits<mrs_natural>::min()

//used for PCM audio => should not be changed!
#define PCM_MAXSHRT 32767 
#define PCM_FMAXSHRT 32767.0f 

#define PI 3.14159265358979323846 //double precision instead of 3.14159265359f
#define TWOPI 6.28318530717958647692 //double precision instead of 6.28318530718f

#define MRSASSERT(f) \
        if (f)       \
             {}      \
        else         \
           MrsLog::mrsAssert(__FILE__, __LINE__)


#define MRSMSG(x) {std::ostringstream oss; MrsLog::mrsMessage((std::ostringstream&)(oss << x));}


#define MRSERR(x) {std::ostringstream oss; MrsLog::mrsErr((std::ostringstream&)(oss << x));}

#define MRSWARN(x) {std::ostringstream oss; MrsLog::mrsWarning((std::ostringstream&)(oss << x));}

#define MRSDIAG(x) {std::ostringstream oss; MrsLog::mrsDiagnostic((std::ostringstream&)(oss << x));}

#define MRS_WARNINGS_OFF MrsLog::warnings_off_ = true;
#define MRS_WARNINGS_ON MrsLog::warnings_off_ = false;


#endif /* !MARSYAS_COMMONHEADER_H */ 
	
