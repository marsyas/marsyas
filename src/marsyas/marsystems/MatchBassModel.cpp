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



#include "MatchBassModel.h"
#include "Metric2.h"


using std::ostringstream;
using std::cout;
using std::endl;

using namespace Marsyas;

MatchBassModel::MatchBassModel(mrs_string name):MarSystem("MatchBassModel", name)
{
  isComposite_ = true;
  addControls();
}

MatchBassModel::MatchBassModel(const MatchBassModel& a):MarSystem(a)
{
  ctrl_nTemplates_ = getctrl("mrs_natural/nTemplates");
  ctrl_nDevision_ = getctrl("mrs_natural/nDevision");
  ctrl_intervals_ = getctrl("mrs_realvec/intervals");
  ctrl_selections_ = getctrl("mrs_realvec/selections");
  ctrl_segmentation_ = getctrl("mrs_realvec/segmentation");
  ctrl_time_ = getctrl("mrs_realvec/time");
  ctrl_freq_ = getctrl("mrs_realvec/freq");
  ctrl_lowFreq_ = getctrl("mrs_real/lowFreq");
  ctrl_highFreq_ = getctrl("mrs_real/highFreq");
  ctrl_rootFreq_ = getctrl("mrs_real/rootFreq");
  ctrl_totalDistance_ = getctrl("mrs_real/totalDistance");
  ctrl_mode_ = getctrl("mrs_string/mode");
  ctrl_calcCovMatrix_ = getctrl("mrs_natural/calcCovMatrix");
  ctrl_covMatrix_ = getctrl("mrs_realvec/covMatrix");
  ctrl_templates_ = getctrl("mrs_realvec/templates");
  ctrl_stdDev_ = getctrl("mrs_real/stdDev");
  ctrl_normalize_ = getctrl("mrs_string/normalize");
  ctrl_distance_ = getctrl("mrs_realvec/distance");
}

MatchBassModel::~MatchBassModel()
{
}

MarSystem*
MatchBassModel::clone() const
{
  return new MatchBassModel(*this);
}

void
MatchBassModel::addControls()
{
  addControl("mrs_natural/nTemplates", 3, ctrl_nTemplates_);
  addControl("mrs_natural/nDevision", 64, ctrl_nDevision_);
  addControl("mrs_realvec/intervals", realvec(), ctrl_intervals_);
  addControl("mrs_realvec/selections", realvec(), ctrl_selections_);
  addControl("mrs_realvec/segmentation", realvec(), ctrl_segmentation_);
  addControl("mrs_realvec/time", realvec(), ctrl_time_);
  addControl("mrs_realvec/freq", realvec(), ctrl_freq_);
  addControl("mrs_real/lowFreq", 40.0, ctrl_lowFreq_);
  addControl("mrs_real/highFreq", 220.0, ctrl_highFreq_);
  addControl("mrs_real/rootFreq", 110.0, ctrl_rootFreq_);
  addControl("mrs_real/totalDistance", 0.0, ctrl_totalDistance_);
  addControl("mrs_string/mode", "estimate", ctrl_mode_);
  addControl("mrs_realvec/templates", realvec(), ctrl_templates_);
  addControl("mrs_natural/calcCovMatrix", 0, ctrl_calcCovMatrix_);
  addControl("mrs_realvec/covMatrix", realvec(), ctrl_covMatrix_);
  addControl("mrs_real/stdDev", 1.0, ctrl_stdDev_);
  addControl("mrs_string/normalize", "none", ctrl_normalize_);
  addControl("mrs_realvec/distance", realvec(), ctrl_distance_);
}

void MatchBassModel::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  mrs_natural i, j, o;
  ostringstream oss;
  realvec tmpvec;
  (void) sender;  //suppress warning of unused parameter(s)
  // get value from controller
  K_ = ctrl_nTemplates_->to<mrs_natural>();
  I_ = ctrl_nDevision_->to<mrs_natural>();
  seg_ = ctrl_segmentation_->to<mrs_realvec>();
  time_ = ctrl_time_->to<mrs_realvec>();
  freq_ = ctrl_freq_->to<mrs_realvec>();
  lowFreq_ = ctrl_lowFreq_->to<mrs_real>();
  highFreq_ = ctrl_highFreq_->to<mrs_real>();
  rootFreq_ = ctrl_rootFreq_->to<mrs_real>();
  templates_ = ctrl_templates_->to<mrs_realvec>();

  // check size
  ctrl_inSamples_->setValue(time_.getSize());
  ctrl_inObservations_->setValue(freq_.getSize());
  ctrl_onObservations_->setValue(inObservations_, NOUPDATE);
  ctrl_onSamples_->setValue(inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(inSamples_, NOUPDATE);
  for(o=0; o<inObservations_; o++)
    oss << "MatchBassModel_" << o << ",";
  ctrl_onObsNames_->setValue(oss.str(), NOUPDATE);

  // calculate log frequency
  logFreq_.create(freq_.getSize());
  for(i=0; i<logFreq_.getSize(); ++i) {
    logFreq_(i) = log(lowFreq_)+(log(highFreq_)-log(lowFreq_))/(double)(logFreq_.getSize()-1)*(double)i;
  }
  // calculate start vector
  start_.create(seg_.getSize());
  i=0;
  j=0;
  while(i<inSamples_ && j<seg_.getSize()) {
    if(seg_(j) <= time_(i)) {
      start_(j) = i;
      j++;
    } else {
      ++i;
    }
  }
  if(j<seg_.getSize()) {
    start_.stretch(j+1);
    start_(j) = i;
  }
  i=0;
  while(freq_(i) < rootFreq_ && i<inObservations_) {
    ++i;
  }
  rootBin_ = i;
  i=0;
  while(freq_(i) < lowFreq_ && i<inObservations_) {
    ++i;
  }
  rootMin_ = i;// - rootBin_;
  i=0;
  while(freq_(i) < highFreq_ && i<inObservations_) {
    ++i;
  }
  rootMax_ = i;// - rootBin_;
  // memory allocation
  invec_.create(rootMax_-rootMin_, inSamples_);
  d_.create(seg_.getSize()-1);
  //for(i=0; i<seg_.getSize()-1; ++i){
  //d_(i) = -rootBin_;
  //}
  ctrl_intervals_->setValue(d_);
  k_.create(seg_.getSize()-1);
  costVector_.create(rootMax_-rootMin_,K_);
  distance_.create(K_,seg_.getSize()-1);

  child_count_t child_count = marsystems_.size();
  if(child_count == 1 && inSamples_ > 0) {
    // configure the metric child MarSystem:
    // the input to metric are the two vectors to process stacked vertically
    i_featVec_.create(rootMax_-rootMin_, I_);
    j_featVec_.create((rootMax_-rootMin_)*2, I_*K_);
    stackedFeatVecs_.create((rootMax_-rootMin_)*2,1);
    marsystems_[0]->setctrl("mrs_natural/inObservations", stackedFeatVecs_.getRows());
    marsystems_[0]->setctrl("mrs_natural/inSamples", 1);
    marsystems_[0]->setctrl("mrs_real/israte", ctrl_israte_->to<mrs_real>());
    oss.clear();
    for(i=0; i<2; ++i) {
      for(o=0; o<rootMax_-rootMin_; o++) {
        oss << "MatchBassModel_" << o << ",";
      }
    }
    marsystems_[0]->setctrl("mrs_string/inObsNames", oss.str());
    marsystems_[0]->update();

    // link covMatrix control
    MarControlPtr ctrl_childCovMat = marsystems_[0]->getctrl("mrs_realvec/covMatrix");
    if(!ctrl_childCovMat.isInvalid())
      ctrl_childCovMat->linkTo(ctrl_covMatrix_);
    metricResult_.create(1,1);
    if(marsystems_[0]->getctrl("mrs_natural/onObservations") != 1 ||
        marsystems_[0]->getctrl("mrs_natural/onSamples") != 1) {
      MRSWARN("MatchBassModel:myUpdate - invalid Child Metric MarSystem (does not output a real value)!");
    }
  } else if(child_count > 1) {
    MRSWARN("MatchBassModel:myUpdate - more than one children MarSystem exist! Only one MarSystem should be added as a metric!");
  }

}

void
MatchBassModel::myProcess(realvec& in, realvec& out)
{
  child_count_t child_count = marsystems_.size();
  mrs_natural i, j, k, l, m, d;
  mrs_real tmpreal, min;
  realvec covMatrix, tmpvec;

  if(inSamples_ > 0) {
    if(child_count == 1) {
      // copy input realvec to output realvec
      for(i=0; i<inSamples_; ++i) {
        for(j=0; j<inObservations_; j++) {
          out(j,i) = in(j,i);
        }
      }

      // normalize features if necessary
      for(i=0; i<I_*K_; ++i) {
        for(j=0; j<(rootMax_-rootMin_)*2; j++) {
          j_featVec_(j,i) = templates_(j,i);
        }
      }
      for(i=0; i<inSamples_; ++i) {
        for(j=rootMin_; j<rootMax_; j++) {
          invec_(j-rootMin_, i) = in(j,i);
        }
      }
      if(ctrl_normalize_->to<mrs_string>() == "MinMax") {
        invec_.normObsMinMax();  // (x - min)/(max - min)
        j_featVec_.normObsMinMax();
      } else if(ctrl_normalize_->to<mrs_string>() == "MeanStd") {
        invec_.normObs();  // (x - mean)/std
        j_featVec_.normObs();
      }
      distance_.stretch(K_,seg_.getSize()-1);

      // calculate the Covariance Matrix from the inputm, in case
      if(ctrl_calcCovMatrix_->to<mrs_natural>() & MatchBassModel::fixedStdDev) {
        covMatrix.create(rootMax_-rootMin_, rootMax_-rootMin_);
        tmpreal = ctrl_stdDev_->to<mrs_real>();
        tmpreal *= tmpreal;
        for(i=0; i<rootMax_-rootMin_; ++i) {
          covMatrix(i,i) = tmpreal;
        }
      } else if(ctrl_calcCovMatrix_->to<mrs_natural>() & MatchBassModel::diagCovMatrix) {
        covMatrix.create(rootMax_-rootMin_, rootMax_-rootMin_);
        invec_.varObs(tmpvec);
        for(i=0; i<rootMax_-rootMin_; ++i) {
          covMatrix(i,i) = tmpvec(i);
        }
      } else if(ctrl_calcCovMatrix_->to<mrs_natural>() & MatchBassModel::fullCovMatrix) {
        covMatrix.create(rootMax_-rootMin_, rootMax_-rootMin_);
        invec_.covariance(covMatrix);
      } else if(ctrl_calcCovMatrix_->to<mrs_natural>() & MatchBassModel::noCovMatrix) {
        ctrl_covMatrix_->setValue(realvec());
      }

      totaldis_ = 0.0;
      tmpvec.stretch(rootMax_-rootMin_, I_);
      for(j=0; j<(int)start_.getSize()-1; j++) {
        tmpreal = 0.0;
        for(i=0; i<rootMax_-rootMin_; ++i) {
          for(k=0; k<K_; k++) {
            costVector_(i,k) = 0;
          }
        }
        for(i=0; i<I_; ++i) {
          for(l=0; l<rootMax_-rootMin_; l++) {
            i_featVec_(l,i) = 0;
            tmpvec(l,i) = 0;
          }
        }
        for(i=0; i<I_; ++i) {
          for(m=(mrs_natural)(((double)i/I_*(start_(j+1)-start_(j)))+start_(j)); m<(int)((double)(i+1)/I_*(start_(j+1)-start_(j)))+start_(j); m++) {
            for(l=0; l<rootMax_-rootMin_; l++) {
              i_featVec_(l,i) += invec_(l, m);
              tmpvec(l,i) ++;
            }
          }
        }
        for(i=0; i<I_; ++i) {
          for(l=0; l<rootMax_-rootMin_; l++) {
            if(tmpvec(l,i) > 0) {
              i_featVec_(l,i) /= tmpvec(l,i);
            }
          }
        }
        for(k=0; k<K_; k++) {
          for(i=0; i<I_; ++i) {
            for(l=0; l<rootMax_-rootMin_; l++) {
              stackedFeatVecs_(l,0) = i_featVec_(l,i);
            }
            for(d=0; d<rootMax_-rootMin_; d++) {
              for(l=0; l<rootMax_-rootMin_; l++) {
                stackedFeatVecs_(l+rootMax_-rootMin_,0) = j_featVec_(l+rootMax_-rootMin_-d,k*I_+i);
              }
              marsystems_[0]->process(stackedFeatVecs_, metricResult_);
              costVector_(d, k) += metricResult_(0,0);
            }
          }
        }
        min = costVector_(0,0);
        d_(j) = 0;
        k_(j) = 0;
        for(d=0; d<rootMax_-rootMin_; d++) {
          for(k=0; k<K_; k++) {
            if(min>costVector_(d, k)) {
              min = costVector_(d,k);
              d_(j) = d;
              k_(j) = k;
            }
          }
        }
        totaldis_ += min;
        for(k=0; k<K_; k++) {
          min = costVector_(0,k);
          for(d=0; d<rootMax_-rootMin_; d++) {
            if(min > costVector_(d,k)) {
              min = costVector_(d,k);
            }
          }
          distance_(k,j) = min;
        }
      }
      ctrl_intervals_->setValue(d_);
      ctrl_selections_->setValue(k_);
      ctrl_totalDistance_->setValue(totaldis_);
      ctrl_distance_->setValue(distance_);
    } else {
      if(child_count == 0) {
        MRSWARN("MatchBassModel::myProcess - no Child Metric MarSystem added");
      } else {
        MRSWARN("MatchBassModel::myProcess - more than on Child MarSystem exists (i.e. invalid metric)");
      }
    }
  } else {
    cout << "Not ready" << endl;
  }
}
