#ifndef MARSYAS_PHISEMFILTER_H
#define MARSYAS_PHISEMFILTER_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{

typedef struct {
  mrs_real zero;
  mrs_real one;
} mrs_realpair;

class PhiSEMFilter: public MarSystem
{
private:
  MarControlPtr numFilters_;
  realvec resVec_;
  realvec freqVec_;
  mrs_realpair* coeffs_;
  mrs_realpair* output_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  PhiSEMFilter(std::string name);
  ~PhiSEMFilter();
  MarSystem* clone() const;
  void myProcess(realvec& in, realvec& out);

};

}//namespace Marsyas

#endif /* !MARSYAS_PHISEMFILTER_H */
