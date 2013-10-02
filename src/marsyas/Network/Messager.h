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

#if !defined(__MESSAGER_H)
#define __MESSSAGER_H

#define MESSAGE_LENGTH  256
#define MAX_MESSAGES 25
#define STK_MIDI        0x0001
#define STK_PIPE        0x0002
#define STK_SOCKET      0x0004
#define MY_FLOAT

#define __STK_REALTIME__

#ifndef WIN32
#include <sys/types.h>
#include <sys/select.h>
#endif

#include <cstring>

#include "Thread.h"
#include "NetworkSocket.h"
#include <marsyas/common_source.h>

#define RT_BUFFER_SIZE 512

extern "C" THREAD_RETURN THREAD_TYPE stdinHandler(void * ptr);

namespace Marsyas
{
/***************************************************/
/*! \class Messager
  \ingroup Networking
  \brief Control message parser.

  This class reads and parses control messages
  from a socket connection or pipe.

  For each call to nextMessage(), the active
  input sources are queried to see if a new
  control message is available.

  This class is primarily for use in
  event loops.

  One of the original goals in creating this
  class was to simplify the message acquisition
  process by removing all threads.  If the
  windoze select() function behaved just like
  the unix one, that would have been possible.
  Since it does not (it can't be used to poll
  STDIN), I am using a thread to acquire
  messages from STDIN, which sends these
  messages via a socket connection to the
  message socket server.  Perhaps in the future,
  it will be possible to simplify things.

  This class is canibalized from the Synthesis
  Toolkit by Perry Cook and Gary Scavone.
*/
/***************************************************/



class Messager // : public Stk
{

public:
  //! Constructor performs initialization based on an input mask and an optional socket port.
  /*!
    The default constructor is set to read input from a SKINI
    scorefile.  The flags STK_MIDI, STK_PIPE, and STK_SOCKET can be
    OR'ed together in any combination for multiple "realtime" input
    source parsing.  An optional socket port number can be specified
    for use when the STK_SOCKET flag is set.  For realtime input
    types, an StkError can be thrown during instantiation.
  */
  Messager(int inputMask = 0, int port = 2001);

  //! Class destructor.
  ~Messager();

  //! Check for a new input message and return the message type.
  /*!
     Return type values greater than zero represent valid messages.
     If an input scorefile has been completely read or all realtime
     input sources have closed, a negative value is returned.  If the
     return type is zero, no valid messages are present.
  */
  long nextMessage(void);

  //! Set the delta time (in samples) returned between valid realtime messages.  This setting has no affect for scorefile messages.
  void setRtDelta(long nSamples);

  //! Return the current message "delta time" in samples.
  long getDelta(void) const;

  std::string getMessage();

  //! Return the current message type.
  long getType() const;

  //! Return the byte two value for the current message.
  float getByteTwo() const;

  //! Return the byte three value for the current message.
  float getByteThree() const;

  //! Return the channel number for the current message.
  long getChannel() const;

protected:

  long type;
  long channel;
  int sources;
  long delta;
  long rtDelta;
  char message[MAX_MESSAGES][MESSAGE_LENGTH];
  unsigned int messageIndex;
  int nMessages;

  // Check socket sources for new messages.
  bool socketMessage(void);

  // Receive and parse socket data.
  bool readSocket(int fd);

  Thread *thread;
  NetworkSocket *soket;

  unsigned int nSockets;
  fd_set mask;
  int maxfd;
  int pipefd;
  int fd[16];
  char error[256];
};

}//namespace Marsyas

#endif // defined(__MESSAGER_H)
