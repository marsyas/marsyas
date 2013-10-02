#ifndef MARSYAS_DTWWD_H
#define MARSYAS_DTWWD_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
   \class DTWWD
   \ingroup MarSystem
   \brief Calculate Alignment from a SimilarityMatrix as input with chroma delta information

   Controls:
   - \b mrs_string/mode        [w] : sets the type of the way in calculation: normal, OnePass, ....
   - \b mrs_string/localPath   [w] : sets the type of the local path: normal, diagonal, ....
   - \b mrs_string/startPos    [w] : sets the start position of the alignment: zero, lowest, ....
   - \b mrs_string/lastpos     [w] : sets the last position of the alignment: end, lowest, ....
   - \b mrs_real/totalDistance [r] : total distance calculated in DTWWD
   - \b mrs_realvec/sizes      [w] : sets the sizes of paralleled matrix for OnePass
   - \b mrs_bool/weight        [w] : enables/disables weighting the local path
   - ¥b mrs_realvec/delta      [w] : sets the delta chroma information (can be anything else)
   - ¥b mrs_real/deltaWeight   [w] : sets the weight value for delta chroma information

   Output size:
   When the control mode is set as normal, output size should be inObservations+inSamples.
   When the contorl mode is set as OnePass, output size should be 3*inSamples.

*/

class DTWWD: public MarSystem
{
private:
  mrs_real totalDis_;
  realvec sizes_;
  realvec costMatrix_;
  realvec matrixPos_;
  realvec alignment_;
  realvec beginPos_;
  realvec endPos_;
  realvec delta_;
  mrs_real weight_;

  MarControlPtr ctrl_mode_;
  MarControlPtr ctrl_localPath_;
  MarControlPtr ctrl_startPos_;
  MarControlPtr ctrl_lastPos_;
  MarControlPtr ctrl_totalDis_;
  MarControlPtr ctrl_sizes_;
  MarControlPtr ctrl_weight_;
  MarControlPtr ctrl_delta_;
  MarControlPtr ctrl_deltaWeight_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  DTWWD(std::string name);
  DTWWD(const DTWWD& a);
  ~DTWWD();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};
}

#endif
