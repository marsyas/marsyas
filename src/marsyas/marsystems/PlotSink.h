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

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
\ingroup IO
\brief Text output sink
Output input data at each tick to sequence numbered text files,
to Marsyas messages (default = stdout), and as a MATLAB plot
if MATLAB engine is being used.

Controls:
- \b mrs_bool/sequence [w]	: when true, this control
activates the writing of sequence numbered files containing the
input data at each tick.
- \b mrs_bool/single_file [w]	: when true, data is written to a single file in addition to multiple numbered files (if mrs_bool/sequence is true).
- \b mrs_bool/no_ticks [w]	: when this and single_file are true, data is written to a single file with no line breaks between ticks.
- \b mrs_string/filename [w]	: base filename used for the output
sequence numbered files.

- \b mrs_bool/messages [w] : when true, activates the writing of the
input data using Marsyas messages (i.e. MRSMSG() - - by default messages are sent to the stdout)
- \b mrs_string/separator	[w] : separator character/string used when
outputting input data using Marsyas messages (i.e. MRSMSG() - by default
messages are sent to the stdout).

- \b mrs_bool/matlab [w]: when true, input data is sent to MATLAB using
MATLAB engine calls (if enabled), to a MATLAB variable of the form "type_name_indata".
- \b mrs_string/matlabCommand [w] : MATLAB command to be evaluated at each process
tick (after sending input data to MATLAB). By default it is set to "plot(type_name_indata);".
*/


class PlotSink: public MarSystem
{
private:
  mrs_natural counter_;
  std::ofstream *single_file_;
  std::string filename_;

  MarControlPtr ctrl_messages_;
  MarControlPtr ctrl_separator_;
  MarControlPtr ctrl_sequence_;
  MarControlPtr ctrl_single_file_;
  MarControlPtr ctrl_no_ticks_;
  MarControlPtr ctrl_filename_;
  MarControlPtr ctrl_matlab_;
  MarControlPtr ctrl_matlabCommand_;

  void addControls();

public:
  PlotSink(std::string name);
  PlotSink(const PlotSink& a);
  ~PlotSink();

  MarSystem* clone() const;

  void myUpdate(MarControlPtr sender);
  void myProcess(realvec& in, realvec& out);
};

}//namespace Marsyas


#endif








