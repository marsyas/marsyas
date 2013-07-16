/*
** Copyright (C) 2013 Steven Ness <sness@sness.net>
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
   nextract: new feature extraction
*/

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector> 
#include "Collection.h"
#include "MarSystemManager.h"
#include "CommandLineOptions.h"

using namespace std;
using namespace Marsyas;

void usage()
{
  cout << "nextract input.mf" << endl;
}

void extract(string inCollectionName)
{
  MarSystemManager mng;

  MarSystem* net = mng.create("Series", "net");
  net->addMarSystem(mng.create("SoundFileSource", "src"));
  net->updControl("SoundFileSource/src/mrs_string/filename", inCollectionName);
  net->addMarSystem(mng.create("Windowing", "ham"));
  net->addMarSystem(mng.create("Spectrum", "spk"));
  net->addMarSystem(mng.create("PowerSpectrum", "pspk"));

  MarSystem* featureFanout = mng.create("Fanout", "featureFanout");
  featureFanout->addMarSystem(mng.create("Centroid", "centroid"));
  featureFanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  featureFanout->addMarSystem(mng.create("MFCC", "mfcc"));
  net->addMarSystem(featureFanout);

  mrs_realvec data;
  while ( net->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>() ){
    cout << net->getctrl("SoundFileSource/src/mrs_real/currentLabel")->to<mrs_real>() << " ";
    data = net->getctrl("mrs_realvec/processedData")->to<mrs_realvec>();
    for (int i = 0; i < data.getRows(); i++) {
      cout << i << ":" << data(i, 0) << " ";
    }
    cout << endl;

    net->tick();
  }


}
  

int
main(int argc, const char **argv)
{
  string inCollectionName;

  if (argc < 3) {
	usage();
	exit(1);
  } else {
	inCollectionName = argv[1];
  }

  extract(inCollectionName);

  exit(0);

}
