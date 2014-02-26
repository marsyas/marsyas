/*
** Copyright (C) 2014 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_SIDECHAIN_INCLUDED
#define MARSYAS_SIDECHAIN_INCLUDED

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup Composites
\brief Process child and discard output.

A single child is processed with its output discarded,
and input data is passed through to output unchanged.
*/

class Sidechain: public MarSystem
{
public:
  Sidechain(std::string name);
  MarSystem* clone() const { return new Sidechain(*this); }

private:
  void myUpdate(MarControlPtr sender);
  void myProcess(realvec& in, realvec& out);

  realvec m_child_out;
};

} // namespace Marsyas

#endif // MARSYAS_SIDECHAIN_INCLUDED
