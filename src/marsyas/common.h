#ifndef MARSYAS_COMMON_H
#define MARSYAS_COMMON_H

/**
  \file common.h
  \ingroup NotmarCore
*/

/************************************************************************/
/*      common includes			                                            */
/************************************************************************/
#if HAVE_CONFIG_H 
#include <marsyas/config.h>
#endif 

#if CMAKE_CONFIG_H
#include "config.h"
#endif

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

/************************************************************************/
/*  Numerical defines                                                   */
/************************************************************************/
#ifdef WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif
#define MAXREAL std::numeric_limits<mrs_real>::max()
#define MINREAL std::numeric_limits<mrs_real>::min()
#define MAXNATURAL std::numeric_limits<mrs_natural>::max()
#define MINNATURAL std::numeric_limits<mrs_natural>::min()

//used for PCM audio => should not be changed!
#define PCM_MAXSHRT 32767 
#define PCM_FMAXSHRT 32767.0f 

#define PI 3.14159265358979323846 //double precision instead of 3.14159265359f
#define TWOPI 6.28318530717958647692 //double precision instead of 6.28318530718f

/************************************************************************/
/* LOGGING MACROS                                                       */
/************************************************************************/
#define MRSMSG(x) {std::ostringstream oss; MrsLog::mrsMessage((std::ostringstream&)(oss << x));}

#define MRSERR(x) {std::ostringstream oss; MrsLog::mrsErr((std::ostringstream&)(oss << x));}

#define MRS_WARNINGS_OFF MrsLog::warnings_off_ = true;
#define MRS_WARNINGS_ON MrsLog::warnings_off_ = false;

#ifdef MARSYAS_LOG_DIAGNOSTICS
#define MRSDIAG(x) {std::ostringstream oss; MrsLog::mrsDiagnostic((std::ostringstream&)(oss << x));}
#else
#define MRSDIAG(x)
#endif

#ifdef MARSYAS_LOG_WARNINGS
#define MRSWARN(x) {std::ostringstream oss; MrsLog::mrsWarning((std::ostringstream&)(oss << x));}
#else 
#define MRSWARN(x) 
#endif 

#ifdef MARSYAS_LOG_DEBUGS
#define MRSDEBUG(x) {std::ostringstream oss; MrsLog::mrsDebug((std::ostringstream&)(oss << x));}
#else
#define MRSDEBUG(x)
#endif 

#ifdef MARSYAS_ASSERTS
#define MRSASSERT(f) \
        if (f)       \
             {}      \
        else         \
           MrsLog::mrsAssert(__FILE__, __LINE__)
#else 
#define MRSASSERT(x) 
#endif 

/************************************************************************/
/*  MATLAB engine macros                                                */
/************************************************************************/
#ifdef MARSYAS_MATLAB
	#include "MATLABengine.h"
  #define MATLAB_PUT(var, name) {MATLABengine::getMatlabEng()->putVariable(var, name);}
	#define MATLAB_GET(name, var) MATLABengine::getMatlabEng()->getVariable(name, var)
	#define MATLAB_EVAL(s) {std::ostringstream oss; MATLABengine::getMatlabEng()->evalString((std::ostringstream&)(oss << s));}
#define MATLAB_CLOSE() MATLABengine::getMatlabEng()->closeMatlabEng()
#else
	#define MATLAB_PUT(var, name)
	#define MATLAB_GET(name, var) -1
	#define MATLAB_EVAL(s) 
       #define MATLAB_CLOSE()
#endif


/************************************************************************/
/*  Sleep MACRO                                                         */
/************************************************************************/
// FIXME: old definition; I don't think it works well.
// #if defined(__WINDOWS_ASIO__) || defined (__WINDOWS_DS__)

// FIXME: do we need to include _CYGWIN and _MINGW as well?
#if defined (MARSYAS_WIN32) || defined (MARSYAS_MINGW)
#include <windows.h> 
#define SLEEP(milliseconds) Sleep((DWORD) milliseconds) 
#else 
#include <unistd.h> 
#define SLEEP(milliseconds) usleep((unsigned long) (milliseconds) * 1000.0) 
#endif 



/************************************************************************/
/*  Mutex MACROS for Multi-threaded Marsyas                             */
/************************************************************************/
#ifdef MARSYAS_QT
	#define MARSYAS_MT //[!]
	#define READ_LOCKER(mutex) QReadLocker locker(&mutex)
	#define WRITE_LOCKER(mutex) QWriteLocker locker(&mutex)
	#define LOCK_FOR_READ(mutex) mutex.lockForRead()
	#define TRY_LOCK_FOR_READ(mutex, timeout) mutex.tryLockForRead(timeout)
	#define LOCK_FOR_WRITE(mutex) mutex.lockForWrite()
	#define TRY_LOCK_FOR_WRITE(mutex, timeout) mutex.tryLockForWrite(timeout)
	#define LOCK(mutex) mutex.lock()
	#define TRY_LOCK(mutex, timeout) mutex.tryLock(timeout)
	#define UNLOCK(mutex) mutex.unlock()
#else
	#define READ_LOCKER(mutex) 
	#define WRITE_LOCKER(mutex)
	#define LOCK_FOR_READ(mutex)
	#define TRY_LOCK_FOR_READ(mutex, timeout) true
	#define LOCK_FOR_WRITE(mutex)
	#define TRY_LOCK_FOR_WRITE(mutex, timeout) true
	#define LOCK(mutex)
	#define TRY_LOCK(mutex, timeout) true
	#define UNLOCK(mutex)
#endif




/************************************************************************/
/*		WIN32 specific                                                    */
/************************************************************************/
//only  relevant for WIN32 MSVC (and ignored by all other platforms)
//For more info about the reason for this #pragma consult:
//http://msdn2.microsoft.com/en-us/library/ttcz0bys.aspx
#ifdef MARSYAS_WIN32
#pragma warning(disable : 4996)
#endif

#endif /* !MARSYAS_COMMON_H */ 
	
