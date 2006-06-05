/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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
    \class NetworkTCPSource
    \brief NetworkTCPSource
    \author Stuart Bray 

    NetworkSource that uses the TCP communication protocol
*/


#include "NetworkTCPSource.h"
#include "SocketException.h"
using namespace std;


//  
// Constructor sets up the server...
//
NetworkTCPSource::NetworkTCPSource( string name ) 
{
  type_ = "NetworkTCPSource";
  name_ = name;
  addControls();
}

NetworkTCPSource::~NetworkTCPSource() {}

MarSystem* NetworkTCPSource::clone() const {
  return new NetworkTCPSource(*this);
}

void NetworkTCPSource::addControls()
{
  addDefaultControls();
  addctrl("real/gain", 1.0); 
  addctrl("natural/dataPort", 5009);
  addctrl("natural/controlsPort", 5010);
  addctrl("bool/notEmpty", true); 
}

string NetworkTCPSource::getClientAddr()
{
  return NetworkSocket::inet_ntoa();
}

void NetworkTCPSource::update()
{
  MRSDIAG("NetworkTCPSource.cpp - NetworkTCPSource:update");
  setctrl("natural/onSamples", getctrl("natural/inSamples"));
  setctrl("natural/onObservations", getctrl("natural/inObservations"));
  setctrl("real/osrate", getctrl("real/israte"));
  defaultUpdate();
}

void NetworkTCPSource::process( realvec& in, realvec& out )
{

  natural inSamples = getctrl("natural/inSamples").toNatural();
  natural inObservations = getctrl("natural/inObservations").toNatural();
  
  checkFlow(in,out);
 
  if (!valid( s_sock)) {
	  refresh();
  }
  
  int status = NetworkSocket::recvTCP( out );

  if ( status < 0 ) {
    throw SocketException ( "Cannot read from socket." );
  } else if ( status == 0 ) {
    throw SocketException ( "Client closed connection." );
  }	
  
  if ( byteSwap ) {
    int i = 0;
    real* data = out.getData();
	  
    for ( i = 0; i < (inObservations * inSamples); i++ ) {
	real tmp = swap( data[i] );
	data[i] = tmp;
    }
  }
}



/*
 *  Function: refresh
 *  Description: this function is called to re-initialize a source
 */
void NetworkTCPSource::refresh() 
{  
  // if we are currently connected, close it...	
  if ( valid( s_sock ) || valid( c_sock ) ) {
    ::close ( s_sock );
    ::close ( c_sock );
  }
	
  // perform the necessary steps...
  if ( ! NetworkSocket::createSocket() ) {
    throw SocketException ( "Could not create server socket." );
    exit(1);
  }

  natural dataPort = getctrl("natural/dataPort").toNatural();
  natural controlsPort = getctrl("natural/controlsPort").toNatural();

  if ( ! NetworkSocket::setupSource ( dataPort, controlsPort ) ) {
    throw SocketException ( "Could not setup TCP source." );
    exit(1);
  }
  
  if ( ! NetworkSocket::accept() ) {
  	throw SocketException ( "NetworkTCPSource: Could not accept TCP Connection.");
  	exit(1);
  }
  
  
}

