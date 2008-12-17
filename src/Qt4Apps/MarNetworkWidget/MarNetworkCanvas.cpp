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

#include "MarNetworkCanvas.h"

#include <vector>

#include <QFile>
#include <QPainter>
#include <QPixmap>

#include <QDragMoveEvent>
#include <QMouseEvent>

MarNetworkCanvas::MarNetworkCanvas(GWidget* parent) : GWidget(parent)
{
	setFixedSize(640,480);
	pressed_=false;
	setAcceptDrops(true);
	icon=QPixmap("icon.png");
/*
	ms = new GMarSystem(this);
	ms->move(20,20);
*/
/*
	ss = new GSeries(this);
	ss->addMarSystem(new GMarSystem("Gain","gain"));
	ss->addMarSystem(new GMarSystem("Filter","filter"));
	ss->move(100,10);
	ss->revalidate();
*/

/*
	pl = new GParallel(this);

	ss = new GSeries(this);
	ss->addMarSystem(new GMarSystem("Gain","gain"));
	ss->addMarSystem(new GMarSystem("Filter","filter"));
	pl->addMarSystem(ss);

	pl->addMarSystem(new GMarSystem("Gain","gain"));

	fo = new GFanout(pl);
	fo->addMarSystem(new GMarSystem("Gain","gain"));
	fo->addMarSystem(new GMarSystem("SoundFileSource","src"));
	pl->addMarSystem(fo);

	pl->addMarSystem(new GMarSystem("SoundFileSource","src"));
	pl->move(200,20);
	pl->revalidate();
*/

/*
	fo = new GFanout(pl);
	fo->addMarSystem(new GMarSystem("Gain","gain"));
	fo->addMarSystem(new GMarSystem("Filter","filter"));
	fo->addMarSystem(new GMarSystem("SoundFileSource","src"));
	fo->move(300,50);
	fo->revalidate();
*/

//	QPushButton* quit = new QPushButton(tr("Quit"),this);
//	quit->setGeometry(62,40,75,30);
//	quit->setFont(QFont("Times", 18, QFont::Bold));

//	connect(quit, SIGNAL(clicked()), qApp, SLOT(quit()));
}
void
MarNetworkCanvas::setNetwork(MarSystem* m)
{
	ms=create(m,this);
	ms->move(40,40);
	ms->revalidate();
}
GMarSystem*
MarNetworkCanvas::create(MarSystem* m, GWidget* p)
{
	std::string t = m->getType();
	GComposite* c=NULL;
	if (t=="Series") {
		c = new GSeries(m,p);
//		c->setStyleSheet("* {background: blue;color:red;}");
	}
	else if (t=="Fanout") {
		c = new GFanout(m,p);
	}
	else if (t=="Parallel") {
		c = new GParallel(m,p);
	}
	else {
		return new GMarSystem(m,p);
	}
	std::vector<MarSystem*> cs = m->getChildren();
	for(unsigned int i=0;i<cs.size();i++) {
		MarSystem* z = cs[i];
		c->addMarSystem(create(z,c));
	}
	connect(c, SIGNAL(resized()), this, SLOT(updateCanvas()));
	return c;
}

void
MarNetworkCanvas::updateCanvas()
{
	ms->revalidate();
}

void
MarNetworkCanvas::paintEvent(QPaintEvent* /* event */)
{
	QPainter painter(this);
}

// #include <iostream>
void
MarNetworkCanvas::loadStyleSheet(const QString &sheetName)
{
//	QFile file(":/qss/" + sheetName + ".qss");
	QFile file("" + sheetName + ".qss");
	file.open(QFile::ReadOnly);
	QString styleSheet = QLatin1String(file.readAll());
//	std::cout << "\"" << styleSheet.toStdString() << "\"" << std::endl;

	this->setStyleSheet(styleSheet);
}

void
MarNetworkCanvas::mousePressEvent(QMouseEvent* event)
{
	if(event->button()==Qt::LeftButton){
		grab_x_=event->pos().x();
		grab_y_=event->pos().y();
		pressed_=true;
		setCursor(Qt::ClosedHandCursor);
	}
	// TEMPORARY TO TEST DRAG AND DROP!
	else if(event->button()==Qt::RightButton){
		drag = new QDrag(this);
		QMimeData* mimeData = new QMimeData;
		mimeData->setData("application/marsyas", "Marsyas");
//		mimeData->setText("Marsyas");
		drag->setMimeData(mimeData);
		drag->setPixmap(icon);

		Qt::DropAction dropAction = drag->start();
	}
}

void
MarNetworkCanvas::mouseReleaseEvent(QMouseEvent* event)
{
	pressed_=false;
	setCursor(Qt::ArrowCursor);
}

void
MarNetworkCanvas::mouseMoveEvent(QMouseEvent* event)
{
	if(pressed_&&ms!=NULL){
		int x=event->pos().x();
		int y=event->pos().y();
		ms->move(ms->x()+(x-grab_x_),ms->y()+(y-grab_y_));
		grab_x_=x;
		grab_y_=y;
	}
}

void
MarNetworkCanvas::dragEnterEvent(QDragEnterEvent *event)
{
	event->ignore(); // if you don't want it, ignore it
//	if(event->mimeData()->hasFormat("text/plain"))
//		event->acceptProposedAction();
}

void
MarNetworkCanvas::dropEvent(QDropEvent *event)
{
	event->ignore();
//	event->acceptProposedAction();
}

