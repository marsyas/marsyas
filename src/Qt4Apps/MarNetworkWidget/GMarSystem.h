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
	\class GMarSystem
	\ingroup MarCanvas
	\brief Graphical MarSystem object for placement on the MarCanvas
	\author Neil Burroughs  inb@cs.uvic.ca
*/

#ifndef G_MARSYSTEM_H
#define G_MARSYSTEM_H

#include <QObject>
#include <QMetaType>

#include <QString>
#include <QEvent> 
#include <QMouseEvent>
#include <QPaintEvent>
#include <QFocusEvent>

#include <QMimeData>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>

#include "MarSystem.h"
#include "GWidget.h"
#include "GTinyButton.h"

#define MARSYAS_MIME_TYPE "application/marsyas"
/*
class MoveHelper
{
public:
	GMarSystem* widget_;
	MarNetworkCanvas* canvas_;

	MoveHelper();
	MoveHelper(GMarSystem* widget, MarNetworkCanvas* canvas);

	void move(int adj_x, int adj_y);
};
*/
class GMarSystem : public GWidget
{
	Q_OBJECT

public:
	GMarSystem(GWidget* parent = 0);
	GMarSystem(MarSystem* m, GWidget* parent = 0);
	GMarSystem(QString type, QString name, GWidget* parent = 0);

	virtual void revalidate();
	GMarSystem* getTopWidget();
	bool isGMarSystem();

public slots:
	virtual void close();
	virtual void updateChildren();

signals:
	void resized();
	void clicked(MarSystem*);

protected:
	void init(MarSystem* m);
	void init(QString tp, QString nm);
	virtual void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void doRelease();
	void mouseMoveEvent(QMouseEvent* event);
	void enterEvent(QEvent* event);
	void leaveEvent(QEvent* event);

	virtual void focusIn();
	virtual void focusOut();

	static const int horiz_buff_width = 5;
	static const int vert_buff_height = 5;
	static const int default_width = 40;
	static const int default_height = 40;
	static const int min_width = 80;

	MarSystem* marsystem_;
	QString name_, type_;
	int font_height_; // for type and name alignment
	GCloseButton close_button_;
	GMarSystem* topwidget_;
	/** mouse button is pressed, used for moving */
	bool pressed_;
	int grab_x_, grab_y_;
	/** widget has been moved, therefore not clicked. */
	bool moved_;
	

//	void drawClose(QPainter& painter);
//	void drawMinimize(QPainter& painter);
};
//------------------------------------------------------------------
class GComposite : public GMarSystem
{
	Q_OBJECT

public:
	GComposite(GWidget* parent = 0);
	GComposite(MarSystem* m, GWidget* parent = 0);
	void revalidate();
	virtual void revalidate_maximized()=0;
	void addMarSystem(GMarSystem* ms);
	void insertMarSystem(GMarSystem* ms, unsigned int p);
	virtual void focusIn();
	virtual void focusOut();
	//void revalidateButtons();

public slots:
	virtual void minimize();
	virtual void maximize();

protected:
	static const int horiz_dist_between_marsym = 20;
	static const int vert_dist_between_marsym = 20;
	static const int vert_buffer = 10;
	static const int horiz_buffer = 10;

	int in_side_buffer;

	std::vector<GMarSystem*> marsyms_;
	QPoint drag_insert_pos_;
	int drag_insert_pos_dim_;
	bool highlight_insert_;

	GMaximizeButton maximize_button_;
	GMinimizeButton minimize_button_;
	bool minimized_;
	void init();
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dragLeaveEvent(QDragLeaveEvent *event);
	virtual void dropEvent(QDropEvent *event);
	virtual void dragMoveEvent(QDragMoveEvent* event);

};
//------------------------------------------------------------------
class GSeries : public GComposite
{
protected:
//	std::vector<GMarSystem> marsyms_;
	virtual void dragMoveEvent(QDragMoveEvent* event);
public:
	GSeries(GWidget* parent = 0);
	GSeries(MarSystem* m, GWidget* parent = 0);
	virtual void revalidate_maximized();
	void paintEvent(QPaintEvent* event);
};

class GParallel : public GComposite
{
protected:
//	std::vector<GMarSystem> marsyms_;
	void dragMoveEvent(QDragMoveEvent* event);

public:
	GParallel(GWidget* parent = 0);
	GParallel(MarSystem* m, GWidget* parent = 0);
	virtual void revalidate_maximized();
	virtual void paintEvent(QPaintEvent* event);
};

class GFanout : public GParallel
{
protected:
//	std::vector<GMarSystem> marsyms_;
public:
	GFanout(GWidget* parent = 0);
	GFanout(MarSystem* m, GWidget* parent = 0);
//	virtual void revalidate();
	void paintEvent(QPaintEvent* event);
};

#endif
