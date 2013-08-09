//
//  SimilarityMatrixWidget.h
//  allAddonsExample
//
//  Created by André Perrotta on 7/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef MARSYAS_SIMILARITY_MATRIX_WIDGET_H
#define MARSYAS_SIMILARITY_MATRIX_WIDGET_H


#include "MarSystemWidget.h"


namespace Marsyas
{

class SimilarityMatrixWidget : public MarSystemWidget
{
private:
  void addControls();
  void updateControls();


public:
  SimilarityMatrixWidget(MarSystem *msys, GraphicalEnvironment* env);
  ~SimilarityMatrixWidget();

  void drawConnections();
};
}

#endif