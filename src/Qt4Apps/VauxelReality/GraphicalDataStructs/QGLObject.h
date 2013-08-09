
#ifndef QGLOBJECT_H_
#define QGLOBJECT_H_

#include "frame.h"

class QGLObject
{
public:
  QGLObject() {};

  virtual ~QGLObject() {}
  virtual void draw() const = 0;

  void setPresetColor( double r, double g, double b, double alpha=1.0 )
  { presetColor[0]=r; presetColor[1]=g; presetColor[2]=b; presetColor[3]=alpha; }

  qglviewer::Frame frame;

private:
  double presetColor[4];
};

#endif // QGLOBJECT_H_

