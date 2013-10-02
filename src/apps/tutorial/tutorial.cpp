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


// This executable contains various tutorial-style functions
// with somewhat more extensive comments that hopefully
// help in understanding how to use the Marsyas audio
// processing software framework.

#include <cstdio>
#include <string>

#include <marsyas/system/MarSystemManager.h>
#include "SoundFileSource.h"
#include "SoundFileSink.h"
#include "Gain.h"
#include "Series.h"
#include <marsyas/CommandLineOptions.h>

using namespace std;
using namespace Marsyas;

// basic approach - similar to Matlab or hand-crafted code
void
sfplay1(string sfName)
{
  cout << "sfplay1: Playing " << sfName << endl;

  // create a soundfilesource for reading samples
  // from the file
  MarSystem* src = new SoundFileSource("src");
  src->updctrl("mrs_string/filename", sfName);
  src->updctrl("mrs_natural/inSamples", 2048);

  // simple MarSystem a gain control
  MarSystem* gain = new Gain("gain");
  gain->updctrl("mrs_real/gain", 2.0);
  gain->updctrl("mrs_natural/inSamples", 2048);

  // SoundFileSink writes to a file
  MarSystem* dest = new SoundFileSink("dest");
  dest->updctrl("mrs_natural/inSamples", 2048);
  dest->updctrl("mrs_string/filename", "ajay.au");

  // write system to output
  cout << (*src) << endl;

  // create two matrices for input/output
  realvec in;
  realvec out;
  realvec gout;

  in.create(src->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
            src->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  out.create(src->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
             src->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  gout.create(src->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
              src->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  // nothing happens until process is called
  // usually for MarSystems the in matrix is processed
  // and the result is written in the out matrix.
  // For SoundFileSources the input is ignored
  // and the out matrix is updated from
  // reading the samples
  // from the file

  gain->updctrl("mrs_real/gain", 0.0);

  cout << (*gain) << endl;
  for (mrs_natural i=0; i < 20; i++)
  {
    // call process seperately for each MarSystem
    src->process(in,out);
    // multiply window with gain
    gain->updctrl("mrs_real/gain",
                  gain->getctrl("mrs_real/gain")->to<mrs_real>() + 0.1);
    gain->process(out, gout);
    dest->process(gout, gout); 	// Just outputs the input to the file
    // and copies it to the output
  }


  delete src;
  delete gain;
  delete dest;

}

// use Composite
void
sfplay2(string sfName)
{
  cout << "sfplay2: Playing " << sfName << endl;

  // Create a series Composite
  MarSystem* series = new Series("series");

  // create a soundfilesource for reading samples
  // from the file
  MarSystem* src = new SoundFileSource("src");
  src->updctrl("mrs_string/filename", sfName);
  src->updctrl("mrs_natural/inSamples", 2048);

  // simple MarSystem a gain control
  MarSystem* gain = new Gain("gain");
  gain->updctrl("mrs_real/gain", 2.0);
  gain->updctrl("mrs_natural/inSamples", 2048);

  // SoundFileSink writes to a file
  MarSystem* dest = new SoundFileSink("dest");
  dest->updctrl("mrs_natural/inSamples", 2048);
  dest->updctrl("mrs_string/filename", "ajay.au");

  // add MarSystems to series Composite
  series->addMarSystem(src);
  series->addMarSystem(gain);
  series->addMarSystem(dest);


  cout << (*series) << endl;

  // create two matrices for input/output
  realvec in;
  realvec out;

  in.create(series->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
            series->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  out.create(series->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
             series->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  // nothing happens until process is called
  // usually for MarSystems the in matrix is processed
  // and the result is written in the out matrix.
  // For SoundFileSources the input is ignored
  // and the out matrix is updated from
  // reading the samples
  // from the file

  // when using a Composite you need to call
  // process only once without having to
  // provide buffers for the intermediate steps

  // when using a composite controls of internal
  // MarSystems can be updated using a OSC-inspired
  // path notation

  series->updctrl("Gain/gain/mrs_real/gain", 0.0);

  for (int i=0; i<20; i++)
  {
    series->updctrl("Gain/gain/mrs_real/gain",
                    series->getctrl("Gain/gain/mrs_real/gain")->to<mrs_real>() + 0.1);
    series->process(in,out);

  }

  // Composite deletes the added MarSystems
  // so you must not delete them
  delete series;
}

// take advantage of Composite
void
sfplay3(string sfName)
{
  cout << "sfplay3: Playing " << sfName << endl;

  // get rid of some stuff taking advantage of
  // composite

  // Create a series Composite
  MarSystem* series = new Series("series");

  // create a soundfilesource for reading samples
  // from the file
  MarSystem* src = new SoundFileSource("src");
  src->updctrl("mrs_string/filename", sfName);

  // simple MarSystem a gain control
  MarSystem* gain = new Gain("gain");
  gain->updctrl("mrs_real/gain", 2.0);

  // SoundFileSink writes to a file
  MarSystem* dest = new SoundFileSink("dest");
  dest->updctrl("mrs_string/filename", "ajay.au");

  // add MarSystems to series Composite
  series->addMarSystem(src);
  series->addMarSystem(gain);
  series->addMarSystem(dest);

  // we only need to change window size at the composite level
  series->updctrl("mrs_natural/inSamples", 4096);

  cout << (*series) << endl;

  // tick method basically calls process
  // with an empty input buffer and an empty output buffer
  // because soundfilesource/sink read/write to files
  // as sideeffects this works


  series->updctrl("Gain/gain/mrs_real/gain", 0.0);
  for (int i=0; i<20; i++)
  {
    series->tick();
    series->updctrl("Gain/gain/mrs_real/gain",
                    series->getctrl("Gain/gain/mrs_real/gain")->to<mrs_real>() + 0.1);
  }

  // Composite deletes the added MarSystems
  // so you must not delete them
  delete series;
}

// take advantage of MarSystemManager
void
sfplay4(string sfName)
{
  cout << "sfplay4: Playing " << sfName << endl;

  MarSystemManager mng;

  // Create a series Composite
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));

  // only update controls from Composite level
  series->updctrl("mrs_natural/inSamples", 128);
  series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  series->updctrl("SoundFileSink/dest/mrs_string/filename", "ajay.wav");

  series->linkctrl("mrs_natural/gain", "Gain/gain/mrs_real/gain");
  while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    series->updctrl("mrs_natural/gain", 2.0);
    series->tick();
  }


  delete series;
}




// add some simple feature extraction
void
sfplay5(string sfName)
{
  cout << "sfplay5: Playing " << sfName << endl;
  MarSystemManager mng;

  // Create a series Composite
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));
  // hamming window -> complex spectrum -> power spectrum
  series->addMarSystem(mng.create("Windowing", "hamming"));
  series->addMarSystem(mng.create("Spectrum", "spk"));
  series->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  series->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType", "power");
  series->addMarSystem(mng.create("Centroid", "centroid"));

  // only update controls from Composite level
  series->updctrl("mrs_natural/inSamples", 4096);
  series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  series->updctrl("SoundFileSink/dest/mrs_string/filename", "ajay.au");

  cout << (*series) << endl;

  realvec in(series->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
             series->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec out(series->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
              series->getctrl("mrs_natural/onSamples")->to<mrs_natural>());



  // play all the file
  while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    series->process(in,out);
    cout << "centroid = " << out(0,0) << endl;
  }

}

// add some simple feature extraction
void
sfplay6(string sfName)
{
  cout << "sfplay6: Playing " << sfName << endl;
  MarSystemManager mng;

  // Create a series Composite
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));
  // hamming window -> complex spectrum -> power spectrum
  series->addMarSystem(mng.create("Windowing", "hamming"));
  series->addMarSystem(mng.create("Spectrum", "spk"));
  series->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  series->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType", "power");

  // fanout of two features that both are
  // calculated on the mangitude spectrum
  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(mng.create("Centroid", "centroid"));
  fanout->addMarSystem(mng.create("Rolloff", "rolloff"));

  // add the fanout to the series network
  series->addMarSystem(fanout);

  // only update controls from Composite level
  series->updctrl("mrs_natural/inSamples", 4096);
  series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  series->updctrl("SoundFileSink/dest/mrs_string/filename", "ajay.au");

  cout << (*series) << endl;

  realvec in(series->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
             series->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec out(series->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
              series->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  // play all the file
  while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    series->process(in,out);
    // output observation 1 and observation 2
    cout << "features = " << out(0,0) << ", " << out(1,0) << endl;
  }

}

// add some simple feature extraction
// if you understand this function you understand
// most of feature extraction in Marsyas
void
sfplay7(string sfName)
{
  cout << "sfplay7: Playing " << sfName << endl;
  MarSystemManager mng;

  // Full series for everything
  MarSystem* total = mng.create("Series", "total");

  // Accumulator accumulates many calls to tick
  // and outputs the whole thing as nTimes samples
  MarSystem* acc = mng.create("Accumulator", "acc");
  acc->updctrl("mrs_natural/nTimes", 100);

  // Create a series Composite
  MarSystem* series = mng.create("Series", "series");
  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));
  // hamming window -> complex spectrum -> power spectrum
  series->addMarSystem(mng.create("Windowing", "hamming"));
  series->addMarSystem(mng.create("Spectrum", "spk"));
  series->addMarSystem(mng.create("PowerSpectrum", "pspk"));
  series->updctrl("PowerSpectrum/pspk/mrs_string/spectrumType", "power");

  // fanout of two features that both are
  // calculated on the mangitude spectrum
  MarSystem* fanout = mng.create("Fanout", "fanout");
  fanout->addMarSystem(mng.create("Centroid", "centroid"));
  fanout->addMarSystem(mng.create("Rolloff", "rolloff"));
  // add the fanout to the series network
  series->addMarSystem(fanout);


  // only update controls from Composite level
  series->updctrl("mrs_natural/inSamples", 512);
  series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  series->updctrl("SoundFileSink/dest/mrs_string/filename", "ajay.au");

  cout << (*series) << endl;

  // play all the file
  // while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  acc->addMarSystem(series);
  total->addMarSystem(acc);

  MarSystem* stats = mng.create("Fanout", "stats");
  stats->addMarSystem(mng.create("Mean", "mn"));
  stats->addMarSystem(mng.create("StandardDeviation", "std"));
  total->addMarSystem(stats);

  // always allocate after updating/adding marsystems
  realvec in(total->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
             total->getctrl("mrs_natural/inSamples")->to<mrs_natural>());
  realvec out(total->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
              total->getctrl("mrs_natural/onSamples")->to<mrs_natural>());



  // only 1 call is needed as accumulate calls internally
  // multiple times process
  total->process(in,out);
  out.write("ajay.plot");
}


void
wavsfplay(string sfName)
{
  cout << "wavsfplay1: Playing " << sfName << endl;

  // create a soundfilesource for reading samples
  // from the file
  MarSystem* src = new SoundFileSource("src");
  src->updctrl("mrs_string/filename", sfName);
  src->updctrl("mrs_natural/inSamples", 2048);

  // simple MarSystem a gain control
  MarSystem* gain = new Gain("gain");
  gain->updctrl("mrs_real/gain", 2.0);
  gain->updctrl("mrs_natural/inSamples", 2048);

  // SoundFileSink writes to a file
  MarSystem* dest = new SoundFileSink("dest");
  dest->updctrl("mrs_natural/inSamples", 2048);
  dest->updctrl("mrs_real/israte", src->getctrl("mrs_real/osrate"));
  dest->updctrl("mrs_natural/nChannels", src->getctrl("mrs_natural/nChannels"));
  dest->updctrl("mrs_string/filename", "ajay.wav");

  // write system to output
  cout << (*src) << endl;

  // create two matrices for input/output
  realvec in;
  realvec out;
  realvec gout;

  in.create(src->getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
            src->getctrl("mrs_natural/inSamples")->to<mrs_natural>());

  out.create(src->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
             src->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  gout.create(src->getctrl("mrs_natural/onObservations")->to<mrs_natural>(),
              src->getctrl("mrs_natural/onSamples")->to<mrs_natural>());

  // nothing happens until process is called
  // usually for MarSystems the in matrix is processed
  // and the result is written in the out matrix.
  // For SoundFileSources the input is ignored
  // and the out matrix is updated from
  // reading the samples
  // from the file

  gain->updctrl("mrs_real/gain", 1.0);

  while (src->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    src->process(in,out);
    // multiply window with gain
    /* gain->updctrl("mrs_real/gain",
       gain->getctrl("mrs_real/gain")->to<mrs_real>() + 0.1); */

    gain->process(out, gout);
    dest->process(gout, gout);
  }


  delete src;
  delete gain;
  delete dest;

}



void
wavsfplay1(string sfName)
{
  cout << "wavsfplay1: Playing " << sfName << endl;

  MarSystemManager mng;

  MarSystem* series = mng.create("Series", "series");


  series->addMarSystem(mng.create("SoundFileSource", "src"));
  series->addMarSystem(mng.create("Gain", "gain"));
  series->addMarSystem(mng.create("SoundFileSink", "dest"));

  series->updctrl("SoundFileSource/src/mrs_string/filename", sfName);


  series->updctrl("Gain/gain/mrs_real/gain", 1.0);
  series->updctrl("SoundFileSink/dest/mrs_natural/nChannels",
                  series->getctrl("SoundFileSource/src/mrs_natural/nChannels"));
  //series->updctrl("SoundFileSink/dest/mrs_real/israte",
  //        series->getctrl("SoundFileSource/src/mrs_real/osrate"));

  series->updctrl("mrs_natural/inSamples", 2048);
  series->updctrl("SoundFileSink/dest/mrs_string/filename", "ajay.wav");


  cout << (*series) << endl;

  while (series->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
  {
    series->tick();
  }
}



void
test_collection(string sfName)
{

  MarSystemManager mng;
  MarSystem* pnet = mng.create("Series", "pnet");
  pnet->addMarSystem(mng.create("SoundFileSource", "src"));
  pnet->addMarSystem(mng.create("AudioSink", "dest"));


  pnet->updctrl("SoundFileSource/src/mrs_string/filename", sfName);
  pnet->updctrl("SoundFileSource/src/mrs_bool/shuffle", true);
  cout << "first round" << endl;


  for (mrs_natural k=0; k < 4; k++)
  {
    pnet->updctrl("SoundFileSource/src/mrs_natural/cindex", k);
    cout << pnet->getctrl("SoundFileSource/src/mrs_string/currentlyPlaying")->to<mrs_string>() << endl;
    for (mrs_natural i=0; i <100; i++)
    {

      pnet->tick();
    }
  }
  cout << "second round" << endl;
  pnet->updctrl("SoundFileSource/src/mrs_string/filename", "music1.mf");
  for (mrs_natural k=0; k < 4; k++)
  {
    pnet->updctrl("SoundFileSource/src/mrs_natural/cindex", k);
    cout << pnet->getctrl("SoundFileSource/src/mrs_string/currentlyPlaying")->to<mrs_string>() << endl;
    for (mrs_natural i=0; i <100; i++)
      // 	while(pnet->getctrl("SoundFileSource/src/mrs_bool/hasData")->to<mrs_bool>())
    {
      pnet->tick();
    }
  }

}


int
main(int argc, const char **argv)
{

  CommandLineOptions cmd_options;
  cmd_options.readOptions(argc, argv);

  vector<string> soundfiles = cmd_options.getRemaining();

  string sfName = soundfiles[0];

  // Matlab style with explicit allocation of buffers
  // and seperate process functions for each MarSystem
  sfplay1(sfName);

  // Matlab style with explicit allocation of buffers
  // but using Series composite
  // sfplay2(sfName);

  // get rid of buffers altogether using the tick method
  // sfplay3(sfName);

  // using MarSystemManager and change the window
  // size on each loop iteration
  // sflay4(sfName);

  // add centroid calculation over the whole file
  // sfplay5(sfName);

  // add centroid and rolloff calculation using Fanout
  // sfplay6(sfName);

  // what if you want to calculate the mean, variances
  // over 100 windows
  // sfplay7(sfName);

  // wavsfplay(sfName);
  // wavsfplay1(sfName);
  // test_collection("music.mf");

}
