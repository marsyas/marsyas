
#ifndef ABSTRACTMARCLUSTERINGVIEWER_H_
#define ABSTRACTMARCLUSTERINGVIEWER_H_

#include <qglviewer.h>

#include "QGLObject.h"

class AbstractMarClusteringViewer : public QGLViewer
{
  Q_OBJECT

public:
  AbstractMarClusteringViewer( QWidget* parent=0 )
    : QGLViewer(parent) {}
  virtual ~AbstractMarClusteringViewer() {}

  virtual void addQGLObject(QGLObject *o) = 0;
  virtual void removeQGLObject(QGLObject *o) = 0;
  virtual void clear() = 0;

public slots:

signals:
  void qglObjectSelected(QGLObject *);

protected:
  virtual void init() = 0;
  virtual QString helpString() const = 0;
  virtual void draw() const = 0;

  // Selection functions
  virtual void drawWithNames() = 0;
  virtual void endSelection(const QPoint&) = 0;

  // Mouse event functions
  virtual void mousePressEvent(QMouseEvent *e) = 0;
  virtual void mouseMoveEvent(QMouseEvent *e) = 0;
  virtual void mouseReleaseEvent(QMouseEvent *e) = 0;

  // Drag and Drop event functions
  virtual void dragEnterEvent(QDragEnterEvent *event) = 0;
  virtual void dragMoveEvent(QDragMoveEvent *event) = 0;
  virtual void dropEvent(QDropEvent *event) = 0;
  virtual void startDrag(Qt::DropActions supportedActions) = 0;

//private:

  virtual void startManipulation() = 0;
  virtual void drawSelectionRectangle() const = 0;
  virtual void addIdToSelection(int id) = 0;
  virtual void removeIdFromSelection(int id) = 0;

  // Current rectangular selection
  QRect rectangle_;

  // Different selection modes
  enum SelectionMode { NONE, ADD, REMOVE };
  SelectionMode selectionMode_;
  QList<int> selection_;

};

#endif // ABSTRACTMARCLUSTERINGVIEWER_H_

