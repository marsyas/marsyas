#include "dockwindow.h"

#include <QMenu>
#include <QMainWindow>

DockWindow::DockWindow(QWidget *parent, Qt::WFlags flags)
  : QDockWidget(parent, flags)
{
  init();
}

DockWindow::DockWindow(const QString & title, QWidget *parent, Qt::WFlags flags)
  : QDockWidget(title, parent, flags)
{
  init();
}


DockWindow::~DockWindow()
{
  delete actionNoDocking_;
}

void
DockWindow::init()
{
  goFloat_ = false;

  actionNoDocking_ = new QAction("No Docking", this);
  actionNoDocking_->setCheckable(true);

  bool r = QObject::connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(onTopLevelChanged(bool)));
  Q_ASSERT(r);
  r = QObject::connect(actionNoDocking_, SIGNAL(toggled(bool)), this, SLOT(onNoDockingToggled(bool)));
  Q_ASSERT(r);
}

void
DockWindow::resizeEvent(QResizeEvent* e)
{
  //store new size of the floating dockwindow
  if(isFloating())
    floatSize_ = e->size();

  QWidget::resizeEvent(e);
}


void
DockWindow::moveEvent(QMoveEvent* e)
{
  if(goFloat_)
  {
    if(floatPos_.isNull())
      move(e->pos());
    else
      move(floatPos_);

    goFloat_ = false; //this is set to false in resizeEvent()
    return;
  }

  //store new position of the floating dockwindow
  if(isFloating())
    floatPos_ = e->pos();

  QWidget::moveEvent(e);
}

void
DockWindow::contextMenuEvent (QContextMenuEvent *e)
{
  //only show context menu if right clicking the DockWindow Title Bar...
  if(!widget()->geometry().contains(e->pos()))
  {
    QMenu* contextMenu = ((QMainWindow*)parent())->createPopupMenu();
    contextMenu->addSeparator();
    contextMenu->addAction(actionNoDocking_);
    contextMenu->exec(QWidget::mapToGlobal(e->pos()));
  }
}

void
DockWindow::onTopLevelChanged(bool topLevel)
{
  if(topLevel) //if changed to floating...
  {
    if(floatSize_.isValid())
      resize(floatSize_);

    goFloat_ = true;
  }
  else
    goFloat_ = false;
}

void
DockWindow::onNoDockingToggled(bool checked)
{
  if(checked) //if no docking...
  {
    setAllowedAreas(0);
  }
  else
  {
    setAllowedAreas(Qt::AllDockWidgetAreas);
  }
}