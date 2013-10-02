/*
 *  ChromaScale.h
 *  testMood
 *
 *  Created by tsunoo on 09/09/10.
 *  Copyright 2009 Emiru Tsunoo. All rights reserved.
 *
 */

#ifndef MARSYAS_CHROMASCALE_H
#define MARSYAS_CHROMASCALE_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup MarSystem
\brief Scale related features of Chroma vector

Controls:

*/

class ChromaScale: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  ChromaScale(std::string name);
  ChromaScale(const ChromaScale& a);
  ~ChromaScale();

  MarSystem* clone() const;

  void myProcess(realvec& in, realvec& out);
};
}

#endif
