#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QMainWindow>
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

  // Sliders to rotate the object on its axes
  QSlider *xSlider;
  QSlider *ySlider;
  QSlider *zSlider;

  // Scale sliders
  QSlider *yScaleSlider;

  QPushButton *playpause_button;

  QMenu*   fileMenu;
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;

  void createMenus();
  void createActions();

  QLabel *powerSpectrumModeLabel;
  QComboBox *powerSpectrumModeCombo;

  QLabel *fftBinsModeLabel;
  QComboBox *fftBinsModeCombo;

  QCheckBox *waterfallCheckBox;
};

#endif
