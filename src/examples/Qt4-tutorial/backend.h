/*
** Copyright (C) 2007 Graham Percival <gperciva@uvic.ca>
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

#include <QObject>
#include <QTimer>
#include "MarSystemManager.h"
#include "MarSystemQtWrapper.h"

#include <iostream>
using namespace std;
using namespace Marsyas;
using namespace MarsyasQt;

class MarBackend: public QObject
{
	Q_OBJECT

public:
	MarBackend();
	~MarBackend();
	void openBackendSoundfile(string fileName);

public slots:
	void setBackendVolume(mrs_natural value);
	void getBackendPosition();

signals:
	void changedBackendPosition(mrs_natural value);

private:
// in order to make the MarSystem act like a Qt object,
// we use this wrapper:
	MarSystemQtWrapper *mrsWrapper;
// ... and these pomrs_naturalers:
	MarControlPtr filenamePtr;
	MarControlPtr gainPtr;
	MarControlPtr positionPtr;

// typical Marsyas network:
	MarSystem *playbacknet;
};

