#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QLCDNumber>
#include <QSlider>
#include <QPushButton>
using namespace std;

class GLWidget;

class Window : public QWidget
{
Q_OBJECT

public:
  Window(string inAudioFileName);

private:
  QSlider *createSlider(int,int,int,int,int);
   QSlider *createRotationSlider();
//   QSlider *createTimerSlider();

  // The OpenGL widget
  GLWidget *glWidget;

  // Sliders to rotate the object on its axes
  QSlider *xSlider;
  QSlider *ySlider;
  QSlider *zSlider;

  // Scale sliders
  QSlider *yScaleSlider;

  // Some sliders to test with
  QSlider *test_xSlider;
  QSlider *test_ySlider;
  QSlider *test_zSlider;

//   // The end position of the object for the animation
//   QSlider *end_xSlider;
//   QSlider *end_ySlider;
//   QSlider *end_zSlider;

//   // The speed of the animation
//   QSlider *speedSlider;

//   // The current time
//   QLCDNumber *currentTimeLCD;

   QPushButton *playpause_button;
};

#endif
