#ifndef MARSYAS_DTW_H
#define MARSYAS_DTW_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
   \class DTW
   \ingroup MarSystem
   \brief Calculate Alignment from a SimilarityMatrix at input




as input format is expected:
in combination with the sizes control - which has to be of size n where n is the number of
sequences, and each element of the control has to be the number of vector-elements in the nth sequence -
the first 1/n part of each row containing the elements of the vectors of the first sequence
the second 1/n part containing the elements of the vectors of the second,
and so on up to nth part containing the elements of the vectors of the nth sequence.




   Controls:
   - \b mrs_string/mode        [w] : 	sets the type of the way in calculation: normal, OnePass, ....
   - \b mrs_string/localPath   [w] : 	sets the type of the local path: normal, diagonal, ....
   - \b mrs_string/startPos    [w] : 	sets the start position of the alignment: zero, lowest, ....
   - \b mrs_string/lastpos     [w] : 	sets the last position of the alignment: end, lowest, ....
   - \b mrs_real/totalDistance [r] : 	total distance calculated in DTW
   - \b mrs_realvec/sizes      [w] : 	sets the sizes of paralleled matrix for OnePass
									i.e. number of elements within each vector sequence.
									at least 2 vector sequences are expected.
									for each additional sequence after the first, a similarity matrix
									 - comparing this sequence to the fist sequence - will be computed.
									the sizes control is used
									to tell the similarity matrix where the vector seqnences end.
									(since the input is a rectangular matrix (i.e. a mrs_realvec) the rest will be ignored)

									CAUTION:	since the number of elements of the sizes control is assumed
												to be the number of seqences to be compared, the dimensionality
												of the vectors is assumed to be
												"numberOfInputRows(i.e. Observations) divided by numberOfSequences(i.e. length of the sizes control)"
												since all vectors have to be of the same dimensionality.
												e.g. the standard case of two sequences would need to have the sizes control of length 2
												with sizes(0) being the length of the fist sequence, sizes(1) being the length of the second;
												and each input observation (i.e. row) containing two input vectors:
												the first half of the row containing the elements of the vectors of the first sequence
												the second half of the row containing the elements of the vectors of the second sequence


   - \b mrs_bool/weight        [w] : 	enables/disables weighting the local path

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
