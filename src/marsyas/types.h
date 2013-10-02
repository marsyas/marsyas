
#ifndef MARSYAS_TYPES_H
#define MARSYAS_TYPES_H

#include <sys/types.h>
#include <complex>

/**
	\file types.h
	\ingroup NotmarCore Certified
*/

namespace Marsyas
{
typedef double mrs_real;
typedef long mrs_natural;
typedef bool mrs_bool;
typedef std::complex<mrs_real> mrs_complex;
typedef std::string mrs_string;

// forward declaration; defined in realvec.h
class realvec;
typedef realvec mrs_realvec;
}

#endif

