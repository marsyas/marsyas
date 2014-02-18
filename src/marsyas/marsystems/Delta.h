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
*/

class Delta: public MarSystem
{
private:
  realvec m_memory;

  void myUpdate(MarControlPtr sender);
  void myProcess(realvec& in, realvec& out);

public:
  Delta(std::string name);
  Delta(const Delta& a);
  MarSystem* clone() const;
};

}
#endif


