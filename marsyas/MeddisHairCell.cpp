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


/** 
    \class MeddisHairCell
    \brief MeddisHairCell for auditory models 

    Directed port from the Auditory toolbox by Malcolm Slaney 
*/

#include "MeddisHairCell.h"
using namespace std;


MeddisHairCell::MeddisHairCell(string name)
{
  type_ = "MeddisHairCell";
  name_ = name;
  
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
  addDefaultControls();
  
  addctrl("bool/subtractSpont", false);
  
}

void 
MeddisHairCell::update()
{
  MRSDIAG("MeddisHairCell.cpp - MeddisHairCell:update");
  
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations").toNatural());
  setctrl("real/osrate", getctrl("real/israte"));
  
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
  dt = 1/getctrl("real/israte").toReal();
  gdt = g*dt;
  ydt = y*dt;
  ldt = l*dt;
  rdt = r*dt;
  xdt = x*dt;

  //initial values
  kt = g*A/(A + B);
  spont = M*y*kt/(l*kt+y*(l + r));
  
  if (numChannels != getctrl("natural/inSamples").toNatural()){
    numChannels = getctrl("natural/inSamples").toNatural();
    c.create(numChannels);
    q.create(numChannels);
    w.create(numChannels);
    for (natural i = 0; i < numChannels; i++){
      c(i) = spont;
      q(i) = c(i)*(l + r)/kt;
      w(i) = c(i)*r/x;
    }
  }

  defaultUpdate();
}

void 
MeddisHairCell::process(realvec& in, realvec& out)
{
  checkFlow(in, out);
  if (mute_) return;
  
  real limitedSt;
  real replenish;
  real eject;
  real loss;
  real reuptake;
  real reprocess;
  bool subtractSpont = getctrl("bool/subtractSpont").toBool();
  for (natural j = 0; j < getctrl("natural/inSamples").toNatural(); j++){
    for (natural i = 0; i < getctrl("natural/inObservations").toNatural(); i++){
      limitedSt = max(in(i,j) + A, (real)0.0);
      kt = gdt*limitedSt/(limitedSt + B);
      replenish = max(ydt*(M - q(i)), (real)0.0);
      eject = kt*q(i);
      loss = ldt*c(i);
      reuptake = rdt*c(i);
      reprocess = xdt*w(i);
      q(i) += replenish - eject + reprocess;
      c(i) += eject - loss - reuptake;
      w(i) += reuptake - reprocess;
      out(i,j) = (subtractSpont) ? max((real)0.0, h*c(i) - spont) : h*c(i);
    }
  }
}

