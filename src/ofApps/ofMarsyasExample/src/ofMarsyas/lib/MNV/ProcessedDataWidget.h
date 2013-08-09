//
//  ProcessedDataWidget.h
//  MNE
//
//  Created by Andre Perrotta on 7/2/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef ProcessedDataWidget_h
#define ProcessedDataWidget_h

#include "Widget.h"
#include "MarControl.h"


namespace Marsyas {

class MarSystemWidget;

class ProcessedDataWidget : public Widget {

protected:

  MarSystemWidget* msysw_;
  MarControlPtr pData_;

  bool showData_;

public:

  ProcessedDataWidget();
  ~ProcessedDataWidget();
  ProcessedDataWidget(MarSystemWidget* msysw, GraphicalEnvironment* env);

  void loadProcessedDataPointer(MarControlPtr pData);

  void update();
  void draw();

  bool mousePressed();

};


}


#endif
