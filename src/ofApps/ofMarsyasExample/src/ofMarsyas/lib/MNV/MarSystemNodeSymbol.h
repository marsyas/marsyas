//
//  MarSystemNodeSymbol.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/8/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_NODE_SYMBOL_H
#define MARSYAS_NODE_SYMBOL_H

#include "Widget.h"

namespace Marsyas
{
class MarSystemNodeSymbol : public Widget
{

private:
  std::string kind_;

public:
  MarSystemNodeSymbol();
  //MarSystemNodeSymbol(std::string kind, int x, int y, int w, int h);
  ~MarSystemNodeSymbol();

  void setup(std::string kind);
  void update(int x, int y, int w, int h);
  void draw(int x, int y);
  //Because they are totaly virtual in base class
  void update();
  void draw();

};
}

#endif
