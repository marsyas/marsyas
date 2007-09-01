
/** 
   \class PartialObject
   \brief Data for rendering a partial
*/

#include <qgl.h>
#include "partialObject.h"

using namespace qglviewer;
using namespace std;

PartialObject::PartialObject()
{
   index_ = 0;
}

PartialObject::~PartialObject()
{
   for_each( peaks_.begin(), peaks_.end(), doDelete<PeakObject>() );
   peaks_.clear();
}

void 
PartialObject::setIndex(int index)
{ index_ = index; }

void 
PartialObject::addPeak(PeakObject* po)
{
   peaks_.append(po);
}

void 
PartialObject::draw(int maxPartialObjectIndex) const
{   
   //float x, y, z;
   glColor3f(.8,.2,.2);   
   
   glBegin(GL_LINES);
   
   double sqrt_amp, norm_index, norm_freq;
   for (int i=0; i<int(peaks_.size()); i++)      
   {
      //peaks_.at(i)->frame.getPosition(x, y, z);
      //glVertex3f( x, y, 0 );
      
      sqrt_amp = sqrt(sqrt(peaks_.at(i)->amp_));
      norm_freq = peaks_.at(i)->freq_ / 5000.;      
      norm_index = (double) index_ / maxPartialObjectIndex;
      glColor3f(1.0f-norm_index, sqrt_amp, norm_index);        
      
      peaks_.at(i)->draw();
   }
   
   glEnd();
}

void 
PartialObject::draw() const
{
   //float x, y, z;
   glColor3f(.8,.2,.2);   
   
   glBegin(GL_LINES);
   
   double sqrt_amp, norm_freq;
   for (int i=0; i<int(peaks_.size()); i++)      
   {
      //peaks_.at(i)->frame.getPosition(x, y, z);
      //glVertex3f( x, y, 0 );
      
      sqrt_amp = sqrt(sqrt(peaks_.at(i)->amp_));
      norm_freq = peaks_.at(i)->freq_ / 5000.;
      glColor3f(1.0f-sqrt_amp, /* 0.0f */ norm_freq, sqrt_amp);        
      peaks_.at(i)->draw();
   }

   glEnd();
   
   //static GLUquadric* quad = gluNewQuadric();
   
   //glPushMatrix();
   //glMultMatrixd(frame.matrix());
   //gluSphere(quad, 0.03, 10, 6);
   //gluCylinder(quad, 0.03, 0.0, 0.09, 10, 1);  
   //gluSphere(quad, 0.007, 5, 3);
   //gluCylinder(quad, 0.007, 0.0, amp_, 5, 1);  
   //gluSphere(quad, 0.007, 5, 3);
   //gluCylinder(quad, 0.007, 0.0, 0.1, 5, 1);    
   //glPopMatrix();
}