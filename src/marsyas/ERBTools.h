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

#ifndef MARSYAS_ERBTOOLS_H
#define MARSYAS_ERBTOOLS_H

namespace Marsyas
{

//
// From ERBTools.h in AIMC
//
class ERBTools {
public:
  static double Freq2ERB(double freq) {
    return 21.4 * log10(4.37 * freq / 1000.0 + 1.0);
  }

  static double Freq2ERBw(double freq) {
    return 24.7 * (4.37 * freq / 1000.0 + 1.0);
  }

  static double ERB2Freq(double erb) {
    return (pow(10, (erb / 21.4)) - 1.0) / 4.37 * 1000.0;
  }
};
}
#endif
