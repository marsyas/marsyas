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

#include "CsvFileSource.h"

using namespace std;
using namespace Marsyas;

CsvFileSource::CsvFileSource(mrs_string name):MarSystem("CsvFileSource",name)
{
  //type_ = "CsvFileSource";
  //name_ = name;

  vfp_ = 0;
  fileObs_ = 0;
  filename_ = EMPTYSTRING;

  addControls();
}


CsvFileSource::~CsvFileSource()
{
  if (vfp_ != NULL)
    fclose(vfp_);
}

void
CsvFileSource::addControls()
{
  addctrl("mrs_bool/hasData", true);
  addctrl("mrs_natural/size", 0);
  addctrl("mrs_string/filename", EMPTYSTRING);
  setctrlState("mrs_string/filename", true);
}



MarSystem*
CsvFileSource::clone() const
{
  return new CsvFileSource(*this);
}



void
CsvFileSource::getHeader(mrs_string filename)
{
  if (vfp_ != NULL) {
    fclose(vfp_);
  }
  // Need to read Csv File Header
  vfp_ = fopen(filename.c_str(), "r");
  if (vfp_)
  {
    // read first line from file
    char buffer[4096];
    char *res;
    res = fgets(buffer, 4096, vfp_);
    if (res == NULL) {
      cout<<"CsvFileSource: error reading file "<<filename<<endl;
    }
    stringstream line(buffer);
    char entry[256];
    fileObs_ = 0;
    while (line.getline(entry, 256, ','))
    {
      fileObs_++;
    }
    setctrl("mrs_natural/onObservations", fileObs_);
    lines_done_ = 0;

    string obs(buffer);
    ctrl_onObsNames_->setValue(obs, NOUPDATE);
    setctrl("mrs_bool/hasData", true);
  } else {
    MRSWARN("CsvFileSource: error reading file " + filename);
  }
}


void
CsvFileSource::myUpdate(MarControlPtr sender)
{
  (void) sender;  //suppress warning of unused parameter(s)
  inObservations_ = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
  israte_ = getctrl("mrs_real/israte")->to<mrs_real>();
  setctrl("mrs_real/osrate", israte_);
  setctrl("mrs_natural/onSamples", inSamples_);

  if (filename_ != getctrl("mrs_string/filename")->to<mrs_string>())
  {
    filename_ = getctrl("mrs_string/filename")->to<mrs_string>();
    if (filename_ == EMPTYSTRING) {
      setctrl("mrs_natural/onObservations", 0);
      return;
    }

    // count lines, subtract 1 for header
    fileSamples_ = 0;
    ifstream input(filename_.c_str());
    string line;
    while (input.good())
    {
      fileSamples_++;
      getline(input, line);
    }
    input.close();
    fileSamples_ -= 2;

    getHeader(filename_);

  }

  setctrl("mrs_natural/onObservations", fileObs_);


}

void
CsvFileSource::myProcess(realvec& in, realvec& out)
{
  (void) in;
  //checkFlow(in,out);
  mrs_natural o,t;

  for (t = 0; t < inSamples_; t++)
  {
    bool notValidLine = true;
    char buffer[4096];
    while (notValidLine)
    {
      char *res;
      res = fgets(buffer, 4096, vfp_);
      if (res == NULL)
      {
        setctrl("mrs_bool/hasData",false);
        return;
      }

      stringstream line(buffer);
      stringstream pline(buffer);
      char entry[256];
      notValidLine = false;
      for (o=0; o < onObservations_; o++)
      {
        line.getline(entry, 256, ',');
        if (!strcmp(entry,""))
        {
          for (mrs_natural j=0; j < o; j++)
            out(j,t) = 0.0;
          notValidLine = true;
        }
        else
          out(o,t) = (mrs_real)atof(entry);

        if (notValidLine) break;
      }
      lines_done_++;
      if (lines_done_ >= fileSamples_)
      {
        setctrl("mrs_bool/hasData",false);
      }
    }
  }
}






