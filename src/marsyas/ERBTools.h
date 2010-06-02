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
  static float Freq2ERB(float freq) {
    return 21.4f * log10(4.37f * freq / 1000.0f + 1.0f);
  }

  static float Freq2ERBw(float freq) {
    return 24.7f * (4.37f * freq / 1000.0f + 1.0f);
  }

  static float ERB2Freq(float erb) {
    return (pow(10, (erb / 21.4f)) - 1.0f) / 4.37f * 1000.0f;
  }
};
}
#endif
