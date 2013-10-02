/*
** Copyright (C) 1998-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_WEKASINK_H
#define MARSYAS_WEKASINK_H

#include <marsyas/system/MarSystem.h>

#include <fstream>
#include <sstream>
#include <iomanip>

namespace Marsyas
{
/**
   \class WekaSink
   \ingroup IO
   \brief Output sink (text) in WEKA (ARFF) format

   Controls:
   - \b mrs_natural/precision [w] : precision of numbers to output.
   - \b mrs_string/filename [w] : name of output file.
   - \b mrs_natural/nLabels [rw] : number of labels.
   - \b mrs_natural/downsample [rw] : divides number of samples.
   - \b mrs_string/labelNames [rw] : names of labels.
   - \b mrs_bool/regression [rw] : classification or regression?
   - \b mrs_bool/putHeader [rw] : add extra information to the .arff
   - \b mrs_bool/inject    [w]  : trigger injection of  data to .arff output out of dataflow
   - \b mrs_string/comment [w]  : inject string to .arff output of of dataflow triggered by inject control
   file.
   - \b mrs_bool/onlyStable [rw] : do not write "unreliable"
     ticks to disk.  "unreliable" means ticks such as the very
     beginning of a ShiftInput before the window is filled.
   - \b mrs_bool/resetStable [w] : if you have set
     onlyStable, then set this to true whenever the input changes
     significantly (i.e. reading a new file).  This control will
     automatically be set to false in the next tick().
*/


class WekaSink: public MarSystem
{
private:
  void addControls();
  void myUpdate(MarControlPtr sender);

  /// The file name of the WEKA file we are currently write to.
  std::string filename_;

  /// The output stream to write the WEKA ARFF data to.
  std::ofstream* mos_;

  /// List of the label names.
  std::vector<std::string> labelNames_;

  /// The decimal precision to use in the floating point notation.
  mrs_natural precision_;
  mrs_natural downsample_;

  MarControlPtr ctrl_regression_;
  MarControlPtr ctrl_inject_;
  MarControlPtr ctrl_injectComment_;
  MarControlPtr ctrl_injectVector_;


  MarControlPtr ctrl_putHeader_;
  MarControlPtr ctrl_precision_;
  MarControlPtr ctrl_downsample_;
  MarControlPtr ctrl_nLabels_;
  MarControlPtr ctrl_labelNames_;
  MarControlPtr ctrl_filename_;
  MarControlPtr ctrl_currentlyPlaying_;

  MarControlPtr ctrl_onlyStable_;
  MarControlPtr ctrl_resetStable_;

  mrs_string prev_playing_;

  mrs_natural stabilizingTicks_;
  mrs_natural count_;

public:
  /// WekaSink constructor.
  WekaSink(std::string name);

  /// WekaSink copy constructor.
  WekaSink(const WekaSink& a);

  /// WekaSink desctructor.
  ~WekaSink();

  MarSystem* clone() const;

  /// Write the WEKA ARFF header to the output file.
  void putHeader(std::string inObsNames);

  void myProcess(realvec& in, realvec& out);

  // Jen's hack for MIREX 05 to annotate produced weka file
  std::ofstream* getOfstream()
  {
    return mos_;
  }

};

}//namespace Marsyas


#endif










