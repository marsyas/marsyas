
/** 
   \class PeakObject
   \brief Data for rendering a single peak
*/

#ifndef PEAKOBJECT_H_
#define PEAKOBJECT_H_

#include <algorithm>
#include "frame.h"

template <class T>
struct doDelete {
   void operator()(T *target) { delete target; }
};  

class PeakObject
{
public :
   PeakObject(double frq, double amp, double pha, double frame, double grp) : freq_(frq), amp_(amp), phase_(pha), frame_(frame), group_(grp) {}
   
   void draw() const;
   
   qglviewer::Frame frame;
   double freq_,amp_,phase_,frame_,group_;
};

#endif // PEAKOBJECT_H_
