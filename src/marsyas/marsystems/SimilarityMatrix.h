#ifndef MARSYAS_SIMILARITYMATRIX2_H
#define MARSYAS_SIMILARITYMATRIX2_H

#include <marsyas/system/MarSystem.h>
#include "Metric.h"
#include <vector>

namespace Marsyas
{
/**
   \class SimilarityMatrix
   \ingroup MarSystem
   \brief Calculate SimilarityMatrix of a (parallel) set of feature vectors at input

   Controls:
   - \b mrs_bool/calcCovMatrix [w] : enables/disables calculation of covariance matrix from input data
   - \b mrs_realvec/covMatrix  [r] : covariance matrix from input data (if disabled, will contain an empty realvec)
   - \b mrs_realvec/sizes      [w] : sizes of each paralleled feature vectors
   - \b mrs_string/normalize   [w] : sets the type of input data normalization to be used: none, MinMax, or MinStd.

 */

class SimilarityMatrix: public MarSystem
{
private:
  realvec i_featVec_;
  realvec j_featVec_;
  realvec stackedFeatVecs_;
  realvec metricResult_;

  realvec sizes_;
  realvec vars_;
  realvec covMatrix_;
  std::vector<realvec> invecs_;

  MarControlPtr ctrl_stdDev_;
  MarControlPtr ctrl_covMatrix_;
  MarControlPtr ctrl_calcCovMatrix_;
  MarControlPtr ctrl_normalize_;
  MarControlPtr ctrl_sizes_;
  //    MarControlPtr ctrl_disMatrix_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  enum covMatrixType {
    noCovMatrix = 0,
    fixedStdDev = 1,
    diagCovMatrix = 2,
    fullCovMatrix = 3
  };

  SimilarityMatrix(std::string name);
  SimilarityMatrix(const SimilarityMatrix& a);
  ~SimilarityMatrix();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};
}

#endif
