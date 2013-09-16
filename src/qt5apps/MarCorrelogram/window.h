#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLCDNumber>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
using namespace std;

class GLWidget;

class Window : public QMainWindow
{
  Q_OBJECT

public slots:
  void about();

public:
  Window(const QString & inAudioFileName);

  QSize minimumSizeHint() const;
  QSize sizeHint() const;

private:
  QSlider *createSlider(int,int,int,int,int);
  QSlider *createRotationSlider();

  QWidget *mainWidget;

  // The OpenGL widget
  GLWidget *glWidget;

  QPushButton *playpause_button;

  QMenu*   fileMenu;
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;

  void createMenus();
  void createActions();

};

#endif
