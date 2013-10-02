#ifndef MARSYAS_PHISEMSOURCE_H
#define MARSYAS_PHISEMSOURCE_H

#include <marsyas/system/MarSystem.h>

#include <cstdlib>

namespace Marsyas
{

class PhiSEMSource: public MarSystem
{
private:
  MarControlPtr numObjects_;
  MarControlPtr systemDecay_;
  MarControlPtr soundDecay_;
  MarControlPtr baseGain_;


  mrs_real shakeEnergy_;
  mrs_real soundLevel_;
  mrs_real gain_;
  mrs_real temp_;
  int	 sample_;

  void addControls();
  void myUpdate(MarControlPtr sender);

  mrs_real noiseTick();
  mrs_real randomFloat(mrs_real max);
  int randomInt(int max);

  mrs_real computeSample();

public:
  static const mrs_real MIN_ENERGY;
  static const mrs_real MAX_ENERGY;

  PhiSEMSource(std::string name);
  PhiSEMSource(const PhiSEMSource& source);
  ~PhiSEMSource();
  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif /* !MARSYAS_PHISEMSOURCE_H */
