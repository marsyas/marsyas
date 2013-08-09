//
//  Widget.h
//  allAddonsExample
//
//  Created by André Perrotta on 6/15/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_WIDGET_H
#define MARSYAS_WIDGET_H

//#include "ofMain.h" FIXME: is this really necessary



namespace Marsyas
{

class GraphicalEnvironment;

class Widget
{
protected:
  GraphicalEnvironment* env_;

  int x_;
  int y_;
  int width_;
  int height_;

  bool state_;

  bool dragLock_;

  bool isMouseOver_;
  double widthForMouse_;
  double heightForMouse_;
  double xForMouse_;
  double yForMouse_;

public:


  Widget();
  Widget(int x0, int y0, int w, int h, GraphicalEnvironment* env);

  virtual ~Widget();

  virtual void setup(int x0, int y0, int w, int h, GraphicalEnvironment* env);

  virtual void update() = 0;
  virtual void draw() = 0;

  virtual void updatePosition(int x0, int y0);
  virtual void updatePosition(int x0, int y0, double zx, double zy, int ox, int oy);
  virtual void updatePosition(int x0, int y0, int w0, int h0);
  virtual void updatePosition(int x0, int y0, int w0, int h0, double zx, double zy, int ox, int oy);


  virtual void toggleState();

  //Mouse methods
  virtual bool mouseOver();
  virtual bool mousePressed();
  virtual bool mouseDragged();
  virtual bool mouseReleased();
  virtual void updateValuesForMouse(double zoomX, double zoomY, int offsetX, int offsetY);

  //getters
  int getX();
  int getY();
  int getWidth();
  int getHeight();
  bool getState();
  GraphicalEnvironment *getEnvironment();

  //setters
  void setEnv(GraphicalEnvironment* env);

  void setX(int x);
  void setY(int y);
  void setWidth(int w);
  void setHeight(int h);
  void setState(bool st);

};
}

#endif