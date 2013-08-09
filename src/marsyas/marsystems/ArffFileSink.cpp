/*
** Copyright (C) 2009 Stefaan Lippens <soxofaan@gmail.com>
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

#include "ArffFileSink.h"

#include <fstream>
#include <iomanip>


using std::ostringstream;
using std::ofstream;
using std::ios_base;
using std::endl;
using std::vector;
using std::fixed;
using std::setprecision;


using namespace Marsyas;

ArffFileSink::ArffFileSink(mrs_string name) : MarSystem("ArffFileSink", name)
{
  os_ = NULL;
  filename_ = "";
  decimationCounter = 0;
  addControls();
}

ArffFileSink::ArffFileSink(const ArffFileSink& a) : MarSystem(a)
{
  os_ = NULL;
  filename_ = "";
  decimationCounter = 0;

  /// All member MarControlPtr have to be explicitly reassigned in
  /// the copy constructor.
  ctrl_floatPrecision_ = getControl("mrs_natural/floatPrecision");
  ctrl_decimationFactor_ = getControl("mrs_natural/decimationFactor");
  ctrl_filename_ = getControl("mrs_string/filename");
}


ArffFileSink::~ArffFileSink()
{
  closeOutput();
}

MarSystem*
ArffFileSink::clone() const
{
  return new ArffFileSink(*this);
}

void
ArffFileSink::addControls()
{
  /// Add any specific controls needed by this MarSystem.
  addControl("mrs_natural/floatPrecision", 6, ctrl_floatPrecision_);
  addControl("mrs_natural/decimationFactor", 1, ctrl_decimationFactor_);
  addControl("mrs_string/filename", "data.arff", ctrl_filename_);
}

void
ArffFileSink::myUpdate(MarControlPtr sender)
{
  /// Use the default MarSystem setup with equal input/output stream format.
  MarSystem::myUpdate(sender);

  // Cache float precision and decimation for usage in myProcess().
  floatPrecision_ = ctrl_floatPrecision_->to<mrs_natural>();
  decimationFactor_ = ctrl_decimationFactor_->to<mrs_natural>();
  // Sanitize decimation factor.
  decimationFactor_ = decimationFactor_ == 0 ? 1 : decimationFactor_;
  // Reset decimation counter variable.
  decimationCounter = 0;

}

/// Helper function for closing the output (if needed).
void
ArffFileSink::closeOutput()
{
  if (os_ != NULL)
  {
    os_->flush();
    os_->close();
    delete os_;
  }
  filename_ = "";
}

/// Prepare the output stream: open file and write header if needed.
void
ArffFileSink::prepareOutput()
{
  // If internal file name differs from the file name we should be writing to:
  // close previous file, open a new file and write header.
  if (filename_ != ctrl_filename_->to<mrs_string>())
  {
    // Close current file.
    closeOutput();
    // Open a new output stream for the new file.
    filename_ = ctrl_filename_->to<mrs_string>();
    os_ = new ofstream;
    os_->open(filename_.c_str());
    if (os_->fail()) {
      // \todo make this cleaner, e.g. by creating and using a dedicated
      // exception for IO/file errors.
      ostringstream oss;
      oss << "[Error in " << __FILE__ << ":" << __LINE__ << "] "
          << "could not open file '" << filename_ << "' for writing.";
      throw ios_base::failure(oss.str());
    }
    // Write ARFF header.
    writeArffHeader();
  }
}


void
ArffFileSink::writeArffHeader()
{
  // General header stuff.
  (*os_) << "% ARFF file Created by Marsyas (ArffFileSink)" << endl;
  (*os_) << "@relation " << filename_ << endl;

  // Print the attributes.
  // Get and output the observation names.
  mrs_string onObsNames = ctrl_onObsNames_->to<mrs_string>();
  vector<mrs_string> attributeNames = stringSplit(onObsNames, ",");
  const mrs_natural onObservations = ctrl_onObservations_->to<mrs_natural>();
  // Print the observation names, if available.
  for (mrs_natural i = 0; i < onObservations; ++i)
  {
    if (i < (mrs_natural)attributeNames.size() && !attributeNames[i].empty())
    {
      (*os_ ) << "@attribute " << attributeNames[i] << " real" << endl;
    }
    else
    {
      (*os_ ) << "@attribute " << "untitled" << i << " real" << endl;
    }
  }

  // End of header, we are ready now for outputting the data.
  (*os_) << "\n@data" << endl;
}

void
ArffFileSink::myProcess(realvec& in, realvec& out)
{
  mrs_natural o,t;
  // Make sure we can write to the output stream.
  prepareOutput();

  // Copy input to output.
  for (t = 0; t < inSamples_; t++)
  {
    for (o = 0; o < inObservations_; o++)
    {
      out(o, t) = in(o, t);
    }
  }

  if (!ctrl_mute_->isTrue()) {
    // Write data to file.
    for (t = 0; t < inSamples_; t++)
    {
      if (decimationCounter % decimationFactor_ == 0)
      {
        for (o = 0; o < inObservations_; o++)
        {
          (*os_) << fixed << setprecision(floatPrecision_) << out(o, t);
          if (o < inObservations_ - 1)
          {
            (*os_) << ",";
          }
        }
        (*os_) << endl;
      }
      decimationCounter ++;
    }
  }
}
