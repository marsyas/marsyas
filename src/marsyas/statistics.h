/*
** Copyright (C) 2011 Eric Nichols <epnichols@gmail.com>
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

#if !defined(__statistics_h)
#define __statistics_h


#include <cmath>
#include <cstdio>
#include <vector>
#include <limits>
#include <marsyas/common_header.h>
#include <marsyas/realvec.h>

namespace Marsyas
{
/**
\class statistics
	\ingroup NotmarCore
\brief Assorted Stastical Routines
	Routines for computing Nth moments around a mean (such as std, skewness, and kurtosis).
*/

class marsyas_EXPORT statistics
{
private:
  static mrs_real momentN(const realvec& zData, const realvec& weights, int n);
  static realvec zDataWeighted(const realvec& data, const realvec& weights, mrs_real mean);

public:

  statistics();
  ~statistics();

  static mrs_real meanWeighted(const realvec& data, const realvec& weights);
  static mrs_real stddevWeighted(const realvec& data, const realvec& weights, mrs_real mean);
  static mrs_real varWeighted(const realvec& data, const realvec& weights, mrs_real mean);
  static mrs_real skewnessWeighted(const realvec& data, const realvec& weights, mrs_real mean);
  static mrs_real kurtosisWeighted(const realvec& data, const realvec& weights, mrs_real mean);
};


} //namespace Marsyas

#endif //__statistics_h
