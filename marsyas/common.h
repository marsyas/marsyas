#ifndef MARSYAS_COMMON_H
#define MARSYAS_COMMON_H 1

#if HAVE_CONFIG_H 
#include <marsyas/config.h> 
#endif 

#include <stdio.h> 
#include <sys/types.h> 

#define MRS_DEFAULT_SLICE_NSAMPLES 512
#define MRS_DEFAULT_SLICE_NOBSERVATIONS 1
#define MRS_DEFAULT_SLICE_SRATE   22050.0


#define MAXSHRT 32767
#define FMAXSHRT 32767.0f



#define MAXREAL 10000000.0





#define MRSERR(x) {ostringstream oss; MrsLog::mrsErr((ostringstream&)(oss << x));}




#if MRSDIAGNOSTIC
#define MRSDIAG(x) {ostringstream oss; MrsLog::mrsDiagnostic ((ostringstream&)(oss << x));} 
#else
#define MRSDIAG(x)
#endif

#if MRSWARNING
#define MRSWARN(x) {ostringstream oss; MrsLog::mrsWarning((ostringstream&)(oss << x));}
#else 
#define MRSWARN(x) 
#endif 


#define MRSDEBUG(x) {ostringstream oss; MrsLog::mrsDebug((ostringstream&)(oss << x));}


#if MRSASSERTE
#define MRSASSERT(f) \
        if (f)       \
             {}      \
        else         \
           MrsLog::mrsAssert(__FILE__, __LINE__)
#else 
  #define MRSASSERT(x) 
#endif 

#define PI 3.14159265359f
#define TWOPI 6.28318530718f

#define FFT_FORWARD 1
#define FFT_INVERSE 0

#define real double   
#define natural long 


#endif /* !MARSYAS_COMMON_H */ 
	
