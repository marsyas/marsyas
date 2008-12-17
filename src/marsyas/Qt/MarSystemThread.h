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
\class MarSystemThread
\brief Wraps a MarSystem network into a QThread

The MarSystemThread creates a separate thread for running sound 
through the MarSystem dataflow network. 
*/

#ifndef MARSYSTEMTHREAD_H
#define MARSYSTEMTHREAD_H

#include "common.h"

#ifdef MARSYAS_QT

#include <QThread>
#include <QMutex> 

#include "MarSystem.h"

namespace Marsyas
{

class MarSystemThread: public QThread
{
  Q_OBJECT
  
private:
	MarSystem* msys_;

	volatile bool stopped_;
	QMutex stopMutex_;

	volatile bool sleep_;
	QMutex sleepMutex_;
	QWaitCondition sleepCondition_;

	void run();

public:
  MarSystemThread(MarSystem* msys);

	void sleep();
	void wakeup();

	void stop();
  
signals: 
	void Empty();

};

}//namespace Marsyas

#endif //MARSYAS_QT

#endif // MARSYSTEMTHREAD_H
