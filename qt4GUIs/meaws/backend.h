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
#include "MarSystemManager.h"
#include "../MarSystemQtWrapper.h"
#include "EvExpr.h"
#include "ExVal.h"
#include "ExCommon.h"

using namespace Marsyas;

class MarBackend: public QObject {
	Q_OBJECT

public:
	MarBackend(int testingMethod);
	~MarBackend();
//	void setIntro(int beats);
//	void playBeat();
	void start();
	void stop();
	void calculate(string filename);
	void setFileName(string filename);

private:
	MarSystemQtWrapper *mrsWrapper;

	int method;
	int introBeats;
// "constructor"
	void makeRecNet();
	void startIntonation();
	void startControl();

// metronome stuff
  EvExpr *e;
  MarSystem *metroNet;
	MarSystem *recNet;
	MarSystem *pitchNet;

	MarSystem *allNet;
  MarControlPtr filenamePtr;

// pitch extracting stuff  (INCOMPLETE)
	void startPitchNet(string sfName);
	void stopPitchNet();
	MarSystem *pitchExtractor;
};

