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
    \class Shredder 

    \brief Split the output of an accumulator

    Split the result an accumulator. Spit several outputs when
    the input is recieved. Used to restore
    the rate of process requests initially change by an accumulator
*/

#include "Shredder.h"

using namespace std;
using namespace Marsyas;
 
Shredder::Shredder(string name):Composite("Shredder", name)
{
  //type_ = "Shredder";
  //name_ = name;
  
  marsystemsSize_ = 0;
  addControls();
}

Shredder::~Shredder()
{
}

MarSystem* 
Shredder::clone() const 
{
  return new Shredder(*this);
}

void 
Shredder::addControls()
{
  addctrl("mrs_natural/nTimes", 5);
  setctrlState("mrs_natural/nTimes", true);
  nTimes_ = 5;
}

void
Shredder::myUpdate()
{
  MRSDIAG("Shredder.cpp - Shredder:myUpdate");
  
  nTimes_ = getctrl("mrs_natural/nTimes")->toNatural();

	// update dataflow component MarSystems in order 
	if (marsystemsSize_ > 0)
	{
		marsystems_[0]->update(); 

		// set input characteristics 
		setctrl("mrs_natural/inSamples", 
			marsystems_[0]->getctrl("mrs_natural/inSamples")->toNatural() * nTimes_);
		setctrl("mrs_natural/inObservations", 
			marsystems_[0]->getctrl("mrs_natural/inObservations"));
		setctrl("mrs_real/israte", 
			marsystems_[0]->getctrl("mrs_real/israte")->toNatural());

		// set output characteristics 
		setctrl("mrs_natural/onSamples", 
			marsystems_[0]->getctrl("mrs_natural/onSamples")->toNatural());
		setctrl("mrs_natural/onObservations", 
			marsystems_[0]->getctrl("mrs_natural/onObservations")->toNatural());
		setctrl("mrs_real/osrate", 
			marsystems_[0]->getctrl("mrs_real/osrate"));
		setctrl("mrs_string/onObsNames", 
			marsystems_[0]->getctrl("mrs_string/onObsNames"));
	}

  tin_.create(marsystems_[0]->getctrl("mrs_natural/inObservations")->toNatural(), 
		marsystems_[0]->getctrl("mrs_natural/inSamples")->toNatural());
}

void
Shredder::updControl(std::string cname, MarControlPtr control)
{
	// remove prefix for synonyms
	string::size_type pos = cname.find(prefix_, 0);
	string shortcname;
	if (pos == 0)
		shortcname = cname.substr(prefix_.length(), cname.length());//no leading "/", as expected for links [!]
	// check for synonyms - call recursively to resolve them 
	map<string, vector<string> >::iterator ei;
	ei = synonyms_.find(shortcname);
	if (ei != synonyms_.end())
	{
		vector<string> synonymList = synonyms_[shortcname];
		vector<string>::iterator si;
		for (si = synonymList.begin(); si != synonymList.end(); ++si)
		{
			updControl(prefix_ + *si, control);
		}
	}
	else
	{
		string childcontrol = cname.substr(prefix_.length()-1, cname.length()-(prefix_.length()-1));//includes leading "/" [!]
		string nchildcontrol = childcontrol.substr(1, childcontrol.length()); //no leading "/" [!]  
		bool controlFound = false;

		// check local controls 
		if (hasControlLocal(cname))
		{
			controlFound = true;
			MarControlPtr oldval = getControl(cname);
			setControl(cname, control);
			if (hasControlState(cname) && (control != oldval)) 
			{
				update(); //update composite
			}
		}
		//shredder specific! [!]
		if((nchildcontrol == "mrs_natural/inSamples"))
		{
			if (marsystemsSize_ > 0)
			{
				mrs_natural val = control->toNatural() / nTimes_;
				marsystems_[0]->updctrl(nchildcontrol, val);//[!]shred input realvec into smaller ones! 
			}
			update();
			return;
		}
		// lmartins: should find a way to avoid this hard-coded check... [!]
		// default controls - semantics of composites 
		if ((nchildcontrol == "mrs_natural/inObservations")||
			(nchildcontrol == "mrs_real/israte")||
			(nchildcontrol == "mrs_string/inObsNames"))
		{
			if (marsystemsSize_ > 0)
			{
				marsystems_[0]->updctrl(nchildcontrol, control);
			}
			update();
			return;
		}
		else//if(!hasControlLocal(cname)) 
		{
			//if control control is not from composite,
			//check if it exists among the children composites and update them
			for (mrs_natural i=0; i< marsystemsSize_; i++)
			{
				if (marsystems_[i]->hasControl(childcontrol))
				{
					controlFound = true;
					marsystems_[i]->updControl(childcontrol, control); //updcontrol or setcontrol?! [!]
					if (marsystems_[i]->hasControlState(childcontrol))
					{
						update();
					}
				}
			}
		}

		if (!controlFound) 
		{
			MRSWARN("Shredder::updControl - Unsupported control name = " + cname);
			MRSWARN("Shredder::updControl - Composite name = " + getName());
		}
	}


}

void 
Shredder::myProcess(realvec& in, realvec& out)
{
	//checkFlow(in,out);

	for (c = 0; c < nTimes_; c++) 
	{
		for (o=0; o < inObservations_; o++)
			for (t = 0; t < inSamples_/nTimes_; t++)
			{
				tin_(o,t) = in(o, t + c * (inSamples_/nTimes_)) ;
			}
		marsystems_[0]->recvControls(); // HACK STU
		marsystems_[0]->process(tin_, out);
		
	}
	/*
	MATLAB_PUT(in, "Schredder_in");
	MATLAB_PUT(out, "Schredder_out");
	MATLAB_EVAL("figure(1);imagesc(Schredder_in(1:2:end, :))");
	MATLAB_EVAL("figure(2);plot(Schredder_out)");
	*/
}







	

	
