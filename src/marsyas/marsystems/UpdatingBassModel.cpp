#include "UpdatingBassModel.h"

using namespace std;
using namespace Marsyas;

UpdatingBassModel::UpdatingBassModel(mrs_string name):MarSystem("MatchBassModel", name)
{
  addControls();
}

UpdatingBassModel::UpdatingBassModel(const UpdatingBassModel& a):MarSystem(a)
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
  ctrl_templates_ = getctrl("mrs_realvec/templates");
  ctrl_counts_ = getctrl("mrs_realvec/counts");
}

UpdatingBassModel::~UpdatingBassModel()
{
}

MarSystem*
UpdatingBassModel::clone() const
{
  return new UpdatingBassModel(*this);
}

void
UpdatingBassModel::addControls()
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
  addControl("mrs_realvec/templates", realvec(), ctrl_templates_);
  addControl("mrs_realvec/counts", realvec(), ctrl_counts_);
}

void UpdatingBassModel::myUpdate(MarControlPtr sender)
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
  d_ = ctrl_intervals_->to<mrs_realvec>();
  k_ = ctrl_selections_->to<mrs_realvec>();
  templates_ = ctrl_templates_->to<mrs_realvec>();
  counts_ = ctrl_counts_->to<mrs_realvec>();

  // check size
  ctrl_inSamples_->setValue(time_.getSize());
  ctrl_inObservations_->setValue(freq_.getSize());
  ctrl_onObservations_->setValue(inObservations_, NOUPDATE);
  ctrl_onSamples_->setValue(inSamples_, NOUPDATE);
  ctrl_osrate_->setValue(inSamples_, NOUPDATE);
  for(o=0; o<inObservations_; o++)
    oss << "UpdatingBassModel_" << o << ",";
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
  if(templates_.getSize() <= 0) {
    templates_.create((rootMax_-rootMin_)*2,K_*I_);
  }
  if(counts_.getSize() <= 0) {
    counts_.create((rootMax_-rootMin_)*2,K_);
  }

}

void
UpdatingBassModel::myProcess(realvec& in, realvec& out)
{
  mrs_natural i, j, k, l, m, tmpnatural;
  mrs_real tmpreal;
  realvec covMatrix, tmpvec;
  if(inSamples_ > 0) {
    // copy input realvec to output realvec
    for(i=0; i<inSamples_; ++i) {
      for(j=0; j<inObservations_; j++) {
        out(j,i) = in(j,i);
      }
    }
    tmpvec = templates_;

    for(k=0; k<K_; k++) {
      for(l=0; l<counts_.getRows(); l++) {
        for(i=0; i<I_; ++i) {
          templates_(l, k*I_+i) *= counts_(l,k);
        }
      }
    }

    // update templates realvec
    for(j=0; j< (int)start_.getSize()-1; j++) {
      for(l=0; l<rootMax_-rootMin_; l++) {
        for(i=0; i<I_; ++i) {
          tmpreal = 0.0;
          tmpnatural = 0;
          for(m=(int)(((double)i/I_*(start_(j+1)-start_(j)))+start_(j)); m<(int)((double)(i+1)/I_*(start_(j+1)-start_(j)))+start_(j); m++) {
            tmpreal += in(rootMin_+l,m);
            tmpnatural ++;
          }
          if(tmpnatural > 0) {
            tmpreal /= (mrs_real)tmpnatural;
          }
          templates_((mrs_natural)(l+(rootMax_-rootMin_)-d_(j)),(mrs_natural)((k_(j)*I_+i))) += tmpreal;
        }
        counts_((mrs_natural)(l+(rootMax_-rootMin_)-d_(j)),(mrs_natural)k_(j))++;
      }
    }
    for(l=0; l<(rootMax_-rootMin_)*2; l++) {
      for(k=0; k<K_; k++) {
        if(counts_(l,k) > 0) {
          for(i=0; i<I_; ++i) {
            templates_(l,k*I_+i) /= counts_(l,k);
          }
        } else {
          for(i=0; i<I_; ++i) {
            templates_(l,k*I_+i) = tmpvec(l,k*I_+i);
          }
        }
      }
    }
    ctrl_templates_->setValue(templates_);
    ctrl_counts_->setValue(counts_);
  } else {
    cout << "Not ready" << endl;
  }
}
