/*
** Copyright (C) 1998-2008 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_WINDOWING_H
#define MARSYAS_WINDOWING_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class Windowing
	\ingroup Processing Basic
	\brief Apply a windowing function (envelope) to the input signal.

	Extra controls:
	- \b mrs_string/type : the window function type, supported values:
		"Hamming", "Hanning", "Triangle", "Bartlett", "Blackman",
		"Blackman-Harris", "Cosine"
	- \b mrs_bool/zeroPhasing: TODO DOCME
	- \b mrs_natural/zeroPadding: After windowing, the signal will be zero-padded to this number of samples
	- \b mrs_natural/size: TODO DOCME
	- \b mrs_real/variance: variance to use for the Gaussian window
	- \b mrs_bool/normalize: TODO DOCME
*/


class Windowing: public MarSystem
{
private:
  realvec envelope_;
  realvec tmp_;
  //mrs_real norm_;
  mrs_natural delta_;

  mrs_natural zeroPadding_;
  mrs_natural size_;

  MarControlPtr ctrl_type_;
  MarControlPtr ctrl_zeroPhasing_;
  MarControlPtr ctrl_zeroPadding_;
  MarControlPtr ctrl_size_;
  MarControlPtr ctrl_variance_;
  MarControlPtr ctrl_normalize_;


  void addcontrols();
  void myUpdate(MarControlPtr sender);

public:
  Windowing(std::string name);
  Windowing(const Windowing& a);
  ~Windowing();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);

};

/// \brief Helper function for generating a raised cosine window function.
void
windowingFillRaisedCosine(realvec& envelope, mrs_real alpha, mrs_real beta);

/**
 * \brief Hamming window function.
 *
 * \ingroup Basic
 * \f[ w(n) = 0.54 - 0.46 \cos \left ( \frac{2\pi n}{N-1} \right) \f]
 * \see windowingFillRaisedCosine()
 * \see Windowing
 */
inline
void
windowingFillHamming(realvec& envelope)
{
  windowingFillRaisedCosine(envelope, 0.54, 0.46);
}

/**
 * \brief Hanning window function.
 *
 * \ingroup Basic
 * \f[ w(n) = 0.5 - 0.5 \cos \left ( \frac{2\pi n}{N-1} \right) \f]
 * \see windowingFillRaisedCosine()
 * \see Windowing
 */
inline
void
windowingFillHanning(realvec& envelope)
{
  windowingFillRaisedCosine(envelope, 0.5, 0.5);
}

/// \brief Triangle window function (non zero endpoints).
void windowingFillTriangle(realvec& envelope);

/// \brief Bartlett window function (triangle window with zero endpoints).
void windowingFillBartlett(realvec& envelope);

/// \brief Gaussian window function.
void windowingFillGaussian(realvec& envelope, mrs_real sigma);

/// \brief Blackman window function.
void windowingFillBlackman(realvec& envelope, mrs_real alpha=0.16);

/// \brief Blackman-Harris window function.
void windowingFillBlackmanHarris(realvec& envelope);

/// \brief Cosine (aka sine) window function.
void windowingFillCosine(realvec& envelope);


}//namespace Marsyas

#endif
