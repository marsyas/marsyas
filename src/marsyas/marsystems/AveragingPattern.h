#ifndef MARSYAS_AVERAGINGPATTERN_H
#define MARSYAS_AVERAGINGPATTERN_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
   \class AveragingPattern
   \ingroup MarSystem
   \brief Calculate Averages accoding to the alignment

   Controls:
   - \b mrs_realvec/sizes       [w] : sets the sizes of paralleled matrix
   - \b mrs_realvec/alignment   [w] : sets the alignment calculated by DTW
   - \b mrs_realvec/counts      [r] : numbers of each templates included in a piece
   - \b mrs_bool/input          [w] : enables/disables including input data in the output, i.e., true->output should be rows:input.rows, cols:inputcols; false->output should be rows:(sizes.getSize-1)*dim, cols:max(sizes(i))
   - \b mrs_realvec/countVector [w] : sets the vector of the numbers of elements for averaging per each frame
   - \b mrs_bool/setCountVector [w] : enables/disables setting countVector

*/

class


  AveragingPattern: public MarSystem
{
private:
  realvec counts_;
  realvec sizes_;
  realvec countvector_;
  realvec average_;
  realvec beginPos_;
  realvec endPos_;

  MarControlPtr ctrl_sizes_;
  MarControlPtr ctrl_alignment_;
  MarControlPtr ctrl_counts_;
  MarControlPtr ctrl_input_;
  MarControlPtr ctrl_countVector_;
  MarControlPtr ctrl_setCountVector_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  AveragingPattern(std::string name);
  AveragingPattern(const AveragingPattern& a);
  ~AveragingPattern();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};
}

#endif
