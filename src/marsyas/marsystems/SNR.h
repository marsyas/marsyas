/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_SNR_H
#define MARSYAS_SNR_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Analysis
	\brief Compute SNR and variations

	Computes the SNR between two mono audio signals.
	One signal is observation 0 and the other is observation 1.

	The output is:

	observation 0 is the Signal/Noise ratio in dB

	\code
	10 * log10(\frac{\sum A^2}{\sum A-B})
	\endcode

	observation 1 is the SNR with optimized gain factor

	\code
	10 * log10(\frac{1}{1 - (\frac{sum A B}{\sqrt{\sum A^2 \sum B^2}})})
	\endcode

	Controls:
	- \b mrs_string/mode [rw] : sets the gain multiplier.
*/

class SNR: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_mode_;
  MarControlPtr ctrl_done_;
  mrs_real nsum_;
  mrs_real nbsum_;
  mrs_real  psum_;
  mrs_real dsum_;
  mrs_real diff_;
  mrs_real r_;
public:
  SNR(std::string name);
  SNR(const SNR& a);
  ~SNR();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

