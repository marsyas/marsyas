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

#ifndef MARSYAS_PLOTSINK_H
#define MARSYAS_PLOTSINK_H

#include "MarSystem.h"

namespace Marsyas
{
/**
\class PlotSink
\ingroup IO
\brief Text output sink
Output input data at each tick to sequence numbered text files,
to Marsyas messages (default = stdout), and as a MATLAB plot 
if MATLAB engine is being used.

Controls:
- \b mrs_bool/sequence	: when true, this control
activates the writing of sequence numbered files containing the
input data at each tick.
- \b mrs_string/filename	: base filename used for the output 
sequence numbered files.
- \b mrs_string/separator	: separator character/string used when
outputting input data using Marsyas messages (i.e. MRSMSG() - by default
messages are sent to the stdout).
*/


	class PlotSink: public MarSystem
	{
	private:
		mrs_natural counter_;

		MarControlPtr ctrl_separator_;
		MarControlPtr ctrl_sequence_;
		MarControlPtr ctrl_filename_;

		void addControls();

	public:
		PlotSink(std::string name);
		PlotSink(const PlotSink& a);
		~PlotSink();

		MarSystem* clone() const;  

		void myProcess(realvec& in, realvec& out);
	};

}//namespace Marsyas


#endif








