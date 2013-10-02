#ifndef MARSYAS_UPDATINGBASSMODEL_H
#define MARSYAS_UPDATINGBASSMODEL_H

#include <marsyas/system/MarSystem.h>
#include <cstdlib>
#include <ctime>

namespace Marsyas
{
/**
   \ingroup MarSystem
   \brief Updating bass-line model for BassMap

   Controls:
   - \b mrs_natural/nTemplates    [w] : sets number of templates
   - \b mrs_natural/nDevision     [w] : sets number of devision of measure
   - \b mrs_realvec/segmentation  [w] : sets measure segmentation
   - \b mrs_realvec/time          [w] : sets corresponding to time (ms)
   - \b mrs_realvec/freq          [w] : sets corresponding to frequency (Hz)
   - \b mrs_realvec/templates     [r] : learned templates line up horizontal
   - \b mrs_realvec/counts        [r] : count up the input bin to average
   - \b mrs_real/lowFreq          [w] : sets the lowest frequency to look at
   - \b mrs_real/highFreq         [w] : sets the highest frequency to look at
   - \b mrs_real/rootFreq         [w] : sets the frequency of root note of the templates
   - \b mrs_realvec/intervals     [r] : intervals of each measures
   - \b mrs_realvec/selections    [r] : selections which of the templates

*/
class UpdatingBassModel: public MarSystem
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
  realvec counts_;
  realvec templates_;
  mrs_real lowFreq_;
  mrs_real highFreq_;
  mrs_real rootFreq_;

  MarControlPtr ctrl_nTemplates_;
  MarControlPtr ctrl_nDevision_;
  MarControlPtr ctrl_segmentation_;
  MarControlPtr ctrl_time_;
  MarControlPtr ctrl_freq_;
  MarControlPtr ctrl_templates_;
  MarControlPtr ctrl_counts_;
  MarControlPtr ctrl_lowFreq_;
  MarControlPtr ctrl_highFreq_;
  MarControlPtr ctrl_rootFreq_;
  MarControlPtr ctrl_intervals_;
  MarControlPtr ctrl_selections_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  UpdatingBassModel(std::string name);
  UpdatingBassModel(const UpdatingBassModel& a);
  ~UpdatingBassModel();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};
}

#endif

