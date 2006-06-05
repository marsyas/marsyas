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
    \class NetworkSocket
    \brief Wrapper class for linux socket calls.  
    \author Stuart Bray

    The main purpose for this class is to wrap TCP and UDP calls 
    for MarSystems that send data over a network.  This class can also 
    be used to setup a simple client and server. 

*/



#ifndef MARSYAS_NETWORKSOCKET_H
#define MARSYAS_NETWORKSOCKET_H



#ifndef WIN32
#include "config.h"

#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#else 


#ifndef CYGWIN
#include <Winsock2.h>
#include "win_config.h"
#else 
#include <cygwin/in.h> 
#endif 

#define MSG_WAITALL NULL  // HACK HACK NOT SURE IF IT WORKS 
#endif


#include "MarSystem.h"
#include "common.h"

#define MAXCONNECT 1


class NetworkSocket: public MarSystem
{
	
 public:
 
  //! default constructor, used by distributed binaries
  NetworkSocket();
  
  //! class constructor that creatse a socket server listening on that port
  NetworkSocket( int port );
  
  //! class constructor - creates a client connection to specified host and port
  NetworkSocket( int port, const std::string& hostname );
  
  //! destructor
  virtual ~NetworkSocket();
  
  //! Close the socket with the given descriptor.
  static void close( int socket );
  
  //! Return the server/client socket descriptor.
  int getSocket( void ) const;

  //! If this is a TCP server, accept the first connection and return the FD
  int accept( void );
  
  //! If enable = false, the socket is set to non-blocking mode.  When first created, sockets are by default in blocking mode.
  static void setBlocking( int socket, bool enable );

  //! Write a buffer over the socket connection.
  int writeBuffer(const void *buffer, long bufferSize, int flags = 0);

  //! Read a buffer via the specified socket. 
  static int readBuffer(int socket, void *buffer, long bufferSize, int flags );
  
  //! returns the address of the client connected
  std::string inet_ntoa( void ) const;
  
  //! checks if a socket descriptor is valid
  static bool valid( int sockfd ); 
  
  //! returns to the caller of the marsystem, the controls from the remote host
  real* const recvControls();
  
protected:
  
  //! creates a socket based on type_
  bool createSocket( void );

  //! routine setup for a server (handles TCP and UDP), assume no controls
  bool setupSource ( natural dataPort = 5009, natural controlsPort = 0 );
  
  //! routine setup for a client to connect to a host, assume no controls
  bool setupSink ( const std::string& host, natural dataPort = 5009, natural controlsPort = 0 );
   
  //! sends an vector to a TCP socket connection
  bool sendTCP ( realvec& in, natural inSamples, 
  			natural inObservations, real israte );
	      
  //! sends an vector to a UDP socket connection
  bool sendUDP ( realvec& in, natural inSamples, 
  			natural inObservations, real israte );
  
  //! receives a vector from a TCP socket connetion
  int recvTCP ( realvec& out ) ;
  
  //! receives a vector from a UDP socket connetion
  int recvUDP ( realvec& out ) ;
  
  //! stub function so this object can be instantiated
  void process( realvec& in, realvec& out );
  
  //! stub function so this object can be instantiated
  void addControls();
 
  //! ensures we have a valid server when recvControls or process is called
  virtual void refresh(){}
  
  int s_sock; 		// server side socket FD for datas
  int c_sock;		// server side socket FD For controls
  int client_c; 	// a new client socket FD (data)
  int client_d;		// a new client socket FD (controls)
  
  
  bool byteSwap;
  real swap( real argument );
  real controls[4];	// a pointer to the controls received
  
private:


  struct sockaddr_in m_addr, c_addr, cliAddr, cliCont;
  int count;
  		 
};

#endif
