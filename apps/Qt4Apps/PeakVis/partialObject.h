
/** 
   \class PartialObject
   \brief Data for rendering a partial
*/

#ifndef PARTIALOBJECT_H_
#define PARTIALOBJECT_H_

#include "frame.h"
#include "peakObject.h"

class PartialObject
{
public :
   PartialObject();
   ~PartialObject();
   
   void setIndex(int index);
   void addPeak(PeakObject* po);
   void draw() const;
   void draw(int maxPartialObjectIndex) const;
   
   //qglviewer::Frame frame;
   int index_;
   //double timeBegin_, timeEnd_;
   double group_;
   QList<PeakObject*> peaks_;
};

#endif // PARTIALOBJECT_H_
