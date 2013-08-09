//
//  Widget.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 6/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "Widget.h"
#include "GraphicalEnvironment.h"
#include "globalVars.h"


using namespace Marsyas;


Widget::Widget()
{
  //printf("\nWidget() Construtor");
  setup(0, 0, 0, 0, NULL);
}

Widget::Widget(int x0, int y0, int w, int h, GraphicalEnvironment* env)
{
  //printf("\nWidget(int x0, int y0, int w, int h, GraphicalEnvironment* env) Construtor");
  setup(x0, y0, w, h, env);
}


Widget::~Widget()
{

}

void Widget::setup(int x0, int y0, int w, int h, GraphicalEnvironment* env)
{


  env_ = env;


  state_ = false; //whatever...

  isMouseOver_ = false;

  updatePosition(x0, y0, w, h);



}

void Widget::updatePosition(int x0, int y0)
{

  x_ = x0;
  y_ = y0;

  xForMouse_ = x_;
  yForMouse_ = y_;
}


void Widget::updatePosition(int x0, int y0, double zx, double zy, int ox, int oy)
{

  x_ = x0;
  y_ = y0;

  updateValuesForMouse(zx, zy, ox, oy);

}


void Widget::updatePosition(int x0, int y0, int w0, int h0)
{
  x_ = x0;
  y_ = y0;

  width_ = w0;
  height_ = h0;

  xForMouse_ = x_;
  yForMouse_ = y_;
  widthForMouse_ = width_;
  heightForMouse_ = height_;

}

void Widget::updatePosition(int x0, int y0, int w0, int h0, double zx, double zy, int ox, int oy)
{
  x_ = x0;
  y_ = y0;

  width_ = w0;
  height_ = h0;

  updateValuesForMouse(zx, zy, ox, oy);

}

void Widget::updateValuesForMouse(double zoomX, double zoomY, int offsetX, int offsetY)
{
  widthForMouse_ = width_ * zoomX;
  heightForMouse_ = height_ * zoomY;
  xForMouse_ = (x_ - offsetX) * zoomX;
  yForMouse_ = (y_ - offsetY) * zoomY + MINIVIEW_HEIGHT + 10;

}


bool Widget::mouseOver()
{
  isMouseOver_ = false;
  if(xForMouse_ <= env_->mouse_->x && (xForMouse_ + widthForMouse_) >= env_->mouse_->x)
  {
    if(yForMouse_ <= env_->mouse_->y && (yForMouse_ + heightForMouse_) >= env_->mouse_->y)
    {
      isMouseOver_ = true;
      return true;
    }
  }

  return false;

}

bool Widget::mousePressed()
{
  if(mouseOver())
  {
    if(env_->mouse_->click == 0)
    {
      toggleState();
      dragLock_ = true;
      return true;
    }
  }
  return false;
}

bool Widget::mouseDragged()
{
  if(mouseOver())
  {
    if(env_->mouse_->drag == 1)
    {
      return true;
    }
  }
  return false;
}

bool Widget::mouseReleased()
{
  dragLock_ = false;
}

void Widget::toggleState()
{
  state_ = !state_;
}



int Widget::getX() {
  return x_;
}

int Widget::getY() {
  return y_;
}

int Widget::getWidth() {
  return width_;
}

int Widget::getHeight() {
  return height_;
}

bool Widget::getState() {
  return state_;
}

GraphicalEnvironment* Widget::getEnvironment() {
  return env_;
}

void Widget::setEnv(GraphicalEnvironment* env) {
  env_ = env;
}

void Widget::setX(int x) {
  x_ = x;
}

void Widget::setY(int y) {
  y_ = y;
}

void Widget::setWidth(int w) {
  width_ = w;
}

void Widget::setHeight(int h) {
  height_ = h;
}

void Widget::setState(bool st) {
  state_ = st;
}
