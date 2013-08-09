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
  void seekPos();

private slots:
  void positionSlider(int);

public:
  Window();
  void play(const QString & fileName);

#if 0
  MarSystemQtWrapper* getMarSystemQtWrapper();
#endif
  QSize minimumSizeHint() const;
  QSize sizeHint() const;
  QSlider *xTransSlider;
  QSlider *xRotSlider;
  QSlider *yRotSlider;
  QSlider *magnitudeCutoffSlider;
  QSlider *numVerticesSlider;
  QSlider *dotSizeSlider;



private:
  QSlider *createSlider(int,int,int,int,int);
  QSlider *createRotationSlider(int min, int max);
  QSlider *createTranslationSlider();

  QWidget *mainWidget;

  // The OpenGL widget
  GLWidget *glWidget;

  // Sliders to rotate the object on its axes
  //   QSlider *zRotSlider;

  //   // Sliders to translate the object

  QSlider *yTransSlider;
  //   QSlider *zTransSlider;

  // Scale sliders
  QSlider *yScaleSlider;

  // Fog slider
  QSlider *fogStartSlider;
  QSlider *fogEndSlider;

  // Data display sliders


  // Song position
  QSlider *posSlider;

  // How fast the display moves
  QSlider *displaySpeedSlider;


  QPushButton *playpause_button;

  QMenu*   fileMenu;
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;

  void createMenus();
  void createActions();

  QLabel *fftBinsLabel;
  QComboBox *fftBinsCombo;

  QLabel *backgroundLabel;
  QComboBox *backgroundCombo;

  QCheckBox *waterfallCheckBox;
};

#endif
