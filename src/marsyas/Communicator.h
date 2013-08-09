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

#if !defined(__Communicator_h)
#define __Communicator_h

#include <string>

namespace Marsyas
{
/**
    \class Communicator
    \ingroup Network
    \brief Abstract base class for communicator

    Abstract base class for a send/receive string message type
    of communicator.
*/


class Communicator
{
public:
  Communicator();
  virtual ~Communicator();
  virtual void send_message(std::string message) =0;
  virtual void receive_message(char *message)=0;
};

}//namespace Marsyas

#endif
