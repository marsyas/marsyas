#include "window.h"
#include "glwidget.h"

#include <QLCDNumber>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

Window::Window(const QString & inAudioFileName)
{

  mainWidget = new QWidget();

  setCentralWidget(mainWidget);

  glWidget = new GLWidget(inAudioFileName);

  glWidget->setMinimumSize(500,500);
  glWidget->setMaximumSize(500,500);

  createActions();
  createMenus();

  // A play/pause button
  playpause_button  = new QPushButton(tr("Play/Pause"));
  // Connect a click signal on the go button to a slot to start the rotation time
  connect(playpause_button, SIGNAL(clicked()), glWidget, SLOT(playPause()));

  // A main layout to hold everything
  QHBoxLayout *layout = new QHBoxLayout;

  // The OpenGL window and the sliders to move it interactively
  QVBoxLayout *gl_layout = new QVBoxLayout;
  gl_layout->addWidget(glWidget);


  // Controls for the animation
  QVBoxLayout *buttons_layout = new QVBoxLayout;
  // buttons_layout->addWidget(powerSpectrumModeLabel);
  // buttons_layout->addWidget(powerSpectrumModeCombo);
  buttons_layout->addWidget(playpause_button);
  gl_layout->addLayout(buttons_layout);

  layout->addLayout(gl_layout);

  // Set the layout for this widget to the layout we just created
  mainWidget->setLayout(layout);

  setWindowTitle(tr("MarSndPeek"));
}

void Window::createMenus()
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  menuBar()->addSeparator();
  helpMenu = menuBar()->addMenu(tr("&Help"));
  helpMenu->addAction(aboutAct);
}

void Window::createActions()
{
  openAct = new QAction(tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, SIGNAL(triggered()), glWidget, SLOT(open()));
  aboutAct = new QAction(tr("&About"), this);
  aboutAct->setStatusTip(tr("Show the application's About box"));
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void
Window::about()
{
  QMessageBox::about(this, tr("Marsyas MarCorrelogram"),
                     tr("Marsyas MarCorrelogram : A graphical user interface for the \n real time generation of correlograms\n \n \n written by sness (c) 2010 GPL - sness@sness.net"));

}

// The minimum size of the widget
QSize Window::minimumSizeHint() const
{
  return QSize(600, 600);
}

// The maximum size of the widget
QSize Window::sizeHint() const
{
  return QSize(800, 800);
}
