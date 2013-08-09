
#include "GlobeQGLViewer.h"

#include "manipulatedFrameSetConstraint.h"

using namespace qglviewer;
using namespace std;

GlobeQGLViewer::GlobeQGLViewer(QWidget *parent)
  : AbstractMarClusteringViewer( parent )
{
  selectionMode_ = NONE;
}

GlobeQGLViewer::~GlobeQGLViewer()
{
  saveStateToFile();
  deleteObjects();
}

void
GlobeQGLViewer::deleteObjects()
{
  // to do
}

void
GlobeQGLViewer::init()
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
GlobeQGLViewer::helpString() const
{
  QString text("<h1>Vauxel Reality v.0.1</h1><br>(&copy; 2007 Jennifer Murdoch, Stephen Hitchner)");
  text += "<h2>(Globe Viewer of Self-Organizing Map)</h2>";

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
GlobeQGLViewer::addQGLObject(QGLObject *o)
{
  // to do
}

void
GlobeQGLViewer::removeQGLObject(QGLObject *o)
{
  //to do
}

void
GlobeQGLViewer::clear()
{
  //to do
}

void
GlobeQGLViewer::draw() const
{
  //to do
}


//   C u s t o m i z e d   d r a g   a n d   d r o p   e v e n t s
void
GlobeQGLViewer::dragEnterEvent(QDragEnterEvent *event)
{
  //to do
}

void
GlobeQGLViewer::dragMoveEvent(QDragMoveEvent *event)
{
  //to do
}

void
GlobeQGLViewer::dropEvent(QDropEvent *event)
{
  //to do
}

void
GlobeQGLViewer::startDrag(Qt::DropActions supportedActions)
{
  // to do
}


//   C u s t o m i z e d   m o u s e   e v e n t s

void
GlobeQGLViewer::mousePressEvent(QMouseEvent* e)
{
  // Start selection. Mode is ADD with Shift key and TOGGLE with Alt key.
  rectangle_ = QRect(e->pos(), e->pos());

  if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::ShiftModifier))
    selectionMode_ = ADD;
  else if ((e->button() == Qt::LeftButton) && (e->modifiers() == Qt::AltModifier))
    selectionMode_ = REMOVE;
  else
  {
    if (e->modifiers() == Qt::ControlModifier)

      startManipulation();
    QGLViewer::mousePressEvent(e);
  }
}

void
GlobeQGLViewer::mouseMoveEvent(QMouseEvent* e)
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

void
GlobeQGLViewer::mouseReleaseEvent(QMouseEvent* e)
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

void
GlobeQGLViewer::drawWithNames()
{
//   for (int i=0; i<int(peakObjects_.size()); i++)
//   {
//      glPushName(i);
//      peakObjects_.at(i)->draw();
//      glPopName();
//   }
//   for (int i=0; i<int(partialObjects_.size()); i++)
//   {
//      glPushName(i);
//      partialObjects_.at(i)->draw();
//      glPopName();
//   }
}

void
GlobeQGLViewer::endSelection(const QPoint&)
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

void
GlobeQGLViewer::startManipulation()
{
  Vec averagePosition;
  ManipulatedFrameSetConstraint* mfsc = (ManipulatedFrameSetConstraint*)(manipulatedFrame()->constraint());
  mfsc->clearSet();

  for (QList<int>::const_iterator it=selection_.begin(), end=selection_.end(); it != end; ++it)
  {
//      mfsc->addObjectToSet(peakObjects_[*it]);
//      averagePosition += peakObjects_[*it]->frame.position();
  }

  if (selection_.size() > 0)
    manipulatedFrame()->setPosition(averagePosition / selection_.size());

}


//   S e l e c t i o n   t o o l s

void
GlobeQGLViewer::addIdToSelection(int id)
{
  if (!selection_.contains(id))
    selection_.push_back(id);
}

void
GlobeQGLViewer::removeIdFromSelection(int id)
{
  selection_.removeAll(id);
}

void
GlobeQGLViewer::drawSelectionRectangle() const
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


