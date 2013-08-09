
#ifndef METROPOLISQGLVIEWER_H_
#define METROPOLISQGLVIEWER_H_

#include <QMouseEvent>

#include "AbstractMarClusteringViewer.h"

class MetropolisQGLViewer : public AbstractMarClusteringViewer
{
  Q_OBJECT

public:
  MetropolisQGLViewer(QWidget *parent=0);
  ~MetropolisQGLViewer();

  void addQGLObject(QGLObject *o);
  void removeQGLObject(QGLObject *o);
  void clear();

public slots:

signals:

protected:
  void init();
  QString helpString() const;
  void draw() const;

  // Selection functions
  void drawWithNames();
  void endSelection(const QPoint&);

  // Mouse event functions
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);
  void mouseReleaseEvent(QMouseEvent *e);

  // Drag and Drop event functions
  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dropEvent(QDropEvent *event);
  void startDrag(Qt::DropActions supportedActions);

  // Helper functions for selection
  void startManipulation();
  void drawSelectionRectangle() const;
  void addIdToSelection(int id);
  void removeIdFromSelection(int id);

  void deleteObjects();
};

#endif // METROPOLISQGLVIEWER_H_
