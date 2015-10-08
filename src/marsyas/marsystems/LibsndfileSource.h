/*
** Copyright (C) 2015 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_SFSOURCE_H
#define MARSYAS_SFSOURCE_H

#include <marsyas/system/MarSystem.h>
#include <sndfile.h>

namespace Marsyas {

using std::string;

class LibsndfileSource : public MarSystem
{
public:
  LibsndfileSource(const string & name);
  LibsndfileSource(const LibsndfileSource &);
  virtual MarSystem * clone() const { return new LibsndfileSource(*this); }
  virtual void myUpdate(MarControlPtr sender);
  virtual void myProcess(realvec& in, realvec& out);
private:
  void close();
  bool open(const string & filename);

  string m_filename;
  SF_INFO m_fileInfo;
  SNDFILE * m_file;
  MarControlPtr m_filenameControl;
  MarControlPtr m_hasDataControl;
};

}

#endif // MARSYAS_SFSOURCE_H
