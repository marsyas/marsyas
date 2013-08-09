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

#if !defined(__Server_h)
#define __Server_h

#include <stdio.h>
#include <signal.h>

#ifndef WIN32
#include <strings.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <errno.h>
#else
#include <io.h>
#include <Winsock2.h>
#include <process.h>
#endif

#include <string>

#include "Talk.h"

namespace Marsyas
{
/**
    \class Server
	\ingroup none
    \brief Basic server class

    Create a server that can handle multiple clients.
Send/receive message strings Communicator methods are supported.
Used for compatibility with the marsyas 0.1 interface.
*/


class Server: public Communicator
{
private:
  int establish(unsigned short portnum);
  int get_connection(int s);
  int write_data(int socket, char *buf, int size);
  int read_data(int socket, char *buf, int size);
  void talk(int socket);
  int channel_;
  int socket_;
  Talk talk_;
public:
  Server();
  ~Server();
  void start(char *machine,int port_num);
  void send_message(std::string message);
  void receive_message(char *message);
};

}//namespace Marsyas

#endif



