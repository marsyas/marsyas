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

#include "GTinyButton.h"

#include <QPainter>
//#include <iostream>

GTinyButton::GTinyButton(QWidget* parent) : QWidget(parent)
{
	setFixedSize(10,10);
	pressed_=false;
	exited_=false;
}
void
GTinyButton::mouseReleaseEvent(QMouseEvent* event)
{
	if(event->x()>=0&&event->x()<width()) {
		if(event->y()>=0&&event->y()<height()){
			emit clicked();
		}
	}
}

GCloseButton::GCloseButton(QWidget* parent) : GTinyButton(parent)
{
}

void
GCloseButton::paintEvent(QPaintEvent* /* event */)
{
	QPainter painter(this);
	painter.setPen(Qt::black);

	int w=width(), h=height();
	painter.drawRect(0,0,w-1,h-1);

	painter.drawLine(2,2,w-3,h-3);
	painter.drawLine(w-3,2,2,h-3);
}

GMinimizeButton::GMinimizeButton(QWidget* parent) : GTinyButton(parent)
{
}

void
GMinimizeButton::paintEvent(QPaintEvent* /* event */)
{
	QPainter painter(this);
	painter.setPen(Qt::black);

	int w=width(), h=height();
	painter.drawRect(0,0,w-1,h-1);

	painter.drawLine(2,3,2,h-3);
	painter.drawLine(2,h-3,6,h-3);
	painter.drawLine(2,h-3,w-3,2);
}

GMaximizeButton::GMaximizeButton(QWidget* parent) : GTinyButton(parent)
{
}

void
GMaximizeButton::paintEvent(QPaintEvent* /* event */)
{
	QPainter painter(this);
	painter.setPen(Qt::black);

	int w=width(), h=height();
	painter.drawRect(0,0,w-1,h-1);

	painter.drawLine(3,2,w-3,2);
	painter.drawLine(w-3,2,w-3,h-4);
	painter.drawLine(2,h-3,w-3,2);
}

