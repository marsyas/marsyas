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
	\class GWidget
	\ingroup MarCanvas
	\brief Generic widget class for things on the MarCanvas
	\author Neil Burroughs  inb@cs.uvic.ca
*/

#ifndef G_WIDGET_H
#define G_WIDGET_H

#include <QWidget>
#include <QPalette>
#include <QObject>
#include <QColor>

#include <QMetaType>

class GWidget : public QWidget
{
	Q_OBJECT

public:
	GWidget(QWidget* parent = 0);

private:
	bool is_focused_;
	bool was_focused_;

protected:
	QColor background_lo_colour_;
	QColor background_hi_colour_;

public:
	void setFocus(bool f);
	bool isFocused();

	void releaseFocus();
	void restoreFocus();

	virtual void focusIn();
	virtual void focusOut();

	void setBackgroundColour(QColor&);
	virtual GWidget* getParent();
	virtual bool isGMarSystem();
};

#endif

