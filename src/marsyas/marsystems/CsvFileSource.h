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

#ifndef MARSYAS_CSVFILESOURCE_H
#define MARSYAS_CSVFILESOURCE_H

#include <marsyas/system/MarSystem.h>

#include <cstdlib>
#include <cstring>

namespace Marsyas
{
/**
	\ingroup IO
	\brief Reads CSV (comma-separated values) text files

	Input file is assumed to begin with a header (the intended
	observation names, separated by commas).  The rest of the file
	should consist of comma-separated values.  This functions
	similarly to Inject, except that it reads from a file instead
	of from a control; this can be useful to add metadata (from a
	file) to improve the machine learning.

	Controls:
	- \b mrs_string/filename [w] : name of file to read
	- \b mrs_bool/hasData [r] : is there any data left?
	- \b mrs_natural/size [r] : filesize in samples
*/


class CsvFileSource: public MarSystem
{
private:

  std::string filename_;

  FILE *vfp_;

  mrs_natural lines_done_;
  mrs_natural fileSamples_;
  mrs_natural fileObs_;
  void addControls();
  void myUpdate(MarControlPtr sender);

public:
  CsvFileSource(std::string name);
  ~CsvFileSource();
  MarSystem* clone() const;

  void getHeader(std::string filename);
  void myProcess(realvec& in, realvec &out);
};

}//namespace Marsyas


#endif     /* !MARSYAS_CSVFILESOURCE_H */


