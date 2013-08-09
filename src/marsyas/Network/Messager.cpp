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


#include "Messager.h"

using namespace std;
using namespace Marsyas;

int socket_port = 2001;

Messager :: Messager(int inputMask, int port)
{

  sources = inputMask;
  rtDelta = RT_BUFFER_SIZE;
  messageIndex = 0;
  nMessages = 0;

  if ( sources ) {

    socket_port = port;

    /* start server */
    soket = new NetworkSocket(port);

    nSockets = 0;
    maxfd = 0;
    FD_ZERO(&mask);

    // UDPATE
    int d = soket->getSocket();

    // add server socket to select list of file descriptors
    FD_SET(d, &mask);
    if (d > maxfd) maxfd = d;

    // The fd array is used to hold the file descriptors for all
    // connected sockets.  This saves some time incrementing through
    // file descriptors when using select().
    for (int i=0; i<16; i++)
      fd[i] = 0;

    // Start the stdin input thread.
    thread = new Thread();
    if ( !thread->start( (THREAD_FUNCTION)&stdinHandler, NULL ) ) {
      sprintf(error, "Messager: Unable to start stdin input thread!");
      // handleError( error, StkError::PROCESS_THREAD );
    }
  }

}

Messager :: ~Messager()
{
  delete soket;
  delete thread;
}

long Messager :: getType() const
{
  return type;
}

long Messager :: getChannel() const
{
  return channel;
}

void Messager :: setRtDelta(long nSamples)
{
  if ( nSamples > 0 )
    rtDelta = nSamples;
  else
    MRSWARN("Messager: setRtDelta less than or equal to zero!");
}

string Messager::getMessage()
{
  return message[(messageIndex-1) % MAX_MESSAGES];
}


long Messager :: getDelta() const
{
  return delta;
}


long Messager :: nextMessage()
{
  //cout << "nextMessage()" << endl;
  /* read the message and remove from circular buffer of message */
  if (nMessages > 0 ) nMessages--;
  type = 0;

  /* if there are no message read a new one from socket */
  if (nMessages == 0)
  {
    /* read a message from socket connection*/
    int hasMessage = socketMessage();
    if (!hasMessage) return type;
  }

  /* msg contains the message */
  string msg = message[messageIndex];
  string exitMsg = "Quit\n";
  messageIndex++;

  if (messageIndex >= MAX_MESSAGES) messageIndex = 0;

  if (msg == exitMsg)
    type = -1;

  if (nMessages == 0)
    return type;
  float temp = 0.0;

  if ( temp >= 0.0 )
    // delta = (long) (temp * Stk::sampleRate());
    delta = (long) (temp * 22050);
  else
    // Ignore negative delta times (absolute time).
    delta = rtDelta;

  return type;
}



bool Messager :: socketMessage()
{

  // FIXME Should rmask have storage-type register? It's address is passed to select.
  // register fd_set rmask; // Old Line
  fd_set rmask; // New Line
  static struct timeval timeout = {0, 0};
  rmask = mask;
  int mys = select(maxfd+1, &rmask, (fd_set *)0, (fd_set *)0, &timeout);

  if (mys)
  {
    // A file descriptor is set.
    // Check if there's a new socket connection available.
    if ( FD_ISSET(soket->getSocket(), &rmask) )
    {
      // Accept and service new connection.
      int newfd = soket->accept();

      if ( newfd < 0 ) {
        sprintf(error, "Messager: Couldn't accept connection request!");
        // handleError(error, StkError::WARNING);
      }
      cout << "connection established" << endl;

      // We assume the first connection will occur for the stdin
      // thread socket.  Since this connection is "hidden" from
      // the user, only print connected messages for subsequent
      // connections.
      if (nSockets == 0)
      {
        pipefd = newfd;
      }

      // Set the socket to non-blocking mode.
      NetworkSocket::setBlocking( newfd, false );

      // Save the descriptor and update the masks.
      fd[nSockets++] = newfd;
      FD_SET(newfd, &mask);
      if ( newfd > maxfd) maxfd = newfd;


      FD_CLR(soket->getSocket(), &rmask);
    }

    // Check client socket connections.
    unsigned int client = 0;
    while ( client < nSockets ) {
      if ( !FD_ISSET(fd[client], &rmask) )
        client++;
      else {

        // This connection has data.
        if ( !readSocket( fd[client] ) ) {
          // The socket connection closed.
          nSockets--;
          if ( nSockets == 0 ) {
            type = -1;
            return false;
          }
          if (client < nSockets) {
            // Move descriptors down in the list.
            for (unsigned int j=client; j<nSockets; j++)
              fd[j] = fd[j+1];
          }
          delta = 0;
          return false;
        }

        if ( !strncmp(message[messageIndex], "Exit", 4) || !strncmp(message[messageIndex], "exit", 4) ) {
          // We have an "Exit" message ... don't try to parse it.
          messageIndex++;
          nMessages--;
          delta = 0;
          return false;
        }
        // Not an "Exit" message ... parse it.
        return true;
      }
    }
  }
  // If we get here, we checked all devices but found no messages.
  delta = rtDelta;
  return false;
}

#include <errno.h>



bool Messager :: readSocket(int fd)
{

  // This method will read all data available from a socket
  // connection, filling the message buffer.  This is necessary
  // because the select() function triggers on socket activity, not on
  // the presence of (buffered) data.  So, whenever activity is
  // indicated, we need to grab all available data.
  char buffer[MESSAGE_LENGTH];
  int index = 0, m = 0, bufferSize = 0;
  int nextMessage;

  nextMessage = (messageIndex + nMessages) % MAX_MESSAGES;
  memset(message[nextMessage], 0, MESSAGE_LENGTH);



  errno = 0;
#ifndef WIN32
  while (bufferSize != -1 && errno != EAGAIN)
  {
#else
  while (bufferSize != SOCKET_ERROR && WSAGetLastError() != WSAEWOULDBLOCK)
  {
#endif


    while (index < bufferSize)
    {
      message[nextMessage][m++] = buffer[index];
      if (buffer[index++] == '\n') {
        m = 0;

        nMessages++;
        nextMessage = (messageIndex + nMessages) % MAX_MESSAGES;


        memset(message[nextMessage], 0, MESSAGE_LENGTH);

      }
    }

    index = 0;



    memset(buffer, 0, MESSAGE_LENGTH);
    bufferSize = NetworkSocket::readBuffer(fd, buffer, MESSAGE_LENGTH, 0);






    if (bufferSize == 0) {
      FD_CLR(fd, &mask);
      NetworkSocket::close( fd );
      cout << "closed socket" << endl;
      return false;
    }
  }

  return true;
}

THREAD_RETURN THREAD_TYPE stdinHandler(void *)
{


  char message[MESSAGE_LENGTH];

  NetworkSocket *s;

  // connects to the server...
  s = new NetworkSocket( socket_port, "localhost" );



  for (;;) {
    memset(message, 0, MESSAGE_LENGTH);

    if ( fgets(message, MESSAGE_LENGTH, stdin) == 0 ) {
      break;
    }

    // Check for an "Exit" message.
    if ( !strncmp(message, "Exit", 4) || !strncmp(message, "exit", 4) )
    {
      break;
    }


    if ( s->writeBuffer( (void *)message, strlen(message), 0) < 0 ) {
      fprintf(stderr, "Messager: stdin thread connection to socket server failed!\n");
      break;
    }

  }
  delete s;
  return NULL;
}

