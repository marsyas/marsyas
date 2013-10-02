/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MARSYAS_CONVERSIONS_H
#define MARSYAS_CONVERSIONS_H

#include <marsyas/common_header.h>

#include <cmath>
#include <string>

namespace Marsyas
{
/**
	\file
	\ingroup Notmar
	\brief Various conversion functions
*/

marsyas_EXPORT mrs_real pitch2hertz(mrs_real pitch);
marsyas_EXPORT mrs_real hertz2pitch(mrs_real hz);

marsyas_EXPORT mrs_natural hertz2samples(mrs_real hz, mrs_real srate);

marsyas_EXPORT mrs_real samples2hertz(mrs_real samples, mrs_real srate);
marsyas_EXPORT mrs_real samples2hertz(mrs_natural samples, mrs_real srate);

marsyas_EXPORT mrs_natural time2samples(std::string time, mrs_real srate);
marsyas_EXPORT mrs_natural time2usecs(std::string time);

marsyas_EXPORT mrs_real amplitude2dB(mrs_real a);
marsyas_EXPORT mrs_real dB2amplitude(mrs_real a);

marsyas_EXPORT mrs_real hertz2octs(mrs_real f, mrs_real middleAfreq = 440.0);

marsyas_EXPORT mrs_real hertz2bark(mrs_real f, mrs_natural mode = 0);
marsyas_EXPORT mrs_real bark2hertz(mrs_real f, mrs_natural mode = 0);

marsyas_EXPORT mrs_real hertz2erb(mrs_real f);
marsyas_EXPORT mrs_real erb2hertz(mrs_real e);

marsyas_EXPORT mrs_real hertz2mel(mrs_real f, bool htk = false);
marsyas_EXPORT mrs_real mel2hertz(mrs_real z, bool htk = false);

marsyas_EXPORT mrs_real bin2hertz(mrs_natural bin, mrs_real nyquistFreq,
                                  mrs_real framerate);

marsyas_EXPORT mrs_natural powerOfTwo(mrs_real v);

marsyas_EXPORT void string2parameters(std::string s, realvec &v, char d);

}//namespace Marsyas

#endif
