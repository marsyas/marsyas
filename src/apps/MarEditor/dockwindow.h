#ifndef DOCKWINDOW_H
#define DOCKWINDOW_H

#include <QDockWidget>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QAction>

class DockWindow : public QDockWidget
{
  Q_OBJECT

public:
  DockWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
  DockWindow(const QString & title, QWidget * parent = 0, Qt::WFlags flags = 0);
  ~DockWindow();

protected:
  virtual void resizeEvent(QResizeEvent* e);
  virtual void moveEvent(QMoveEvent* e);
  virtual void contextMenuEvent (QContextMenuEvent *e);

private:
  QSize floatSize_;
  QPoint floatPos_;
  bool goFloat_;
  QAction* actionNoDocking_;

  void init();

private slots:
  void onTopLevelChanged(bool topLevel);
  void onNoDockingToggled(bool checked);
};

#endif // DOCKWINDOW_H

