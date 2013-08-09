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


// Basic Audio Processing checks
#include "basicChecks.h"

// TODO: move
void
basic_vibrato(string infile, string outfile)
{
  MarSystem* pnet = mng.create("Series", "pnet");
  addSource( pnet, infile );
  pnet->addMarSystem(mng.create("Vibrato", "vib"));
  addDest( pnet, outfile);

  pnet->updctrl("Vibrato/vib/mrs_real/mod_freq", 10.0);
  while (pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    pnet->tick();
  }
  delete pnet;
}

void
basic_delay(string infile, string outfile)
{
  MarSystem* pnet = mng.create("Series", "pnet");
  addSource( pnet, infile );
  pnet->addMarSystem(mng.create("Delay", "delay"));
  pnet->updctrl("Delay/delay/mrs_natural/delaySamples", 16);
  pnet->updctrl("Delay/delay/mrs_real/feedback", (mrs_real) 0.5);
  addDest( pnet, outfile);

  while (pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    pnet->tick();
  }
  delete pnet;
}


// Produces "settings that are not supported in Marsyas" .au files.  :(
void
basic_downsample(string infile, string outfile)
{
  MarSystem* pnet = mng.create("Series", "pnet");
  addSource( pnet, infile );
  pnet->addMarSystem(mng.create("DownSampler", "down"));
  addDest( pnet, outfile);

  while (pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    pnet->tick();
  }
  delete pnet;
}

void
basic_negative(string infile, string outfile)
{
  MarSystem* pnet = mng.create("Series", "pnet");
  addSource( pnet, infile );
  pnet->addMarSystem(mng.create("Negative", "inv"));
  addDest( pnet, outfile);

  while (pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    pnet->tick();
  }
  delete pnet;
}

void
basic_shifter(string infile, string outfile)
{
  MarSystem* pnet = mng.create("Series", "pnet");
  addSource( pnet, infile );
  pnet->addMarSystem(mng.create("Shifter", "shift"));
  pnet->updctrl("Shifter/shift/mrs_natural/shift", 16);
  addDest( pnet, outfile);

  while (pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    pnet->tick();
  }
  delete pnet;
}

void
basic_shiftInput(string infile, string outfile)
{
  cout<<"CURRENTLY BROKEN!";
  /*
    MarSystem* pnet = mng.create("Series", "pnet");
    addSource( pnet, infile );
    pnet->addMarSystem(mng.create("ShiftInput", "shift"));
    pnet->updctrl("mrs_natural/inSamples", 256);
    pnet->updctrl("ShiftInput/shift/mrs_natural/winSize", 512);
    addDest( pnet, outfile);

    while (pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
    {
    pnet->tick();
    }
    delete pnet;
  */
}

void
basic_windowing(string infile, string outfile)
{
  MarSystem* pnet = mng.create("Series", "pnet");
  addSource( pnet, infile );
  pnet->addMarSystem(mng.create("Windowing", "win"));
  pnet->updctrl("Windowing/win/mrs_string/type", "Hanning");
  addDest( pnet, outfile);

  while (pnet->getctrl("mrs_bool/hasData")->to<mrs_bool>())
  {
    pnet->tick();
  }
  delete pnet;
}
