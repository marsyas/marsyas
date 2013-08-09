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

#include "Server.h"

using namespace std;
using namespace Marsyas;

#define NLCHARS "\x0D\x0A"		// Newline and Carriage return 
#define MAX_MESSAGE 256

#ifdef WIN32
#define MAXHOSTNAMELEN 1024
#endif

/* Code to establish a socket */

Server::Server()
{
}



Server::~Server()
{
  close(socket_);
}



int
Server::establish(unsigned short portnum)
{
  char myname[MAXHOSTNAMELEN+1];
  int s;

  struct sockaddr_in sa;
  struct hostent *hp;

  //bzero((char *)&sa, sizeof(struct sockaddr_in)); /* Clear out address */
  memset((void *)&sa, '\0', sizeof(struct sockaddr_in));
  gethostname(myname, MAXHOSTNAMELEN);	/* Who are we */

  hp = gethostbyname(myname);		/* get our address info */
  if (hp ==  NULL)                      /* we don't exist !? */
  {
    return (-1);
  }

  sa.sin_family = hp->h_addrtype;	/* host address */
  sa.sin_port   = htons(portnum);	/* port number */
  if ((s = socket(AF_INET, SOCK_STREAM,0)) < 0)	/* create socket */
  {
    return(-1);
  }

  if (bind(s,(struct sockaddr*)&sa,sizeof(struct sockaddr_in)) < 0)
  {
    return (-1);			/* bind address to socket */
  }

  listen(s,3);				/* max # of queued connects */
  return (s);
}


/* Accept a connection on a socket that has been created
using the establish() function. s is the socket created
with establish
*/

int
Server::get_connection(int s)
{
  struct sockaddr_in isa;		// address of socket
  unsigned int i;               	// size of address


  int t;				// socket of connection


  i = sizeof(isa);			// find socket address
  getsockname(s,(sockaddr *)&isa, (socklen_t *)&i);		// for accept() */
  if ((t = accept(s,(sockaddr *)&isa, (socklen_t *)&i)) < 0)	// accept connection if there is one
    return -1;
  return(t);
}



/* WrIte a specified number of characters to the socket. */
int
Server::write_data(int socket, char *buf, int size)
{
  int bcount;				// counts bytes read
  int br;				// bytes read this pass

  bcount = 0;
  br = 0;

  while(bcount < size) 		// Loop until full buffer
  {
    if ((br = write(socket, buf, size-bcount)) > 0)
    {
      bcount += br;			// increment byte counter
      buf += br;			// move buffer ptr for next read
    }
    if (br < 0)
      return -1;			// signal an error to the caller
  }
  return bcount;
}





/* Read a specified number of characters into a buffer
 */

int
Server::read_data(int socket, char *buf, int size)
{
  int bcount;				// counts bytes read
  int br;				// bytes read this pass

  bcount = 0;
  br = 0;

  while(bcount < size) 		// Loop until full buffer
  {
    if ((br = read(socket, buf, size-bcount)) > 0)
    {
      bcount += br;			// increment byte counter
      buf += br;			// move buffer ptr for next read
    }
    if (br < 0)
      return -1;			// signal an error to the caller
  }

  return bcount;
}











static void
fireman(int ignore)
{
  int wstatus;
  (void) ignore;
  (void) fireman; // This function is no longer used, so warning is raised
  while(wait3((int *)&wstatus, WNOHANG, NULL) >= 0);
}


void
Server::receive_message(char *message)
{
  char *mes = message;
  int bytes_read;
  bytes_read = recv(channel_, mes, MAX_MESSAGE,0);

  message[bytes_read] = '\0';
}


void
Server::send_message(string message)
{
  // Do not send \0. Just send the characters
  // Hack to work with Java.
  //printf("Server send message\n");
  send(channel_, message.c_str(), message.length(), 0);
}



void
Server::start(char *machine, int port_num)
{
  (void) machine;

  if ((socket_ = establish(port_num)) < 0)
  {
    perror("establish");
    exit(1);
  }
  // signal(SIGCHLD, fireman);             // this eliminates zombies



  while(1)				// loop for phone calls
  {
    if ((channel_ = get_connection(socket_)) <0)	// get connection
    {
      if (errno == EINTR)		// EINTR might happen on accept(),
        continue;			// try again
      perror("accept");		// bad
      exit(1);
    }
    switch(fork())			// try to handle connection
    {
    case -1:			// we're the child, do something
      perror("fork");
      close(socket_);
      exit(1);
    case 0:
      talk_.init(this);			// Initialize talk module
      char buf[256];
      while(1)
      {
        receive_message(buf);
        talk_.process(buf);
      }
      exit(0);
    default:			// we're the parent so look for
      continue;			// another connection
    }
  }
}


