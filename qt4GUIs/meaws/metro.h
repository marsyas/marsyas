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
#include "../MarSystemQtWrapper.h"

using namespace Marsyas;

class Metro: public QObject {
	Q_OBJECT

public:
	Metro();
	~Metro();

// communication with the QT front-end
public slots:
  void setTempo(int tempo);
	void setIntro(int beats);
	void stopMetro();
	void startMetro();
	void beat();

private:
	int tempo;
	int introBeats;

	QTimer *timer;


// audio stuff
	MarSystemQtWrapper *mrsWrapper;
  MarSystem *metroNet;
  MarControlPtr positionPtr;
};

