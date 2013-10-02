//
//  MarSystemWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 6/17/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_MARSYSTEM_WIDGET_H
#define MARSYAS_MARSYSTEM_WIDGET_H

#include "Widget.h"
#include "ofMain.h"


#undef PI //FIXME: lmartins!!!

#include <marsyas/system/MarSystemManager.h>

#include "GraphicalParameters.h"




namespace Marsyas
{
class MarControlWidget;
class MaximizeButton;
class MarSystemNodeSymbol;
class GraphicalEnvironment;
class ProcessedDataWidget;

class MarSystemWidget : public Widget
{
protected:

  /// Parent MarSystem (if in a composite, otherwise it's NULL)
  MarSystemWidget* parent_;

  /// Type of MarSystem
  std::string type_;

  /// Name of instance
  std::string name_;

  /// /type_/name_/
  std::string prefix_;

  /// /parent0Type/parent0Name/.../parentNType/parentNName/type_/name_/
  /// in case this MarSystem is part of a composite
  /// this is the absolute path to it
  std::string absPath_;

  /// flag if this is a composite
  bool isComposite_;

  /// children
  std::vector <MarSystemWidget*> children_;

  //pointer to corresponding MarSystem
  MarSystem *msys_;

  virtual void setup(MarSystem *msys);

  //reference tables stuff
  MarSystemWidget* topMostParent_;

  //////////////////////////////////////
  //controls stuff
  std::map<MarSystem*, MarSystemWidget*> msysMap_;
  //std::map<MarControlPtr, MarControlWidget*> ctrlMap_; //FIXME: make this protected

  std::vector<MarControlWidget*> ctrlWidgets_;

  ofPoint hiddenControlPosition_;
  //////////////////////////////////////


  ///////////////////////////////////
  //Probing
  //////////////////////////////////
  ProcessedDataWidget* probe_;
  int osRate_;
  int onSamples_;



  ////////////////////////////////////
  //drawing stuff
  ///////////////////////////////////

  MarSystemNodeSymbol* InputNodeSymbol_;
  MarSystemNodeSymbol* OutputNodeSymbol_;


  MaximizeButton* mBtn_;

  int childrenTotalWidth_;
  int childrenTotalHeight_;

  bool drawChildren_;
  bool showChildrenConnections_;

  int nameWidth_;
  int nameHeight_;

  //fonts
  float sizeScale_;
  float fontScale_;

  //methods to be overrided by derived classes
  virtual void updateChildrenWidgets();
  virtual void updateSize();
  virtual void drawChildrenWidgets();

  virtual void updateControlWidgets();
  virtual void drawControlWidgets();

  virtual void drawConnections();


  int auxCounter;

public:
  MarSystemWidget();
  MarSystemWidget(MarSystem *msys, GraphicalEnvironment* env);
  MarSystemWidget(int x0, int y0, int w, int h, MarSystem *msysm, GraphicalEnvironment* env);

  virtual ~MarSystemWidget();

  //MarSystem pointer getter/setter
  void setMarSystem(MarSystem *msys);
  MarSystem* getMarSystem();

  void setTopMostParent();
  MarSystemWidget* getTopMostParent();

  void addMarSystemWidget(MarSystemWidget *msysWidget);

  //overrides from Widget
  void update();
  void draw();

  void setParent(MarSystemWidget *msysWidget);

  void insertToMsysMap(MarSystem* msys, MarSystemWidget* msysw);

  //getters
  virtual string getName();
  std::vector<MarSystemWidget*> getChildren();
  MarSystemWidget* getParent();
  //FIXME: use const to prevent changes to the MarControlWidget* !!!
  std::vector<MarControlWidget*> getControlWidgets();
  ofPoint getHiddenControlPosition();

  MarControlWidget* getMarControlWidgetFromCtrlMap(MarControlPtr ptr);

  MarSystemWidget* getMarSystemWidgetFromMapByName(string name);
  MarSystemWidget* getMarSystemWidgetFromMap(MarSystem* msys);

  int getOsRate();
  int getOnSamples();

  //controls
  std::map<MarControlPtr, MarControlWidget*> ctrlMap_; //FIXME: make this protected
  void createControlWidgets();
  void createControlLinkWidgets();

  //mouse
  bool mouseOver();
  bool mousePressed();
  bool mouseDragged();
  bool mouseReleased();

  bool isMaximized();



  ////////////////////////////////////
  //drawing stuff
  ///////////////////////////////////
  int maxChildrenWidth_;
  void updateButtonPosition();

  void showFamilyUp();

  void switchChildrenConnections(bool state);
  void switchLinkedControlConnections(bool state);

  //for debugging purposes
  void debugger();
  std::map<int, MarControlPtr> testMap;

};



}


















#endif