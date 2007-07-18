/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.princeton.edu>
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
    \class Summary
	\ingroup Machine

    When the mode control is set to "predict" then then classifications are tracked
	when done control is set, then the confusion matrix is shown


*/

#include "Summary.h"

using namespace std;
using namespace Marsyas;

Summary::Summary(string name):MarSystem("Summary",name)
{
  //type_ = "Summary";
  //name_ = name;

	addControls();
}


Summary::~Summary()
{
}


MarSystem* 
Summary::clone() const 
{
  return new Summary(*this);
}

void 
Summary::addControls()
{
  addctrl("mrs_string/mode", "train");
  addctrl("mrs_natural/nLabels", 1);
  setctrlState("mrs_natural/nLabels", true);
  addctrl("mrs_bool/done", false);
  addctrl("mrs_natural/prediction", 0);
  addctrl("mrs_string/labelNames", "Music,Speech");
  setctrlState("mrs_string/labelNames", true);
}

void
Summary::myUpdate(MarControlPtr sender)
{
  MRSDIAG("Summary.cpp - Summary:myUpdate");
  
  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
   
  mrs_natural nlabels = getctrl("mrs_natural/nLabels")->toNatural();

  if (confusionMatrix.getRows() != nlabels)
  {
	  //cout << "nlabels =" << nlabels<<endl;
	  confusionMatrix.create(nlabels, nlabels);  
  }
  labelNames = getctrl("mrs_string/labelNames")->toString();

}

void 
Summary::myProcess(realvec& in, realvec& out)
{
  //checkFlow(in,out);
  string mode = getctrl("mrs_string/mode")->toString();
  mrs_natural nlabels = getctrl("mrs_natural/nLabels")->toNatural();
  mrs_natural l;
  mrs_natural prediction = 0;
  
  mrs_real label;
  //cout << "mode=" << mode << endl;
  
  if (mode == "train")  
    {
      for (t=0; t < inSamples_; t++)
	{
	  label = in(inObservations_-1, t);	
	  //labelSizes_((int)label) = labelSizes_((int)label) + 1;
	  out(0,t) = label;
	  out(1,t) = label;
	}
    }


  if (mode == "predict")
    {
      for (t=0; t < inSamples_; t++)
	{    
		mrs_natural y = (mrs_natural)in(inObservations_-2, t);	  
		mrs_natural x = (mrs_natural)in(inObservations_-1, t);
	confusionMatrix(y,x)++;	  
	//cout << "(y,x) (" << y << ","<< x << ")"<< endl;
  
	out(0,t) = y;
	out(1,t) = x;
	}

    }
  
  if (getctrl("mrs_bool/done")->toBool())
    {
		if(!labelNames.size())
			labelNames = ",";
		
		string::size_type from = 0;
		string::size_type to = labelNames.find(",");
		
		mrs_natural correct = 0;
		mrs_natural total = 0;
		for(mrs_natural x = 0;x<confusionMatrix.getCols();x++)
			cout << x << "\t";
		cout << endl;
		for(mrs_natural y = 0;y<confusionMatrix.getRows();y++)
		{
			for(mrs_natural x = 0;x<confusionMatrix.getCols();x++)
			{
				mrs_natural value = (mrs_natural)confusionMatrix(y, x);
				total += value;				
				if(x == y)
					correct += value;
				cout << value << "\t";
			}
			cout << "| ";
			if(from < labelNames.size())
			{
				cout << y << " is " << labelNames.substr(from, to - from);
				from = to + 1;
				to = labelNames.find(",", from);
				if(to == string::npos)
					to = labelNames.size();
			}
			cout << endl;
		}
		
		cout << (total > 0 ? correct * 100 / total: 0) << "% classified correctly (" << correct << "/" << total << ")" << endl;
	}
}







	

	
