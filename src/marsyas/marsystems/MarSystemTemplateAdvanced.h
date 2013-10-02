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

#ifndef MARSYAS_MARSYSTEMTEMPLATEADVANCED_H
#define MARSYAS_MARSYSTEMTEMPLATEADVANCED_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class MarSystemTemplateAdvanced
	\ingroup Special
	\brief Advanced example on how to use composite ad custom controls efficiently in MarSystems
*/

// declare a new custom control to be used by
// this MarSystem (e.g. a header for a file)
struct MyHeader
{
  std::string someString;
  mrs_natural someValue;
  bool someFlag;
  realvec someVec;

  // some operators are mandatory for all controls!
  // so we must declare and define them for our custom controls
  marsyas_EXPORT friend bool operator==(const MyHeader& hdr1, const MyHeader& hdr2);
  marsyas_EXPORT friend bool operator!=(const MyHeader& hdr1, const MyHeader& hdr2);
  marsyas_EXPORT friend MyHeader operator+(MyHeader& hdr1, MyHeader& hdr2);
  marsyas_EXPORT friend MyHeader operator-(MyHeader& hdr1, MyHeader& hdr2);
  marsyas_EXPORT friend MyHeader operator*(MyHeader& hdr1, MyHeader& hdr2);
  marsyas_EXPORT friend MyHeader operator/(MyHeader& hdr1, MyHeader& hdr2);
  marsyas_EXPORT friend std::ostream& operator<<(std::ostream& os, const MyHeader& hdr);
  marsyas_EXPORT friend std::istream& operator>>(std::istream& is, MyHeader& hdr);
};

// alias to avoid excessive verbosity (see addControls() definition in .cpp)
typedef MarControlValueT<MyHeader> MyHeaderT;

class marsyas_EXPORT MarSystemTemplateAdvanced: public MarSystem
{
private:
  std::string someString_;

  //Add specific controls needed by this MarSystem.
  void addControls();

  //"Pointers" to controls allow efficient access to their values.
  //(for clarity sake, we use the ctrl_ prefix so these "pointers"
  //can be easily identified through out the code.. but this is not
  //mandatory, just recommended)
  MarControlPtr ctrl_header_;

  void myUpdate(MarControlPtr sender);

public:
  MarSystemTemplateAdvanced(std::string name);
  MarSystemTemplateAdvanced(const MarSystemTemplateAdvanced& a);//copy ctor
  ~MarSystemTemplateAdvanced();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif //MARSYAS_MARSYSTEMTEMPLATEADVANCED_H

