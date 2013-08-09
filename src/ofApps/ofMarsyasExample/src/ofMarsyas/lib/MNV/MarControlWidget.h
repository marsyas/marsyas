//
//  MarControlWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 8/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_CONTROL_WIDGET_H
#define MARSYAS_CONTROL_WIDGET_H

#include "Widget.h"
#include "MarControl.h"
#include <vector>
#include <string>
#include "GraphicalParameters.h"


namespace Marsyas
{

class MarSystem;
class MarSystemWidget;
class MarControlLinkWidget;
class MarControlLinksDiagramWidget;


class MarControlWidget : public Widget
{
private:
  MarControlPtr ctrl_;
  MarSystem *msys_;
  MarSystemWidget *msysw_;

  std::string name_;
  std::string absName_;

  //FIXME: links

  std::vector<std::pair<MarControlPtr, MarControlPtr> > linkedControls_;


  std::vector<MarControlLinkWidget*> *links_;



  void drawLinks();

public:
  MarControlWidget(MarControlPtr ctrl, MarSystemWidget *msysw);
  ~MarControlWidget();

  //widget overrides
  void update();
  void draw();


  //visualization
  bool isSelected_;
  void highlightControlWidget();

  bool showLinks_;

  //getters
  std::string getName();
  MarControlPtr getMarControlPtr()
  {
    return ctrl_;
  };

  MarSystemWidget* getMarSystemWidget();

  bool hasConnections;

  //FIXME: linking system...
  void setupLinks();
  bool hasLinks();
  std::vector<MarControlLinkWidget*>* getLinks();

  bool mouseOver();
  bool mousePressed();

  void debugger();
};

}

#endif