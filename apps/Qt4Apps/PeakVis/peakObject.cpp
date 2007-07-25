
/** 
   \class PeakObject
   \brief Data for rendering a single peak
*/

#include <qgl.h>
#include "peakObject.h"

using namespace qglviewer;
using namespace std;

void PeakObject::draw() const
{
   static GLUquadric* quad = gluNewQuadric();
   
   glPushMatrix();
   glMultMatrixd(frame.matrix());
   //gluSphere(quad, 0.03, 10, 6);
   //gluCylinder(quad, 0.03, 0.0, 0.09, 10, 1);  
   //gluSphere(quad, 0.007, 5, 3);
   gluCylinder(quad, 0.007, 0.0, amp_, 5, 1);  
   glPopMatrix();
}

