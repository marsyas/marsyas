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
    \class Cascade
	\ingroup Composites
    \brief Cascade of MarSystem objects

	Long description, BBB.

	Controls: none.
*/

#include "Cascade.h"

using namespace std;
using namespace Marsyas;

Cascade::Cascade(string name):MarSystem("Cascade", name)
{
	isComposite_ = true;
}

Cascade::~Cascade()
{
  deleteSlices();
}

void 
Cascade::deleteSlices()
{
  vector<realvec *>::const_iterator iter;
  for (iter = slices_.begin(); iter != slices_.end(); iter++) {
    delete *(iter);
  }
  slices_.clear();
}

MarSystem* 
Cascade::clone() const 
{
  return new Cascade(*this);
}

void 
Cascade::myUpdate(MarControlPtr sender)
{
  if (marsystemsSize_ != 0) 
	{
    marsystems_[0]->update();
    
    setctrl("mrs_natural/inSamples", marsystems_[0]->getctrl("mrs_natural/inSamples"));
    setctrl("mrs_natural/inObservations", marsystems_[0]->getctrl("mrs_natural/inObservations"));
    setctrl("mrs_real/israte", marsystems_[0]->getctrl("mrs_real/israte"));  
    
    mrs_natural onObservations = marsystems_[0]->getctrl("mrs_natural/onObservations")->toNatural();
    
    for (mrs_natural i=1; i < marsystemsSize_; i++) 
		{
      //lmartins: setctrl or updctrl?!? [?]
	  marsystems_[i]->setctrl("mrs_natural/inSamples", marsystems_[i-1]->getctrl("mrs_natural/onSamples"));
      marsystems_[i]->setctrl("mrs_natural/inObservations", marsystems_[i-1]->getctrl("mrs_natural/onObservations"));
      marsystems_[i]->setctrl("mrs_real/israte", marsystems_[i-1]->getctrl("mrs_real/osrate"));
      marsystems_[i]->update();
      onObservations += marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural();
    }
    
    setctrl("mrs_natural/onSamples", marsystems_[0]->getctrl("mrs_natural/onSamples"));
    setctrl("mrs_natural/onObservations", onObservations);
    setctrl("mrs_real/osrate", marsystems_[0]->getctrl("mrs_real/osrate"));
    
    // update buffers between components
    if ((mrs_natural)slices_.size() < marsystemsSize_) 
		{
      slices_.resize(marsystemsSize_, NULL);
    }
    
    for (mrs_natural i = 0; i < marsystemsSize_; i++) 
		{
      if (slices_[i] != NULL) 
			{
				if ((slices_[i])->getRows() != marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural() || 
						(slices_[i])->getCols() != marsystems_[i]->getctrl("mrs_natural/onSamples")->toNatural()) 
				{
					delete slices_[i];
					slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural(), marsystems_[i]->getctrl("mrs_natural/onSamples")->toNatural());
				}
      }
      else 
			{
				slices_[i] = new realvec(marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural(), marsystems_[i]->getctrl("mrs_natural/onSamples")->toNatural());
      }

      (slices_[i])->setval(0.0);
    }
		//defaultUpdate();
  }
}
      
void 
Cascade::myProcess(realvec& in, realvec& out)
{
  checkFlow(in, out);
  
  mrs_natural outIndex = 0;
  mrs_natural localIndex = 0;
  
  if (marsystemsSize_ == 1) 
	{
    marsystems_[0]->process(in, out);
  }
  else if (marsystemsSize_ > 1) 
	{
    marsystems_[0]->process(in, *(slices_[0]));
    localIndex = marsystems_[0]->getctrl("mrs_natural/onObservations")->toNatural();
    for (o = 0; o < localIndex; o++) 
		{
      for (t = 0; t < onSamples_; t++) 
			{
				out(outIndex + o,t) = (*(slices_[0]))(o,t);
      }
    }
    outIndex += localIndex;
    for (mrs_natural i = 1; i < marsystemsSize_; i++) 
		{
      marsystems_[i]->process(*(slices_[i-1]), *(slices_[i]));
      localIndex = marsystems_[i]->getctrl("mrs_natural/onObservations")->toNatural();
      for (o = 0; o < localIndex; o++) 
			{
				for (t = 0; t < onSamples_; t++) 
				{
					out(outIndex + o,t) = (*(slices_[i]))(o,t);
				}
      }
      outIndex += localIndex;
    }
  }
}

