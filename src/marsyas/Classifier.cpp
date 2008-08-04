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

#include "Classifier.h"

using namespace std;
using namespace Marsyas;

Classifier::Classifier(string name):MarSystem("Classifier", name)
{
  addControls();
  classifierSwitchInitialized_ = false;
  classifierSwitch_ = NULL;
}

Classifier::Classifier(const Classifier& a) : MarSystem(a)
{
  classifierSwitchInitialized_ = false;
  classifierSwitch_ = NULL;
}


Classifier::~Classifier()
{
  // delete classifierSwitch_;
}

MarSystem*
Classifier::clone() const
{
  return new Classifier(*this);
}

void
Classifier::addControls()
{
  addctrl("mrs_string/classifierType", "ZeroRClassifier");
  addctrl("mrs_string/mode", "train");
  setctrlState("mrs_string/mode", true); 
  addctrl("mrs_natural/nClasses", 1);
  setctrlState("mrs_natural/nClasses", true);
}

void
Classifier::myUpdate(MarControlPtr sender)
{

  MRSDIAG("Classifier.cpp - Classifier:myUpdate");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

  
  if (!classifierSwitchInitialized_)
    {
      classifierSwitch_ = new Fanout("classifierSwitch");
      MarSystem* gaussianClassifier = new GaussianClassifier("gcl");
      MarSystem* zeroRClassifier = new ZeroRClassifier("zcl");      
      classifierSwitch_->addMarSystem(gaussianClassifier);
      classifierSwitch_->linkctrl("mrs_string/mode", 
				  "GaussianClassifier/gcl/mrs_string/mode");
      classifierSwitch_->linkctrl("mrs_natural/nClasses", 
				  "GaussianClassifier/gcl/mrs_natural/nClasses");
      
      classifierSwitch_->addMarSystem(zeroRClassifier);
      classifierSwitch_->linkctrl("mrs_string/mode", 
				  "ZeroRClassifier/zcl/mrs_string/mode");
      classifierSwitch_->linkctrl("mrs_natural/nClasses", 
				  "ZeroRClassifier/zcl/mrs_natural/nClasses");
      classifierSwitchInitialized_ = true;
    }

  if (classifierSwitchInitialized_) 
    {
      classifierSwitch_->setctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples"));
      classifierSwitch_->setctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations"));
      classifierSwitch_->setctrl("mrs_real/israte", getctrl("mrs_real/israte"));
      classifierSwitch_->updctrl("mrs_natural/disable", 0);      
      mode_ = getctrl("mrs_string/mode")->to<mrs_string>(); 
      cout << "mode_ = " << mode_ << endl;
      mrs_natural nClasses = getctrl("mrs_natural/nClasses")->to<mrs_natural>();
      classifierSwitch_->updctrl("mrs_string/mode", mode_);
      classifierSwitch_->updctrl("mrs_natural/nClasses", nClasses);
    }
  



}

void
Classifier::myProcess(realvec& in, realvec& out)
{
  if (classifierSwitchInitialized_) 
    {
      classifierSwitch_->process(in, out);
    }
}


