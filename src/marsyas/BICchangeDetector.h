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

#ifndef BICCHANGEDETECTOR_H
#define BICCHANGEDETECTOR_H

#include "MarSystem.h"
#include "QGMMModel.h"

namespace Marsyas
{
/** 
    \class BICchangeDetector
    \ingroup MachineLearning
    \brief detect changes between to succeeding segments using BIC

    Detect changes between two succeeding segments using divergenceShape
    (or BattacharyyaShape), and validation using BIC and quasi-GMM modeling

    Implementation by Luis Gustavo Martins - lmartins@inescporto.pt
*/


	class Memory;

	class BICchangeDetector: public MarSystem
	{
	private: 
		mrs_natural segFrames_; //number of frames per audio segment
		mrs_natural minSegFrames_; //minimum nr of frames to avoid ill-cov matrices
		mrs_natural segHop_;
		mrs_natural nfeats_;
		realvec C1_;
		realvec C2_;
		realvec C3_;
		realvec C4_;
		mrs_real dist12_;
		mrs_real dist34_;
		mrs_real BICdist_;
		
		//dynamic thresholding parameters
		mrs_natural nrPrevDists_;
		Memory* prevDists_;
		realvec pdists_;
		mrs_real dynThres_;
		mrs_real alpha1_;

		//BIC parameter
		mrs_real lambda_;

		//quasi-GMM model
		QGMMModel QGMMmodel_;
		
		MarControlPtr ctrl_reset_;
		MarControlPtr ctrl_alpha1_;
		MarControlPtr ctrl_lambda_;

		void addControls();
		void myUpdate(MarControlPtr sender);

	public:
		BICchangeDetector(std::string name);
		BICchangeDetector(const BICchangeDetector& a);
		~BICchangeDetector();
		MarSystem* clone() const;  

		void myProcess(realvec& in, realvec& out);
	};

}//namespace Marsyas

#endif
