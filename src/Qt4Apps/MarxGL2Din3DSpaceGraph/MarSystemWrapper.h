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
    \class MarSystemWrapper
    \brief Wraps a MarSystem network into a Qt-like object with signals/slots

    The MarSystemWrapper is the generic way of interfacing a GUI written 
in Qt 4 with Marsyas. It creates a separate thread for running sound 
through the MarSystem dataflow network. The network is communicates 
with the Qt code through the standard signal/slot mechanism 
adjusted to reflect Marsyas controls. 
*/

#ifndef MARSYSTEMWRAPPER_H
#define MARSYSTEMWRAPPER_H

#include <QObject>
#include <QCheckBox>
#include <QThread>
#include <QString>

#include <string> 

//#include "common_source.h"
#include "MarSystemManager.h"


class MarSystemWrapper: public QThread
{
  Q_OBJECT
  
public:
  MarSystemWrapper(Marsyas::MarSystem* msys);
  
  
public slots:

  void updctrl(QString cname, Marsyas::MarControlValue value);
  Marsyas::MarControlValue getctrl(std::string cname);
  
  void play();
  void pause();
  void run();
  
signals: 
  void ctrlChanged(QString cname, Marsyas::MarControlValue value);
  void posChanged(int val);
  
private:
  QString cur_cname;
  Marsyas::MarControlValue cur_value;

  Marsyas::MarSystem* msys_;			// the underlying MarSystem
  
  std::vector<QString> cnames_;
  std::vector<Marsyas::MarControlValue> cvalues_;
  
  bool guard_;
  bool pause_;
  bool empty_;
  
  bool running_;  
  
};

#endif // MARSYSTEMWRAPPER_H
