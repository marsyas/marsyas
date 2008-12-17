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

#include "GMarSystem.h"
#include <QPainter>
#include <QFontMetrics>
#include <QPalette>
#include <QColor>

//#include <iostream>
//#include <stdio.h>

GMarSystem::GMarSystem(GWidget* parent) : GWidget(parent)
{
	marsystem_=NULL;
	init("MarSystem","frank");
}

GMarSystem::GMarSystem(MarSystem* m, GWidget* parent) : GWidget(parent)
{
	init(m);
}

GMarSystem::GMarSystem(QString t, QString n, GWidget* parent) : GWidget(parent)
{
	init(t,n);
}

void
GMarSystem::init(MarSystem* m)
{
	marsystem_=m;
	init(m->getType().c_str(),m->getName().c_str());
	pressed_=false;
}
void
GMarSystem::close()
{
	// TODO: close should delete object from network,
	//       mix in a little hand waving for good measure

	doRelease();
}
void
GMarSystem::init(QString tp, QString nm)
{
	type_=tp; name_=nm;
	topwidget_=NULL;
	close_button_.setParent(this);
	close_button_.setVisible(false);
	connect(&close_button_, SIGNAL(clicked()), this, SLOT(close()));

	background_lo_colour_=QColor(176,192,192);
	background_hi_colour_=QColor(192,208,208);

	QPalette newPalette = palette();
	newPalette.setColor(QPalette::Active, QPalette::Window, background_lo_colour_);
	newPalette.setColor(QPalette::Inactive, QPalette::Window, background_lo_colour_);

	newPalette.setColor(QPalette::Active, QPalette::WindowText, Qt::black);
	newPalette.setColor(QPalette::Inactive, QPalette::WindowText, Qt::black);

	setPalette(newPalette);

//	setBackgroundRole(background_lo_colour_);
	setAutoFillBackground(true);
	setMouseTracking(true);

	// need to revalidate but do we call this one?
	GMarSystem::revalidate();
}

void
GMarSystem::paintEvent(QPaintEvent* /* event */)
{
	QPainter painter(this);
	painter.setPen(Qt::black);
	painter.drawRect(0,0,width()-1,height()-1);

	int hx = height()-font_height_;
	painter.drawText(0,0,width(),hx,Qt::AlignCenter,type_);//tr("Neil"));
	painter.drawText(0,font_height_,width(),hx,Qt::AlignCenter,name_);//tr("Neil"));
}

void
GMarSystem::mousePressEvent(QMouseEvent* event)
{
	if(event->button()==Qt::LeftButton){
		grab_x_=event->pos().x();
		grab_y_=event->pos().y();
		pressed_=true;
		topwidget_=getTopWidget();
	}
}

void
GMarSystem::mouseReleaseEvent(QMouseEvent* )//event)
{
//	if (mouseEvent->button() != Qt::LeftButton) return;
//	setBackgroundRole(QPalette::Midlight);
	doRelease();
	if(!moved_){
		emit clicked(marsystem_);
	}
}
/**
 * we need to do some relase stuff when a minimize/maximize button is pressed.
 * The actual release event is handled by the tiny button not the GMarSystem.
 */
void
GMarSystem::doRelease()
{
	pressed_=false;
	setCursor(Qt::ArrowCursor);
}

void
GMarSystem::mouseMoveEvent(QMouseEvent* event)
{
//cout << "MoveEvent: Pressed=" << pressed_ << endl;
	if(pressed_&&(topwidget_!=NULL)){
		if(!moved_){
			moved_=true;
			setCursor(Qt::ClosedHandCursor);
		}
		int x=event->pos().x();
		int y=event->pos().y();
		topwidget_->move(topwidget_->x()+(x-grab_x_),topwidget_->y()+(y-grab_y_));
	}
}

void
GMarSystem::focusIn()
{
	setBackgroundColour(background_hi_colour_);//background_hi_colour_);
	close_button_.setVisible(true);
}
void
GMarSystem::focusOut()
{
	setBackgroundColour(background_lo_colour_);//background_lo_colour_);
	close_button_.setVisible(false);
}

void
GMarSystem::enterEvent(QEvent* /* event */)
{
	setFocus(true);
}

void
GMarSystem::leaveEvent(QEvent* /* event */)
{
//	if (mouseEvent->button() != Qt::LeftButton) return;
	setFocus(false);
}

void
GMarSystem::revalidate()
{
	QFontMetrics fm = fontMetrics();
	// discover width of widget so that type and name labels fit inside with buffers
	int tw = fm.width(type_);
	int nw = fm.width(name_);
	int ww = ((tw>nw) ? tw : nw) + (horiz_buff_width << 1);
	if (ww<default_width)
		ww=default_width;

	font_height_ = fm.height();
	int h = (fm.height() << 1) + 2 + (vert_buff_height  << 1);
	int hh = (h>default_height) ? h : default_height;

	setFixedSize(ww,hh);
	close_button_.move(width()-10,0);
}

GMarSystem*
GMarSystem::getTopWidget()
{
	GWidget* p=getParent();
	if(p->isGMarSystem()){
		return ((GMarSystem*)p)->getTopWidget();
	}
	return this;
}

bool
GMarSystem::isGMarSystem()
{
	return true;
}
//------------------------------------------------------------------

GComposite::GComposite(GWidget* parent) : GMarSystem(parent)
{
	init();
}
GComposite::GComposite(MarSystem* m, GWidget* parent) : GMarSystem(m,parent)
{
	init();
}
void
GComposite::init()
{
	setAcceptDrops(true);
	highlight_insert_=false;
	background_lo_colour_=QColor(176,192,192);
	background_hi_colour_=QColor(192,208,208);
	minimize_button_.setParent(this);
	maximize_button_.setParent(this);
	minimize_button_.setVisible(false);
	maximize_button_.setVisible(false);
	minimized_=false;
	connect(&minimize_button_, SIGNAL(clicked()), this, SLOT(minimize()));
	connect(&maximize_button_, SIGNAL(clicked()), this, SLOT(maximize()));
}

void
GComposite::focusIn()
{
	GMarSystem::focusIn();
	if(minimized_)
		maximize_button_.setVisible(true);
	else
		minimize_button_.setVisible(true);
}

void
GComposite::focusOut()
{
	GMarSystem::focusOut();
	minimize_button_.setVisible(false);
	maximize_button_.setVisible(false);
}

void
GComposite::minimize()
{
	minimized_=true;
	for(unsigned int i=0;i<marsyms_.size();i++) {
		GMarSystem* m = marsyms_[i];
		m->setVisible(false);
	}
	maximize_button_.setVisible(true);
	minimize_button_.setVisible(false);
	// MouseReleaseEvent is handled by the tiny button,
	// so we need to toggle our pressed_ flag
	doRelease();
	emit resized();
}
void
GComposite::maximize()
{
	minimized_=false;
	for(unsigned int i=0;i<marsyms_.size();i++) {
		GMarSystem* m = marsyms_[i];
		m->setVisible(true);
	}
	// MouseReleaseEvent is handled by the tiny button,
	// so we need to toggle our pressed_ flag
	doRelease();
	emit resized();
}
void
GMarSystem::updateChildren()
{
	revalidate();
	emit resized();
}

void
GComposite::addMarSystem(GMarSystem* ms)
{
	if (ms!=NULL) {
		marsyms_.push_back(ms);
		ms->setParent(this);
		connect(ms, SIGNAL(resized()), this, SLOT(updateChildren()));
	}
}

void
GComposite::insertMarSystem(GMarSystem* ms, unsigned int p)
{
	if (ms!=NULL) {
		if (p>=marsyms_.size()) {
			addMarSystem(ms);
		}
		else {
			marsyms_.insert(marsyms_.begin() + p, ms);
			ms->setParent(this);
			connect(ms, SIGNAL(resized()), this, SLOT(updateChildren()));
		}
	}
}

void
GComposite::revalidate()
{
	if(minimized_) {
		GMarSystem::revalidate();
	}
	else {
		revalidate_maximized();
		close_button_.move(width()-10,0);
	}
	maximize_button_.move(width()-19,0);
	minimize_button_.move(width()-19,0);
}

#include <iostream>

void
GComposite::dragEnterEvent(QDragEnterEvent *event)
{
	if(event->mimeData()->hasFormat(MARSYAS_MIME_TYPE))
		event->acceptProposedAction();
	else
		event->ignore();
}

void
GComposite::dragLeaveEvent(QDragLeaveEvent*)//event)
{
	highlight_insert_=false;
	update(); // eliminate insert highlighting
}

void
GComposite::dropEvent(QDropEvent *event)
{
	if(event->mimeData()->hasFormat(MARSYAS_MIME_TYPE)){
		event->acceptProposedAction();
		highlight_insert_=false;
		update(); // eliminate insert highlighting
	}
}

void
GComposite::dragMoveEvent(QDragMoveEvent*)// event)
{
}
//------------------------------------------------------------------
GSeries::GSeries(GWidget* parent) : GComposite(parent)
{
	setFixedSize(120,60);
	in_side_buffer=GComposite::horiz_buffer;
}
GSeries::GSeries(MarSystem* m, GWidget* parent) : GComposite(m, parent)
{
	setFixedSize(120,60);
	in_side_buffer=GComposite::horiz_buffer;
}

void
GSeries::revalidate_maximized()
{
	int max_height=0;
//	int lhs = in_side_buffer;
	int lhs = horiz_dist_between_marsym;

	for (unsigned int i=0;i<marsyms_.size();i++) {
		GMarSystem* m = marsyms_[i];
		m->revalidate();
		if (m->height()>max_height)
			max_height=m->height();
		m->move(lhs,m->y());
		lhs = lhs + m->width() + horiz_dist_between_marsym;
	}
	// lhs is now the width of this widget
//	lhs = lhs - horiz_dist_between_marsym + GComposite::horiz_buffer;
	// now that we know the max height we can vertically center each widget
	int mh = max_height >> 1;
	for (unsigned int i=0;i<marsyms_.size();i++) {
		GMarSystem* m = marsyms_[i];
		int h = m->height() >> 1;
		int t = mh - h + GComposite::vert_buffer;
		m->move(m->x(),t);

	}
	setFixedSize(lhs,max_height + GComposite::vert_buffer + GComposite::vert_buffer);
}

void
GSeries::paintEvent(QPaintEvent* event)
{
	if (minimized_){
		GMarSystem::paintEvent(event);
	}
	else{
		QPainter painter(this);
		painter.setPen(Qt::black);
		painter.drawRect(0,0,width()-1,height()-1);

		painter.drawLine(0,height()>>1,width(),height()>>1);
		if(highlight_insert_){
//			painter.setPen(Qt::gray);
			int x=drag_insert_pos_dim_;
			painter.fillRect(x-3,20,7,height()-40,Qt::gray);
//			painter.drawEllipse(drag_insert_pos_.x()-5,drag_insert_pos_.y()-5,10,10);
		}
	}
//	int hx = height_-font_height_;
//	painter.drawText(0,0,width_,hx,Qt::AlignCenter,type_);//tr("Neil"));
//	painter.drawText(0,font_height_,width_,hx,Qt::AlignCenter,name_);//tr("Neil"));
}

void
GSeries::dragMoveEvent(QDragMoveEvent* event)
{
	int x = event->pos().x();
	int y = event->pos().y();

//	int midY=height()>>1;
//	if(y>=(midY-40)&&y<=(midY+40)){
	int s=0;
	int p=x;
	if(p>s) {
		for(unsigned int i=0;i<marsyms_.size();i++){
			GMarSystem* m=marsyms_[i];
			int mp=m->x();
			if(p<mp){
				// highlight between lx -> m->x()
				drag_insert_pos_dim_=(mp+s)>>1;
				highlight_insert_=true;
				update();
				return;
			}
			else{
				s=mp+m->width();
				if(p<s){
					highlight_insert_=false;
					update();
					return;
				}
			}
		}
		if(p<width()){
			drag_insert_pos_dim_=(width()+s)>>1;
			highlight_insert_=true;
		}
		else
			highlight_insert_=false;
		update();
	}
	else if(highlight_insert_) {
		highlight_insert_=false;
		update();
	}
}

GParallel::GParallel(GWidget* parent) : GComposite(parent)
{
	setFixedSize(120,60);
	in_side_buffer=GComposite::horiz_buffer;
}
GParallel::GParallel(MarSystem* m, GWidget* parent) : GComposite(m,parent)
{
	setFixedSize(120,60);
	in_side_buffer=GComposite::horiz_buffer;
}

void
GParallel::dragMoveEvent(QDragMoveEvent* event)
{
	int x = event->pos().x();
	int y = event->pos().y();

	int s=0;
	int p=y;
	if(p>s){
		for(unsigned int i=0;i<marsyms_.size();i++){
			GMarSystem* m=marsyms_[i];
			int mp=m->y();
			if(p<mp){
				// highlight between lx -> m->x()
				drag_insert_pos_dim_=(mp+s)>>1;
				highlight_insert_=true;
				update();
				return;
			}
			else{
				s=mp+m->height();
				if(p<s) {
					highlight_insert_=false;
					update();
					return;
				}
			}
		}
		if(p<height()){
			drag_insert_pos_dim_=(height()+s)>>1;
			highlight_insert_=true;
		}
		else
			highlight_insert_=false;
		update();
	}
	else if(highlight_insert_) {
		highlight_insert_=false;
		update();
	}
}


void
GParallel::paintEvent(QPaintEvent* event)
{
	if (minimized_){
		GMarSystem::paintEvent(event);
	}
	else{
		QPainter painter(this);
		painter.setPen(Qt::black);
		painter.drawRect(0,0,width()-1,height()-1);

		// there's got to be a more efficient way to get the midpoints
		for (unsigned int i=0;i<marsyms_.size();i++) {
			GMarSystem* m = marsyms_[i];
			int mp = m->y() + (m->height() >> 1);
			painter.drawLine(0,mp,width(),mp);
		}
	}
}

void
GParallel::revalidate_maximized()
{
	int my_ht = GComposite::vert_buffer;
//	int my_ht = GComposite::vert_dist_between_marsym;
	int max_width = 0;

	for (unsigned int i=0;i<marsyms_.size();i++) {
		GMarSystem* m = marsyms_[i];
		m->revalidate();
		m->move(in_side_buffer,my_ht);
		my_ht += m->height() + GComposite::vert_dist_between_marsym;
		if (m->width()>max_width)
			max_width=m->width();
	}
	// lhs is now the width of this widget
	my_ht = my_ht - GComposite::vert_dist_between_marsym + GComposite::vert_buffer;
//	setFixedSize(max_width + in_side_buffer + GComposite::horiz_buffer,my_ht);
	setFixedSize(max_width + (GComposite::horiz_dist_between_marsym << 1),my_ht);
}

GFanout::GFanout(GWidget* parent) : GParallel(parent)
{
	in_side_buffer=GComposite::horiz_buffer + GComposite::horiz_buffer;
}

GFanout::GFanout(MarSystem* m, GWidget* parent) : GParallel(m,parent)
{
	in_side_buffer=GComposite::horiz_buffer + GComposite::horiz_buffer;
}

void
GFanout::paintEvent(QPaintEvent* event)
{
	if (minimized_){
		GMarSystem::paintEvent(event);
	}
	else{
		QPainter painter(this);
		painter.setPen(Qt::black);
		painter.drawRect(0,0,width()-1,height()-1);

		int my_midy = height() >> 1;
		// there's got to be a more efficient way to get the midpoints
		for (unsigned int i=0;i<marsyms_.size();i++) {
			GMarSystem* m = marsyms_[i];
			int mp = m->y() + (m->height() >> 1);
			painter.drawLine(0,my_midy,m->x(),mp);
			painter.drawLine(m->x(),mp,width(),mp);
		}
		if(highlight_insert_){
//			painter.setPen(Qt::gray);
			int y=drag_insert_pos_dim_;
			painter.fillRect(20,y-3,width()-40,7,Qt::gray);
//			painter.drawEllipse(drag_insert_pos_.x()-5,drag_insert_pos_.y()-5,10,10);
		}

	}
}
