/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MARSYAS_FANOUT_H
#define MARSYAS_FANOUT_H

#include "MarSystem.h"

namespace Marsyas
{
/** 
\class Fanout
\ingroup Composites
\brief Fanout of MarSystem objects

Combines a series of MarSystem objects to a single MarSystem 
corresponding to executing the System objects one after the other 
in sequence. 

Controls:
- \b mrs_natural/disable [w] : passes in the number of the child
  MarSystem to disable.
- \b mrs_natural/enable	[w] : pass in the number of the child MarSystem to
  enable.  (by default all children are enabled)
- \b mrs_bool/probe	: deprecated.

\deprecated if Fanout mrs_bool/probe really is depreciated, chop it.
UPDATE: lmartins said that _all_ probes are deprecated.  Chop them all
after the merge.
*/


class Fanout: public MarSystem
{
private:
  void addControls();
	void myUpdate(MarControlPtr sender);
  void deleteSlices();  

  realvec enabled_;
  realvec localIndices_;
  
  bool probe_;
  mrs_natural disable_;
	mrs_natural enable_;
  
public:
  Fanout(std::string name);
  ~Fanout();
  MarSystem* clone() const;    
 
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

	

	
