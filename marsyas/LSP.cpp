
/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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
\class LSP
\brief Compute Linear Spectral Pair (LSP) coefficients 
Takes the output of ::LPCwarped() and calculates the corresponding LSP values.
See the LPCwarped class implementation.

Luís Gustavo Martins - lmartins@inescporto.pt
May 2006
*/

#include <algorithm>
#include <vector>

#include "LSP.h"
#include "NumericLib.h"
#include "MATLABengine.h"

#ifdef _MATLAB_ENGINE_
//#define _MATLAB_LSP_
#endif

using namespace std;
using namespace Marsyas;

LSP::LSP(string name):MarSystem("LSP",name)
{
	//type_ = "LSP";
	//name_ = name;

	addControls();
}

LSP::~LSP()
{
}

MarSystem* 
LSP::clone() const 
{
	return new LSP(*this);
}

void 
LSP::addControls()
{
	addctrl("mrs_natural/order", (mrs_natural)10);
	setctrlState("mrs_natural/order", true); 
	addctrl("mrs_real/gamma", (mrs_real)1.0);
}

void
LSP::localUpdate()
{ 
	MRSDIAG("LSP.cpp - LSP:localUpdate");

	order_ = getctrl("mrs_natural/order").toNatural();

	setctrl("mrs_natural/onObservations", order_);
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

	//LSP features names
	ostringstream oss;
	for (mrs_natural i = 0; i < order_; i++)
		oss << "LSP_" << i+1 << ",";
	setctrl("mrs_string/onObsNames", oss.str());
}

void 
LSP::process(realvec& in, realvec& out)
{
	NumericLib numLib;
	
	checkFlow(in,out);

	mrs_real gamma = getctrl("mrs_real/gamma").toReal();
	vector<mrs_real> ak(order_);

	if( gamma != 1.0)
		for(mrs_natural j = 0; j < order_ ; j++)
		{
			ak[j] = in(j)*pow(gamma, (double)j+1)*(-1.0);//apply pole-shifting
		}
	else
		for(mrs_natural j = 0; j < order_ ; j++)
		{
			ak[j] = in(j)*(-1.0); //no pole-shifting applied
		}

		vector<mrs_complex> P(order_+2);
		vector<mrs_complex> Q(order_+2);
		vector<mrs_complex> Proots(order_+1); 
		vector<mrs_complex> Qroots(order_+1); 

		P[order_+1] = polar(1.0, 0.0);
		Q[order_+1] = polar(1.0, 0.0);
		for(mrs_natural k = 0; k < order_; k++)
		{
			P[order_-k] = polar(ak[k] + ak[order_-1-k], 0.0);
			Q[order_-k] = polar(ak[k] - ak[order_-1-k], 0.0);
		}
		P[0] = polar(1.0, 0.0);
		Q[0] = polar(-1.0, 0.0);

		if (!numLib.polyRoots(P, false, order_+1, Proots))//P has only real coefs => complexCoefs = false
			MRSERR("LSP::process() - numerical error in polynomial root calculation!");
		if(!numLib.polyRoots(Q, false, order_+1, Qroots))//Q has only real coefs => complexCoefs = false
			MRSERR("LSP::process() - numerical error in polynomial root calculation!")

		mrs_real phase;
		vector<mrs_real> out_vec;
		for(mrs_natural k = 0; k <= order_; k++)
		{
			phase = arg(Proots[k]);
			if((phase > 0) && (phase < PI))
			{
				out_vec.push_back(phase);
			}
		}
		for(mrs_natural k = 0; k <= order_; k++)
		{
			phase = arg(Qroots[k]);
			if((phase > 0) && (phase < PI))
			{
				out_vec.push_back(phase);
			}
		}
		sort(out_vec.begin(), out_vec.end()); //sorts LSP freqs into ascending order

		//output sorted LSP frequencies
		for(mrs_natural i = 0; i < order_; i++)
			out(i) = out_vec[i];

#ifdef _MATLAB_LSP_
		MATLABengine::getMatlabEng()->putVariable(order_, "LSP_order");
		MATLABengine::getMatlabEng()->putVariable(in, "LSP_in");
		MATLABengine::getMatlabEng()->putVariable(P, "LSP_P");
		MATLABengine::getMatlabEng()->putVariable(Q, "LSP_Q");
		MATLABengine::getMatlabEng()->putVariable(Proots, "LSP_Proots");
		MATLABengine::getMatlabEng()->putVariable(Qroots, "LSP_Qroots");
		MATLABengine::getMatlabEng()->putVariable(out_vec, "LSP_out1");
		MATLABengine::getMatlabEng()->putVariable(out, "LSP_out2");
		MATLABengine::getMatlabEng()->evalString("LSP_test(LSP_order, LSP_in, LSP_P, LSP_Q, LSP_Proots, LSP_Qroots, LSP_out1, LSP_out2);");
#endif
}


