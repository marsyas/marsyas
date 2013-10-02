/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include <marsyas/system/MarControlManager.h>
#include "MarSystemTemplateAdvanced.h"
#include "../common_source.h"

using std::ostringstream;
using std::ofstream;
using std::endl;
using std::cout;
using std::ios;

using namespace Marsyas;

MarSystemTemplateAdvanced::MarSystemTemplateAdvanced(mrs_string name):MarSystem("MarSystemTemplateAdvanced", name)
{
  // Add any specific controls needed by this MarSystem
  // (default controls all MarSystems should have
  // were already added by MarSystem::addControl(),
  // called by :MarSystem(name) constructor).
  // If no specific controls are needed by a MarSystem
  // there is no need to implement and call this addControl()
  // method (see for e.g. Rms.cpp)
  addControls();
}

MarSystemTemplateAdvanced::MarSystemTemplateAdvanced(const MarSystemTemplateAdvanced& a) : MarSystem(a)
{
  // IMPORTANT!
  // All member "pointers" to controls have to be
  // explicitly reassigned here, at the copy ctor.
  // Otherwise these member "pointers" would be invalid!
  ctrl_header_ = getctrl("mrs_myheader/hdrname");
}

MarSystemTemplateAdvanced::~MarSystemTemplateAdvanced()
{
}

MarSystem*
MarSystemTemplateAdvanced::clone() const
{
  return new MarSystemTemplateAdvanced(*this);
}

void
MarSystemTemplateAdvanced::addControls()
{
  someString_ = "";

  // register new custom control in MarControlManager
  MarControlManager *mcm = MarControlManager::getManager();
  if (!mcm->isRegistered("mrs_myheader"))
  {
    mcm->registerPrototype("mrs_myheader", new MyHeaderT());
    // if we do not define a typedef for our custom control
    // (see .h), we must then use a equivalent, but somewhat
    // more verbose call:
    //
    // mcm->registerPrototype("mrs_myheader", new MarControlValueT<MyHeader>());
  }

  // Ask MarControlManager to create a new instance
  // of our "custom" myHeader control
  ctrl_header_ = mcm->create("mrs_myheader");

  // create a temporary header and fill it
  // with the desired parameter values
  MyHeader myh;
  myh.someString = "abcd";
  myh.someValue = 50;
  myh.someFlag = true;
  myh.someVec.create(10);

  // Alternatively, we could ask our newly created control
  // for a copy of it that we can then use for setting its parameters
  // (no advantage to get a reference, because it would const...)
  // 	MyHeader myh = ctrl_header_->to<MyHeader>();
  // 	myh.someString = "abcd";
  // 	myh.someValue = 50;
  // 	myh.someFlag = true;
  // 	myh.someVec.create(10);

  // use the temporary header object to set
  // our custom control values
  ctrl_header_->setValue(myh);

  // finally add it as control to this MarSystem, and remember
  // to register its member variable (third parameter)
  addctrl("mrs_myheader/hdrname", ctrl_header_, ctrl_header_);

  // it's also possible to set a custom control with state!
  ctrl_header_->setState(true);
}

void
MarSystemTemplateAdvanced::myUpdate(MarControlPtr sender)
{
  MRSDIAG("MarSystemTemplateAdvanced.cpp - MarSystemTemplateAdvanced:myUpdate");

  MarSystem::myUpdate(sender);

  //get a reference to our custom control
  const MyHeader& hdr = ctrl_header_->to<MyHeader>();

  //e.g. write header to some place (e.g. a file)
  if(someString_ != hdr.someString)
  {
    ofstream out(hdr.someString.c_str());
    out << hdr;
    someString_ = hdr.someString;
  }
}

void
MarSystemTemplateAdvanced::myProcess(realvec& in, realvec& out)
{
  mrs_natural t,o;
  //get a reference to our custom control
  const MyHeader& hdr = ctrl_header_->to<MyHeader>();

  //reopen (for append) our example output file,
  //to which we already have written the header in localUpdate()
  ofstream outfile(hdr.someString.c_str(), ios::app);

  // This is a dummy example where all input data is
  // just passed to its output unchanged, and simultaneously
  // written to the output file...
  for (o=0; o < inObservations_; o++)
    for (t = 0; t < inSamples_; t++)
    {
      out(o,t) = in(o,t);
      outfile << in(o,t) << endl;
    }
}
/************************************************************************/
/* Custom Control Operators                                                                     */
/************************************************************************/
// some operators are mandatory for all controls!
// so we must declare and define them for our custom controls

// Use a namespace block instead of "Marsyas::", or Doxygen will complain.
namespace Marsyas {

bool
operator==(const MyHeader& hdr1, const MyHeader& hdr2)
{
  // here we consider that two headers are equal if all their
  // parameter values are equal
  return (hdr1.someString == hdr2.someString &&
          hdr1.someValue == hdr2.someValue &&
          hdr1.someFlag == hdr2.someFlag &&
          hdr1.someVec == hdr2.someVec)	;
}

bool
operator!=(const MyHeader& hdr1, const MyHeader& hdr2)
{
  // here we consider that two headers are equal if all their
  // parameter values are equal
  return (hdr1.someString != hdr2.someString ||
          hdr1.someValue != hdr2.someValue ||
          hdr1.someFlag != hdr2.someFlag ||
          hdr1.someVec != hdr2.someVec)	;
}

MyHeader
operator+(MyHeader& hdr1, MyHeader& hdr2)
{
  (void) hdr1; (void) hdr2;
  MRSASSERT(0);	// not a valid operation for this example header control
  return MyHeader();
}

MyHeader
operator-(MyHeader& hdr1, MyHeader& hdr2)
{
  (void) hdr1; (void) hdr2;
  MRSASSERT(0);	// not a valid operation for this example header control
  return MyHeader();
}

MyHeader
operator*(MyHeader& hdr1, MyHeader& hdr2)
{
  (void) hdr1; (void) hdr2;
  MRSASSERT(0);	// not a valid operation for this example header control
  return MyHeader();
}

MyHeader
operator/(MyHeader& hdr1, MyHeader& hdr2)
{
  (void) hdr1; (void) hdr2;
  MRSASSERT(0);	// not a valid operation for this example header control
  return MyHeader();
}

std::ostream&
operator<<(std::ostream& os, const MyHeader& hdr)
{
  os << "# MARSYAS mrs_myHeader" << endl;
  os << "# someString = " << hdr.someString << endl;
  os << "# someValue = " << hdr.someValue << endl;
  os << "# someFlag = " << hdr.someFlag << endl;
  os << "# someVec = " << hdr.someVec << endl;

  return os;
}

std::istream&
operator>>(std::istream& is, MyHeader& hdr)
{
  mrs_string skip;
  is >> skip >> skip >> skip;

  if(skip != "mrs_myHeader")
  {
    MRSWARN("MyHeader::operator>> error reading stream");
    return is;
  }

  is >> skip >> skip >> skip;
  is >> hdr.someString;

  is >> skip >> skip >> skip;
  is >> hdr.someValue;

  is >> skip >> skip >> skip;
  is >> hdr.someFlag;

  is >> skip >> skip >> skip;
  is >> hdr.someVec;

  return is;
}

} // namespace Marsyas
