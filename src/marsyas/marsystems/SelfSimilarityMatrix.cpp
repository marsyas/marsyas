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

#include "SelfSimilarityMatrix.h"

using namespace std;
using namespace Marsyas;

SelfSimilarityMatrix::SelfSimilarityMatrix(mrs_string name):MarSystem("SelfSimilarityMatrix", name)
{
  isComposite_ = true;
  addControls();
}

SelfSimilarityMatrix::SelfSimilarityMatrix(const SelfSimilarityMatrix& a) : MarSystem(a)
{
  ctrl_covMatrix_ = getctrl("mrs_realvec/covMatrix");
  ctrl_calcCovMatrix_ = getctrl("mrs_natural/calcCovMatrix");
  ctrl_normalize_ = getctrl("mrs_string/normalize");
  ctrl_stdDev_ = getctrl("mrs_real/stdDev");

  ctrl_mode_ = getctrl("mrs_natural/mode");
  ctrl_instanceIndexes_ = getctrl("mrs_realvec/instanceIndexes");
  ctrl_nInstances_ = getctrl("mrs_natural/nInstances");
  ctrl_done_ = getctrl("mrs_bool/done");
}

SelfSimilarityMatrix::~SelfSimilarityMatrix()
{
}

MarSystem*
SelfSimilarityMatrix::clone() const
{
  return new SelfSimilarityMatrix(*this);
}

void
SelfSimilarityMatrix::addControls()
{
  addControl("mrs_realvec/covMatrix", realvec(), ctrl_covMatrix_);
  addControl("mrs_natural/calcCovMatrix", SelfSimilarityMatrix::noCovMatrix, ctrl_calcCovMatrix_);
  addControl("mrs_string/normalize", "none", ctrl_normalize_);
  addControl("mrs_real/stdDev", 1.0, ctrl_stdDev_);

  addControl("mrs_natural/mode", SelfSimilarityMatrix::outputDistanceMatrix, ctrl_mode_);
  ctrl_mode_->setState(true);
  addControl("mrs_realvec/instanceIndexes", realvec(), ctrl_instanceIndexes_);
  addControl("mrs_natural/nInstances", -1, ctrl_nInstances_);
  addControl("mrs_bool/done", false, ctrl_done_);
}

void
SelfSimilarityMatrix::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)

  if(this->getctrl("mrs_natural/mode")->to<mrs_natural>() ==  SelfSimilarityMatrix::outputDistanceMatrix)
  {
    // The output similarity matrix is a squared matrix with
    // dimension equal to the number of elements (i.e. samples)
    // in the input

    //forward flow propagation
    ctrl_onObservations_->setValue(ctrl_inSamples_, NOUPDATE);
    ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
    ctrl_osrate_->setValue(ctrl_osrate_, NOUPDATE); //[?]
    ostringstream oss;
    for(mrs_natural o=0; o < ctrl_onObservations_->to<mrs_natural>(); ++o)
      oss << "SelfSimilarityMatrix_" << o << ",";
    ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

    //if the metric MarSystem exists and at least there is an element
    // to process at the input (it may not exist!), configure child marSystem.
    // Otherwise do nothing to it.
    child_count_t child_count = marsystems_.size();
    if(child_count == 1 && inSamples_ > 0)
    {
      //allocate realvec for the pair of stacked feature vectors
      //to be used in the similarity computation
      i_featVec_.create(ctrl_inObservations_->to<mrs_natural>());
      j_featVec_.create(ctrl_inObservations_->to<mrs_natural>());
      stackedFeatVecs_.create(ctrl_inObservations_->to<mrs_natural>()*2, 1);

      //configure the metric child MarSystem:
      //the input to metric are the two vector to process stacked vertically
      marsystems_[0]->setctrl("mrs_natural/inObservations", ctrl_inObservations_->to<mrs_natural>()*2);
      marsystems_[0]->setctrl("mrs_natural/inSamples", 1);
      marsystems_[0]->setctrl("mrs_real/israte", ctrl_israte_->to<mrs_real>()); //[?]
      oss.clear();
      oss << ctrl_inObsNames_->to<mrs_string>() << ctrl_inObsNames_->to<mrs_string>(); //[?]
      marsystems_[0]->setctrl("mrs_string/inObsNames", oss.str());
      marsystems_[0]->update();

      //link covMatrix control
      MarControlPtr ctrl_childCovMat = marsystems_[0]->getctrl("mrs_realvec/covMatrix");
      if(!ctrl_childCovMat.isInvalid())
        ctrl_childCovMat->linkTo(ctrl_covMatrix_);

      //allocate space for the metric result (must be a real value)
      metricResult_.create(1,1);

      //check if the child is a valid metric MarSystem
      if(marsystems_[0]->getctrl("mrs_natural/onObservations") != 1 ||
          marsystems_[0]->getctrl("mrs_natural/onSamples") != 1)
      {
        MRSWARN("SelfSimilarityMatrix::myUpdate - invalid Child Metric MarSystem (does not output a real value)!");
      }
    }
    else if(child_count > 1)
    {
      MRSWARN("similarityMatrix::myUpdate - more than one children MarSystem exists! Only one MarSystem should be added as a metric!");
    }
  }
  else if(this->getctrl("mrs_natural/mode")->to<mrs_natural>() ==  SelfSimilarityMatrix::outputPairDistance)
  {
    // The output similarity matrix is a single value with
    // the distance between the two instances (column vectors) at the input

    //forward flow propagation
    ctrl_onObservations_->setValue(1, NOUPDATE);
    ctrl_onSamples_->setValue(1, NOUPDATE);
    ctrl_osrate_->setValue(ctrl_osrate_, NOUPDATE); //???:
    ostringstream oss;
    oss << "Distance";
    ctrl_onObsNames_->setValue("Distance", NOUPDATE);

    //if the metric MarSystem exists and at least there is an element
    // to process at the input (it may not exist!), configure child marSystem.
    // Otherwise do nothing to it.
    child_count_t child_count = marsystems_.size();
    if(child_count == 1 && inSamples_ > 0)
    {
      //allocate realvec for the pair of stacked feature vectors
      //to be used in the similarity computation
      i_featVec_.create(ctrl_inObservations_->to<mrs_natural>());
      j_featVec_.create(ctrl_inObservations_->to<mrs_natural>());
      stackedFeatVecs_.create(ctrl_inObservations_->to<mrs_natural>()*2, 1);

      //configure the metric child MarSystem:
      //the input to metric are the two vector to process stacked vertically
      marsystems_[0]->setctrl("mrs_natural/inObservations", ctrl_inObservations_->to<mrs_natural>()*2);
      marsystems_[0]->setctrl("mrs_natural/inSamples", 1);
      marsystems_[0]->setctrl("mrs_real/israte", ctrl_israte_->to<mrs_real>()); //???:
      oss.clear();
      oss << ctrl_inObsNames_->to<mrs_string>() << ctrl_inObsNames_->to<mrs_string>(); //???:
      marsystems_[0]->setctrl("mrs_string/inObsNames", oss.str());
      marsystems_[0]->update();

      //link covMatrix control
      MarControlPtr ctrl_childCovMat = marsystems_[0]->getctrl("mrs_realvec/covMatrix");
      if(!ctrl_childCovMat.isInvalid())
        ctrl_childCovMat->linkTo(ctrl_covMatrix_);

      //allocate space for the metric result (must be a real value)
      metricResult_.create(1,1);

      MarControlAccessor acc(ctrl_instanceIndexes_);
      realvec& instIdxs = acc.to<mrs_realvec>();
      instIdxs.create(0.0, 1, 2); //init row vector

      ctrl_done_->setValue(false, NOUPDATE);

      //check if the child is a valid metric MarSystem
      if(marsystems_[0]->getctrl("mrs_natural/onObservations") != 1 ||
          marsystems_[0]->getctrl("mrs_natural/onSamples") != 1)
      {
        MRSWARN("SelfSimilarityMatrix::myUpdate - invalid Child Metric MarSystem (does not output a real value)!");
      }
    }
    else if(child_count > 1)
    {
      MRSWARN("similarityMatrix::myUpdate - more than one children MarSystem exists! Only one MarSystem should be added as a metric!");
    }
  }

}

void
SelfSimilarityMatrix::myProcess(realvec& in, realvec& out)
{
  if(this->getctrl("mrs_natural/mode")->to<mrs_natural>() ==  SelfSimilarityMatrix::outputDistanceMatrix)
  {
    //check if there are any elements to process at the input
    //(in some cases, they may not exist!) - otherwise, do nothing
    //(i.e. output will be zeroed out)
    if(inSamples_ > 0)
    {
      child_count_t child_count = marsystems_.size();
      if(child_count == 1)
      {
        mrs_natural nfeats = in.getRows();

        //normalize input features if necessary
        if(ctrl_normalize_->to<mrs_string>() == "MinMax")
          in.normObsMinMax(); // (x - min)/(max - min)
        else if(ctrl_normalize_->to<mrs_string>() == "MeanStd")
          in.normObs(); // (x - mean)/std

        //calculate the Covariance Matrix from the input, if defined
        if(ctrl_calcCovMatrix_->to<mrs_natural>() & SelfSimilarityMatrix::fixedStdDev)
        {
          //fill covMatrix diagonal with fixed value (remaining values are zero)
          MarControlAccessor acc(ctrl_covMatrix_);
          realvec& covMatrix = acc.to<mrs_realvec>();
          covMatrix.create(inObservations_, inObservations_);
          mrs_real var = ctrl_stdDev_->to<mrs_real>();
          var *= var;
          for(mrs_natural i=0; i< inObservations_; ++i)
          {
            covMatrix(i,i) = var;
          }
        }
        else if(ctrl_calcCovMatrix_->to<mrs_natural>() & SelfSimilarityMatrix::diagCovMatrix)
        {
          in.varObs(vars_); //FASTER -> only get the vars for each feature
          mrs_natural dim = vars_.getSize();
          //fill covMatrix diagonal with var values (remaining values are zero)
          MarControlAccessor acc(ctrl_covMatrix_);
          realvec& covMatrix = acc.to<mrs_realvec>();
          covMatrix.create(dim, dim);
          for(mrs_natural i=0; i< dim; ++i)
          {
            covMatrix(i,i) = vars_(i);
          }
        }
        else if(ctrl_calcCovMatrix_->to<mrs_natural>() & SelfSimilarityMatrix::fullCovMatrix)
        {
          MarControlAccessor acc(ctrl_covMatrix_);
          realvec& covMatrix = acc.to<mrs_realvec>();
          in.covariance(covMatrix); //SLOWER -> estimate the full cov matrix
        }
        else if(ctrl_calcCovMatrix_->to<mrs_natural>() == SelfSimilarityMatrix::noCovMatrix)
        {
          ctrl_covMatrix_->setValue(realvec());
        }

        for(mrs_natural i=0; i < in.getCols(); ++i)
        {
          in.getCol(i, i_featVec_);
          for(mrs_natural j=0; j <= i; ++j)
          {
            in.getCol(j, j_featVec_);

            //stack i and j feat vecs
            for(mrs_natural r=0; r < nfeats; ++r)
            {
              stackedFeatVecs_(r, 0) = i_featVec_(r);
              stackedFeatVecs_(r+nfeats, 0) = j_featVec_(r);
            }
            //do the metric calculation for these two feat vectors
            //and store it in the similarity matrix (which is symmetric)
            marsystems_[0]->process(stackedFeatVecs_, metricResult_);
            out(i,j) = metricResult_(0,0);
            //metric should be symmetric!
            out(j, i) = out(i, j);
          }
        }
      }
      else
      {
        out.setval(0.0);
        if(child_count == 0)
        {
          MRSWARN("SelfSimilarityMatrix::myProcess - no Child Metric MarSystem added - outputting zero similarity matrix!");
        }
        else
        {
          MRSWARN("SelfSimilarityMatrix::myProcess - more than one Child MarSystem exists (i.e. invalid metric) - outputting zero similarity matrix!");
        }
      }
    }

    //MATLAB_PUT(out, "simMatrix");
    //MATLAB_EVAL("figure(1);imagesc(simMatrix);");

    //MATLAB_PUT(out, "simMat");
    //MATLAB_EVAL(name_+"=["+name_+",simMat(:)'];");
  }
  else if(this->getctrl("mrs_natural/mode")->to<mrs_natural>() ==  SelfSimilarityMatrix::outputPairDistance)
  {
    if(inSamples_ == 2) //we always need two column vector instances at input
    {
      child_count_t child_count = marsystems_.size();
      if(child_count == 1)
      {
        MarControlAccessor acc(ctrl_instanceIndexes_);
        realvec& instIdxs = acc.to<mrs_realvec>();
        mrs_natural i = mrs_natural(instIdxs(0));
        mrs_natural j = mrs_natural(instIdxs(1));

        //check for out of bound indexes (which could have been set
        //by someone outside changing the value of the ctrl_instanceIndexes control)
        mrs_natural nInstances = ctrl_nInstances_->to<mrs_natural>();
        if(i >= nInstances || j >= nInstances)
          ctrl_done_->setValue(true);


        if(!ctrl_done_->isTrue())
        {
          mrs_natural nfeats = in.getRows();

          //COMPUTE DISTANCE between the two column vector at input
          in.getCol(0, i_featVec_);
          in.getCol(1, j_featVec_);

          //stack i and j feat vecs
          for(mrs_natural r=0; r < nfeats; ++r)
          {
            stackedFeatVecs_(r, 0) = i_featVec_(r);
            stackedFeatVecs_(r+nfeats, 0) = j_featVec_(r);
          }

          //do the metric calculation for these two feat vectors
          //and send it to the output
          marsystems_[0]->process(stackedFeatVecs_, out);
          //out(0) = metricResult_(0,0);
        }
        else
        {
          //Self Similarity has completed all pair-wise similarity computations
          //so, it will just send zero valued values and a warning
          out(0) = 0.0;
          MRSWARN("SelfSimilarityMatrix::myProcess - no more pairwise similarity computations to be performed - outputting zero similarity value!")
        }

        //Select indexes for next pair of instances for distance computation
        //Similarity matrix is tringular simetric, so we should just compute
        //half of it (including diagonal). These indexes are to be used by some
        //source MarSystem that has a control linked to ctrl_instanceIndexes (e.g. WekaSource)
        if (i < j)
          ++i;
        else
        {
          ++j;
          i = 0;
        }
        if (j >= nInstances)
        {
          ctrl_done_->setValue(true);
          j = -1; //used to signal that there are no more instance pairs to compute
          i = -1; //used to signal that there are no more instance pairs to compute
        }
        else
          ctrl_done_->setValue(false);

        //set indexes into the ctrl_instanceIndexes_ control
        instIdxs(0) = i;
        instIdxs(1) = j;
      }
      else
      {
        out.setval(0.0);
        if(child_count == 0)
        {
          MRSWARN("SelfSimilarityMatrix::myProcess - no Child Metric MarSystem added - outputting zero similarity value!");
        }
        else
        {
          MRSWARN("SelfSimilarityMatrix::myProcess - more than one Child MarSystem exists (i.e. invalid metric) - outputting zero similarity value!");
        }
      }
    }
  }
}
