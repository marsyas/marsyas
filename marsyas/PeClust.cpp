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
    \class PeClust
    \brief Cluster peaks from peConvert

  
*/

#include "PeClust.h"

#ifdef _MATLAB_ENGINE_
#include "MATLABengine.h"
#endif 

using namespace std;
using namespace Marsyas;

PeClust::PeClust(string name):MarSystem("PeClust", name)
{
 
	addControls();
}


PeClust::~PeClust()
{
}

MarSystem* 
PeClust::clone() const 
{
  return new PeClust(*this);
}

void 
PeClust::addControls()
{
  //Add specific controls needed by this MarSystem.
  addctrl("mrs_numerical/nbClust", 2);
}

// void
// PeClust::localUpdate()
// {
//   
// lmartins: since this is the default MarSystem::localUpdate()
// (i.e. does not alters input data format) it's not needed to
// override it here!
// see also Limiter.cpp for another example
//   
//   MRSDIAG("PeClust.cpp - PeClust:localUpdate");
//   
//   setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
//   setctrl("mrs_natural/onObservations", getctrl("mrs_natural/inObservations"));
//   setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
//   setctrl("mrs_string/onObsNames", getctrl("mrs_string/inObsNames"));
//   
// 	//defaultUpdate(); no longer needed here. Done at MarSystem::update()  
//}



void PeClust::peaks2M (realvec in, realvec out)
{
int i,j;

for (i=0 ; i<inObservations_ ; i++)
for (j=0 ; j<inSamples_ ; j++)
{

}
}

void PeClust::peaks2V (realvec in, realvec out)
	{

	}


void 
PeClust::process(realvec& in, realvec& out)
{
  checkFlow(in,out);

  mrs_natural nbClusters_ = getctrl("mrs_natural/nbClusters").toNatural();
  
  peaks2M(in, data_);

	peaks2V(data_, out);
}







	
