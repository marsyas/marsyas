/*
** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "NetworkUDPSink.h"
#include "SocketException.h"

using namespace std;
using namespace Marsyas;

NetworkUDPSink::NetworkUDPSink(string name):NetworkSocket("NetworkUDPSink",name)
{
  addControls();
}

NetworkUDPSink::~NetworkUDPSink()
{
}

MarSystem* NetworkUDPSink::clone() const {
  return new NetworkUDPSink( *this );
}

void NetworkUDPSink::addControls() {
  addctrl("mrs_real/gain", 1.0);
  addctrl("mrs_string/host", "localhost");
  addctrl("mrs_natural/dataPort", 5009);
  addctrl("mrs_natural/controlsPort", 5010);
}

void NetworkUDPSink::myProcess( realvec& in, realvec& out ) {

  //checkFlow(in,out);

  if ( !valid(s_sock) )
    refresh();

  if ( !NetworkSocket::sendUDP( in, getctrl("mrs_natural/inSamples")->to<mrs_natural>(),
                                getctrl("mrs_natural/inObservations")->to<mrs_natural>(),
                                getctrl("mrs_real/israte")->to<mrs_real>() ) ) {
    throw SocketException ( "Could not write to socket." );
  }

}

void NetworkUDPSink::refresh() {

  // if we are currently connected, close it...
  if ( valid( s_sock ) || valid( c_sock ) ) {
    ::close ( s_sock );
    ::close ( c_sock );
  }

  if ( ! NetworkSocket::createSocket() ) {
    throw SocketException ( "Could not create client socket." );
  }

  mrs_natural dataPort = getctrl("mrs_natural/dataPort")->to<mrs_natural>();
  mrs_natural controlsPort = getctrl("mrs_natural/controlsPort")->to<mrs_natural>();

  if ( ! NetworkSocket::setupSink ( getctrl("mrs_string/host")->to<mrs_string>(),
                                    dataPort, controlsPort ) ) {
    throw SocketException ( "Could not setup address structure." );
  }
}
