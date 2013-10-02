#ifndef ABSTRACTEXTRACTOR_H
#define ABSTRACTEXTRACTOR_H

#include <QObject>

#include <marsyas/realvec.h>

class AbstractMarExtractor : public QObject
{
  Q_OBJECT

public:
  AbstractMarExtractor() {}
  virtual ~AbstractMarExtractor() {}

  virtual void setup() = 0;

public slots:
  virtual void extract(Marsyas::realvec& dest) = 0;
};


#endif /* ABSTRACTEXTRACTOR_H */

