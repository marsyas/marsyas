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

#ifndef MARSYAS_YIN_H
#define MARSYAS_YIN_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class Yin
	\ingroup Analysis
	\brief Pitch detection using the YIN algorithm

	This algorithm was developped by A. de Cheveigne and H. Kawahara and
	published in:

	De Cheveigne, A., Kawahara, H. (2002) "YIN, a fundamental frequency
	estimator for speech and music", J. Acoust. Soc. Am. 111, 1917-1930.

	See http://recherche.ircam.fr/equipes/pcm/pub/people/cheveign.html

	This code was adapted from aubio (http://aubio.org) by sness.

    THIS MARSYSTEM IS VERY SIMILAR TO AUBIO_YIN, BUT DO NOT DELETE
    IT BECAUSE I NEED IT.  - Graham

	Controls:
	- \b mrs_real/tolerance [w] : sets the tolerance of the yin algorithm
	- \b mrs_real/frequency_min [w] : limits the search to
      frequencies above or equal to this
	- \b mrs_real/frequency_max [w] : limits the search to
      frequencies below or equal to this (set to 0 to disable)
*/

class Yin: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  MarControlPtr ctrl_tolerance_;
  MarControlPtr ctrl_frequency_min_;
  MarControlPtr ctrl_frequency_max_;

  double aubio_quadfrac(double s0, double s1, double s2, double pf);
  double vec_quadint_min(realvec * x,unsigned int pos, unsigned int span);
  unsigned int vec_min_elem(realvec *s);
  mrs_realvec yin_buffer_realvec_;

public:
  Yin(std::string name);
  Yin(const Yin& a);
  ~Yin();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}

#endif

