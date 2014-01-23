#ifndef MARSYAS_COMMONHEADER_H
#define MARSYAS_COMMONHEADER_H

/**
	\file common_header.h
	\ingroup NotmarCore

    This file should:
      - only contain system-independent, configure-independent material.
      - be included in all .h files
      - be installed by cmake
*/

/************************************************************************/
/*      common includes                                                 */
/************************************************************************/

#include <marsyas/types.h>
#include <marsyas/MrsLog.h>

#include <cstdio>
#include <sys/types.h>
#include <complex>
#include <limits>
#include <cassert>

/************************************************************************/
/*      common constants */
/************************************************************************/
namespace Marsyas
{
// constants
const mrs_natural MRS_DEFAULT_SLICE_NSAMPLES = 512;
const mrs_natural MRS_DEFAULT_SLICE_NOBSERVATIONS = 1;
const mrs_real    MRS_DEFAULT_SLICE_SRATE = 22050.0;

const mrs_natural FFT_FORWARD = 1;
const mrs_natural FFT_INVERSE = 0;

const mrs_string EMPTYSTRING = "MARSYAS_EMPTY";
#undef max //WTF??
#undef min //WTF??

const mrs_real MAXREAL = std::numeric_limits<mrs_real>::max();
const mrs_real MINREAL = std::numeric_limits<mrs_real>::min();
const mrs_natural MAXNATURAL = std::numeric_limits<mrs_natural>::max();
const mrs_natural MINNATURAL = std::numeric_limits<mrs_natural>::min();

// double precision
#ifdef PI
#undef PI //to avoid clashes with other macros defined for PI in other libs... 
#endif
const mrs_real PI = 3.14159265358979323846;
const mrs_real TWOPI = 6.28318530717958647692;


} //namespace Marsyas

/************************************************************************/
/*  common defines                                                      */
/************************************************************************/

//used for PCM audio => should not be changed!
#define PCM_MAXSHRT 32767
#define PCM_FMAXSHRT 32767.0f
#define PCM_MAXINT 2147483647
#define PCM_FMAXINT 2147483647.0f

#ifndef NDEBUG
#define MARSYAS_ASSERTS
#endif

#define MRSASSERT(f) assert(f);

#define MRSERR(x) {std::ostringstream oss; MrsLog::mrsErr((std::ostringstream&)(oss << x));}

#define MRSWARN(x) {std::ostringstream oss; MrsLog::mrsWarning((std::ostringstream&)(oss << x));}

#define MRS_WARNINGS_OFF MrsLog::warnings_off_ = true;
#define MRS_WARNINGS_ON MrsLog::warnings_off_ = false;
#define MRS_MESSAGES_OFF MrsLog::messages_off_ = true;
#define MRS_MESSAGES_ON MrsLog::messages_off_ = false;

//	Macros for marking functions as deprecated.
//	DEPRECATED(void OldFunc(int a, float b));

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif

#if defined(_MSC_VER)
# define MARSYAS_ALIGN(X) __declspec(align(X))
#elif defined(__GNUC__)  && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 8))
# define MARSYAS_ALIGN(X) __attribute__ ((aligned (X)))
#else
# define MARSYAS_ALIGN(X) alignas(X)
#endif

#endif /* !MARSYAS_COMMONHEADER_H */

