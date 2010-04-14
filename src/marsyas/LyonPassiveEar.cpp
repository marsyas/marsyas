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


#include "LyonPassiveEar.h"
#include "Series.h"
#include "Filter.h"
#include "Cascade.h"
#include "basis.h"
#include <sstream>

using namespace std;
using namespace Marsyas;

mrs_realvec LyonPassiveEar::lyonSecondOrderFilter (mrs_real midFreq, mrs_real q, mrs_real sRate)
{
	//function filts = SecondOrderFilter(f,q,fs)
	//cft = f'/fs;
	//rho = exp(- pi * cft ./ q');
	//		  theta = 2 * pi * cft .* sqrt(1-1 ./(4*q'.^2));
	//		  filts = [ ones(size(rho)) -2*rho.*cos(theta) rho.^2 ];
	mrs_realvec	result (3);
	mrs_real cft	= midFreq / sRate,
			rho		= exp (-PI * cft / q);

	result(0)	= 1;
	result(1)	= -2 * rho * cos (TWOPI * cft * sqrt (1 - 1.0/(4*sqr(q))));
	result(2)	= sqr(rho);
	return result;
}

mrs_real LyonPassiveEar::lyonFreqResp (mrs_realvec firCoeffs, mrs_realvec iirCoeffs, mrs_real freq, mrs_real sRate, mrs_bool inDb)
{
	//function mag=FreqResp(filter,f,fs)
	//	cf = exp(i*2*pi*f/fs);
	//mag = (filter(3) + filter(2)*cf + filter(1)*cf.^2) ./ ...
	//	(filter(5) + filter(4)*cf + cf.^2);
	//mag = 20*log10(abs(mag));
	mrs_complex cf	= mrs_complex (cos (TWOPI * freq / sRate), sin (TWOPI * freq / sRate));
	mrs_complex mag	= (firCoeffs(2) + firCoeffs(1) * cf + firCoeffs(0) * (cf*cf)) / (iirCoeffs(2) + iirCoeffs(1) * cf + (cf*cf));
	mrs_real	res = sqrt (sqr(mag.real ()) + sqr (mag.imag ()));

	return inDb? 20.0/log(10.0) * log (res) : res;
}

mrs_real LyonPassiveEar::lyonSetGain (mrs_realvec firCoeffs, mrs_realvec iirCoeffs, mrs_real newGain, mrs_real freq, mrs_real sRate)
{
	//function filter = SetGain(filter, desired, f, fs)
	//	oldGain = 10^(FreqResp(filter, f, fs)/20);
	//filter(1:3) = filter(1:3)*desired/oldGain;

	return newGain / lyonFreqResp (firCoeffs, iirCoeffs, freq, sRate, false);
}

Filter*		LyonPassiveEar::lyonCreateFilter (mrs_realvec b, mrs_realvec a, mrs_string name)
{
	Filter *filter = new Filter(name);
	filter->setctrl("mrs_realvec/ncoeffs", b);
	filter->setctrl("mrs_realvec/dcoeffs", a);

	return filter;
}


LyonPassiveEar::LyonPassiveEar(string name):MarSystem("LyonPassiveEar", name),
filterBank_ (0),
numFilterChannels_ (0),
fs_ (0)
{
	//type_ = "LyonPassiveEar";
	//name_ = name;

	addControls();
}


LyonPassiveEar::~LyonPassiveEar()
{
	if (filterBank_)
	{
		delete filterBank_;
		filterBank_	= 0;
	}
}

MarSystem* 
LyonPassiveEar::clone() const 
{
	// not sure this will work without calling myUpdate - probably not...
	return new LyonPassiveEar(*this);
}


void 
LyonPassiveEar::addControls()
{
	addctrl("mrs_natural/decimFactor", 1);
	addctrl("mrs_real/earQ", 8.0F);
	addctrl("mrs_real/stepFactor", 0.25F);
	addctrl("mrs_bool/channelDiffActive", true);
	addctrl("mrs_bool/agcActive", true);
	addctrl("mrs_real/decimTauFactor", 3.0F);

	addctrl("mrs_realvec/centerFreqs", centerFreqs_);

	setctrlState("mrs_natural/decimFactor", true);
	setctrlState("mrs_real/earQ", true);
	setctrlState("mrs_real/stepFactor", true);
	setctrlState("mrs_bool/channelDiffActive", true);
	setctrlState("mrs_bool/agcActive", true);
	setctrlState("mrs_real/decimTauFactor", true);

	setctrlState("mrs_realvec/centerFreqs", false);
}

mrs_bool LyonPassiveEar::setParametersIntern ()
{
	mrs_bool	updateMe			= false;

	updateMe	|= (filterBank_ == 0);

	// update controls
	if (decimFactor_		!= getctrl ("mrs_natural/decimFactor")->to<mrs_natural>())
	{
		updateMe			= true;
		decimFactor_		= getctrl ("mrs_natural/decimFactor")->to<mrs_natural>();
	}
	if (earQ_				!= getctrl ("mrs_real/earQ")->to<mrs_real>())
	{
		updateMe			= true;
		earQ_				= getctrl ("mrs_real/earQ")->to<mrs_real>();
	}
	if (stepFactor_			!= getctrl ("mrs_real/stepFactor")->to<mrs_real>())
	{
		updateMe			= true;
		stepFactor_			= getctrl ("mrs_real/stepFactor")->to<mrs_real>();
	}
	if (channelDiffActive_	!= getctrl ("mrs_bool/channelDiffActive")->to<mrs_bool>())
	{
		updateMe			= true;
		channelDiffActive_	= getctrl ("mrs_bool/channelDiffActive")->to<mrs_bool>();
	}
	if (agcActive_			!= getctrl ("mrs_bool/agcActive")->to<mrs_bool>())
	{
		updateMe			= true;
		agcActive_			= getctrl ("mrs_bool/agcActive")->to<mrs_bool>();
	}
	if (decimTauFactor_		!= getctrl ("mrs_real/decimTauFactor")->to<mrs_real>())
	{
		updateMe			= true;
		decimTauFactor_		= getctrl ("mrs_real/decimTauFactor")->to<mrs_real>();
	}
	if (fs_					!= getctrl ("mrs_real/israte")->to<mrs_real>())
	{
		updateMe			= true;
		fs_					= getctrl ("mrs_real/israte")->to<mrs_real>();
	}

	return updateMe;
}

void LyonPassiveEar::updateControlsIntern ()
{
	setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
	setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));

	filterBank_->updctrl("mrs_natural/inObservations", getctrl("mrs_natural/inObservations")->to<mrs_natural>());
	filterBank_->updctrl("mrs_natural/inSamples", getctrl("mrs_natural/inSamples")->to<mrs_natural>());
	filterBank_->updctrl("mrs_natural/onSamples", getctrl("mrs_natural/onSamples")->to<mrs_natural>());
	//filterBank_->updctrl("mrs_real/israte", getctrl("mrs_real/israte")->to<mrs_real>());

	if (numFilterChannels_)
	{
		updctrl ("mrs_realvec/centerFreqs", centerFreqs_);
		setctrl("mrs_natural/onObservations", numFilterChannels_*getctrl("mrs_natural/inObservations")->to<mrs_natural>());
		filterBank_->setctrl("mrs_natural/onObservations", getctrl("mrs_natural/onObservations")->to<mrs_natural>());
	}
}

void 
LyonPassiveEar::myUpdate(MarControlPtr sender)
{
	(void) sender;
	MarSystem::myUpdate (sender);
	
	//(void) sender;
	MRSDIAG("LyonPassiveEar.cpp - LyonPassiveEar:myUpdate");

	//FilterBank creation
	const mrs_real Eb				= 1000.0;
	const mrs_real EarZeroOffset	= 1.5;
	const mrs_real EarSharpness		= 5.0;
	const mrs_real EarPremphCorner	= 300.0;
	mrs_natural i,numChans;
	mrs_real lowFreq;
	mrs_real topFreq				= .5 * getctrl ("mrs_real/israte")->to<mrs_real>();
	ostringstream name;

	if (!setParametersIntern ())
	{
		this->updateControlsIntern ();
		return;
	}

	//% Find top frequency, allowing space for first cascade filter.
	//	topf = fs/2.0;
	//  topf = topf - (sqrt(topf^2+Eb^2)/EarQ*StepFactor*EarZeroOffset)+ ...
	//	sqrt(topf^2+Eb^2)/EarQ*StepFactor;
	topFreq	= topFreq - (sqrt (sqr(topFreq) + sqr(Eb)) / earQ_ * stepFactor_ * EarZeroOffset) + 
		sqrt (sqr(topFreq) + sqr(Eb)) / earQ_ * stepFactor_;

	//% Find place where CascadePoleQ < .5
	//	lowf = Eb/sqrt(4*EarQ^2-1);
	//  NumberOfChannels = floor((EarQ*(-log(lowf + sqrt(lowf^2 + Eb^2)) + ...
	//	log(topf + sqrt(Eb^2 + topf^2))))/StepFactor);
	lowFreq	= Eb / sqrt (4 * sqr(earQ_) - 1); 
	numChans= (mrs_natural)(floor ((earQ_ * (-log (lowFreq + sqrt (sqr(lowFreq) + sqr(Eb))) +
		log (topFreq + sqrt (sqr(Eb) + sqr(topFreq)))))/stepFactor_) + .1); // add .1 to ensure correct cast...

	//% Now make an array of CenterFreqs..... This expression was derived by
	//	% Mathematica by integrating 1/EarBandwidth(cf) and solving for f as a 
	//	% function of filterctrl number.
	//	cn = 1:NumberOfChannels;
	//  CenterFreqs = (-((exp((cn*StepFactor)/EarQ)*Eb^2)/ ...
	//	(topf + sqrt(Eb^2 + topf^2))) + ...
	//	(topf + sqrt(Eb^2 + topf^2))./exp((cn*StepFactor)/EarQ))/2;
	centerFreqs_.create (numChans);
	for (i = 0; i < numChans; i++)
		centerFreqs_(i) = (-((exp(((i+1)*stepFactor_)/earQ_) * sqr(Eb))/
		(topFreq + sqrt(sqr(Eb) + sqr(topFreq)))) + (topFreq + sqrt(sqr(Eb) + sqr(topFreq)))/ exp(((i+1)*stepFactor_)/earQ_))*.5;

	// free memory if necessary...
	if (filterBank_)
	{
		delete filterBank_;
		filterBank_	= 0;
	}
	filterBank_ = new Cascade("LyonFilterBank");
	mrs_realvec a(3),b(3); // we will use second order filters only

	//% Finally, let's design the front filters.
	//	front(1,:) = SetGain([0 1 -exp(-2*pi*EarPremphCorner/fs) 0 0], 1, fs/4, fs);
	//topPoles = SecondOrderFilter(topf,CascadePoleQ(1), fs);
	//front(2,:) = SetGain([1 0 -1 topPoles(2:3)], 1, fs/4, fs);
	b(0)	= a(1) = a(2) = 0;
	b(1)	= a(0) = 1;
	b(2)	= -exp(-TWOPI*EarPremphCorner/fs_);
	b		*= lyonSetGain (b, a, 1.0F, fs_*.25F, fs_);

	name.str("");
	name << "front_" << 0;
	filterBank_->addMarSystem(lyonCreateFilter (b, a, name.str()));

	b(0)	= 1;
	b(1)	= 0;
	b(2)	= -1;
	a		= lyonSecondOrderFilter (topFreq, centerFreqs_(0)/(sqrt(sqr(centerFreqs_(0)) + sqr(Eb))/earQ_), fs_);
	b		*= lyonSetGain (b, a, 1.0F, fs_*.25F, fs_);

	name.str("");
	name << "front_" << 1;
	filterBank_->addMarSystem(lyonCreateFilter (b, a, name.str()));

	for (i = 0; i < numChans; i++)
	{
		mrs_real EarBandwidth, CascadeZeroCF, CascadeZeroQ, CascadePoleCF, CascadePoleQ;

		//% OK, now we can figure out the parameters of each stage filter.
		//	EarBandwidth = sqrt(CenterFreqs.^2+Eb^2)/EarQ;
		//CascadeZeroCF = CenterFreqs +	EarBandwidth * StepFactor * EarZeroOffset;
		//CascadeZeroQ = EarSharpness*CascadeZeroCF./EarBandwidth;
		//CascadePoleCF = CenterFreqs;
		//CascadePoleQ = CenterFreqs./EarBandwidth;
		EarBandwidth	= sqrt(sqr(centerFreqs_(i)) + sqr(Eb))/earQ_;
		CascadeZeroCF	= centerFreqs_(i) + EarBandwidth * stepFactor_ * EarZeroOffset;
		CascadeZeroQ	= EarSharpness * CascadeZeroCF / EarBandwidth;
		CascadePoleCF	= centerFreqs_(i);
		CascadePoleQ	= centerFreqs_(i)/EarBandwidth;

		b	= lyonSecondOrderFilter (CascadeZeroCF, CascadeZeroQ, fs_);
		a	= lyonSecondOrderFilter (CascadePoleCF, CascadePoleQ, fs_);

		//% Now we can set the DC gain of each stage.
		//	dcgain(2:NumberOfChannels)=CenterFreqs(1:NumberOfChannels-1)./ ...
		//	CenterFreqs(2:NumberOfChannels);
		//dcgain(1) = dcgain(2);
		//for i=1:NumberOfChannels
		//	filters(i,:) = SetGain(filters(i,:), dcgain(i), 0, fs);
		//end
		b	*= (i == 0) ? lyonSetGain (b, a, centerFreqs_(i)/centerFreqs_(i+1), 0, fs_) :
			lyonSetGain (b, a, centerFreqs_(i-1)/centerFreqs_(i), 0, fs_);

		name.str("");
		name << "filter_" << i;
		filterBank_->addMarSystem(lyonCreateFilter (b, a, name.str()));
	}

	numFilterChannels_	= numChans + 2; // plus two front filters...

	this->updateControlsIntern (); 
}

void 
LyonPassiveEar::myProcess(realvec& in, realvec& out)
{
	if(getctrl("mrs_bool/mute")->to<mrs_bool>()) return;

	//out.stretch (numFilterChannels_*getctrl("mrs_natural/onObservations")->to<mrs_natural>(), onSamples_);

	filterBank_->process(in, out);
}

