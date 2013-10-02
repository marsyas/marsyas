/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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


#ifndef MARSYAS_LPC_H
#define MARSYAS_LPC_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis

	\brief Compute Warped LPC coefficients, Pitch and Power [STILL UNDER TESTING!].

	Linear Prediction Coefficients (LPC). Features commonly used
	in Speech Recognition research. This class is a modification of the original
	Marsyas0.1 LPC class.  The following differences apply:
	- order now reflects the LPC order (and returns \<order\> coefficients plus pitch and gain)
	- It is possible to define a pole-shifting parameter, gamma (default value = 1.0 => no shifting)
	- It is possible to define a warping factor, lambda (defualt value = 0.0 => no warping)

	\author Luís Gustavo Martins - lmartins@inescporto.pt
	\date May 2006
*/


class LPC: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_coeffs_;
  MarControlPtr ctrl_power_;
  MarControlPtr ctrl_pitch_;
  mrs_natural order_;
  mrs_natural featureMode_;

  realvec Zs_;
  realvec temp_;

  /**Warped autocorrelation for LPC calculation
  *Based on the code from: http://www.musicdsp.org/showone.php?id=137
  *Also estimates the pitch (only tested for lambda = 0), and updates the pitch_ member variable
  *@param in input audio frame
  *@param r autocorrelation output vector size (LPCorder + 1)
  *@param pitch pitch estimate calculated by autocorrelation
  *@param lambda frequency resolution (warp)
  */
  void autocorrelationWarped(const realvec& in, realvec& r, mrs_real& pitch, mrs_real lambda);

  /**Levinson-Durbin Recursion Algorithm
  *Based on the code from: http://www.musicdsp.org/showone.php?id=137
  *@param r input vector of autocorrelation coeffs
  *@param a output vector with the alpha LPC coeffs => a = [1 a(1) a(2) ... a(order_-1)]
  *@param k output vector with the reflection coeffs
  *@param e prediction error
  */
  void LevinsonDurbin(const realvec& r, realvec& a, realvec& k, mrs_real& e);

  /**LPC RMS Prediction Error
  *Updates the power_ member variable with the calculated value of the RMS perdiction error
  *@param data audio frame
  *@param coeffs LPC alpha coeffs
  */
  mrs_real predictionError(const realvec& data, const realvec& coeffs);

  /*implementation from peter Kabal
     *http://www-mmsp.ece.mcgill.ca/Documents/Software/index.html
   *
   * Returns the dot product of two vectors x1 and x2 of size N
   */
  mrs_real VRfDotProd (mrs_real * x1, mrs_real * x2, mrs_natural N);

  /* Computes the Nt (order+1) reflection coefficients cor from a time serie x of size Nx
  */
  void SPautoc (mrs_real * x, mrs_natural Nx, mrs_real * cor, mrs_natural Nt);

  /* Computes the Np autocorrelation coefficients pc [a(1) ... a(order)] from the reflections coefficients rxx
     and return the prediction error
   */
  mrs_real SPcorXpc (mrs_real * rxx, mrs_real * pc, mrs_natural Np);

public:
  LPC(std::string name);
  LPC(const LPC&);
  ~LPC();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif



