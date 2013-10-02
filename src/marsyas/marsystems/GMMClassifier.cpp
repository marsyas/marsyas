/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "GMMClassifier.h"
#include "../common_source.h"
#include <marsyas/NumericLib.h>


using std::ostringstream;
using std::vector;

using namespace Marsyas;

GMMClassifier::GMMClassifier(mrs_string name):MarSystem("GMMClassifier",name)
{
  prev_mode_= "predict";
  classSize_ = -1;
  featSize_ = -1;
  nMixtures_ = -1;
  addControls();
}


GMMClassifier::GMMClassifier(const GMMClassifier& a):MarSystem(a)
{
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_nClasses_ = getctrl("mrs_natural/nClasses");
  ctrl_nMixtures_ = getctrl("mrs_natural/nMixtures");
  ctrl_iterations_ = getctrl("mrs_natural/iterations");
  ctrl_kiterations_ = getctrl("mrs_natural/kiterations");
  ctrl_eiterations_ = getctrl("mrs_natural/eiterations");

  prev_mode_ = "predict";
  classSize_ = -1;
  featSize_ = -1;
  nMixtures_ = -1;
}


GMMClassifier::~GMMClassifier()
{
}


MarSystem*
GMMClassifier::clone() const
{
  return new GMMClassifier(*this);
}

void
GMMClassifier::addControls()
{
  addctrl("mrs_string/mode", "train", ctrl_mode_);
  ctrl_mode_->setState(true);

  addctrl("mrs_natural/nClasses", -1, ctrl_nClasses_);
  ctrl_nClasses_->setState(true);

  addctrl("mrs_natural/nMixtures", -1, ctrl_nMixtures_);
  ctrl_nMixtures_->setState(true);

  addctrl("mrs_natural/iterations", 200, ctrl_iterations_);
  addctrl("mrs_natural/kiterations", 100, ctrl_kiterations_);
  addctrl("mrs_natural/eiterations", 20, ctrl_eiterations_);
}

void
GMMClassifier::initialize()
{
  mrs_natural trainSize = trainMatrix_.getCols();

  realvec temp(featSize_);
  realvec randstep(featSize_);

  mrs_natural seedSize = 5; //FIXME: hardcoded; change to a control?
  mrs_real rind;
  rind = ((mrs_real)rand() / (mrs_real)(RAND_MAX))*trainSize;

  for (mrs_natural cl=0; cl < classSize_; cl++)
  {
    for (mrs_natural k=0; k < nMixtures_; k++)
    {
      ////////////////////////////////////////////////////////
      // Compute feature Means for current class and mixture
      ////////////////////////////////////////////////////////
      temp.setval(0.0);
      for (mrs_natural c=0; c < seedSize; ++c)
      {
        //randomly select a number of feat.vectors from a class
        rind = ((mrs_real)rand() / (mrs_real)(RAND_MAX))*trainSize;
        while (trainMatrix_(labelRow_,(mrs_natural)rind)!= cl)
          rind = ((mrs_real)rand() / (mrs_real)(RAND_MAX))*trainSize;

        //accumulate randomly selected feature vector into temp realvec
        for(mrs_natural f=0; f < featSize_; ++f)
          temp(f) += trainMatrix_(f, (mrs_natural)rind);
      }
      temp /= seedSize; //compute mean
      //store result for current class and mixture
      for(mrs_natural f=0; f < featSize_; ++f)
        means_[cl](f, k) = temp(f);

      ///////////////////////////////////////////////////////////
      // Compute feature Variances for current class and mixture
      ///////////////////////////////////////////////////////////
      // count the number of examples of this class in the training matrix
      mrs_natural classExamples = 0;
      vector<mrs_natural> classCols;
      for(mrs_natural c=0; c < trainSize; ++c)
        if(trainMatrix_(labelRow_, c) == cl)
        {
          classExamples++;
          classCols.push_back(c);
        }

      // copy all feature vector from this class into a temp matrix
      // so we can compute some statistics (i.e. variance)
      realvec classFeatMatrix(featSize_, classExamples);
      for(mrs_natural c=0; c < classExamples; ++c)
      {
        for(mrs_natural f=0; f < featSize_; ++f)
          classFeatMatrix(f, c) = trainMatrix_(f, classCols[c]);
      }

      //compute variance of the features (i.e. observations)
      classFeatMatrix.varObs(temp);

      //store result for current class and mixture
      for(mrs_natural f=0; f < featSize_; ++f)
        vars_[cl](f, k) = temp(f);
    }

    //////////////////////////////////////////////////////////////
    // Compute feature Covariances for current class and mixture
    //////////////////////////////////////////////////////////////
    for (mrs_natural k=0; k < nMixtures_; k++)
      for (mrs_natural f=0; f < featSize_; f++)
      {
        if (vars_[cl](f,k) != 0.0)
          covars_[cl](f,k) = 1.0 / vars_[cl](f,k);
        else
          covars_[cl](f,k) = 0.0;
      }

    //////////////////////////////////////////////////////////////
    // Set initial weights for current class and mixture
    //////////////////////////////////////////////////////////////
    weights_[cl].setval(1.0 / nMixtures_);
  }

  ///////////////////////////////////////////
  // Perform K-Means
  ///////////////////////////////////////////
  mrs_real dist = 0.0;
  mrs_natural min_k = 0;

  likelihoods_.create(classSize_, nMixtures_);

  for (mrs_natural i=0; i < kiterations_; ++i)
  {
    likelihoods_.setval(0.0);

    //init omeans_ with the values of means_
    for (mrs_natural cl = 0; cl < classSize_; cl++)
      for (mrs_natural k=0; k < nMixtures_; k++)
        for (mrs_natural f=0; f < featSize_; f++)
        {
          omeans_[cl](f,k) = means_[cl](f,k);
        }

    // set each class's means to zero (for all mixtures)
    for (mrs_natural cl=0; cl < classSize_; cl++)
    {
      means_[cl].setval(0.0);
    }

    //for all the feature vectors (i.e. examples) in the trainMatrix...
    for (mrs_natural c=0; c < trainSize; ++c)
    {
      mrs_real min = 100000000;

      //get this feature vector class label
      mrs_natural cl = (mrs_natural)trainMatrix_(labelRow_, c);
      trainMatrix_.getCol(c, temp);

      // look for the minimum distance of this training feat. vec
      // to the existing mixtures
      for (mrs_natural k=0; k < nMixtures_; k++)
      {
        //get omean and covar for each mixture in this class
        realvec omean;
        omeans_[cl].getCol(k, omean);
        realvec covar;
        covars_[cl].getCol(k, covar);

        //compute distance between feat.vector and the mean vector
        dist = NumericLib::mahalanobisDistance(temp, omean, covar);

        if (dist < min)
        {
          min = dist;
          min_k = k;
        }
      }

      //update closest mixture with the current feature vector
      for (mrs_natural f=0; f < featSize_; f++)
      {
        means_[cl](f, min_k) += temp(f);
      }

      // increment the counter of the current class and selected mixture
      likelihoods_(cl,min_k)++;
    }

    //compute means for all classes
    for (mrs_natural cl=0; cl < classSize_; cl++)
    {
      for (mrs_natural k=0; k < nMixtures_; k++)
        for (mrs_natural f=0; f < featSize_; f++)
        {
          if (likelihoods_(cl,k) != 0.0)
            means_[cl](f,k) /= likelihoods_(cl, k);
        }
      // cout << "KMEANS CLASS = " << cl << endl;
      // cout << "Means = " << means_[cl] << endl;
    }
  }//end of k-means iterations

  classSizes_.create(classSize_);
  sum_.create(classSize_);
  likelihoods_.create(classSize_, nMixtures_);
  accumVec_.create(featSize_); //FIXME: row?
  temp_.create(featSize_); //FIXME: ?
  sprobs_.create(classSize_,nMixtures_);

  probs_.reserve(classSize_);
  ssprobs_.reserve(classSize_);
  for (mrs_natural cl=0; cl < classSize_; ++cl)
  {
    probs_.push_back(realvec(trainSize, nMixtures_));
    ssprobs_.push_back(realvec(featSize_, nMixtures_));
  }
}

mrs_real
GMMClassifier::gaussian(mrs_natural cl, mrs_natural k, realvec& vec)
{
  mrs_real res;
  mrs_real temp;
  mrs_real det = 1.0;

  for (mrs_natural f=0; f < featSize_; f++)
    det *=  (vars_[cl])(f,k);

  res = 1 / (factor_ * det);

  realvec mean;
  means_[cl].getCol(k, mean);
  realvec covar;
  covars_[cl].getCol(k, covar);
  temp = NumericLib::mahalanobisDistance(vec, mean, covar);

  res *= exp(-temp*0.5);

  return res;
}

void
GMMClassifier::doEM()
{
  realvec featVec;
  mrs_natural cl;

  //init to zero
  classSizes_.setval(0.0);
  sum_.setval(0.0);
  sprobs_.setval(0.0);
  accumVec_.setval(0.0);
  for (cl=0; cl < classSize_; cl++)
    ssprobs_[cl].setval(0.0);

  mrs_natural trainSize = trainMatrix_.getCols();
  mrs_real prob;
  mrs_real sum;

  //for all feat.vecs in trainMatrix...
  for (mrs_natural c=0; c < trainSize; ++c)
  {
    //get class label of current feature vector
    cl = (mrs_natural)trainMatrix_(labelRow_, c);
    classSizes_(cl)++;
    sum = 0.0;

    //get current feature vector
    trainMatrix_.getCol(c, featVec);

    //calculate the probability of the feat.Vector
    //belonging to each one of the mixtures
    for (mrs_natural k=0; k < nMixtures_; k++)
    {
      //compute the probablity p(x|k)
      likelihoods_(cl,k) = gaussian(cl,k, featVec);
      //accumulated probability (Sum_k[p(x|k)])
      sum += likelihoods_(cl,k);
    }

    //for each mixture...
    for (mrs_natural k=0; k < nMixtures_; k++)
    {
      // compute posteriori probablility:
      // p(k|x) = p(x|k)/sum[p(x|k)] = p(x|k)/p(x)
      if (sum != 0.0)
        prob = likelihoods_(cl,k) / sum;
      else
      {
        prob = 0.0000000001;
      }
      //store posteriori probabilities (p(k|x))
      probs_[cl](c,k) = prob;

      //posterior probability for each class (p(cl|x))
      sprobs_(cl,k) += prob;

      //compute x*p(k|x)
      temp_ = featVec;
      temp_ *= prob;

      //compute p(cl|x)*x
      ssprobs_[cl].getCol(k, accumVec_);
      accumVec_ += temp_;

      //store it
      for(mrs_natural f=0; f < featSize_; ++f)
        ssprobs_[cl](f, k) = accumVec_(f);
    }
  }

  for (cl = 0; cl < classSize_; cl++)
    for (mrs_natural k=0; k < nMixtures_; k++)
    {
      weights_[cl](k) = sprobs_(cl,k) / classSizes_(cl);
      ssprobs_[cl].getCol(k, temp_);
      if (sprobs_(cl,k) != 0.0)
      {
        //compute p(cl|x)*x/p(cl|x)
        temp_ /= sprobs_(cl,k);
        //store it
        for(mrs_natural f=0; f < featSize_; ++f)
          means_[cl](f,k) = temp_(f);
      }
    }

  for (cl=0; cl < classSize_; cl++)
    ssprobs_[cl].setval(0.0);


  //for each feat.Vec in the trainMatrix...
  for (mrs_natural t=0; t < trainSize; t++)
  {
    //get its class label
    cl = (mrs_natural)trainMatrix_(labelRow_, t);

    //get the feat.Vector
    trainMatrix_.getCol(t, featVec);

    //for each mixture, compute:
    // p(x|k)(x-uk)(x-uk)T
    for (mrs_natural k=0; k < nMixtures_; k++)
    {
      prob = (probs_[cl])(t,k);
      temp_ = featVec;
      realvec means;
      means_[cl].getCol(k, means);
      temp_ -= means;
      temp_.sqr();
      temp_ *= prob;

      ssprobs_[cl].getCol(k, accumVec_);
      accumVec_ += temp_;

      //store it
      for(mrs_natural f=0; f < featSize_; ++f)
        ssprobs_[cl](f, k) = accumVec_(f);
    }
  }

  for (cl = 0; cl < classSize_; cl++)
  {
    for (mrs_natural k=0; k < nMixtures_; k++)
    {
      ssprobs_[cl].getCol(k, temp_);
      temp_ *= (1.0 / (sprobs_(cl,k)));// -1.0)); //FIXME: confirm this?

      //store it
      for(mrs_natural f=0; f < featSize_; ++f)
        vars_[cl](f, k) = temp_(f);
    }

    for (mrs_natural k=0; k < nMixtures_; k++)
      for (mrs_natural f=0; f < featSize_; f++)
      {
        if (vars_[cl](f, k) > 0.0)
          covars_[cl](f, k) = 1.0 / (vars_[cl](f, k));
        else
        {
          covars_[cl](f, k) = 10000000.0;
          vars_[cl](f, k) = 0.0000001;
        }
      }
  }
}

void
GMMClassifier::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  MRSDIAG("GMMClassifier.cpp - GMMClassifier:myUpdate");

  setctrl("mrs_natural/onSamples", getctrl("mrs_natural/inSamples"));
  setctrl("mrs_natural/onObservations", (mrs_natural)2);
  setctrl("mrs_real/osrate", getctrl("mrs_real/israte"));
  setctrl("mrs_string/onObsNames", "GT_label, Predicted_label,");

  mrs_string mode = getctrl("mrs_string/mode")->to<mrs_string>();

  mrs_natural classSize = getctrl("mrs_natural/nClasses")->to<mrs_natural>();
  mrs_natural nMixtures = getctrl("mrs_natural/nMixtures")->to<mrs_natural>();
  mrs_natural featSize = inObservations_-1; //last observation at input is the label row!

  // Initialize internal variables
  // (check for changes in nr of classes, nr of features or
  // nr of gaussian mixtures)
  if((classSize != classSize_) || (nMixtures != nMixtures_) ||
      (featSize != featSize_))
  {
    classSize_ = classSize;
    nMixtures_ = nMixtures;
    featSize_ = featSize;
    labelRow_ = featSize_;

    factor_ = pow(sqrt(TWOPI), (mrs_real)featSize_);

    //init
    means_.clear();
    omeans_.clear();
    vars_.clear();
    covars_.clear();
    weights_.clear();
    means_.reserve(classSize_);
    omeans_.reserve(classSize_);
    vars_.reserve(classSize_);
    covars_.reserve(classSize_);
    weights_.reserve(classSize_);

    // populate above vectors with realvecs for each class
    for (mrs_natural cl=0; cl < classSize_; cl++)
    {
      realvec cmeans(featSize_, nMixtures_);
      realvec ocmeans(featSize_, nMixtures_);
      realvec cvars(featSize_, nMixtures_);
      realvec ccovars(featSize_, nMixtures_);
      realvec cweights(nMixtures_);

      // Vectors of realvec for each class
      means_.push_back(cmeans);
      omeans_.push_back(ocmeans);
      vars_.push_back(cvars);
      covars_.push_back(ccovars);
      weights_.push_back(cweights);
    }
  }

  //change from TRAIN to PREDICT mode
  if ((prev_mode_ == "train") && (mode == "predict"))
  {
    initialize();

    for (mrs_natural i=0; i < iterations_ ; ++i)
    {
      doEM();
    }

    prev_mode_ = mode;
  }
}

void
GMMClassifier::myProcess(realvec& in, realvec& out)
{
  mrs_string mode = ctrl_mode_->to<mrs_string>();

  // reset
  if ((prev_mode_ == "predict") && (mode == "train"))
  {
    //just drop all accumulated feature vectors and
    //copy take the new ones from the input
    trainMatrix_ = in;
  }

  if (mode == "train")
  {
    MRSASSERT(trainMatrix_.getRows() == inObservations_);

    //stretch to acommodate input feat Vecs
    mrs_natural storedFeatVecs = trainMatrix_.getCols();
    trainMatrix_.stretch(inObservations_, storedFeatVecs + inSamples_);

    //append input data
    for(mrs_natural c=0; c < inSamples_; ++c)
      for(mrs_natural r = 0; r < inObservations_; ++r)
        trainMatrix_(r, c+storedFeatVecs) = in(r,c);
  }

  if (mode == "predict")
  {
    mrs_real maxProb = 0.0;
    mrs_natural maxClass = 0;
    mrs_real prob;
    mrs_real dist;
    realvec vec;
    realvec means;
    realvec covars;

    MRSASSERT(trainMatrix_.getRows() == inObservations_);

    for(mrs_natural t=0; t < inSamples_; ++t)
    {

      in.getCol(t, vec);

      for (mrs_natural cl=0; cl < classSize_; cl++)
      {
        for (mrs_natural k=0; k < nMixtures_; k++)
        {
          means_[cl].getCol(k, means);
          covars_[cl].getCol(k, covars);
          dist = NumericLib::mahalanobisDistance(vec, means, covars);
          likelihoods_(cl,k) = weights_[cl](k) / dist;
        }
        prob = 0.0;
        for (mrs_natural k=0; k < nMixtures_; k++)
        {
          prob += likelihoods_(cl,k);
        }
        if (prob > maxProb)
        {
          maxProb = prob;
          maxClass = cl;
        }
      }
      out(0,t) = in(labelRow_, t);
      out(1,t) = (mrs_real)maxClass; //FIXME: what about he maxProb (i.e. Confidence)?
    }
  }

  prev_mode_ = mode;
}
