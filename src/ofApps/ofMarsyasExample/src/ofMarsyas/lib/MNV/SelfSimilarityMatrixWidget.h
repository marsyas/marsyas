//
//  SelfSimilarityMatrixWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef MARSYAS_SELF_SIMILARITY_MATRIX_WIDGET_H
#define MARSYAS_SELF_SIMILARITY_MATRIX_WIDGET_H

#include "MarSystemWidget.h"

namespace Marsyas
{

class SelfSimilarityMatrixWidget : public MarSystemWidget
{
private:
  void addControls();
  void updateControls();


public:
  SelfSimilarityMatrixWidget(MarSystem *msys, GraphicalEnvironment* env);
  ~SelfSimilarityMatrixWidget();


  void drawConnections();
};
}

#endif