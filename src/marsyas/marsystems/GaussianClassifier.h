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

#ifndef MARSYAS_GAUSSIANCLASSIFIER_H
#define MARSYAS_GAUSSIANCLASSIFIER_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
    \class GaussianClassifier
	\ingroup MachineLearning
    \brief Single Gaussian multidimensional classifier

    Simple multidimensional Gaussian classifier. The classifier
    parameters (or model) are stored in the the theta control.  When the
    mode control is set to "train" the input slice is used to accumulate
    information for training.  The actual final theta calculation is
    completed when the control train is set to true. That can accomodate
    non-incremental or batch training. The labels control is used to
    provide ground truth about the label(s). The output of the classifier is
    the ground truth label(s) when the mode control is train.

    When the mode control is set to "predict" then the output
    of the classifier is the predicted labels using the trained
    parameter vector theta.

	Basically there are four states based on control transitions
	(previous value of mode (PVM); current value of mode(CVM)):
	1. PVM: predict;	CVM: train		==> reset the classifier
	2. PVM: train;		CVM: train		==> train or accumulate samples for batch classifiers
	3. PVM: train;		CVM: predict	==> finalize things like calculating means and variances to prepare for prediction
	4. PVM: predict;	CVM: predict	==> just predict

    This MarSystems serves as a prototypical classification/regression
    MarSystem.

*/


class GaussianClassifier: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);



  // realvec means_;
  // realvec covars_;
  realvec labelSizes_;
  MarControlPtr ctrl_mode_;
  MarControlPtr ctrl_nClasses_;
  MarControlPtr ctrl_done_;
  MarControlPtr ctrl_means_;
  MarControlPtr ctrl_covars_;

  mrs_string prev_mode_;



public:
  GaussianClassifier(std::string name);
  GaussianClassifier(const GaussianClassifier& a);

  ~GaussianClassifier();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif



