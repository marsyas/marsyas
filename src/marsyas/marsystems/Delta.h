/*
 *  Delta.h
 *  testDelta
 *
 *  Created by tsunoo on 09/05/07.
 *  Copyright 2009 Emiru Tsunoo. All rights reserved.
 *
 */

#ifndef MARSYAS_DELTA_H
#define MARSYAS_DELTA_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\ingroup Processing
	\brief Delta of input sequence

	Controls:
	- \b mrs_bool/absolute [w] : selects absolute or not
	- \b mrs_bool/sum [w] : selects take sum or not
	- \b mrs_bool/normalize [w] : selects normalize or not
	- \b mrs_natural/normSize [w] : sets the size of the frame for normalizing
	- \b mrs_bool/positive [w] : selects remaining only positive value or not
*/

class Delta: public MarSystem
{
private:
  realvec delta_;
  realvec prevData_;
  realvec normVec_;
  mrs_bool absolute_;
  mrs_bool sum_;
  mrs_bool normalize_;
  mrs_natural size_;
  mrs_bool positive_;

  MarControlPtr ctrl_absolute_;
  MarControlPtr ctrl_sum_;
  MarControlPtr ctrl_normalize_;
  MarControlPtr ctrl_normSize_;
  MarControlPtr ctrl_positive_;

  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  Delta(std::string name);
  Delta(const Delta& a);
  ~Delta();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};

}
#endif


