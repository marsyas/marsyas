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

#include "MeddisHairCell.h"
#include "../common_source.h"
#include <algorithm>

using std::ostringstream;
using std::max;
using std::min;

using namespace Marsyas;

MeddisHairCell::MeddisHairCell(mrs_string name):MarSystem("MeddisHairCell",name)
{
  //type_ = "MeddisHairCell";
  //name_ = name;

  numChannels = 0;

  addControls();
}

MeddisHairCell::~MeddisHairCell()
{
}

MarSystem*
MeddisHairCell::clone() const
{
  return new MeddisHairCell(*this);
}

void MeddisHairCell::addControls()
{
  addctrl("mrs_bool/subtractSpont", false);
}

void
MeddisHairCell::myUpdate(MarControlPtr sender)
{
  MRSDIAG("MeddisHairCell.cpp - MeddisHairCell:myUpdate");

//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  MarSystem::myUpdate(sender); //lmartins: what about the feature names?!? [?]

  //parameters
  M = 1;
  A = 5;
  B = 300;
  g = 2000;
  y = 5.05f;
  l = 2500;
  r = 6580;
  x = 66.31f;
  h = 50000;

  //internal constants
  dt = 1/getctrl("mrs_real/israte")->to<mrs_real>();
  gdt = g*dt;
  ydt = y*dt;
  ldt = l*dt;
  rdt = r*dt;
  xdt = x*dt;

  //initial values
  kt = g*A/(A + B);
  spont = M*y*kt/(l*kt+y*(l + r));

  if (numChannels != getctrl("mrs_natural/inSamples")->to<mrs_natural>()) {
    numChannels = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
    c.create(numChannels);
    q.create(numChannels);
    w.create(numChannels);
    for (mrs_natural i = 0; i < numChannels; ++i) {
      c(i) = spont;
      q(i) = c(i)*(l + r)/kt;
      w(i) = c(i)*r/x;
    }
  }
}

void
MeddisHairCell::myProcess(realvec& in, realvec& out)
{
  checkFlow(in, out);
  //lmartins: if (mute_) return;
  if(getctrl("mrs_bool/mute")->to<mrs_bool>()) return;

  mrs_real limitedSt;
  mrs_real replenish;
  mrs_real eject;
  mrs_real loss;
  mrs_real reuptake;
  mrs_real reprocess;
  bool subtractSpont = getctrl("mrs_bool/subtractSpont")->to<mrs_bool>();
  for (mrs_natural j = 0; j < getctrl("mrs_natural/inSamples")->to<mrs_natural>(); j++) {
    for (mrs_natural i = 0; i < getctrl("mrs_natural/inObservations")->to<mrs_natural>(); ++i) {
      limitedSt = max(in(i,j) + A, (mrs_real)0.0);
      kt = gdt*limitedSt/(limitedSt + B);
      replenish = max(ydt*(M - q(i)), (mrs_real)0.0);
      eject = kt*q(i);
      loss = ldt*c(i);
      reuptake = rdt*c(i);
      reprocess = xdt*w(i);
      q(i) += replenish - eject + reprocess;
      c(i) += eject - loss - reuptake;
      w(i) += reuptake - reprocess;
      out(i,j) = (subtractSpont) ? max((mrs_real)0.0, h*c(i) - spont) : h*c(i);
    }
  }
}
