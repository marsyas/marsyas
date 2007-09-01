/*
**
** Copyright (C) 2005 George Tzanetakis. All rights reserved.
**
*/ 

#include <QtGui>

#include "mainwindow.h"
#include "mdichild.h"

MainWindow::MainWindow()
{
  workspace = new QWorkspace;
  setCentralWidget(workspace);
  connect(workspace, SIGNAL(windowActivated(QWidget *)),
	  this, SLOT(updateMenus()));
  windowMapper = new QSignalMapper(this);
  connect(windowMapper, SIGNAL(mapped(QWidget *)),
	  workspace, SLOT(setActiveWindow(QWidget *)));
  
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();
  updateMenus();
  
  readSettings();
  setWindowTitle(tr("Musescape"));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  workspace->closeAllWindows();
  if (activeMdiChild()) {
    event->ignore();
  } else {
    writeSettings();
    event->accept();
  }
}

void MainWindow::newFile()
{
  MdiChild *child = createMdiChild();
  child->newFile();
  child->show();
}

void MainWindow::open()
{
  QString fileName = QFileDialog::getOpenFileName(this);
  if (!fileName.isEmpty()) {
    MdiChild *existing = findMdiChild(fileName);
    if (existing) {
      workspace->setActiveWindow(existing);
      return;
    }

    MdiChild *child = createMdiChild();
    if (child->loadFile(fileName)) {
      statusBar()->showMessage(tr("File loaded"), 2000);
      child->show();
    } else {
      child->close();
    }
  }
}

void MainWindow::save()
{
  if (activeMdiChild()->save())
    statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::saveAs()
{
  if (activeMdiChild()->saveAs())
    statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWindow::cut()
{
  // activeMdiChild()->cut();
}

void MainWindow::copy()
{
  // activeMdiChild()->copy();
}

void MainWindow::paste()
{
  // activeMdiChild()->paste();
}

void MainWindow::about()
{
  QMessageBox::about(this, tr("About MDI"),
		     tr("The <b>MDI</b> example demonstrates how to write multiple "
			"document interface applications using Qt."));
}

void MainWindow::updateMenus()
{
  bool hasMdiChild = (activeMdiChild() != 0);
  saveAct->setEnabled(hasMdiChild);
  saveAsAct->setEnabled(hasMdiChild);
  pasteAct->setEnabled(hasMdiChild);
  closeAct->setEnabled(hasMdiChild);
  closeAllAct->setEnabled(hasMdiChild);
  tileAct->setEnabled(hasMdiChild);
  cascadeAct->setEnabled(hasMdiChild);
  nextAct->setEnabled(hasMdiChild);
  previousAct->setEnabled(hasMdiChild);
  separatorAct->setVisible(hasMdiChild);

  /* bool hasSelection = (activeMdiChild() &&
		       activeMdiChild()->textCursor().hasSelection());
  */ 
  // cutAct->setEnabled(hasSelection);
  // copyAct->setEnabled(hasSelection);
}

void MainWindow::updateWindowMenu()
{
  windowMenu->clear();
  windowMenu->addAction(closeAct);
  windowMenu->addAction(closeAllAct);
  windowMenu->addSeparator();
  windowMenu->addAction(tileAct);
  windowMenu->addAction(cascadeAct);
  windowMenu->addSeparator();
  windowMenu->addAction(nextAct);
  windowMenu->addAction(previousAct);
  windowMenu->addAction(separatorAct);

  QList<QWidget *> windows = workspace->windowList();
  separatorAct->setVisible(!windows.isEmpty());

  for (int i = 0; i < windows.size(); ++i) {
    MdiChild *child = qobject_cast<MdiChild *>(windows.at(i));

    QString text;
    if (i < 9) {
      text = tr("&%1. %2").arg(i + 1)
	.arg(child->userFriendlyCurrentFile());
    } else {
      text = tr("%1. %2").arg(i + 1)
	.arg(child->userFriendlyCurrentFile());
    }
    QAction *action  = windowMenu->addAction(text);
    action->setCheckable(true);
    action ->setChecked(child == activeMdiChild());
    connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
    windowMapper->setMapping(action, child);
  }
}

MdiChild *MainWindow::createMdiChild()
{
  MdiChild *child = new MdiChild;
  workspace->addWindow(child);

  /* connect(child, SIGNAL(copyAvailable(bool)),
	  cutAct, SLOT(setEnabled(bool)));
  connect(child, SIGNAL(copyAvailable(bool)),
	  copyAct, SLOT(setEnabled(bool)));
  */ 

  return child;
}

void MainWindow::createActions()
{
  newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
  newAct->setShortcut(tr("Ctrl+N"));
  newAct->setStatusTip(tr("Create a new file"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

  openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
  saveAct->setShortcut(tr("Ctrl+S"));
  saveAct->setStatusTip(tr("Save the document to disk"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  saveAsAct = new QAction(tr("Save &As..."), this);
  saveAsAct->setStatusTip(tr("Save the document under a new name"));
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  exitAct = new QAction(tr("E&xit"), this);
  exitAct->setShortcut(tr("Ctrl+Q"));
  exitAct->setStatusTip(tr("Exit the application"));
  connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

  cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
  cutAct->setShortcut(tr("Ctrl+X"));
  cutAct->setStatusTip(tr("Cut the current selection's contents to the "
			  "clipboard"));
  connect(cutAct, SIGNAL(triggered()), this, SLOT(cut()));

  copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
  copyAct->setShortcut(tr("Ctrl+C"));
  copyAct->setStatusTip(tr("Copy the current selection's contents to the "
			   "clipboard"));
  connect(copyAct, SIGNAL(triggered()), this, SLOT(copy()));

  pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
  pasteAct->setShortcut(tr("Ctrl+V"));
  pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
			    "selection"));
  connect(pasteAct, SIGNAL(triggered()), this, SLOT(paste()));

  closeAct = new QAction(tr("Cl&ose"), this);
  closeAct->setShortcut(tr("Ctrl+F4"));
  closeAct->setStatusTip(tr("Close the active window"));
  connect(closeAct, SIGNAL(triggered()),
	  workspace, SLOT(closeActiveWindow()));

  closeAllAct = new QAction(tr("Close &All"), this);
  closeAllAct->setStatusTip(tr("Close all the windows"));
  connect(closeAllAct, SIGNAL(triggered()),
	  workspace, SLOT(closeAllWindows()));

  tileAct = new QAction(tr("&Tile"), this);
  tileAct->setStatusTip(tr("Tile the windows"));
  connect(tileAct, SIGNAL(triggered()), workspace, SLOT(tile()));

  cascadeAct = new QAction(tr("&Cascade"), this);
  cascadeAct->setStatusTip(tr("Cascade the windows"));
  connect(cascadeAct, SIGNAL(triggered()), workspace, SLOT(cascade()));

  nextAct = new QAction(tr("Ne&xt"), this);
  nextAct->setShortcut(tr("Ctrl+F6"));
  nextAct->setStatusTip(tr("Move the focus to the next window"));
  connect(nextAct, SIGNAL(triggered()),
	  workspace, SLOT(activateNextWindow()));

  previousAct = new QAction(tr("Pre&vious"), this);
  previousAct->setShortcut(tr("Ctrl+Shift+F6"));
  previousAct->setStatusTip(tr("Move the focus to the previous "
			       "window"));
  connect(previousAct, SIGNAL(triggered()),
	  workspace, SLOT(activatePreviousWindow()));

  separatorAct = new QAction(this);
  separatorAct->setSeparator(true);

  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

  aboutQtAct = new QAction(tr("About &Qt"), this);
  aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
  connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(saveAsAct);
  fileMenu->addSeparator();
  fileMenu->addAction(exitAct);

  editMenu = menuBar()->addMenu(tr("&Edit"));
  editMenu->addAction(cutAct);
  editMenu->addAction(copyAct);
  editMenu->addAction(pasteAct);

  windowMenu = menuBar()->addMenu(tr("&Window"));
  connect(windowMenu, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

  menuBar()->addSeparator();

  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
  helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
  fileToolBar = addToolBar(tr("File"));
  fileToolBar->addAction(newAct);
  fileToolBar->addAction(openAct);
  fileToolBar->addAction(saveAct);

  editToolBar = addToolBar(tr("Edit"));
  editToolBar->addAction(cutAct);
  editToolBar->addAction(copyAct);
  editToolBar->addAction(pasteAct);
}

void MainWindow::createStatusBar()
{
  statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
  QSettings settings("Trolltech", "MDI Example");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(400, 400)).toSize();
  move(pos);
  resize(size);
}

void MainWindow::writeSettings()
{
  QSettings settings("Trolltech", "MDI Example");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
}

MdiChild *MainWindow::activeMdiChild()
{
  return qobject_cast<MdiChild *>(workspace->activeWindow());
}

MdiChild *MainWindow::findMdiChild(const QString &fileName)
{
  QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

  foreach (QWidget *window, workspace->windowList()) {
    MdiChild *mdiChild = qobject_cast<MdiChild *>(window);
    if (mdiChild->currentFile() == canonicalFilePath)
      return mdiChild;
  }
  return 0;
}
