/*
** Copyright (C) 2000 George Tzanetakis <gtzan@cs.princeton.edu>
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
    \class MultiPitch
    \brief Multiple pitch extractor

    Multiple pitch extractor based on the paper by 
T.Tolonen and M.Karjalainen "A computationally efficient multipitch analysis 
model IEEE Trans.on Speech and Audio Processing vol 8, num 6. 2000.
*/




#include "MultiPitch.h"

using namespace std;
using namespace Marsyas;


MultiPitch::MultiPitch(string name):MarSystem("MultiPitch", name)
{
	//Add any specific controls needed by MultiPitch
	//(default controls all MarSystems should have
	//were already added by MarSystem::addControl(), 
	//called by :MarSystem(name) constructor).
	//If no specific controls are needed by a MarSystem
	//there is no need to implement and call this addControl()
	//method (see for e.g. Rms.cpp)
	//addControls();

	lpf1_=NULL;
	lpf2_=NULL;
	hpf1_=NULL;
	hwr_=NULL;
	hwr2_=NULL;
	hwr3_=NULL;
	autocorlo_=NULL;
	autocorhi_=NULL;
	sum_=NULL;
	pe2_=NULL;
	pe3_=NULL;
	pe4_=NULL;

	fan_=NULL;
	hinet_=NULL;
	lonet_=NULL;

	net_=NULL;
	par_=NULL;
	p_=NULL;
}

MultiPitch::MultiPitch(const MultiPitch& orig) : MarSystem(orig)
{
	// For any MarControlPtr in a MarSystem 
	// it is necessary to perform this getctrl 
	// in the copy constructor in order for cloning to work
	
	//ctrl_offStart_ = getctrl("mrs_real/offStart");
	if(lpf1_!=NULL)lpf1_=orig.lpf1_->clone();
	else lpf1_=NULL;
	if(lpf2_!=NULL)lpf2_=orig.lpf2_->clone();
	else lpf2_=NULL;
	if(hpf1_!=NULL)hpf1_=orig.hpf1_->clone();
	else hpf1_=NULL;
	if(hwr_!=NULL)hwr_=orig.hwr_->clone();
	else hwr_=NULL;
	if(hwr2_!=NULL)hwr2_=orig.hwr2_->clone();
	else hwr2_=NULL;
	if(hwr3_!=NULL)hwr3_=orig.hwr3_->clone();
	else hwr3_=NULL;
	if(autocorlo_!=NULL)autocorlo_=orig.autocorlo_->clone();
	else autocorlo_=NULL;
	if(autocorhi_!=NULL)autocorhi_=orig.autocorhi_->clone();
	else autocorhi_=NULL;
	if(sum_!=NULL)sum_=orig.sum_->clone();
	else sum_=NULL;
	if(pe2_!=NULL)pe2_=orig.pe2_->clone();
	else pe2_=NULL;
	if(pe3_!=NULL)pe3_=orig.pe3_->clone();
	else pe3_=NULL;
	if(pe4_!=NULL)pe4_=orig.pe4_->clone();
	else pe4_=NULL;
	if(fan_!=NULL)fan_=orig.fan_->clone();
	else fan_=NULL;
	if(hinet_!=NULL)hinet_=orig.hinet_->clone();
	else hinet_=NULL;
	if(lonet_!=NULL)lonet_=orig.lonet_->clone();
	else lonet_=NULL;
	if(net_!=NULL)net_=orig.net_->clone();
	else net_=NULL;
	if(par_!=NULL)par_=orig.par_->clone();
	else par_=NULL;
	if(p_!=NULL)p_=orig.p_->clone();
	else p_=NULL;


}



MarSystem* 
MultiPitch::clone() const 
{
	return new MultiPitch(*this);
}


void
MultiPitch::myUpdate(MarControlPtr sender)
{
	//cout << "------updating multipitch" << endl;
	MarSystem::myUpdate(sender);
	mrs_string inObsNames = ctrl_inObsNames_->to<mrs_string>();
	
	setControl("mrs_natural/onSamples",  (mrs_natural)(ctrl_inSamples_->to<mrs_natural>()/2));  
	setControl("mrs_natural/onObservations",  (mrs_natural)(ctrl_inObservations_->to<mrs_natural>()));  

	// Add Names of The Observations to the observation names.
	//inObsNames+="";

	// Add prefix to the observation names.
	ctrl_onObsNames_->setValue(obsNamesAddPrefix(inObsNames, "AutoCorrelationFFT_"), NOUPDATE);

	mrs_realvec num_low, denom_low;
	mrs_realvec num_high, denom_high;
	
	num_low.create(3); 
	denom_low.create(3);
	num_high.create(3); 
	denom_high.create(3);

	//coefs are for butter(2,1000) and butter(2,1000,'high') 
	num_low(0)=0.1207f; num_low(1)=0.2415f; num_low(2)=0.1207f;
	denom_low(0)=1.0f; denom_low(1)=-0.8058f; denom_low(2)=0.2888f;

	num_high(0)=0.5236f; num_high(1)=-1.0473f; num_high(2)=0.5236f;
	denom_high(0)=1.0f; denom_high(1)=-0.8058f; denom_high(2)=0.2888f;  	
	
	if (lpf1_==NULL) lpf1_ = new Filter("lpf1");	
	if (lpf2_==NULL) lpf2_ = new Filter("lpf2");
	if (hpf1_==NULL) hpf1_ = new Filter("hpf1");
	lpf1_->setctrl("mrs_realvec/ncoeffs", num_low);
	lpf1_->setctrl("mrs_realvec/dcoeffs", denom_low);
	lpf2_->setctrl("mrs_realvec/ncoeffs", num_low);
	lpf2_->setctrl("mrs_realvec/dcoeffs", denom_low);
	hpf1_->setctrl("mrs_realvec/ncoeffs", num_high);
	hpf1_->setctrl("mrs_realvec/dcoeffs", denom_high);


	
	if (hwr_==NULL) hwr_ = new HalfWaveRectifier("hwr");
	if (hwr2_==NULL) hwr2_ = new HalfWaveRectifier("hwr2");
	if (hwr3_==NULL) hwr3_ = new HalfWaveRectifier("hwr3");
	if (autocorlo_==NULL) autocorlo_ = new AutoCorrelationFFT("fftlo");
	if (autocorhi_==NULL) autocorhi_ = new AutoCorrelationFFT("ffthi");
	

//	pe2_ = new PeakEnhancer(inSize_, 2);
//	pe3_ = new PeakEnhancer(inSize_, 3);
//	pe4_ = new PeakEnhancer(inSize_, 4);

	
	
	if (pe2_==NULL) pe2_ = new PeakEnhancer("pe2");
	if (pe3_==NULL) pe3_ = new PeakEnhancer("pe3");
	if (pe4_==NULL) pe4_ = new PeakEnhancer("pe4");
	pe2_->setctrl("mrs_natural/itnum",2);
	pe3_->setctrl("mrs_natural/itnum",3);
	pe4_->setctrl("mrs_natural/itnum",4);
	

	//lowp_.create(inSamples_);
	//highp_.create(inSamples_);
	//sum_.create(inSamples_);


	if (net_==NULL) net_ = new Series("net");
	if (fan_==NULL) fan_ = new Fanout("fan");
	if (hinet_==NULL) hinet_ = new Series("hinet");
	
	if (lonet_==NULL) lonet_ = new Series("lonet");
	if (sum_==NULL) sum_ = new Sum("sum");


	lonet_->addMarSystem(lpf1_);
	lonet_->addMarSystem(hwr_);
	lonet_->addMarSystem(autocorlo_);
	hinet_->addMarSystem(hpf1_);
	hinet_->addMarSystem(hwr2_);
	hinet_->addMarSystem(lpf2_);
	hinet_->addMarSystem(autocorhi_);

	fan_->addMarSystem(hinet_);
	fan_->addMarSystem(lonet_);

	net_->addMarSystem(fan_);
	net_->addMarSystem(sum_);
	net_->addMarSystem(pe2_);
	net_->addMarSystem(pe3_);
	net_->addMarSystem(pe4_);
	net_->addMarSystem(hwr3_);
	//cout << "net done" << endl;
	if (par_==NULL) par_=new Parallel("par");


	for(int o=0;o<onObservations_;o++)
	{
		stringstream str;

		p_=net_->clone();
		str << "net"<<o;
		p_->setName(str.str());
		str.flush();
		//cout << "cloned" << endl;
		par_->addMarSystem(p_);
		//cout << "added" << endl;
		
	}
	//cout << *par_ << endl;

	
}


void 
MultiPitch::myProcess(realvec& in, realvec& out) 
{
	par_->process(in,out);
}


MultiPitch::~MultiPitch()
{

	if (lpf1_!=NULL) delete lpf1_;
	if (lpf2_!=NULL) delete lpf2_;
	if (hpf1_!=NULL) delete hpf1_;
	if (hwr_!=NULL) delete hwr_;
	if (hwr2_!=NULL) delete hwr2_;
	if (hwr3_!=NULL) delete hwr3_;
	if (autocorlo_!=NULL) delete autocorhi_;
	if (autocorhi_!=NULL) delete autocorlo_;
	if (sum_!=NULL) delete sum_;
	if (pe2_!=NULL) delete pe2_;
	if (pe3_!=NULL) delete pe3_;
	if (pe4_!=NULL) delete pe4_;

	if (fan_!=NULL) delete fan_;
	if (hinet_!=NULL) delete hinet_;
	if (lonet_!=NULL) delete lonet_;

	if (net_!=NULL) delete net_;
	if (par_!=NULL) delete par_;
	if (p_!=NULL) delete p_;
	
}

	
