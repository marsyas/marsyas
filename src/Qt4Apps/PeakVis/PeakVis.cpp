
/** 
   \class PeakVis
   \brief QGLViewer for spectrogram peak files (.peak)
*/

#include <QMouseEvent>

#include "manipulatedFrameSetConstraint.h"
#include "PeakVis.h"
#include "peakView.h"

using namespace qglviewer;
using namespace std;
using namespace Marsyas;

#ifdef LORIS_LIB
using namespace Loris;
#endif

const double PeakVis::MAX_FREQ_RENDERED = 5000.;

PeakVis::PeakVis(QWidget *parent)
: QGLViewer(parent)
{            
   selectionMode_ = NONE; 
}

PeakVis::~PeakVis()
{
   saveStateToFile();   
   deleteObjects();
} 

void
PeakVis::deleteObjects()
{
   for_each( peakObjects_.begin(), peakObjects_.end(), doDelete<PeakObject>() );
   for_each( partialObjects_.begin(), partialObjects_.end(), doDelete<PartialObject>() );
   peakObjects_.clear();
   partialObjects_.clear();
}

void
PeakVis::init()
{
   // A ManipulatedFrameSetConstraint will apply displacements to the selection
   setManipulatedFrame(new ManipulatedFrame());
   manipulatedFrame()->setConstraint(new ManipulatedFrameSetConstraint());
   
   Camera *cam = camera();
   cam->setFieldOfView(0.785398);
   cam->setZNearCoefficient(0.005);
   cam->setZClippingCoefficient(1.73205);
   cam->setPosition(Vec(1.77648, 0.534191, 0.92479));
   cam->setOrientation(Quaternion(-0.318955, -0.323034, -0.701596, -0.549254));
      
   // Used to display semi-transparent reflection rectangle
   glBlendFunc(GL_ONE, GL_ONE);
   
   restoreStateFromFile();
   help();   
}

QString 
PeakVis::helpString() const
{
   QString text("<h1>PeakVis v.0.1</h1><br>(&copy; 2007 Jennifer Murdoch)");
   text += "<h2>This will (not) change your life...</h2>";
   text += "My first OpenGL app!... Hello World!<br><br><br>";
      
   text += "<b>Rotate</b> the camera around the scene using the <b>left mouse button</b>.<br>";
   text += "<b>Pan</b> the camera using the <b>right mouse button</b>.<br><br>";
   
   text += "<b>Object selection</b> is preformed using the left mouse button. Press <b>Shift</b> to add objects ";
   text += "to the selection, and <b>Alt</b> to remove objects from the selection.<br><br>";
   text += "Individual objects (click on them) as well as rectangular regions (click and drag mouse) can be selected. ";
   text += "To do this, the selection region size is modified and the <code>endSelection()</code> function ";
   text += "has been overloaded so that <i>all</i> the objects of the region are taken into account ";
   text += "(the default implementation only selects the closest object).<br><br>";
   text += "The selected objects can then be manipulated by pressing the <b>Control</b> key. ";
   text += "Other set operations (parameter edition, deletion...) can also easily be applied to the selected objects.";   
   return text;
}

void
PeakVis::addPeakObjects(realvec &data)
{   
   double nbFrames = data(0,4);
   double firstFrame = data(1,peakView::pkFrame);
   //float maxFreq = 5000.; //(float)data(0,1)/2.;  
   
   float scaled_x, scaled_y;
   
//   peakObjects_.clear();
//   partialObjects_.clear();
   deleteObjects();
   selection_.clear();
   
   setSelectBufferSize(data.getRows() * 5);   
   
   for( int i=1 ; i<data.getRows() ; i++ )
   {
      // Only store those partials/peaks with frequency < 5000 Hz      
      if( data(i,peakView::pkFrequency) < MAX_FREQ_RENDERED )
      {
         PeakObject* o = new PeakObject(data(i,peakView::pkFrequency), 
                                        data(i,peakView::pkAmplitude),
                                        data(i,peakView::pkPhase),
                                        data(i,peakView::pkFrame),
                                        data(i,peakView::pkGroup));
         
         scaled_x = (float)((data(i,peakView::pkFrame)-firstFrame)/nbFrames);
         scaled_y = (float)(data(i,peakView::pkFrequency)/MAX_FREQ_RENDERED);      
         o->frame.setPosition( Vec( scaled_y, scaled_x, 0.0f ) );
         peakObjects_.append(o);
      }
   }
   
   updateGL();   
}

void
PeakVis::addPartialObjects(realvec &data)
{
   double nbFrames = data(0,4);
   double firstFrame = data(1,peakView::pkFrame);
   
   float scaled_x, scaled_y;
   
//   peakObjects_.clear();
//   partialObjects_.clear();
   deleteObjects();
   selection_.clear();   
   
   setSelectBufferSize(data.getRows() * 5);   

   int first = 1, second = 1;
   
   while( second < data.getRows() )
   {
      PartialObject* o = new PartialObject();
      while( second < data.getRows() && data(first,(mrs_natural)peakView::pkBin) == data(second,(mrs_natural)peakView::pkBin) )
      {
         // Only store those partials/peaks with frequency < 5000 Hz
         if( data( second, peakView::pkFrequency ) < MAX_FREQ_RENDERED )
         {
            PeakObject* peak = new PeakObject( data( second, peakView::pkFrequency ),
                                               data( second, peakView::pkAmplitude ),
                                               data( second, peakView::pkPhase ),
                                               data( second, peakView::pkFrame ), 
                                               data( second, peakView::pkGroup ) );
            
            scaled_x = (float)((data(second,peakView::pkFrame)-firstFrame)/(nbFrames));
            scaled_y = (float)(data(second,peakView::pkFrequency)/MAX_FREQ_RENDERED);          
            peak->frame.setPosition( Vec( scaled_y, scaled_x, 0.0f ) );
            o->addPeak(peak);
         }
         second++;
      }
      if( o->peaks_.size() > 0 )
      {
         //o->setIndex( data( second, peakView::pkBin ) ); // 'pkBin' contains partial index for now         
         o->setIndex( partialObjects_.size() ); // Do this instead to get consecutive indicies
         partialObjects_.append(o);
      }
      else
         delete o;
      
      first = second;
   }
   
   updateGL();
}

#ifdef LORIS_LIB   
void 
PeakVis::addPartialObjects(PartialList &data)
{
   PartialList::iterator it;
   double startTime = MAXREAL, endTime = 0, nbPeaks = 0;
   float scaled_x, scaled_y;
   
//   peakObjects_.clear();
//   partialObjects_.clear();
   deleteObjects();
   selection_.clear();   
   
   // Allows us to set the position of the partialObject here
   // But loop IS required so that we can determine how to set buffer size - must have)
   for( it = data.begin() ; it != data.end() ; ++it )
   {
      if( it->startTime() < startTime )
         startTime = it->startTime();
      if( it->endTime() > endTime )
         endTime = it->endTime();
      nbPeaks += it->size();
   }
   
   setSelectBufferSize(nbPeaks * 5);   
   
   for( it = data.begin() ; it != data.end() ; ++it )
   {
      PartialObject* o = new PartialObject();      
      for( Partial_Iterator pit = it->begin() ; pit != it->end() ; ++pit )
      {
         // Only store those partials/peaks with frequency < 5000 Hz         
         if( pit->frequency() < MAX_FREQ_RENDERED )
         {
            PeakObject* peak = new PeakObject(pit->frequency(), pit->amplitude(), pit->phase(), pit.time(), 0);         
            scaled_x = (float)((pit.time()-startTime)/(endTime-startTime));
            scaled_y = (float)(pit->frequency()/MAX_FREQ_RENDERED);  
            peak->frame.setPosition( Vec( scaled_y, scaled_x, 0.0f ) );
            o->addPeak(peak);
         }
      }
      if( o->peaks_.size() > 0 )
      {
         o->setIndex( partialObjects_.size() );  
         partialObjects_.append(o);
      }
      else
         delete o;
   }
   
   updateGL();      
}
#endif


//  D r a w i n g   f u n c t i o n

void PeakVis::draw()
{
   // Draws selected objects only.
   //glColor3f(0.9, 0.3, 0.3);
   glColor3f(0.,1.,0.);
   for (QList<int>::const_iterator it=selection_.begin(), end=selection_.end(); it != end; ++it)
      peakObjects_.at(*it)->draw();
   
   // Draws all the peakObjects. Selected ones are not repainted because of GL depth test.
   glColor3f(0.8, 0.8, 0.8);
   double sqrt_amp, norm_freq;
   for (int i=0; i<int(peakObjects_.size()); i++)
   {
      sqrt_amp = sqrt(sqrt(peakObjects_.at(i)->amp_));
      norm_freq = peakObjects_.at(i)->freq_ / MAX_FREQ_RENDERED;
      glColor3f(1.0f-sqrt_amp, /* 0.0f */ norm_freq, sqrt_amp);      
      peakObjects_.at(i)->draw();
   }
   
   // Draws all the partialObjects. Selected ones are not repainted because of GL depth test.
   for (int i=0; i<int(partialObjects_.size()); i++)
   {   
      //partialObjects_.at(i)->draw();
      partialObjects_.at(i)->draw( partialObjects_.size() );
   }
   
   // Draws manipulatedFrame (the set's rotation center)
   if (manipulatedFrame()->isManipulated())
   {
      glPushMatrix();
      glMultMatrixd(manipulatedFrame()->matrix());
      drawAxis(0.5);
      glPopMatrix();
   }
   
   // Draws rectangular selection area. Could be done in postDraw() instead.
   if (selectionMode_ != NONE)
      drawSelectionRectangle();
   
   glPushMatrix();
   glMultMatrixd(manipulatedFrame()->matrix());
   glColor3f( 0.9, 0.3, 0.3 );  
   drawAxis(1.15);
   QGLWidget::renderText(0.5,-0.15,0.1,"Frequency");
   QGLWidget::renderText(-0.15,0.5,0.1,"Time Frame");
   QGLWidget::renderText(0.05,0.05,0.3,"Amplitude");  
   //  setSceneCenter(Vec(0.5, 0.5, 0));
   //  setSceneRadius(0.75);
   glTranslatef(0.5, 0.5, 0.);  
   drawGrid( 0.5f, 20 );
   glPopMatrix();     
}


//   C u s t o m i z e d   m o u s e   e v e n t s

void PeakVis::mousePressEvent(QMouseEvent* e)
{
   // Start selection. Mode is ADD with Shift key and TOGGLE with Alt key.
   rectangle_ = QRect(e->pos(), e->pos());
	
   if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::ShiftModifier))
      selectionMode_ = ADD;
   else
      if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::AltModifier))
         selectionMode_ = REMOVE;
   else
   {
      if (e->modifiers() == Qt::ControlModifier)      
         
         startManipulation();
      QGLViewer::mousePressEvent(e);
   }
}

void PeakVis::mouseMoveEvent(QMouseEvent* e)
{
   if (selectionMode_ != NONE)
   {
      // Updates rectangle_ coordinates and redraws rectangle
      
      rectangle_.setBottomRight(e->pos());
      updateGL();
   }
   else
      QGLViewer::mouseMoveEvent(e);
}

void PeakVis::mouseReleaseEvent(QMouseEvent* e)
{
   if (selectionMode_ != NONE)
   {
      // Actual selection on the rectangular area.
      // Possibly swap left/right and top/bottom to make rectangle_ valid.
      rectangle_ = rectangle_.normalized();
      
      // Define selection window dimensions
      setSelectRegionWidth(rectangle_.width());
      setSelectRegionHeight(rectangle_.height());
      // Compute rectangle center and perform selection
      select(rectangle_.center());
      // Update display to show new selected objects
      updateGL();
   }
   else
      QGLViewer::mouseReleaseEvent(e);
}


//   C u s t o m i z e d   s e l e c t i o n   p r o c e s s

void PeakVis::drawWithNames()
{
   for (int i=0; i<int(peakObjects_.size()); i++)
   {
      glPushName(i);
      peakObjects_.at(i)->draw();
      glPopName();
   }
   for (int i=0; i<int(partialObjects_.size()); i++)
   {
      glPushName(i);
      partialObjects_.at(i)->draw();
      glPopName();
   }   
}

void PeakVis::endSelection(const QPoint&)
{
   // Flush GL buffers
   glFlush();
   
   // Get the number of objects that were seen through the pick matrix frustum. Reset GL_RENDER mode.
   GLint nbHits = glRenderMode(GL_RENDER);
   
   if (nbHits > 0)
   {
      // Interpret results : each object created 4 values in the selectBuffer().
      // (selectBuffer())[4*i+3] is the id pushed on the stack.
      for (int i=0; i<nbHits; ++i)
         switch (selectionMode_)
         {
            case ADD    : addIdToSelection((selectBuffer())[4*i+3]); break;
            case REMOVE : removeIdFromSelection((selectBuffer())[4*i+3]);  break;
            default : break;
         }
   }
   selectionMode_ = NONE;
}

void PeakVis::startManipulation()
{   
   Vec averagePosition;
   ManipulatedFrameSetConstraint* mfsc = (ManipulatedFrameSetConstraint*)(manipulatedFrame()->constraint());
   mfsc->clearSet();
   
   for (QList<int>::const_iterator it=selection_.begin(), end=selection_.end(); it != end; ++it)
   {
      mfsc->addObjectToSet(peakObjects_[*it]);
      averagePosition += peakObjects_[*it]->frame.position();
   }
   
   if (selection_.size() > 0)
      manipulatedFrame()->setPosition(averagePosition / selection_.size());
    
}


//   S e l e c t i o n   t o o l s

void PeakVis::addIdToSelection(int id)
{
   if (!selection_.contains(id))
      selection_.push_back(id);
}

void PeakVis::removeIdFromSelection(int id)
{
   selection_.removeAll(id);
}

void PeakVis::drawSelectionRectangle() const
{
   startScreenCoordinatesSystem();
   glDisable(GL_LIGHTING);
   glEnable(GL_BLEND);
   
   glColor4f(0.0, 0.0, 0.3, 0.3);
   glBegin(GL_QUADS);
   glVertex2i(rectangle_.left(),  rectangle_.top());
   glVertex2i(rectangle_.right(), rectangle_.top());
   glVertex2i(rectangle_.right(), rectangle_.bottom());
   glVertex2i(rectangle_.left(),  rectangle_.bottom());
   glEnd();
   
   glLineWidth(2.0);
   glColor4f(0.4, 0.4, 0.5, 0.5);
   glBegin(GL_LINE_LOOP);
   glVertex2i(rectangle_.left(),  rectangle_.top());
   glVertex2i(rectangle_.right(), rectangle_.top());
   glVertex2i(rectangle_.right(), rectangle_.bottom());
   glVertex2i(rectangle_.left(),  rectangle_.bottom());
   glEnd();
   
   glDisable(GL_BLEND);
   glEnable(GL_LIGHTING);
   stopScreenCoordinatesSystem();
}


