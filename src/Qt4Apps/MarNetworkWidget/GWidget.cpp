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

#include "GWidget.h"
#include <iostream>

GWidget::GWidget(QWidget* parent) : QWidget(parent)
{
	is_focused_=false;
	was_focused_=false;
}

bool
GWidget::isFocused()
{
	return is_focused_;
}
void
GWidget::focusIn()
{
}
void
GWidget::focusOut()
{
}

GWidget*
GWidget::getParent()
{
	return (GWidget*)parentWidget();
}

void
GWidget::setFocus(bool f)
{
	if(f){
		is_focused_=true;
		was_focused_=false;
		GWidget* p=getParent();
		if(p!=NULL) {
			p->releaseFocus();
		}
		focusIn();
	}
	else{
		if(is_focused_) {
			is_focused_=false;
			was_focused_=true;
		}
		GWidget* p=getParent();
		if(p!=NULL) {
			p->restoreFocus();
		}
		focusOut();
	}
}

void
GWidget::restoreFocus()
{
	if(was_focused_){
		is_focused_=true;
		was_focused_=false;
		focusIn();
	}
}
void
GWidget::releaseFocus()
{
	if(is_focused_) {
		was_focused_=true;
		is_focused_=false;
		focusOut();
	}
}

void
GWidget::setBackgroundColour(QColor& col)
{
	QPalette p = palette();
	p.setColor(QPalette::Active, QPalette::Window, col);
	p.setColor(QPalette::Inactive, QPalette::Window, col);
	setPalette(p);
}

bool
GWidget::isGMarSystem()
{
	return false;
}

