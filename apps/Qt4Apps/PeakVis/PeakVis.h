
/** 
   \class PeakVis
   \brief QGLViewer for spectrogram peak files (.peak)
*/

#ifndef PEAKVIS_H
#define PEAKVIS_H

#include <qglviewer.h>

#include "peakObject.h"
#include "partialObject.h"
#include "realvec.h"

#ifdef LORIS_LIB
#include "PartialList.h"
#endif

class PeakVis : public QGLViewer
{
   
public :
   PeakVis(QWidget *parent);
   ~PeakVis();
   
   void addPeakObjects(Marsyas::realvec &data); 
   void addPartialObjects(Marsyas::realvec &data);
   
#ifdef LORIS_LIB   
   void addPartialObjects(Loris::PartialList &data);       
#endif   
   
   static const double MAX_FREQ_RENDERED;
   
protected :     
   virtual void draw();
   virtual void init();
   virtual QString helpString() const;
   
   // Selection functions
   virtual void drawWithNames();
   virtual void endSelection(const QPoint&);
   
   // Mouse events functions
   virtual void mousePressEvent(QMouseEvent *e);
   virtual void mouseMoveEvent(QMouseEvent *e);
   virtual void mouseReleaseEvent(QMouseEvent *e);       
   
private:       
      
   void startManipulation();
   void drawSelectionRectangle() const;
   void addIdToSelection(int id);
   void removeIdFromSelection(int id);
   
   void deleteObjects();
   
   // Current rectangular selection
   QRect rectangle_;
   
   // Different selection modes
   enum SelectionMode { NONE, ADD, REMOVE };
   SelectionMode selectionMode_;
   
   QList<PeakObject*> peakObjects_;
   QList<PartialObject*> partialObjects_;
   QList<int> selection_;
   
   //qglviewer::Frame frame;   
   
};

#endif // PEAKVIS_H

