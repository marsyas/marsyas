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
	\class GTinyButton
	\ingroup MarCanvas
	\brief Graphical tiny button for placement within GMarSystem objects on the canvas, current actions are Close, Minimize, Maximize
	\author Neil Burroughs  inb@cs.uvic.ca
*/

#ifndef G_TINY_BUTTON_H
#define G_TINY_BUTTON_H

#include <QWidget>
#include <QPalette>
#include <QObject>
#include <QColor>
#include <QPainter>
#include <QLine>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QEvent>

#include <QMetaType>

class GTinyButton : public QWidget
{
	Q_OBJECT

	bool pressed_, exited_;
public:
	static const char CLOSE    = 1;
	static const char MINIMIZE = 2;
	static const char MAXIMIZE = 4;

	GTinyButton(QWidget* parent = 0);

protected:
	virtual void paintEvent(QPaintEvent* event)=0;
	void mouseReleaseEvent(QMouseEvent* event);


signals:
	void clicked();
};

class GCloseButton : public GTinyButton
{
public:
	GCloseButton(QWidget* parent = 0);
protected:
	virtual void paintEvent(QPaintEvent* event);
};

class GMinimizeButton : public GTinyButton
{
public:
	GMinimizeButton(QWidget* parent = 0);
protected:
	virtual void paintEvent(QPaintEvent* event);
};

class GMaximizeButton : public GTinyButton
{
public:
	GMaximizeButton(QWidget* parent = 0);
protected:
	virtual void paintEvent(QPaintEvent* event);
};

#endif

