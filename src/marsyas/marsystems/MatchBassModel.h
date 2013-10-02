#ifndef MARSYAS_MATCHBASSMODEL_H
#define MARSYAS_MATCHBASSMODEL_H

#include <marsyas/system/MarSystem.h>
#include <cstdlib>
#include <ctime>

namespace Marsyas
{
/**
   \class MatchBassModel
   \ingroup MarSystem
   \brief Match bass-line model for BassMap

   Controls:
   - \b mrs_bool/calcCovMatrix    [w] : enables/disables calculation of covariance matrix from input data
   - \b mrs_realvec/covMatrix     [r] : covariance matrix from input data (if disabled, will contain an empty realvec)
   - \b mrs_natural/nTemplates    [w] : sets number of templates
   - \b mrs_natural/nDevision     [w] : sets number of devision of measure
   - \b mrs_realvec/segmentation  [w] : sets measure segmentation
   - \b mrs_realvec/time          [w] : sets corresponding to time (ms)
   - \b mrs_realvec/freq          [w] : sets corresponding to frequency (Hz)
   - \b mrs_real/totalDistance    [w] : total distance value
   - \b mrs_real/lowFreq          [w] : sets the lowest frequency to look at
   - \b mrs_real/highFreq         [w] : sets the highest frequency to look at
   - \b mrs_real/rootFreq         [w] : sets the frequency of root note of the templates
   - \b mrs_realvec/templates     [r] : reference templates line up horizontal
   - \b mrs_realvec/intervals     [r] : intervals of each measures
   - \b mrs_realvec/selections    [r] : selections which of the templates
   - \b mrs_string/mode           [w] : selects "initialize" or "estimate"
   - \b mrs_real/stdDev           [w] : sets standard devision
   - \b mrs_string/normalize      [w] : sets the type of input data normalization to be used: none, MinMax, or MinStd.
   - (J\(Bb mrs_realvecc/distance     [r] : sets the distance vector

*/
class MatchBassModel: public MarSystem
{
private:
  mrs_natural K_;
  mrs_natural I_;
  mrs_natural rootMin_;
  mrs_natural rootMax_;
  mrs_natural rootBin_;
  realvec d_;
  realvec k_;
  realvec seg_;
  realvec time_;
  realvec freq_;
  realvec logFreq_;
  realvec start_;
  realvec templates_;
  realvec covMatrix_;
  realvec i_featVec_;
  realvec j_featVec_;
  realvec stackedFeatVecs_;
  realvec metricResult_;
  realvec invec_;
  realvec costVector_;
  realvec distance_;
  mrs_real lowFreq_;
  mrs_real highFreq_;
  mrs_real rootFreq_;
  mrs_real totaldis_;

  MarControlPtr ctrl_nTemplates_;
  MarControlPtr ctrl_nDevision_;
  MarControlPtr ctrl_segmentation_;
  MarControlPtr ctrl_time_;
  MarControlPtr ctrl_freq_;
  MarControlPtr ctrl_totalDistance_;
  MarControlPtr ctrl_lowFreq_;
  MarControlPtr ctrl_highFreq_;
  MarControlPtr ctrl_rootFreq_;
  MarControlPtr ctrl_templates_;
  MarControlPtr ctrl_intervals_;
  MarControlPtr ctrl_selections_;
  MarControlPtr ctrl_mode_;
  MarControlPtr ctrl_calcCovMatrix_;
  MarControlPtr ctrl_covMatrix_;
  MarControlPtr ctrl_stdDev_;
  MarControlPtr ctrl_normalize_;
  MarControlPtr ctrl_distance_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  enum covMatrixType {
    noCovMatrix = 0,
    fixedStdDev = 1,
    diagCovMatrix = 2,
    fullCovMatrix = 3
  };

  MatchBassModel(std::string name);
  MatchBassModel(const MatchBassModel& a);
  ~MatchBassModel();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};
}

#endif


