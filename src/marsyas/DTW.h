#ifndef MARSYAS_DTW_H
#define MARSYAS_DTW_H

#include "MarSystem.h"

namespace Marsyas
{
  /**
     \class DTW
     \ingroup MarSystem
     \breaf Calculate Alignment from a SimilarityMatrix at input

     Controls:
     - \b mrs_string/mode        [w] : sets the type of the way in calculation: normal, OnePass, ....
     - \b mrs_string/localPath   [w] : sets the type of the local path: normal, diagonal, ....
     - \b mrs_string/startPos    [w] : sets the start position of the alignment: zero, lowest, ....
     - \b mrs_string/lastpos     [w] : sets the last position of the alignment: end, lowest, ....
     - \b mrs_real/totalDistance [r] : total distance calculated in DTW
     - \b mrs_realvec/sizes      [w] : sets the sizes of paralleled matrix for OnePass
     - \b mrs_bool/weight        [w] : enables/disables weighting the local path 

     Output size:
     When the control mode is set as normal, output size should be inObservations+inSamples.
     When the contorl mode is set as OnePass, output size should be 3*inSamples.

  */

  class DTW: public MarSystem
  {
  private:
    mrs_real totalDis_;
    realvec sizes_;
    realvec costMatrix_;
    realvec matrixPos_;
    realvec alignment_;
    realvec beginPos_;
    realvec endPos_;
    
    MarControlPtr ctrl_mode_;
    MarControlPtr ctrl_localPath_;
    MarControlPtr ctrl_startPos_;
    MarControlPtr ctrl_lastPos_;
    MarControlPtr ctrl_totalDis_;
    MarControlPtr ctrl_sizes_;
    MarControlPtr ctrl_weight_;

    void addControls();
    void myUpdate(MarControlPtr sender);

  public:
    DTW(std::string name);
    DTW(const DTW& a);
    ~DTW();
    
    MarSystem* clone() const;
    
    void myProcess(realvec& in, realvec& out);
  };
}

#endif
