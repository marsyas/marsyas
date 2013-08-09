//
//  SeriesWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 6/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_SERIES_WIDGET_H
#define MARSYAS_SERIES_WIDGET_H


#include "MarSystemWidget.h"

//#undef PI //FIXME

class testApp;

namespace Marsyas
{

class SeriesWidget : public MarSystemWidget
{
private:
  void addControls();
  void updateControls();


public:
  SeriesWidget(MarSystem* msys, GraphicalEnvironment* env);
  ~SeriesWidget();


  void drawConnections();
};
}

#endif