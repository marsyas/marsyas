/*
** Copyright (C) 2000-2010 George Tzanetakis <gtzan@cs.uvic.ca>
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



#include <marsyas/system/MarSystemManager.h>
using namespace Marsyas;
using namespace std;
#include <marsyas/common_source.h>

#define CLOSE_ENOUGH 0.0001

int
printHelp(string progName)
{
  MRSDIAG("audioCompare.cpp - printUsage");
  cerr << "audioCompare, MARSYAS" << endl;
  cerr << "-------------------------" << endl;
  cerr << "Usage: " <<endl;
  cerr << progName<<" file1 file2"<<endl;
  return (1);
}

int
isClose(string infile1, string infile2)
{
  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");

  MarSystem* invnet = mng.create("Series", "invnet");
  invnet->addMarSystem(mng.create("SoundFileSource", "src2"));
  invnet->updControl("SoundFileSource/src2/mrs_string/filename", infile2);
  invnet->addMarSystem(mng.create("Negative", "neg"));

  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(mng.create("SoundFileSource", "src1"));
  fanout->updControl("SoundFileSource/src1/mrs_string/filename", infile1);
  fanout->addMarSystem(invnet);

  pnet->addMarSystem(fanout);
  pnet->addMarSystem(mng.create("Sum", "sum"));
  pnet->linkControl("mrs_bool/hasData",
                    "Fanout/fanout/SoundFileSource/src1/mrs_bool/hasData");

  mrs_natural i;
  mrs_natural samples =
    pnet->getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  while ( pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>() )
  {
    pnet->tick();
    const realvec& processedData =
      pnet->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    for (i=0; i<samples; ++i)
    {
      //  useful for tweaking CLOSE_ENOUGH
      //cout<<processedData(i)<<" ";
      if ( abs(processedData(i)) > CLOSE_ENOUGH )
      {
        delete pnet;
        return(1);
      }
    }
  }
  delete pnet;
  return 0;
}


int
main(int argc, const char **argv)
{
  string progName = argv[0];
  if (argc != 3)
  {
    return printHelp(progName);
  }

  string file1 = argv[1];
  string file2 = argv[2];

  return isClose(file1, file2);
}

