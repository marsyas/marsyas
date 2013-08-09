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

#include "NetworkSocket.h"

using namespace std;
using namespace Marsyas;


/**
* Constructor used by all Marsystems of type NetworkSocket
*/
NetworkSocket::NetworkSocket(string type, string name) : MarSystem(type,name), s_sock(-1), c_sock (-1), client_c(-1),
  client_d(-1)
{

#ifdef WIN32
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(1,1);

  WSAStartup(wVersionRequested, &wsaData);
  MRSERR("Incompatible Windows socket library version!");
#endif
}

/**
 * Constructor: sets up a basic server listening on port
 */
NetworkSocket::NetworkSocket( int port ) : MarSystem("NetworkSocket", "NetworSocketPrototype"), s_sock(-1), c_sock(-1), client_c(-1),
  client_d(-1)

{

#ifdef WIN32
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(1,1);

  WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested) {
    MRSERR("Incompatible Windows socket library version!");
  }
#endif

  createSocket();
  setupSource(port);
}


/**
 * Constructor: connects a client to a server on port
 */
NetworkSocket::NetworkSocket(int port, const string& hostname ) : MarSystem("NetworkSocket", "NetworkSocketPrototype"), s_sock(-1),
  c_sock(-1), client_c(-1), client_d(-1)
{

#ifdef WIN32
  WSADATA wsaData;
  WORD wVersionRequested = MAKEWORD(1,1);

  WSAStartup(wVersionRequested, &wsaData);
  if (wsaData.wVersion != wVersionRequested) {
    MRSERR("Incompatible Windows socket library version!");
    exit(1);
  }
#endif

  createSocket();
  setupSink( hostname, port );
}

//! destructor
NetworkSocket::~NetworkSocket() {
#ifndef WIN32
  if ( valid( s_sock ) ) {
    close ( s_sock );
  }
  if ( valid( c_sock ) ) {
    close( c_sock );
  }
  if ( valid( client_d ) ) {
    close ( client_d );
  }
  if ( valid( client_c ) ) {
    close( client_c );
  }
#else
  if ( valid (s_sock) ) {
    closesocket( s_sock );
  }
  if ( valid( c_sock ) ) {
    closesocket( c_sock );
  }
  if ( valid ( client_d ) ) {
    closesocket( client_d );
  }
  if ( valid ( client_c ) ) {
    closesocket( client_c );
  }
  WSACleanup();
#endif
}



MarSystem*
NetworkSocket::clone() const
{
  return new NetworkSocket(*this);
}



/**
 * Function: create
 * Description: creates a socket based on getType() for controls and data
 */
bool NetworkSocket::createSocket() {

  // Close an existing connection if it exists.
  if ( valid( s_sock ) ) {
    close( s_sock );
  }
  if ( valid( c_sock ) ) {
    close( c_sock );
  }
  if ( getType() == "NetworkUDPSource" || getType() == "NetworkUDPSink" ) {
    s_sock = socket( AF_INET, SOCK_DGRAM, 0 );
    c_sock = socket( AF_INET, SOCK_DGRAM, 0 );
  } else if ( getType() == "NetworkTCPSource" || getType() == "NetworkTCPSink" ) {
    s_sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    c_sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
  } else if ( getType() == "NetworkSocket" ) {
    s_sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    return ( valid( s_sock ) ); // used by Messager
  }

  // check for valid socket descriptor...
  if ( !valid( s_sock ) || !valid( c_sock ) ) {
    MRSERR("Invalid  socket descriptor.");
    return false;
  }

  // reuse the port if its in time-wait state...
  int flag = 1;
  if ( setsockopt( s_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &flag,
                   sizeof ( flag ) ) == -1 ) {
    return false;
  }
  if ( setsockopt( c_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &flag,
                   sizeof ( flag ) ) == -1 ) {
    return false;
  }
  return true;
}


/**
 * Function: setupSource
 * Description: Routine network setup for UDP / TCP server
 */
bool NetworkSocket::setupSource ( mrs_natural dataPort, mrs_natural controlsPort ) {

  if ( getType() == "NetworkTCPSource" ) {
    if ( !valid(s_sock) || !valid(c_sock) ) {
      MRSERR("Not a valid socket descriptor.");
      return false;
    }
  } else if ( getType() == "NetworkSource" ) {
    if ( !valid(s_sock) ) {
      MRSERR("Not a valid socket descriptor.");
      return false;
    }
  }

  // address structure for datas
  m_addr.sin_family = AF_INET;
  m_addr.sin_addr.s_addr = htonl( INADDR_ANY );
  m_addr.sin_port = htons( dataPort );

  if ( ::bind ( s_sock, ( struct sockaddr * ) &m_addr,
                sizeof ( m_addr ) ) == -1) {
    return false;
  }

  // setup a second listener for controls if its a MarSystem
  if ( controlsPort != 0 ) {
    c_addr.sin_family = AF_INET;
    c_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    c_addr.sin_port = htons( controlsPort );

    if ( ::bind ( c_sock, ( struct sockaddr * ) &c_addr,
                  sizeof ( c_addr ) ) == -1) {
      return false;
    }
  }

  // if its a UDP socket, we are done
  if (getType() == "NetworkUDPSource")
    return true;

  // listen for data port connection then data
  if ( ::listen ( s_sock, MAXCONNECT ) == -1 ) {
    return false;
  }


  cout << "TCP Server (data) listening on port: " << dataPort << endl;

  if ( controlsPort != 0 ) {
    if ( ::listen ( c_sock, MAXCONNECT ) == -1 ) {
      return false;
    }
    cout << "TCP Server (controls) listening on port: " << controlsPort << endl;
  }

  return true;
}


/**
 * Function: setupSink
 * Description: Routine setup for client to connect to host
 */
bool NetworkSocket::setupSink( const string& host, mrs_natural dataPort, mrs_natural controlsPort ) {

  struct hostent* h = gethostbyname( host.c_str() );

  if( h == NULL ) {
    cout << "Unknown host: " << host << endl;
    return false;
  }

  // setup server address structure...
  m_addr.sin_family = AF_INET;
  memcpy( (char *) &m_addr.sin_addr.s_addr, h->h_addr_list[0], h->h_length );
  m_addr.sin_port = htons( dataPort );

  // if we're a MarSystem, we need to have a controls line
  if ( controlsPort != 0 ) {
    c_addr.sin_family = AF_INET;
    memcpy( (char*) &c_addr.sin_addr.s_addr, h->h_addr_list[0], h->h_length );
    c_addr.sin_port = htons( controlsPort );
  }


  // if its a UDP socket, we are done
  /* if ( getType() == "NetworkUDPSink" )
  	return true;
  */

  // connect on the data port first
  if ( ::connect ( s_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) ) < 0 ) {
    MRSERR("Couldnt connect to server: ");
    return false;
  }

  cout << "Connected to " << host << " on the data port: " << dataPort << endl;

  // if we're a MarSystem then we connect on a controls line also
  if ( controlsPort != 0 ) {

    if ( ::connect ( c_sock, ( sockaddr * ) &c_addr, sizeof ( c_addr ) ) < 0 ) {
      MRSERR("Couldnt connect to server: ");
      // strerror(errno));
      return false;
    }
    cout << "Connected to " << host << " on the controls port: " << controlsPort << endl;
  }

  return true;
}


/**
 * Function: sendUDP
 * Description: Sends a vector to a UDP socket connection. In this function,
 * 				the controls are included in the realvec as the first three
 * 				elements.
 */
bool
NetworkSocket::sendUDP(realvec& in, mrs_natural inSamples,
                       mrs_natural inObservations, mrs_real israte )
{

  // create buffer of proper size to include control data...
  mrs_real* buffer = new mrs_real[inSamples + 3];

  buffer[0] = ( mrs_real ) inSamples;
  buffer[1] = ( mrs_real ) inObservations;
  buffer[2] = israte;

  // now fill the buffer with samples...
  mrs_real* data = in.getData();
  int i;

  for ( i = 3; i < inSamples + 3; i++ ) {
    buffer[i] = *(data + i - 3);
  }

  if ( ::sendto ( s_sock, buffer, sizeof(mrs_real) *
                  (inSamples+3), 0, (struct sockaddr*)&m_addr, sizeof(m_addr) ) <= 0) {
    MRSERR("Could not send vector over UDP socket.");
    delete buffer;
    return false;
  }

  delete buffer;
  return true;
}


/**
 * Function: sendTCP
 * Description: Sends a vector to a TCP socket connection.  In this function,
 * 				we can send the controls as a seperate packet because with
 * 				TCP we know the next packet will be a vector.
 */
bool NetworkSocket::sendTCP ( realvec& in, mrs_natural inSamples,
                              mrs_natural inObservations, mrs_real israte )  {

  // for TCP we can send the controls seperately
  mrs_real ctrl_send[4];
  ctrl_send[0] = 1.0; // for endien check
  ctrl_send[1] = (mrs_real) inSamples;
  ctrl_send[2] = (mrs_real) inObservations;
  ctrl_send[3] = israte;


  if ( ::send(c_sock, ctrl_send, sizeof( mrs_real )*4, 0 ) <= 0 ) {
    MRSERR("sendTCP: Could not send TCP control data.");
    return false;
  }

  // vector data...
  if ( ::send ( s_sock, in.getData(), sizeof(mrs_real) *
                (inSamples*inObservations), 0 ) <= 0 ) {
    MRSERR("sendTCP: Could not send TCP vector data.");
    return false;
  }

  return true;
}


/**
 * Function: recvUDP
 * Description: Receive a vector from a peer on a UDP socket connection.
 */
int NetworkSocket::recvUDP ( realvec& out )  {

  int status = 0;

  // assume a size 512 vector
  mrs_natural inSamples = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  mrs_natural inObservations = getctrl("mrs_natural/inObservations")->to<mrs_natural>();


  mrs_real* buffer = new mrs_real[(inSamples*inObservations)+3];
  int cliLen = sizeof(cliAddr);
#ifndef MARSYAS_CYGWIN
  status = ::recvfrom(s_sock, buffer, sizeof(mrs_real) *
                      ((inSamples*inObservations)+3), MSG_WAITALL,
                      (struct sockaddr *) &cliAddr, (socklen_t *)&cliLen);
#else
  status = ::recvfrom(s_sock, buffer, sizeof(mrs_real) *
                      ((inSamples*inObservations)+3), MSG_NOSIGNAL,
                      (struct sockaddr *) &cliAddr, (socklen_t *)&cliLen);

#endif

  if ( status <= 0 ) {
    MRSERR("Could not receive UDP data...");
    return 0;
  } else {
    updctrl("mrs_natural/inSamples", (mrs_natural) buffer[0]);
    updctrl("mrs_natural/inObservations",(mrs_natural) buffer[1]);
    updctrl("mrs_real/israte", buffer[2]);
  }

  // process the data to an output vector...
  out.create( inObservations, inSamples );
  mrs_real* data = out.getData();

  int i;
  for ( i = 0; i < 512; i++ ) {
    // *( data + i ) = buffer[i+3];
  }

  // delete [] buffer;
  return status;
}


/*
 * Function: recvControls()
 *
 * Description: This function is called by the controller of the high level
 * 		marsystem, in order to receive controls for the system and
 * 		perform the necessary updates.
 *
 */
mrs_real* const NetworkSocket::recvControls() {


  // only TCP MarSystems use this function
  /* if ( getType() == "NetworkUDPSource" ) {
    return 0;
  }
  */

  if ( !valid( s_sock ) || !valid( c_sock ) ) {
    refresh();
  }


  int ctrl_status = 0;

#ifndef MARSYAS_CYGWIN
  ctrl_status = ::recv( client_c, controls, sizeof( mrs_real )*4, MSG_WAITALL );
#else
  ctrl_status = ::recv( client_c, controls, sizeof( mrs_real )*4, MSG_NOSIGNAL);
#endif

  if (ctrl_status <= 0) {
    MRSERR("recvControls: No TCP control data.");
    updctrl("mrs_bool/hasData", false);
    return 0;
  }

  if ( controls[0] != 1.0 ) {

    byteSwap = true;
    updctrl("mrs_natural/inSamples", (mrs_natural)swap(controls[1]));
    updctrl("mrs_natural/inObservations", (mrs_natural)swap(controls[2]));
    updctrl("mrs_real/israte", swap(controls[3]));

    // return proper values
    controls[1] = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
    controls[2] = getctrl("mrs_natural/inObservations")->to<mrs_natural>();
    controls[3] = getctrl("mrs_real/israte")->to<mrs_real>();

  } else
  {
    updctrl("mrs_natural/inSamples",(mrs_natural) controls[1]);
    updctrl("mrs_natural/inObservations", (mrs_natural) controls[2]);
    updctrl("mrs_real/israte", controls[3]);
  }
  return controls;
}


/**
 * Function: recvTCP
 * Description: Receive a vector from a peer on a TCP socket connection.
 */
int NetworkSocket::recvTCP ( realvec& out )
{

  // create our slice with updated controls
  mrs_natural inSamples = getctrl("mrs_natural/inSamples")->to<mrs_natural>();
  mrs_natural inObservations = getctrl("mrs_natural/inObservations")->to<mrs_natural>();

  out.create(inObservations, inSamples);
  mrs_real* data = out.getData();


#ifndef MARSYAS_CYGWIN
  return ::recv ( client_d, data, sizeof(mrs_real)*(inObservations*inSamples),
                  MSG_WAITALL );
#else
  return ::recv ( client_d, data, sizeof(mrs_real)*(inObservations*inSamples),
                  MSG_NOSIGNAL);
#endif

}


/**
 * Function: setBlocking
 * Description: enable / disable a sockets blocking status
 */
void NetworkSocket::setBlocking( int socket, bool enable )
{
  if ( !valid( socket ) )
    return;

#ifdef WIN32
  unsigned long non_block = !enable;
  ioctlsocket( socket, FIONBIO, &non_block );
#else
  int tmp = ::fcntl(socket, F_GETFL, 0);
  if ( tmp >= 0 )
    tmp = ::fcntl( socket, F_SETFL, enable ?
                   (tmp &~ O_NONBLOCK) : (tmp | O_NONBLOCK) );
#endif

}


/**
 * Function: accept
 * Description: Called by a TCP server to accept a client connection.
 */
int NetworkSocket :: accept( void )
{

  // make sure its a TCP server
  if ( ! ( getType() == "NetworkSocket" || getType() == "NetworkTCPSource" ) )
  {
    return -1;
  }


  int cliLen = sizeof( cliAddr );
  client_d = ::accept( s_sock, (struct sockaddr *) &cliAddr, (socklen_t *) &cliLen);

  if( client_d < 0 ) {
    MRSERR("NetworkSocket::accept(): Could not accept connection");
    return -1;
  }
  else
  {

    // if its Messager, we're done.
    if ( getType() == "NetworkSocket" )
      return client_d;

    cout << "Client connected on data port: " <<
         getctrl("mrs_natural/dataPort")->to<mrs_natural>() << endl;


    // if we're a MarSystem, accept a connection for controls also
    if ( getType() == "NetworkTCPSource" )
    {

      int cliLen = sizeof( cliCont );
      client_c = ::accept( c_sock, (struct sockaddr *) &cliCont, (socklen_t *) &cliLen);

      if ( client_c < 0) {
        MRSERR("NetworkSocket::accept(): Cannot accept connection");
        return -1;
      }

      cout << "Client connected on controls port: " <<
           getctrl("mrs_natural/controlsPort")->to<mrs_natural>() << endl;

    }
  }

  return client_d;
}


/**
 * Function: close
 * Description: Closes a socket with descriptor sockfd
 */
void NetworkSocket :: close( int sockfd )
{
  if ( !valid( sockfd ) )
    return;

#ifndef WIN32
  ::close( sockfd );
#else
  ::closesocket( sockfd );
#endif
}



/**
 * Function: inet_ntoa
 * Description: Returns the address of the client connected.
 */
string NetworkSocket::inet_ntoa() const {
  return ::inet_ntoa(cliAddr.sin_addr);
}


/**
 * Function: valid
 * Description: Static function to validate a socket file descriptor.
 */
bool NetworkSocket::valid(int sockfd) {
  return (sockfd != -1);
}


/**
 * Function: getSocket
 * Description: Returns the socket descriptor for this connection.
 */
int NetworkSocket :: getSocket( void ) const {
  return s_sock;
}


/**
 * Function: writeBuffer
 * Description: Write a buffer over the socket connection.
 * 				Returns the number of bytes written or -1 if an error occurs.
 */
int NetworkSocket :: writeBuffer(const void *buffer, long bufferSize, int flags ) {
  if ( !valid( s_sock ) )
    return -1;
  return ::send( s_sock, (const char *)buffer, bufferSize, flags );
}


/**
 * Function: readBuffer
 * Description: Read a buffer via the specified socket.
 * 				Returns the number of bytes read or -1 if an error occurs.
 */
int NetworkSocket :: readBuffer(int socket, void *buffer, long bufferSize, int flags ) {
  if ( !valid( socket ) )
    return -1;
  return ::recv( socket, (char *)buffer, bufferSize, flags );
}


/**
 * Function: swap
 * Description: converts endianness
 */
mrs_real NetworkSocket::swap ( mrs_real argument ) {

  typedef union X {
    mrs_real num;
    char x[ sizeof(mrs_real) ];
  };

  X s1, s2;

  s1.num = argument;
  s2.x[0] = s1.x[3];
  s2.x[1] = s1.x[2];
  s2.x[2] = s1.x[1];
  s2.x[3] = s1.x[0];
  return s2.num;
}

//! stub function so we can use an object of this type
void NetworkSocket::myProcess( realvec& in, realvec& out ) {}
