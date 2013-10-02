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

#ifndef MARSYAS_GMMCLASSIFIER_H
#define MARSYAS_GMMCLASSIFIER_H

#include <marsyas/system/MarSystem.h>
#include <vector>

namespace Marsyas
{
/**
 \class GMMClassifier
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

 WARNING: This MarSystem is a "work in progress" and is not in a working state.
 It is still missing some coding, testing and validation of results. In case
 you're interested in using or developing a GMM Classifier in Marsyas, please
 contact lgustavomartins@gmail.com or the Marsyas mailing lists.

 */


class GMMClassifier: public MarSystem
{
private:
  mrs_real factor_; // a constant

  mrs_natural labelRow_;

  std::vector<realvec> probs_;
  std::vector<realvec> ssprobs_;

  std::vector<realvec> vars_;
  std::vector<realvec> covars_;
  std::vector<realvec> means_;
  std::vector<realvec> omeans_;
  std::vector<realvec> weights_;

  realvec trainMatrix_;
  realvec likelihoods_;
  realvec classSizes_;
  realvec accumVec_;
  realvec temp_;
  realvec sprobs_;
  realvec sum_;

  mrs_string prev_mode_;
  MarControlPtr ctrl_mode_;

  MarControlPtr ctrl_nClasses_;

  mrs_natural featSize_;			// Nr of features
  mrs_natural classSize_;			// Nr of classes (aka labels)
  mrs_natural nMixtures_;			// Nr of Gaussian mixtures
  mrs_natural iterations_;		// EM algorithm iterations
  mrs_natural kiterations_;		// K-MEANS initialization iterations
  mrs_natural eiterations_;		// Evaluation-training iterations
  MarControlPtr ctrl_nMixtures_;
  MarControlPtr ctrl_iterations_;
  MarControlPtr ctrl_kiterations_;
  MarControlPtr ctrl_eiterations_;

  void initialize();
  mrs_real gaussian(mrs_natural cl, mrs_natural k, realvec& vec);
  void doEM();
  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  GMMClassifier(std::string name);
  GMMClassifier(const GMMClassifier& a);
  ~GMMClassifier();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif



