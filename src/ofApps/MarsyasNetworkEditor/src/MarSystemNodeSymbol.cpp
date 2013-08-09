//
//  MarSystemNodeSymbol.cpp
//  allAddonsExample
//
//  Created by André Perrotta on 7/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#include "ofMain.h"
#include "MarSystemNodeSymbol.h"


using namespace Marsyas;

MarSystemNodeSymbol::MarSystemNodeSymbol()
{
  setup("foo");
}

/*
MarSystemNodeSymbol::MarSystemNodeSymbol(std::string kind, int x, int y, int w, int h)
{
	Widget::setup(x, y, w, h, NULL);
	kind_ = kind;
}
*/

MarSystemNodeSymbol::~MarSystemNodeSymbol()
{

}


void MarSystemNodeSymbol::setup(std::string kind)
{
  kind_ = kind;
}


void MarSystemNodeSymbol::update()
{

}

void MarSystemNodeSymbol::draw()
{

}

void MarSystemNodeSymbol::update(int x, int y, int w, int h)
{
  updatePosition(x, y);
}

void MarSystemNodeSymbol::draw(int x, int y)
{

  ofSetColor(0, 0, 0);
  ofFill();
  ofCircle(x, y, 20);
  ofSetColor(255, 255, 0);
  ofNoFill();
  ofCircle(x, y, 20);

}