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


#include "statistics.h"
#include <algorithm>

using std::ostringstream;
using std::numeric_limits;
using std::endl;
using std::cout;
using std::cerr;
using std::min;
using std::max;


using namespace Marsyas;

statistics::statistics()
{
}

statistics::~statistics()
{
}

// PRIVATE

// Returns the nth moment about the mean for data that's already been converted to z-scores.
mrs_real
statistics::momentN(const realvec& zData, const realvec& weights, int n)
{
  if (zData.getSize() != weights.getSize())
  {
    MRSERR("statistics::momentN - wrong size for weights vector!");
    return -1.0;
  }

  mrs_real sum = 0.0;

  for (mrs_natural i=0; i < zData.getSize(); i++)
  {
    sum += weights(i) * pow(zData(i), n);
  }

  return sum;
}


// Computes z scores for a set of data, given a mean.
// The mean is needed so that this fxn can be used to compute moments about 0
// (instead of about the actual mean).
realvec
statistics::zDataWeighted(const realvec& data, const realvec& weights, mrs_real mean)
{
  realvec zData;
  zData.create(data.getSize());

  if (data.getSize() != weights.getSize())
  {
    MRSERR("statistics::zDataWeighted - wrong size for weights vector!");
    return zData;
  }

  mrs_real sd = statistics::stddevWeighted(data, weights, mean);

  if (sd == 0)
  {
    MRSWARN("statistics::zDataWeighted - standard deviation is 0.");
    return zData;
  }

  for (mrs_natural i=0; i < zData.getSize(); i++)
    zData(i) = (data(i) - mean) / sd;

  return zData;
}



// PUBLIC


// Computes the mean for a vector where each element has an
// associated weight (i.e. computes the weighted mean).
mrs_real
statistics::meanWeighted(const realvec& data, const realvec& weights)
{
  if (data.getSize() != weights.getSize())
  {
    MRSERR("statistics::meanWeighted - wrong size for weights vector!");
    return -1.0;
  }

  mrs_real sum = 0.0;
  mrs_real weightsSum = 0.0;

  for (mrs_natural i=0; i < data.getSize(); i++)
  {
    mrs_real w = weights(i);

    sum += w * data(i);
    weightsSum += w;
  }

  if (weightsSum != 0.0) sum /= weightsSum;
  return sum;
}


// Computes the standard deviation for a vector where each element has an
// associated weight.
// Assumes weights sum to 1.
mrs_real
statistics::stddevWeighted(const realvec& data, const realvec& weights, mrs_real mean)
{
  mrs_real variance = varWeighted(data, weights, mean);
  return sqrt(variance);
}

// Computes the variance for a vector where each element has an
// associated weight.
// Assumes weights sum to 1.
mrs_real
statistics::varWeighted(const realvec& data, const realvec& weights, mrs_real mean)
{
  if (data.getSize() != weights.getSize())
  {
    MRSERR("statistics::varWeighted - wrong size for weights vector!");
    return -1.0;
  }

  mrs_real sum = 0.0;

  for (mrs_natural i=0; i < data.getSize(); i++)
  {
    mrs_real w = weights(i);
    mrs_real diff = data(i) - mean;

    sum += w * diff * diff;
  }

  return sum;
}

// Computes the weighted skewness (3rd moment) about the given mean value.
// Assumes weights sum to 1
mrs_real
statistics::skewnessWeighted(const realvec& data, const realvec& weights, mrs_real mean)
{
  realvec zData = zDataWeighted(data, weights, mean);
  return statistics::momentN(zData, weights, 3);
}


// Computes the weighted kurtosis (4rd moment) about the given mean value.
// Assumes weights sum to 1
mrs_real
statistics::kurtosisWeighted(const realvec& data, const realvec& weights, mrs_real mean)
{
  realvec zData = zDataWeighted(data, weights, mean);
  return statistics::momentN(zData, weights, 4) - 3.0;
}

