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

#ifndef MARSYAS_ARFFFILESINK_H
#define MARSYAS_ARFFFILESINK_H

#include <marsyas/system/MarSystem.h>

namespace Marsyas
{
/**
	\class ArffFileSink
	\ingroup IO
	\brief Writes slices to a file in (Weka) ARFF format.

	ArffFileSink writes the input realvec data it receives to a file in ARFF
	format (Attribute-Relation File Format). The ARFF format is relatively easy
	and consists of a simple header, describing the observation channels
	(called 'attributes' in ARFF), and a straightforward data body.

	ARFF files are typically used with the Weka machine learning software.

	ArffFileSink is basically a simple rewrite of WekaSink. The problem with
	WekaSink is that it contains a fair amount of assumptions about the usage
	and purpose of the generated Weka files, which limits the flexibility of
	WekaSink. Fixing this in WekaSink without breaking backwards compatibility
	would be very hard and messy.

	\see WekaSink

	Controls:
	- \b mrs_natural/floatPrecision [w]: the precision to use for the decimal
		notation.
	- \b mrs_natural/decimationFactor [w]: the decimation factor for data to
		file writing. Note that the flow from standard realvec input port to
		standard realvec output port is undecimated.
	- \b mrs_string/filename [w]: the name of the file to write to.
*/

class ArffFileSink: public MarSystem
{
private:

  /// Add specific controls needed by this MarSystem.
  void addControls();

  /// Reads changed controls and sets up variables if necessary.
  void myUpdate(MarControlPtr sender);

  /// The output stream to write the WEKA ARFF data to.
  std::ofstream* os_;

  /// Name of the file we are writing to.
  mrs_string filename_;

  /// Local cache for the decimal precision to use in the floating point notation.
  mrs_natural floatPrecision_;

  /// Local cache of the decimation factor for data writing.
  mrs_natural decimationFactor_;

  /// Internal counter for handling the decimation.
  mrs_natural decimationCounter;

  /// MarControl for setting the filename of the file to write to.
  MarControlPtr ctrl_filename_;

  /// MarControl for setting the floating point precision.
  MarControlPtr ctrl_floatPrecision_;

  /// MarControl for setting the decimation factor for the data writing.
  MarControlPtr ctrl_decimationFactor_;


  /// Helper function for closing the output (if needed).
  void closeOutput();

  /// Preparation of the output stream (open file, write header if necessary).
  void prepareOutput();

  /// Write ARFF header.
  void writeArffHeader();

public:
  /// ArffFileSink constructor.
  ArffFileSink(std::string name);

  /// ArffFileSink copy constructor.
  ArffFileSink(const ArffFileSink& a);

  /// ArffFileSink destructor.
  ~ArffFileSink();

  /// Implementation of the MarSystem::clone() method.
  MarSystem* clone() const;

  /// Implementation of the MarSystem::myProcess method.
  void myProcess(realvec& in, realvec& out);
};

}
//namespace Marsyas

#endif
//MARSYAS_ARFFFILESINK_H

