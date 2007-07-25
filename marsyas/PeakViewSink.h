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

/** 
\class PeakViewSink
\ingroup IO
\brief This MarSystem accumulates realvecs with peak information (using peakView) 
arriving at its input and when receiving a "done" control trigger, saves the peak data
to the specified file in .peak format. 
*/

#ifndef MARSYAS_PEAKVIEWSINK_H
#define MARSYAS_PEAKVIEWSINK_H

#include "MarSystem.h"	

#include <fstream>
#include <iostream>

using namespace std;

namespace Marsyas
{

	class PeakViewSink: public MarSystem
	{
	private: 
		mrs_natural count_;
		string tmpFilename_;
		ofstream tmpFile_;
		realvec accumData_;

		MarControlPtr ctrl_accumulate2Disk_;
		MarControlPtr ctrl_fs_;
		MarControlPtr ctrl_frameSize_;
		MarControlPtr ctrl_done_;
		MarControlPtr ctrl_filename_;

		void done();
		void addControls();
		void myUpdate(MarControlPtr sender);

	public:
		PeakViewSink(std::string name);
		PeakViewSink(const PeakViewSink& a);
		~PeakViewSink();
		MarSystem* clone() const;  

		void myProcess(realvec& in, realvec& out);
	};

} //namespace Marsyas

#endif // MARSYAS_PEAKVIEWSINK_H

