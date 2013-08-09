
#ifndef VAUXELOBJECT_H_
#define VAUXELOBJECT_H_

#include "QGLObject.h"

class VauxelObject : public QGLObject
{
public:
  VauxelObject();
  ~VauxelObject();

  void draw() const;
};

#endif // VAUXELOBJECT_H_