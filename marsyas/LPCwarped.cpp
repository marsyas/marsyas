
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
\class LPCwarped
\brief Compute Warped LPC coefficients, Pitch and Power [STILL UNDER TESTING!].

Linear Prediction Coefficients (LPC). Features commonly used 
in Speech Recognition research. This class is a modification of the original
Marsyas0.1 LPC class.  The following differences apply: 
- order now reflects the LPC order (and returns <order> coefficients plus pitch and gain)
- It is possible to define a pole-shifting parameter, gamma (default value = 1.0 => no shifting)
- It is possible to define a warping factor, lambda (defualt value = 0.0 => no warping)

Luís Gustavo Martins - lmartins@inescporto.pt
May 2006
*/

#include "LPCwarped.h"
#include "MATLABengine.h"

using namespace std;
using namespace Marsyas;

#ifdef _MATLAB_ENGINE_
//#define _MATLAB_LPCwarped_
#endif

LPCwarped::LPCwarped(string name)
{
	type_ = "LPCwarped";
	name_ = name;
	
	addControls();
}

LPCwarped::~LPCwarped()
{
}

MarSystem* 
LPCwarped::clone() const 
{
	return new LPCwarped(*this);
}

void 
LPCwarped::addControls()
{
	addDefaultControls();

	addctrl("mrs_natural/order", (mrs_natural)10); 
	setctrlState("mrs_natural/order", true); 
	addctrl("mrs_real/lambda", (mrs_real)0.0);	
	addctrl("mrs_real/gamma", (mrs_real)1.0);
}

void
LPCwarped::update()
{ 
	MRSDIAG("LPCwarped.cpp - LPCwarped:update");

	order_ = getctrl("mrs_natural/order").toNatural();

	setctrl("mrs_natural/onObservations", (mrs_natural)(order_+2)); // <order_> coefs + pitch value + power value
	setctrl("mrs_natural/onSamples", (mrs_natural)1);
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

	//LPCwarped features names
	ostringstream oss;
	for (mrs_natural i = 0; i < order_; i++)
		oss << "LPCwarped_" << i+1 << ",";
	oss << "LPCwarped_Pitch," << "LPCwarped_Gain,";
	setctrl("mrs_string/onObsNames", oss.str());

	temp_.create(order_ ,order_);
	Zs_.create(order_);

	defaultUpdate();

	//MATLAB engine stuff - for testing and validation purposes only!
#ifdef _MATLAB_LPCwarped_
	MATLABengine::getMatlabEng()->evalString("LPCwarped_pitch = [];");
#endif

}

//perhaps this method could become an independent MarSystem in the future...
void
LPCwarped::autocorrelationWarped(realvec& in, realvec& r, mrs_real lambda) 
{
	//*Based on the code from: http://www.musicdsp.org/showone.php?id=137

	//find the order-P autocorrelation array, R, for the sequence x 
	//of length L and using a warping factor of lambda

	mrs_real* x = in.getData();
	mrs_natural L = in.getSize();
	mrs_real* R = r.getData();
	mrs_natural P = in.getSize()/2;//order_;

	mrs_real* dl = new mrs_real[L];
	mrs_real* Rt = new mrs_real[L];
	mrs_real r1,r2,r1t;
	R[0]=0;
	Rt[0]=0;
	r1=0;
	r2=0;
	r1t=0;
	for(mrs_natural k=0; k<L;k++)
	{
		Rt[0]+=x[k]*x[k];

		dl[k]=r1-lambda*(x[k]-r2);
		r1 = x[k];
		r2 = dl[k];
	}
	for(mrs_natural i=1; i<=P; i++)
	{
		Rt[i]=0;
		r1=0;
		r2=0;
		for(mrs_natural k=0; k<L;k++)
		{
			Rt[i]+=dl[k]*x[k];

			r1t = dl[k];
			dl[k]=r1-lambda*(r1t-r2);
			r1 = r1t;
			r2 = dl[k];
		}
	}

	for(long i=0; i<=P; i++)
		R[i]=Rt[i];

	delete[] dl;
	delete[] Rt;

	//----------------------------------------------------
	// estimate pitch 
	//----------------------------------------------------
	mrs_real temp = r(0);
	//set peak searching start point to 2% of total window size [?]
	mrs_natural j = (mrs_natural)(in.getSize() * 0.02); 
	//detect first local minimum...
	while (r(j) < temp && j < in.getSize()/2)
	{
		temp = r(j);
		j++;
	}
	//... and then from there, detect higher peak => period estimate!
	temp = 0.0;
	for (mrs_natural i=j; i < in.getSize() * 0.5; i++)
	{
		if (r(i) > temp) 
		{
			j = i;
			temp = r(i);
		}
	}

	// This code is from the original Marsyas0.1 AutoCorrelation class
	//this seems like some sort of Narrowed Autocorrelation (NAC)... [?][!]
	//however, it does not seem to fit the NAC definition...
	//so, what is this for??
// 	mrs_real norm = 1.0 / (mrs_real)in.getSize();
// 	mrs_natural k = in.getSize()/2;
// 	for (mrs_natural i=0; i < in.getSize()/2; i++) 
// 	r(i) *= (k-i) * norm;

	//if autocorr peak not very prominent => not a good period estimate
	//so discard it...
	if ((r(j) / r(0)) < 0.4) j=0;
	//avoid detection of too high fundamental freqs (e.g. harmonics)?
	if (j > in.getSize()/4) j = 0;
	
	//pitch_ gets in fact the period (i.e. autocorr lag)
	//measured in time samples... maybe this should be converted
	//to a more convenient representation (e.g. pitch in Hz).
	//Such a conversion would have to depend on the warp factor lambda... [!]
	pitch_  = (mrs_real)j;

//MATLAB engine stuff - for testing and validation purposes only!
#ifdef _MATLAB_LPCwarped_
	MATLABengine::getMatlabEng()->putVariable(pitch_, "pitch");
	MATLABengine::getMatlabEng()->evalString("LPCwarped_pitch = [LPCwarped_pitch pitch];");
#endif
}

//Perhaps this method could become a member of realvec...
void
LPCwarped::LevinsonDurbin(realvec& r, realvec& a, realvec& k)
{
	//*Based on the code from: http://www.musicdsp.org/showone.php?id=137
	
	mrs_natural P = order_;
	mrs_real* R = r.getData();
	mrs_real* A = a.getData();
	mrs_real* K = k.getData();

	mrs_real Am1[62];

	if(R[0]==0.0) {
		for(mrs_natural i=1; i<=P; i++)
		{
			K[i]=0.0;
			A[i]=0.0;
		}}
	else {
		mrs_real km,Em1,Em;
		mrs_natural k,s,m;

		for (k=0;k<=P;k++){
			A[k]=0;
			Am1[k]=0; }

		A[0]=1;
		Am1[0]=1;
		km=0;
		Em1=R[0];

		for (m=1;m<=P;m++)                //m=2:N+1
		{
			mrs_real err=0.0f;                //err = 0;

			for (k=1;k<=m-1;k++)          //for k=2:m-1
				err += Am1[k]*R[m-k];     // err = err + am1(k)*R(m-k+1);

			km = (R[m]-err)/Em1;          //km=(R(m)-err)/Em1;
			K[m-1] = -km;
			A[m]=km;                      //am(m)=km;

			for (k=1;k<=m-1;k++)          //for k=2:m-1
				A[k]=Am1[k]-km*Am1[m-k];  // am(k)=am1(k)-km*am1(m-k+1);

			Em=(1-km*km)*Em1;             //Em=(1-km*km)*Em1;

			for(s=0;s<=P;s++)             //for s=1:N+1
				Am1[s] = A[s];            // am1(s) = am(s)

			Em1 = Em;                     //Em1 = Em;
		}
	}
}

void 
LPCwarped::predictionError(realvec& data, realvec& coeffs)
{
	mrs_natural i,j;
	mrs_real power = 0.0;
	mrs_real error, tmp;

	//load delay line with input data...
	for (i=0; i< order_; i++) 
	{
		Zs_(i) = data(order_-i-1);
	}
	//apply LPC filter and estimate RMS of the error (=~ LPC Gain)
	mrs_natural count = 0;
	for (i=order_; i<data.getSize() ; i++)
	{
		tmp = 0.0;
		for (j=0; j< order_; j++) tmp += Zs_(j) * coeffs(j);
		for (j=order_-1; j>0; j--) Zs_(j) = Zs_(j-1);
		Zs_(0) = data(i);
		error = data(i) - tmp;
		power += error * error;
		count ++;
	}
	power_ = sqrt(power/(mrs_real)count);//=~LPC Gain
}

void 
LPCwarped::process(realvec& in, realvec& out)
{
	checkFlow(in,out); 

	mrs_natural i;

//MATLAB engine stuff - for testing and validation purposes only!
#ifdef _MATLAB_LPCwarped_
	MATLABengine::getMatlabEng()->putVariable(in, "LPC_in");
	MATLABengine::getMatlabEng()->putVariable(order_, "LPC_order");
	MATLABengine::getMatlabEng()->putVariable(getctrl("mrs_real/gamma").toReal(), "LPC_gamma");
#endif

	//-------------------------
	// warped autocorrelation
	//-------------------------
	//calculate warped autocorrelation coefs
	realvec r(in.getSize());
	//this also estimates the pitch_ - does it work if lambda != 0 [?]
	autocorrelationWarped(in, r, getctrl("mrs_real/lambda").toReal()); 

	//--------------------------
	// Levison-Durbin recursion
	//--------------------------
	//Calculate LPC alpha and reflections coefs
	//using Levison-Durbin recursion
	realvec a(order_+1);
	realvec k(order_+1); //reflection coefs
	LevinsonDurbin(r, a, k);

	//--------------------------
	// LPC coeffs
	//--------------------------
	//output LPC coefs
	// y(n) = x(n) - a(1)x(n-1) - ... - a(order_-1)x(n-order_-1)
	// a = [1 a(1) a(2) ... a(order_-1)]
	// out = [a(1) a(2) ... a(order_-1)]
	for(i=0; i < order_; i++)
		out(i) = a(i+1);

	//--------------------------
	// LPC Pole-shifting
	//--------------------------
	//verify if Z-Plane pole-shifting should be performed...
	mrs_real gamma = getctrl("mrs_real/gamma").toReal();
	if(gamma != 1.0)
		for(mrs_natural j = 0; j < order_; j++)
			out(j) = (out(j) * pow(gamma, (double)j+1));

	//---------------------------
	// RMS Prediction Error
	//---------------------------
	//calculate RMS prediction error of LPC model (=> power_)
	predictionError(in, out);

	//------------------------------
	//output pitch and power values
	//------------------------------
	out(order_) = pitch_; // lag in samples <= from ::autocorrelationWarped(...) - does it work if lambda != 0 [?]
	out(order_+1) = power_; // RMS prediction error <= from ::predictionError(...)

//MATLAB engine stuff - for testing and validation purposes only!
#ifdef _MATLAB_LPCwarped_
	MATLABengine::getMatlabEng()->putVariable(out, "LPC_out");
	MATLABengine::getMatlabEng()->evalString("LPC_test");
#endif

}


