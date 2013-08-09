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

#include "NetworkUDPSource.h"
#include "SocketException.h"

using namespace std;
using namespace Marsyas;

//
// Constructor sets up the server...
//
NetworkUDPSource::NetworkUDPSource(string name) : NetworkSocket("NetworkUDPSource",name)
{
  //type_ = "NetworkUDPSource";
  //name_ = name;

  addControls();
}

NetworkUDPSource::~NetworkUDPSource()
{
}

MarSystem* NetworkUDPSource::clone() const {
  return new NetworkUDPSource(*this);
}

void NetworkUDPSource::addControls()
{
  addctrl("mrs_real/gain", 1.0);
  addctrl("mrs_natural/dataPort", 5009);
  addctrl("mrs_natural/controlsPort", 5010);
  addctrl("mrs_bool/hasData", true);
}


string NetworkUDPSource::getClientAddr()
{
  return NetworkSocket::inet_ntoa();
}

void NetworkUDPSource::myProcess( realvec& in, realvec& out )
{
  //checkFlow(in,out);

  if ( !valid( s_sock ) )
    refresh();

  int status = NetworkSocket::recvUDP ( out );

  if ( status < 0 ) {
    throw SocketException ( "Cannot read from socket." );
  } else if ( status == 0 ) {
    throw SocketException ( "Client closed connection." );
  }


}

void NetworkUDPSource::refresh()
{

  if ( valid( s_sock ) || valid( c_sock ) ) {
    ::close ( c_sock );
    ::close ( s_sock );
  }
  if ( ! NetworkSocket::createSocket() ) {
    throw SocketException ( "Could not create server socket." );
  }

  cout << "Waiting for data on port: " << getctrl("mrs_natural/dataPort") << endl;

  mrs_natural dataPort = getctrl("mrs_natural/dataPort")->to<mrs_natural>();
  mrs_natural controlsPort = getctrl("mrs_natural/controlsPort")->to<mrs_natural>();

  if ( ! NetworkSocket::setupSource ( dataPort, controlsPort ) ) {
    throw SocketException ( "Could not bind to port." );
  }
}

