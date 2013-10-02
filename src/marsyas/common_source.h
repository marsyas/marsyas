#ifndef MARSYAS_COMMON_H
#define MARSYAS_COMMON_H

/**
  \file common_source.h
  \ingroup NotmarCore

  This file should:
    - contain system-dependent, configure-dependent material
    - be included in .cpp files
    - NOT be installed with cmake
*/

/************************************************************************/
/*      common includes			                                            */
/************************************************************************/
#include "config.h"

#include <cstdio>
#include <sys/types.h>
#include <complex>
#include <limits>
#include <marsyas/MrsLog.h>


/************************************************************************/
/*  Numerical defines                                                   */
/************************************************************************/
#ifdef WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#endif

/************************************************************************/
/* LOGGING MACROS                                                       */
/************************************************************************/



#ifdef MARSYAS_LOG_DEBUGS
#define MRSDEBUG(x) {std::ostringstream oss; MrsLog::mrsDebug((std::ostringstream&)(oss << x));}
#else
#define MRSDEBUG(x)
#endif



#ifdef MARSYAS_LOG_DIAGNOSTICS
# define MRSDIAG(x) {std::ostringstream oss; MrsLog::mrsDiagnostic((std::ostringstream&)(oss << x));}
#else
# define MRSDIAG(x)
#endif


#ifdef MARSYAS_LOG_MESSAGES
# define MRSMSG(x) {std::ostringstream oss; MrsLog::mrsMessage((std::ostringstream&)(oss << x));}
#else
# define MRSMSG(x)
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
#define MATLAB_PUT(var, name) {}
#define MATLAB_GET(name, var) {}
//#define MATLAB_GET(name, var) -1
#define MATLAB_EVAL(s)  {}
#define MATLAB_CLOSE()  {}
#endif


/************************************************************************/
/*  Sleep MACRO                                                         */
/************************************************************************/
// FIXME: old definition; I don't think it works well.
// #if defined(__WINDOWS_ASIO__) || defined (__WINDOWS_DS__)

// FIXME: do we need to include _CYGWIN and _MINGW as well?

#ifdef MARSYAS_WIN32 //|| defined (MARSYAS_MINGW)
#include <windows.h>
#define SLEEP(milliseconds) Sleep((DWORD) milliseconds)
#else
#include <unistd.h>
#define SLEEP(milliseconds) usleep((unsigned long) (milliseconds) * 1000.0)
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

