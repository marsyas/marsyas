/*
** Copyright (C) 1998-2011 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "../common_source.h"
#include "Krumhansl_key_finder.h"

using std::ostringstream;
using namespace Marsyas;

Krumhansl_key_finder::Krumhansl_key_finder(mrs_string name) : MarSystem("Krumhansl_key_finder", name)
{
  /// Add any specific controls needed by this MarSystem.
  addControls();
}

Krumhansl_key_finder::Krumhansl_key_finder(const Krumhansl_key_finder& a) : MarSystem(a)
{

  ctrl_key_ = getctrl("mrs_natural/key");
  ctrl_key_name_ = getctrl("mrs_string/key_name");

}


Krumhansl_key_finder::~Krumhansl_key_finder()
{
}

MarSystem*
Krumhansl_key_finder::clone() const
{
  return new Krumhansl_key_finder(*this);
}

void
Krumhansl_key_finder::addControls()
{
  /// Add any specific controls needed by this MarSystem.
  addctrl("mrs_natural/key", 0, ctrl_key_);
  addctrl("mrs_string/key_name", "C", ctrl_key_name_);

}


void
Krumhansl_key_finder::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Krumhansl_key_finder.cpp - Krumhansl_key_finder:myUpdate");

  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);


  major_profile_.create(12);
  minor_profile_.create(12);
  scores_.create(24);


  major_profile_(0) = 6.35;
  major_profile_(1) = 2.23;
  major_profile_(2) = 3.48;
  major_profile_(3) = 2.33;
  major_profile_(4) = 4.38;
  major_profile_(5) = 4.09;
  major_profile_(6) = 2.52;
  major_profile_(7) = 5.19;
  major_profile_(8) = 2.39;
  major_profile_(9) = 3.66;
  major_profile_(10) = 2.29;
  major_profile_(11) = 2.88;


  minor_profile_(0) = 6.33;
  minor_profile_(1) = 2.68;
  minor_profile_(2) = 3.52;
  minor_profile_(3) = 5.38;
  minor_profile_(4) = 2.6;
  minor_profile_(5) = 3.53;
  minor_profile_(6) = 2.54;
  minor_profile_(7) = 4.75;
  minor_profile_(8) = 3.98;
  minor_profile_(9) = 2.69;
  minor_profile_(10) = 3.34;
  minor_profile_(11) = 3.17;

  key_names_.push_back("A");
  key_names_.push_back("A#");
  key_names_.push_back("B");
  key_names_.push_back("C");
  key_names_.push_back("C#");
  key_names_.push_back("D");
  key_names_.push_back("D#");
  key_names_.push_back("E");
  key_names_.push_back("F");
  key_names_.push_back("F#");
  key_names_.push_back("G");
  key_names_.push_back("G#");


  key_names_.push_back("a");
  key_names_.push_back("a#");
  key_names_.push_back("b");
  key_names_.push_back("c");
  key_names_.push_back("c#");
  key_names_.push_back("d");
  key_names_.push_back("d#");
  key_names_.push_back("e");
  key_names_.push_back("f");
  key_names_.push_back("f#");
  key_names_.push_back("g");
  key_names_.push_back("g#");



}

void
Krumhansl_key_finder::myProcess(realvec& in, realvec& out)
{

  scores_.setval(0.0);

  // Correlate with each major/minor key profile
  // for (k = 0; k < 12; k++)
  // 	{
  // 		for (o = 0; o < inObservations_; o++)
  // 		{
  // 			scores_(k) += in((o+k)%12,0) * major_profile_(o);
  // 		}
  // 	}


  for (mrs_natural o = 0; o < inObservations_; o++)
    for (mrs_natural k=0; k < 12; k++)
    {
      scores_(k)    += (in((o+k)%12,0)  * major_profile_(o));
      scores_(k+12) += (in((o+k)%12,0)  * minor_profile_(o));

    }


  mrs_real max_score = 0.0;
  mrs_natural max_index = 0;

  for (mrs_natural k=0; k < 24; k++)
  {
    if (scores_(k) >= max_score)
    {
      max_score = scores_(k);
      max_index = k;
    }
  }


  ctrl_key_->setValue(max_index, NOUPDATE);
  ctrl_key_name_->setValue(key_names_[max_index], NOUPDATE);

  out.setval(0.0);
  if (max_index >= 12)
    max_index = max_index -12;
  out(max_index,0) = 1.0;
}
