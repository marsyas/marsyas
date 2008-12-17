/*
** Copyright (C) 1998-2009 George Tzanetakis <gtzan@cs.uvic.ca>
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
	\class MarNetworkCanvas
	\ingroup MarCanvas
	\brief Canvas for drawing MarSystem networks
	\author Neil Burroughs  inb@cs.uvic.ca
*/

#ifndef MAR_NETWORK_CANVAS_H
#define MAR_NETWORK_CANVAS_H

#include <QPaintEvent>
#include <QMouseEvent>
#include <QString>

#include <QMimeData>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDropEvent>

#include <QObject>
#include <QMetaType>

#include "GWidget.h"
#include "MarSystem.h"
#include "GMarSystem.h"

class MarNetworkCanvas : public GWidget
{
	Q_OBJECT

private:
	GMarSystem* ms;
	int grab_x_, grab_y_; bool pressed_;
	QDrag* drag;


	QPixmap icon;

public:
	MarNetworkCanvas(GWidget* parent = 0);
//	void clear();
	void setNetwork(MarSystem* m);
	GMarSystem* create(MarSystem* m, GWidget* p);
	void loadStyleSheet(const QString &sheetName);

protected:
	void paintEvent(QPaintEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void dragEnterEvent(QDragEnterEvent*);
	virtual void dropEvent(QDropEvent*);

public slots:
	virtual void updateCanvas();
};

#endif

